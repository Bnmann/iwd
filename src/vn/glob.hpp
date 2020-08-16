#ifndef VN__GLOB_HPP
#define VN__GLOB_HPP

#include <filesystem>
#include <optional>
#include <ranges>
#include <string_view>
#include <vector>
#include <vn/directory.hpp>
#include <vn/fnmatch.hpp>
#include <vn/iterator_range.hpp>

namespace vn {

struct recursive_glob_iterator
{
  using value_type = std::filesystem::recursive_directory_iterator::value_type;
  using reference = std::filesystem::recursive_directory_iterator::reference;
  using pointer = std::filesystem::recursive_directory_iterator::pointer;
  using iterator_category =
    std::filesystem::recursive_directory_iterator::iterator_category;
  using difference_type =
    std::filesystem::recursive_directory_iterator::difference_type;

  recursive_glob_iterator(
    const vn::directory& directory,
    const vn::fnmatch& pattern);

  recursive_glob_iterator();

  reference operator*();
  pointer operator->();
  recursive_glob_iterator& operator++();
  bool operator==(const recursive_glob_iterator&) noexcept;
  bool operator!=(const recursive_glob_iterator&) noexcept;

private:
  void increment();
  reference ref();
  bool is_end() const;
  void increment_impl();
  bool skip_invalid_items();

  std::filesystem::recursive_directory_iterator _iterator;
  std::optional<vn::fnmatch> _pattern;
};

vn::iterator_range<recursive_glob_iterator>
recursive_glob(
  const vn::directory& directory,
  const vn::fnmatch& files_matching_pattern);

} // namespace vn

#endif
