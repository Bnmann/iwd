#ifndef IWD__DOMAIN_HPP
#define IWD__DOMAIN_HPP

#include <iwd/cmake.hpp>
#include <iwd/directories.hpp>
#include <iwd/executable.hpp>
#include <memory>
#include <vn/directory.hpp>

namespace iwd {

struct domain
{
  explicit domain(const vn::directory& iwd_directory);

  const directories& dirs() const noexcept;
  const cmake& cmake() const noexcept;
  const executable& git() const noexcept;

private:
  directories _directories;
  iwd::cmake _cmake;
  executable _git;
};

} // namespace iwd

#endif
