import hashlib
import re
from collections import UserDict

ARGUMENT_REGEX = re.compile(r'(-D)?(?P<key>\w+)=(?P<value>[\w/]+)')
EXPRESSION_RULE = re.compile(
    r'\$\((?P<key>\w+)\)')


def make_cmake_arg(key, value):
    return f'-D{key}={value}'


class Configuration(UserDict):
    def __init__(self, initial_data):
        super().__init__(initial_data)

    def as_cmake_args(self):
        return make_cmake_arguments(self)

    def resolve_variables(self, parent):
        return resolve_configuration_variables(self, parent)

    @staticmethod
    def from_arguments(arguments):
        result = Configuration(None)
        for argument in arguments:
            match = ARGUMENT_REGEX.match(argument)
            if match is None:
                raise Exception(
                    "Invalid cmake argument passed `" + argument + "`")
            else:
                groups = match.groupdict()
                result[groups['key']] = groups['value']
        return result


def make_cmake_arguments(configuration):
    return [make_cmake_arg(key, value) for key, value in configuration.items()]


def resolve_configuration_variables(configuration, parent):
    for k, v in configuration.items():
        def subimpl(matchobj):
            value = matchobj.groupdict()['key']
            return parent[value]
        configuration[k] = EXPRESSION_RULE.sub(subimpl, v)
