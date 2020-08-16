#ifndef VN__VERSION_HPP
#define VN__VERSION_HPP

#include <cstdio>
#include <optional>
#include <string>
#include <string_view>

namespace vn {

struct semantic_version
{
  semantic_version(
    std::size_t major,
    std::size_t minor,
    std::size_t patch,
    const std::optional<std::string>& pre_release = std::nullopt,
    const std::optional<std::string>& build_meta = std::nullopt);

  std::size_t major() const noexcept;
  std::size_t minor() const noexcept;
  std::size_t patch() const noexcept;

  bool operator==(const semantic_version& other) const noexcept;
  bool operator!=(const semantic_version& other) const noexcept;
  bool operator>(const semantic_version& other) const noexcept;
  bool operator<(const semantic_version& other) const noexcept;
  bool operator<=(const semantic_version& other) const noexcept;
  bool operator>=(const semantic_version& other) const noexcept;

  int compare(const semantic_version& other) const noexcept;

  std::string str() const;
  static std::optional<semantic_version> try_parse(std::string_view);
  const std::optional<std::string>& pre_release() const noexcept;
  const std::optional<std::string>& build_metadata() const noexcept;

private:
  std::size_t _major;
  std::size_t _minor;
  std::size_t _patch;
  std::optional<std::string> _pre_release_version;
  std::optional<std::string> _build_metadata;
};
} // namespace vn

#endif
