#include "iwd/cmake_configuration.hpp"
#include "iwd/domain.hpp"
#include "iwd/requirement.hpp"
#include <filesystem>
#include <vector>

namespace iwd {

std::string
name_version(const iwd::json::requirement& requirement);

std::vector<iwd::json::requirement>
parse_requirements(const std::filesystem::path& iwd_config_path);

struct requirement_handler
{
  requirement_handler(
    const iwd::domain& domain,
    const iwd::json::requirement& req);

  void source();
  void configure(const iwd::cmake_configuration& configuration);
  void install();

private:
  void patch();
  bool is_cmake_build() const noexcept;
  iwd::domain _domain;
  std::unique_ptr<vn::directory> _source_directory;
  std::unique_ptr<vn::directory> _build_directory;
  iwd::json::requirement _requirement;
};

} // namespace iwd
