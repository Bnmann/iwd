#ifndef IWD__CMAKE_CONFIGURATION_HPP
#define IWD__CMAKE_CONFIGURATION_HPP

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace iwd {

using quicktype_config_t =
  std::shared_ptr<std::map<std::string, nlohmann::json>>;

struct cmake_configuration
{
  cmake_configuration(quicktype_config_t initial_data);
  cmake_configuration(std::map<std::string, std::string> initial_data);

  static cmake_configuration from_arguments(
    const std::vector<std::string>& arguments);

  cmake_configuration override_with(const cmake_configuration& parent) const;

  std::vector<std::string> as_cmake_args() const;

  void set_argument(const std::string& key, const std::string& value);
  std::optional<std::string> get_argument(const std::string& key) const;

private:
  std::optional<nlohmann::json> get_argument_obj(const std::string& key) const;
  quicktype_config_t::element_type configuration_copy() const;
  quicktype_config_t _configuration;
};

} // namespace iwd

#endif
