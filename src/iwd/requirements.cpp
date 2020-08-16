#include "iwd/requirements.hpp"

#include "iwd/download_file.hpp"
#include "iwd/extract_tarfile.hpp"
#include "iwd/git_clone.hpp"
#include "iwd/git_program.hpp"
#include "iwd/logging.hpp"
#include "iwd/patch.hpp"
#include "iwd/requirement.hpp"
#include "iwd/schema.hpp"
#include "iwd/subprocess.hpp"
#include "nlohmann/json-schema.hpp"
#include "nlohmann/json.hpp"
#include <exception>
#include <fstream>
#include <stdexcept>
#include <system_error>
#include <vn/file.hpp>
#include <vn/string_utils.hpp>

namespace iwd {

namespace {

bool
download_and_extract(
  const std::string& source,
  const std::filesystem::path& download_destination,
  const std::filesystem::path& extract_destination)
{
  if (!std::filesystem::exists(download_destination)) {
    info("Downloading {}", source);
    iwd::download_file(source, download_destination);
  }
  if (!std::filesystem::exists(extract_destination)) {
    info("Extracting {}", download_destination.string());
    iwd::extract_tarfile(
      download_destination, vn::directory::create(extract_destination));
    return true;
  }
  return false;
}

std::string cmake_program_name = "cmake";

template<typename... Args>
std::vector<std::string>
make_args(Args&&... args)
{
  return std::vector<std::string>{ std::forward<Args>(args)... };
}

} // namespace

std::string
name_version(const iwd::json::requirement& req)
{
  return vn::join("", req.name(), "-", req.version());
}

std::vector<json::requirement>
parse_requirements(const std::filesystem::path& iwd_config_path)
{
  auto object = nlohmann::json::parse(vn::read_whole_file(iwd_config_path));

  nlohmann::json_schema::json_validator validator;
  try {
    validator.set_root_schema(nlohmann::json::parse(kSchema));
  } catch (const std::exception& e) {
    throw std::runtime_error(
      "Failed to set root schema :" + std::string(e.what()));
  }
  try {
    validator.validate(object);
  } catch (const std::exception& e) {
    throw std::runtime_error(
      "Failed to validate object :" + std::string(e.what()));
  }
  // TODO - There is potentially huge copy of the resources here
  return iwd::json::specification(object).requirements();
}

requirement_handler::requirement_handler(
  const iwd::domain& domain,
  const iwd::json::requirement& req)
  : _domain(domain)
  , _source_directory(nullptr)
  , _requirement(req)
{}

void
requirement_handler::source()
{
  const auto namever = name_version(_requirement);
  const auto source_url = _requirement.url();
  const auto source_path = _domain.dirs().source_directory().path() / namever;
  const auto download_path = _domain.dirs().cache_directory().path() / namever;
  bool patch_required = false;

  if (vn::ends_with(source_url, ".git")) {
    if (!std::filesystem::exists(source_path)) {
      info("Cloning {}", source_url);
      iwd::git_program::clone_parameters param = {};
      param.url = source_url;
      param.target_directory = source_path;
      param.branch = _requirement.version();
      param.depth = 1;
      param.recursive = true;
      _domain.git().clone(param);
      patch_required = true;
    }
  } else if (vn::ends_with(source_url, "tar.gz")) {
    patch_required =
      download_and_extract(source_url, download_path, source_path);

    // Most of the archives contain single root directory that contains the
    // actual content. If that is the case, make the _source_directory variable
    // point to this single directory instead of the calculated source location.
    // This makes setup way easier for the user.
    // TODO - Make it so, that if that is the case, the intermediate directory
    // is moved as a source
    auto source_directory = vn::directory(source_path);
    const auto file_count = std::distance(
      source_directory.list().begin(), source_directory.list().end());
    if (file_count == 1) {
      const auto first_item = source_directory.list().begin()->path();
      if (std::filesystem::is_directory(first_item)) {
        _source_directory = std::make_unique<vn::directory>(first_item);
      }
    }

  } else {
    throw std::runtime_error(fmt::format(
      "Unsupported url \"{}\". Url must point to either tar.gz.or git "
      "repository",
      source_url));
  }
  if (!_source_directory) {
    _source_directory = std::make_unique<vn::directory>(source_path);
  }

  if (patch_required) {
    patch();
  }
}

void
requirement_handler::configure(const iwd::cmake_configuration& root)
{
  const auto config =
    cmake_configuration(_requirement.configuration().value_or(
                          std::map<std::string, std::string>{}))
      .override_with(root);

  const auto build_path =
    _domain.dirs().build_directory().path() / name_version(_requirement);

  if (!_build_directory) {
    _build_directory =
      std::make_unique<vn::directory>(vn::directory::create(build_path));
  }

  // Return early if this is not a cmake build
  if (!is_cmake_build()) {
    return;
  }

  // Find the path to CMakeLists directory
  const auto cmake_source_directory = _requirement.build().cmake_directory()
    ? _source_directory->path() / *_requirement.build().cmake_directory()
    : _source_directory->path();

  _domain.cmake().configure(
    vn::directory(cmake_source_directory), *_build_directory, config);
}

void
requirement_handler::patch()
{
  if (auto patches = _requirement.patch(); patches) {
    for (auto& patch : *patches) {
      switch (patch.type()) {
        case iwd::json::patch_object::patch_type::append:
          patch_append_file(*_source_directory, patch);
          break;
        case iwd::json::patch_object::patch_type::replace:
          patch_replace_file(*_source_directory, patch);
          break;
        case iwd::json::patch_object::patch_type::apply:
          patch_apply(
            _domain.git(),
            _domain.invocation_directory(),
            *_source_directory,
            patch);
          break;
        case iwd::json::patch_object::patch_type::create:
          patch_create_file(*_source_directory, patch);
          break;
      }
    }
  }
}

void
requirement_handler::install()
{
  if (!is_cmake_build()) {
    return;
  }
  _domain.cmake().install(*_build_directory);
}

bool
requirement_handler::is_cmake_build() const noexcept
{
  return _requirement.build().cmake_build();
}

} // namespace iwd
