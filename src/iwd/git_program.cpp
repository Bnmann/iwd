#include "iwd/git_program.hpp"
#include "iwd/executable.hpp"
#include "iwd/logging.hpp"
#include "iwd/subprocess.hpp"
#include "vn/semantic_version.hpp"
#include "vn/string_utils.hpp"
#include <regex>

namespace iwd {

namespace {
std::regex git_version_regex(
  R"__regex__(git version (\d+\.\d+\.\d+))__regex__");
}

git_program
git_program::create(executable exe)
{
  const auto version_output = exe.check_output({ "--version" });
  std::smatch match;
  if (!std::regex_search(version_output, match, git_version_regex)) {
    throw std::runtime_error(
      "Could not parse git version. Is the git executable correct ?");
  }
  const auto version = vn::semantic_version::try_parse(match[1].str());
  if (!version.has_value()) {
    throw std::runtime_error("Could not parse git version as semver. Is the "
                             "git version correct ?");
  }

  return git_program(exe, version.value());
}

const vn::semantic_version&
git_program::version() const noexcept
{
  return _version;
}

void
git_program::clone(const clone_parameters& param) const
{
  std::vector<std::string> args = { "clone" };
  if (param.depth) {
    args.push_back("--depth");
    args.push_back(std::to_string(*param.depth));
  }
  if (param.recursive) {
    args.push_back("--recursive");
  }
  if (param.branch) {
    args.push_back("--branch");
    args.push_back(*param.branch);
  }
  args.push_back(param.url);
  args.push_back(param.target_directory.generic_string());
  info(
    "git {}",
    vn::join_range(" ", vn::make_iterator_range(args.begin(), args.end())));
  _executable.check_call(args);
}

void
git_program::apply(const apply_parameters& param) const
{
  std::vector<std::string> args = {
    "apply",
    "--directory",
    param.directory.generic_string(),
    "--unsafe-paths",
    param.patch.generic_string(),
  };
  info(
    "git {}",
    vn::join_range(" ", vn::make_iterator_range(args.begin(), args.end())));
  _executable.check_call(args);
}

git_program::git_program(executable exec, vn::semantic_version version)
  : _executable(exec)
  , _version(version)
{}

} // namespace iwd
