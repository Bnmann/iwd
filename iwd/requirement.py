import glob
import hashlib
import json
import logging
import os
import shutil
import subprocess
import tarfile
import urllib.request
from collections import namedtuple

from . import patch_util
from .configuration import Configuration
from .directories import Directories


def required_argument(name, dictlike):
    if name not in dictlike.keys():
        raise Exception(f'Missing required argument {name}')
    return dictlike[name]


def optional_argument(name, dictlike, default=None):
    if name not in dictlike.keys():
        return default
    return dictlike[name]


def name_version(requirement):
    return f'{requirement.name}-{requirement.version}'


class Requirement:
    def __init__(self, **kwargs):
        self.name = required_argument('name', kwargs)
        self.version = required_argument('version', kwargs)
        self.url = required_argument('url', kwargs)
        self.cmake_build = optional_argument('cmake_build', kwargs, True)
        self.copy = optional_argument('copy', kwargs, [])
        self.configuration = Configuration(
            optional_argument('configuration', kwargs, {}))
        self.cmake_directory = optional_argument(
            'cmake_directory', kwargs, None)
        self.patches = optional_argument('patch', kwargs, [])

    def get_hash(self, hash_obj=None):
        m = hashlib.sha256() if hash_obj is None else hash_obj
        m.update(self.name.encode())
        m.update(self.url.encode())
        m.update(self.version.encode())
        return m

    def install(self, configuration: Configuration, directories: Directories, force_config=None, force_generator=None):
        self.configuration.resolve_variables(configuration)
        source_dir = override_source_directory(
            self, download(self, directories))
        apply_patches(source_dir, self.patches)
        if self.cmake_build:
            build_dir = directories.make_build_directory(name_version(self))
            cmake_args = self.configuration.as_cmake_args() + configuration.as_cmake_args()
            cmake_call_base = ['cmake', '-S', source_dir, '-B', build_dir]
            if force_generator is not None:
                cmake_call_base += ['-G', force_generator]
            subprocess.check_call(cmake_call_base + cmake_args)
            install_args = ['cmake', '--build',
                            build_dir, '--target', 'install']
            if force_config is not None:
                install_args += ['--config', force_config]
            subprocess.check_call(install_args)
            dump_build_info(configuration, self,
                            build_dir, directories.install)
        copy_dependencies(source_dir, directories, self.copy)


def apply_patch_check_file(file):
    if not os.path.isfile(file):
        raise Exception(
            'Failed to apply patch on file {} because it does not exist'.format(file))


def apply_replace_patch(source_directory, patch):
    target_file = os.path.join(
        source_directory, required_argument('file', patch))
    apply_patch_check_file(target_file)
    string = required_argument('pattern', patch)
    replacement = required_argument('text', patch)
    patch_util.replace_in_file(target_file, string, replacement)


def apply_append_patch(source_directory, patch):
    target_file = os.path.join(
        source_directory, required_argument('file', patch))
    apply_patch_check_file(target_file)
    text = required_argument('text', patch)
    patch_util.append_text(target_file, text)


def apply_create_patch(source_directory, patch):
    target_file = os.path.join(
        source_directory, required_argument('file', patch))
    text = required_argument('text', patch)
    with open(target_file, 'w') as f:
        if isinstance(text, list):
            for line in text:
                f.write(line + '\n')
        else:
            f.write(text)


PATCH_COMMAND_MAPPINGS = {
    'replace': apply_replace_patch,
    'append': apply_append_patch,
    'create': apply_create_patch
}


def apply_patches(source_directory, patches_arr):
    logging.debug('Applying patches in %s', source_directory)
    for patch in patches_arr:
        patch_type = required_argument('type', patch)
        if patch_type in PATCH_COMMAND_MAPPINGS:
            PATCH_COMMAND_MAPPINGS[patch_type](source_directory, patch)
        else:
            raise Exception(f'Invalid patch type {patch_type}')


def copy_dependencies(source_directory, directories: Directories, copy_targets: list):
    for target_options in copy_targets:
        source_dir = source_directory
        keep_paths = optional_argument('keep-paths', target_options, True)
        rel_source_dir = optional_argument(
            'source-directory', target_options, None)
        if rel_source_dir is not None:
            source_dir = os.path.join(source_directory, rel_source_dir)
        expression = required_argument('sources', target_options)
        destination = required_argument('destination', target_options)
        search_pattern = source_dir + "/" + expression
        # Assume destination is directory
        for target_file_path in glob.iglob(search_pattern, recursive=True):

            if keep_paths:
                relpath = os.path.relpath(target_file_path, source_dir)
                dest_file_path = os.path.join(
                    directories.install,
                    destination,
                    relpath)
            else:
                dest_file_path = os.path.join(
                    directories.install,
                    destination,
                    os.path.basename(target_file_path))
            dest_directory = os.path.dirname(dest_file_path)
            os.makedirs(dest_directory, exist_ok=True)
            shutil.copy2(target_file_path, dest_file_path)


def override_source_directory(requirement: Requirement, source_directory: str):
    if requirement.cmake_directory is not None:
        return os.path.join(source_directory, requirement.cmake_directory)
    return source_directory


def download(requirement: Requirement, directories: Directories):
    logging.debug('Downloading requirement %s', requirement.url)
    # TODO - Detect if tar contains only one folder, or packs sources without it
    if requirement.url.endswith('.git'):
        source_dir = os.path.join(
            directories.source, name_version(requirement))
        git_clone(requirement.url, source_dir, requirement.version)
        return source_dir
    elif requirement.url.endswith('.tar.gz'):
        req_file = download_requirement(requirement, directories)
        return untargz(req_file, directories.source)[0]
    else:
        raise Exception(
            'Invalid url provided, must end with either .git, or .tar.gz')


def untargz(targzfile_path: str, output_directory: str):
    logging.debug('Extracting %s to %s', targzfile_path, output_directory)
    with tarfile.open(targzfile_path, 'r:gz') as tar:
        def is_within_directory(directory, target):
            
            abs_directory = os.path.abspath(directory)
            abs_target = os.path.abspath(target)
        
            prefix = os.path.commonprefix([abs_directory, abs_target])
            
            return prefix == abs_directory
        
        def safe_extract(tar, path=".", members=None, *, numeric_owner=False):
        
            for member in tar.getmembers():
                member_path = os.path.join(path, member.name)
                if not is_within_directory(path, member_path):
                    raise Exception("Attempted Path Traversal in Tar File")
        
            tar.extractall(path, members, numeric_owner=numeric_owner) 
            
        
        safe_extract(tar, path=output_directory)
        return [os.path.join(output_directory, x.name) for x in tar.getmembers()]


def download_requirement(requirement: Requirement, directories: Directories):
    name = requirement.get_hash().hexdigest()
    download_file_path = os.path.join(directories.cache, name)
    if not os.path.isfile(download_file_path):
        # TODO - When possible, this should print nice download status
        urllib.request.urlretrieve(
            requirement.url, filename=download_file_path)
    return download_file_path


def git_clone(url, target_directory, version):
    if not os.path.isdir(target_directory):
        subprocess.check_call([
            'git', 'clone', '--depth', '1', '--recursive', '--branch', version,  url, str(
                target_directory)
        ])


def dump_build_info(configuration: Configuration, requirement: Requirement, build_dir: str, install_directory_hash: str):
    from .json_encoder import JsonEncoder
    with open(os.path.join(build_dir, 'iwd-build-info.json'), 'w') as f:
        json.dump({
            'user-configuration': configuration,
            'requirement': requirement
        }, f, indent=4, cls=JsonEncoder)
