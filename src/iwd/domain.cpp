#include "iwd/git_program.hpp"
#include <iwd/domain.hpp>

namespace iwd {

domain::domain(
  const vn::directory& invocation_directory,
  const vn::directory& iwd_directory)
  : invocation_directory_(invocation_directory)
  , _directories(iwd_directory.path())
  , _cmake(cmake_program::create(find_executable_or_throw("cmake")))
  , _git(git_program::create(find_executable_or_throw("git")))
{}

const directories&
domain::dirs() const noexcept
{
  return _directories;
}

const vn::directory&
domain::invocation_directory() const noexcept
{
  return invocation_directory_;
}

const cmake_program&
domain::cmake() const noexcept
{
  return _cmake;
}

const git_program&
domain::git() const noexcept
{
  return _git;
}

} // namespace iwd
