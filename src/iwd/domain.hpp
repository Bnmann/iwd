#ifndef IWD__DOMAIN_HPP
#define IWD__DOMAIN_HPP

#include "iwd/git_program.hpp"
#include <iwd/cmake_program.hpp>
#include <iwd/directories.hpp>
#include <iwd/executable.hpp>
#include <memory>
#include <vn/directory.hpp>

namespace iwd {

struct domain
{
  explicit domain(
    const vn::directory& invocation_directory,
    const vn::directory& iwd_directory);

  const directories& dirs() const noexcept;
  const vn::directory& invocation_directory() const noexcept;
  const cmake_program& cmake() const noexcept;
  const git_program& git() const noexcept;

private:
  vn::directory invocation_directory_;
  directories _directories;
  cmake_program _cmake;
  git_program _git;
};

} // namespace iwd

#endif
