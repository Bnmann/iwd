#include <filesystem>
#include <nlohmann/json.hpp>

#include "iwd/cmake_configuration.hpp"
#include "iwd/directories.hpp"
#include "iwd/domain.hpp"
#include "iwd/download_file.hpp"
#include "iwd/git_clone.hpp"
#include "iwd/logging.hpp"
#include "iwd/parse_args.hpp"
#include "iwd/requirements.hpp"
#include "vn/directory.hpp"
#include <vn/file.hpp>
#include <vn/string_template.hpp>
#include <vn/string_utils.hpp>

#include <iostream>

constexpr auto iwd_cmake_file_template =
  "set(IWD_INSTALL_PREFIX $(INSTALL_PREFIX))\n"
  "set(CMAKE_PREFIX_PATH $(INSTALL_PREFIX))";

void
check_cmake_version(const iwd::cmake_program& cmake)
{
  if (cmake.version() < vn::semantic_version(3, 13, 0)) {
    throw std::runtime_error("IWD requires cmake version > 3.13");
  }
}

int
main(int argc, const char** argv)
{
  const auto args = iwd::parse_args(argc, argv);

  if (args.clean_first) {
    auto working_directory =
      iwd::directories(args.build_directory).working_directory().path();
    iwd::info("Cleaning iwd directory {}", working_directory.generic_string());
    std::filesystem::remove_all(working_directory);
  }

  const auto directories = iwd::directories(args.build_directory);
  const auto domain =
    iwd::domain(vn::directory::current(), vn::directory(args.build_directory));

  check_cmake_version(domain.cmake());

  const auto requirements = iwd::parse_requirements("iwd.json");
  auto configuration =
    iwd::cmake_configuration::from_arguments(args.cmake_arguments);
  configuration.set_argument(
    "CMAKE_INSTALL_PREFIX", directories.install_directory().path().string());

  for (const auto& req : requirements) {
    iwd::requirement_handler handler(domain, req);
    handler.source();
    handler.configure(configuration);
    handler.install();
  }

  const auto iwd_cmake_contents = vn::render_template(
    iwd_cmake_file_template,
    [&directories](const std::string& key) -> std::optional<std::string> {
      if (key == "INSTALL_PREFIX") {
        return directories.install_directory().path().generic_string();
      }
      return std::nullopt;
    });

  vn::create_text_file(args.build_directory / "iwd.cmake", iwd_cmake_contents);

  return 0;
}
