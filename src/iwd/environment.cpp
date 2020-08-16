
#include "iwd/environment.hpp"
#include "vn/path.hpp"
#include "vn/string_utils.hpp"
#include <cstdlib>
#include <optional>

#if defined(IWD_BUILD_LINUX)
constexpr auto kPathSeparator = ":";
#elif defined(IWD_BUILD_WINDOWS)
constexpr auto kPathSeparator = ";";
#endif

namespace iwd {

std::optional<std::string>
get_environment_variable(const std::string& name)
{
  if (char* var = getenv(name.c_str()); var) {
    return std::string(var);
  }
  return std::nullopt;
}

std::vector<std::filesystem::path>
get_environment_path()
{
  const auto path = get_environment_variable("PATH").value_or("");
  const auto result = vn::split(path, ":");
  return std::vector<std::filesystem::path>{ result.begin(), result.end() };
}

} // namespace iwd
