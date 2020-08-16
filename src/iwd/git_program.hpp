#ifndef IWD__GIT_PROGRAM_HPP_
#define IWD__GIT_PROGRAM_HPP_

#include "iwd/executable.hpp"
#include "vn/semantic_version.hpp"
#include <filesystem>
#include <optional>

namespace iwd {

struct git_program
{
  static git_program create(executable);

  const vn::semantic_version& version() const noexcept;

  struct clone_parameters
  {
    std::string url;
    std::filesystem::path target_directory;
    std::optional<std::string> branch;
    std::optional<std::size_t> depth;
    bool recursive;
  };

  void clone(const clone_parameters& param) const;

  struct apply_parameters
  {
    std::filesystem::path patch;
    std::filesystem::path directory;
  };

  void apply(const apply_parameters& param) const;

private:
  git_program(executable exec, vn::semantic_version version);

  executable _executable;
  vn::semantic_version _version;
};

} // namespace iwd

#endif
