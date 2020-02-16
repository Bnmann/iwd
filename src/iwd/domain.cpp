#include <iwd/domain.hpp>

namespace iwd {

domain::domain(const vn::directory& iwd_directory)
  : _directories(iwd_directory.path())
  , _cmake(cmake_program::create(find_executable_or_throw("cmake")))
  , _git(find_executable_or_throw("git"))
{}

const directories&
domain::dirs() const noexcept
{
  return _directories;
}

const cmake_program&
domain::cmake() const noexcept
{
  return _cmake;
}

const executable&
domain::git() const noexcept
{
  return _git;
}

} // namespace iwd
