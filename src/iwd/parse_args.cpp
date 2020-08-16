#include "iwd/parse_args.hpp"

#include <argparse.hpp>
#include <filesystem>
#include <iostream>

namespace iwd {

iwd::arguments
parse_args(int argc, const char** argv)
{
  iwd::arguments args;
  auto parser = argparse::ArgumentParser();

  parser.add_argument("--clean-first")
    .help("Remove all of iwd files before running")
    .default_value(false)
    .implicit_value(true);

  parser.add_argument("-B", "--build-dir")
    .action(
      [&args](const std::string& value) { args.build_directory = value; });

  parser.add_argument("--config")
    .help("When using multi config cmake generator, force specific "
          "configuration to build")
    .action([&args](const std::string& value) { args.config = value; });

  parser.add_argument("-G", "--generator")
    .help("Force specific cmake generator to be used")
    .action([&args](const std::string& value) { args.generator = value; });

  parser.add_argument("cmake_args")
    .action([&args](const std::string& value) {
      args.cmake_arguments.push_back(value);
    })
    .remaining();

  try {
    parser.parse_args(argc, argv);
  } catch (const std::runtime_error& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << parser;
    exit(0);
  }
  args.clean_first = parser.get<bool>("--clean-first");

  return args;
}
} // namespace iwd
