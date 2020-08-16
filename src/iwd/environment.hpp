#ifndef IWD__ENVIRONMENT_HPP_
#define IWD__ENVIRONMENT_HPP_

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace iwd {

[[nodiscard]] std::optional<std::string>
get_environment_variable(const std::string& name);

[[nodiscard]] std::vector<std::filesystem::path>
get_environment_path();

} // namespace iwd

#endif
