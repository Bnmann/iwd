#include <vn/glob.hpp>

#include <functional>
#include <regex>
#include <vn/directory.hpp>
#include <vn/fnmatch.hpp>
#include <vn/path.hpp>

namespace vn {

namespace fs = std::filesystem;

namespace {
std::regex magic_check = std::regex(R"__regex__(([*?[]]))__regex__");

bool
has_magic(const std::string& pathname)
{
  std::smatch match;
  return std::regex_search(pathname, match, magic_check);
}

bool
is_hidden(const fs::path& fpath)
{
  return fpath.c_str()[0] == '.';
}

} // namespace

recursive_glob_iterator::recursive_glob_iterator(
  const vn::directory& directory,
  const vn::fnmatch& pattern)
  : _iterator(directory.path())
  , _pattern(pattern)
{
}

recursive_glob_iterator::recursive_glob_iterator()
  : _iterator()
  , _pattern(std::nullopt)
{
}

recursive_glob_iterator::reference
recursive_glob_iterator::operator*()
{
  return ref();
}

recursive_glob_iterator::pointer
recursive_glob_iterator::operator->()
{
  return _iterator.operator->();
}

bool
recursive_glob_iterator::operator==(
  const recursive_glob_iterator& other) noexcept
{
  return _iterator == other._iterator;
}

bool
recursive_glob_iterator::operator!=(
  const recursive_glob_iterator& other) noexcept
{
  return _iterator != other._iterator;
}

recursive_glob_iterator&
recursive_glob_iterator::operator++()
{
  increment();
  return *this;
}

void
recursive_glob_iterator::increment()
{
  increment_impl();
  while (!is_end() && !skip_invalid_items()) {
  }
}

void
recursive_glob_iterator::increment_impl()
{
  ++_iterator;
}

recursive_glob_iterator::reference
recursive_glob_iterator::ref()
{
  return *_iterator;
}

bool
recursive_glob_iterator::is_end() const
{
  return _iterator == std::filesystem::recursive_directory_iterator();
}

bool
recursive_glob_iterator::skip_invalid_items()
{
  if (is_hidden(ref().path())) {
    increment_impl();
    return false;
  }

  if (
    ref().path().has_parent_path() &&
    is_hidden(ref().path().parent_path().filename())) {
    _iterator.pop();
    return false;
  }

  if (
    ref().is_regular_file() && _pattern &&
    !std::invoke(*_pattern, ref().path())) {
    increment_impl();
    return false;
  }

  if (_pattern && !ref().is_regular_file()) {
    increment_impl();
    return false;
  }

  return true;
}

vn::iterator_range<recursive_glob_iterator>
recursive_glob(
  const vn::directory& directory,
  const vn::fnmatch& files_matching_pattern)
{
  return vn::make_iterator_range(
    recursive_glob_iterator(directory, files_matching_pattern),
    recursive_glob_iterator());
}

} // namespace vn
