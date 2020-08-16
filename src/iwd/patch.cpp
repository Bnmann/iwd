#include "iwd/git_program.hpp"
#include "iwd/logging.hpp"
#include "vn/directory.hpp"
#include "vn/string_utils.hpp"
#include <iwd/patch.hpp>
#include <regex>

#include <fstream>
#include <vn/file.hpp>

namespace iwd {

void
patch_create_file(
  const vn::directory& source_directory,
  const iwd::json::patch_object& patch)
{
  std::ofstream ofs;
  const auto fpath = source_directory.path() / patch.file();
  info("Attempting to create {} file", fpath.generic_string());
  ofs.open(fpath);
  if (!ofs.is_open()) {
    throw std::runtime_error(
      "Patch operation failed: could not open file:" + fpath.generic_string());
  }
  for (auto line : patch.text()) {
    ofs << line;
  }
}

void
patch_append_file(
  const vn::directory& source_directory,
  const iwd::json::patch_object& patch)
{
  std::ofstream ofs;
  const auto fpath = source_directory.path() / patch.file();
  info("Attempting to append text to {} ", fpath.generic_string());
  ofs.open(fpath, std::ios::app);
  if (!ofs.is_open()) {
    throw std::runtime_error(
      "Patch operation failed: could not open file:" + fpath.generic_string());
  }
  for (auto line : patch.text()) {
    ofs << line << "\n";
  }
}

void
patch_replace_file(
  const vn::directory& source_directory,
  const iwd::json::patch_object& patch)
{
  const auto fpath = source_directory.path() / patch.file();
  const auto text = vn::read_whole_file(fpath);

  std::regex pattern(patch.pattern().value());

  info("Replacing text in {}", fpath.generic_string());

  std::ofstream ofs;
  if (ofs.open(fpath); !ofs.is_open()) {
    throw std::runtime_error(
      "Patch operation failed: could not open file:" + fpath.generic_string());
  }
  const auto replacement_lines = patch.text();
  const std::string replacement = vn::join_range(
    "\n",
    vn::make_iterator_range(
      replacement_lines.begin(), replacement_lines.end()));

  ofs << std::regex_replace(text, pattern, replacement);
}

void
patch_apply(
  const git_program& git,
  const vn::directory& working_directory,
  const vn::directory& source_directory,
  const iwd::json::patch_object& patch)
{
  const auto patch_file = working_directory.path() / patch.file();
  git_program::apply_parameters params;
  params.directory = source_directory.path();
  params.patch = patch_file;
  git.apply(params);
}

} // namespace iwd
