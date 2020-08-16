#ifndef IWD__REQUIREMENT_HPP_
#define IWD__REQUIREMENT_HPP_

#include "nlohmann/json.hpp"
#include "vn/semantic_version.hpp"
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace iwd::json {

struct json_object
{
  explicit json_object(nlohmann::json j);

  template<typename T>
  std::optional<T> try_retrieve(const std::string& name) const
  {
    if (json_.contains(name)) {
      return json_[name];
    }
    return std::nullopt;
  }

  template<typename T>
  T retrieve(const std::string& name) const
  {
    if (auto v = try_retrieve<T>(name); v) {
      return *v;
    }
    throw std::runtime_error("Failed to find property: " + name);
    return {};
  }

  template<typename T>
  std::vector<T> to_vector(const nlohmann::json::array_t& arr) const
  {
    std::vector<T> result;
    result.reserve(arr.size());
    std::transform(
      arr.begin(), arr.end(), std::back_inserter(result), [](auto v) {
        return T(v);
      });
    return result;
  }

  template<typename T>
  std::vector<T> retrieve_array(const std::string& name) const
  {
    return to_vector<T>(retrieve<nlohmann::json::array_t>(name));
  }

  template<typename T>
  std::optional<std::vector<T>> try_retrieve_array(
    const std::string& name) const
  {
    if (auto arr = try_retrieve<nlohmann::json::array_t>(name); arr) {
      return to_vector<T>(*arr);
    }
    return std::nullopt;
  }

private:
  nlohmann::json json_;
};

struct build_object : json_object
{
  using json_object::json_object;
  [[nodiscard]] bool cmake_build() const;
  [[nodiscard]] std::optional<std::string> cmake_directory() const;
};

struct copy_object : json_object
{
  using json_object::json_object;
  [[nodiscard]] bool keep_paths() const;
  [[nodiscard]] std::optional<std::string> source_directory() const;
  [[nodiscard]] std::string sources() const;
  [[nodiscard]] std::string destination() const;
};

struct patch_object : json_object
{
  using json_object::json_object;
  enum class patch_type
  {
    replace,
    append,
    create,
    apply
  };
  [[nodiscard]] patch_type type() const;
  [[nodiscard]] std::string file() const;
  [[nodiscard]] std::vector<std::string> text() const;
  [[nodiscard]] std::optional<std::string> pattern() const;
};

struct requirement : json_object
{
  using json_object::json_object;
  [[nodiscard]] std::string name() const;
  [[nodiscard]] std::string version() const;
  [[nodiscard]] std::string url() const;
  [[nodiscard]] build_object build() const;
  [[nodiscard]] std::optional<std::vector<copy_object>> copy() const;
  [[nodiscard]] std::optional<std::vector<patch_object>> patch() const;
  [[nodiscard]] std::optional<std::map<std::string, std::string>>
  configuration() const;
};

struct specification : json_object
{
  using json_object::json_object;
  [[nodiscard]] std::string version();
  [[nodiscard]] std::vector<requirement> requirements() const;
};

} // namespace iwd::json

#endif
