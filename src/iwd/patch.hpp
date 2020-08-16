#ifndef IWD__PATCH_HPP
#define IWD__PATCH_HPP

#include "iwd/git_program.hpp"
#include "iwd/requirement.hpp"
#include <filesystem>
#include <string>
#include <string_view>
#include <vn/directory.hpp>

namespace iwd {

void
patch_create_file(
  const vn::directory& source_directory,
  const iwd::json::patch_object& patch);

void
patch_append_file(
  const vn::directory& source_directory,
  const iwd::json::patch_object& patch);

void
patch_replace_file(
  const vn::directory& source_directory,
  const iwd::json::patch_object& patch);

void
patch_apply(
  const git_program& git,
  const vn::directory& working_directory,
  const vn::directory& source_directory,
  const iwd::json::patch_object& patch);

} // namespace iwd

#endif
