#ifndef IWD__ARGUMENTS_HPP
#define IWD__ARGUMENTS_HPP

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace iwd {

constexpr auto default_build_dir_name = "build";

struct arguments
{
  std::filesystem::path build_directory =
    std::filesystem::current_path() / default_build_dir_name;
  std::optional<std::string> config;
  std::optional<std::string> generator;
  std::vector<std::string> cmake_arguments;
  bool clean_first;
};

} // namespace iwd

#endif
