#include <iwd/cmake_program.hpp>
#include <iwd/logging.hpp>
#include <regex>
#include <string>
#include <vector>
#include <vn/iterator_range.hpp>
#include <vn/semantic_version.hpp>
#include <vn/string_utils.hpp>

namespace iwd {

namespace {
std::regex cmake_version_regex(
  R"__regex__(cmake version (\d+\.\d+\.\d+))__regex__");
}

cmake_program
cmake_program::create(executable exe)
{
  const auto version_output = exe.check_output({ "--version" });
  std::smatch match;
  if (!std::regex_search(version_output, match, cmake_version_regex)) {
    throw std::runtime_error(
      "Could not parse cmake version. Is the cmake executable correct ?");
  }
  const auto version = vn::semantic_version::try_parse(match[1].str());
  if (!version.has_value()) {
    throw std::runtime_error("Could not parse cmake version as semver. Is the "
                             "cmake version correct ?");
  }

  return cmake_program(exe, version.value());
}

const vn::semantic_version&
cmake_program::version() const noexcept
{
  return _version;
}

cmake_program::cmake_program(executable exe, vn::semantic_version version)
  : _cmake_executable(exe)
  , _version(version)
{
}

void
cmake_program::configure(
  const vn::directory& source_directory,
  const vn::directory& build_directory,
  const cmake_configuration& config) const
{
  const auto configuration_args = config.as_cmake_args();
  auto args = std::vector<std::string>{
    "-S",
    source_directory.path().generic_string(),
    "-B",
    build_directory.path().generic_string(),
  };
  args.insert(args.end(), configuration_args.begin(), configuration_args.end());
  info(
    "cmake {}",
    vn::join_range(" ", vn::make_iterator_range(args.begin(), args.end())));
  _cmake_executable.check_call(args);
}

void
cmake_program::install(const vn::directory& build_directory) const
{
  const auto args = std::vector<std::string>{
    "--build", build_directory.path().generic_string(), "--target", "install"
  };
  info(
    "cmake {}",
    vn::join_range(" ", vn::make_iterator_range(args.begin(), args.end())));
  _cmake_executable.check_call(args);
}

} // namespace iwd
