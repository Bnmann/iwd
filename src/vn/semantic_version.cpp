#include <vn/semantic_version.hpp>

#include <charconv>
#include <regex>
#include <sstream>
#include <vector>
#include <vn/string_conversions.hpp>
#include <vn/string_utils.hpp>

namespace vn {
namespace {
constexpr auto semver_rule =
  R"__regex__(^(0|[1-9]\d*)\.(0|[1-9]\d*)\.(0|[1-9]\d*)(?:-((?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*)(?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:\+([0-9a-zA-Z-]+(?:\.[0-9a-zA-Z-]+)*))?$)__regex__";
std::regex semver_regex(semver_rule);

std::regex digit_rule("[0-9]+");

int
numerical_compare_chunk(std::string_view a, std ::string_view b) noexcept
{
  return vn::try_convert<int>(a).value_or(0) -
    vn::try_convert<int>(b).value_or(0);
}

// -1 if a lower precedence 1 if higher

int
semver_compare_chunk(std::string_view a, std ::string_view b) noexcept
{
  const auto a_digits = vn::has_digits_only(a);
  const auto b_digits = vn::has_digits_only(b);

  if (a_digits && b_digits) {
    return numerical_compare_chunk(a, b);
  }

  if (a_digits && !b_digits) {
    return -1;
  }

  if (!a_digits && b_digits) {
    return 1;
  }

  return a.compare(b);
}

std::vector<std::string_view>
split_pr_semver_chunk(std::string_view text) noexcept
{
  return vn::split(text, ".");
}

int
semver_compare_pr(std::string_view a_pr, std::string_view b_pr) noexcept
{
  const auto a_chunks = split_pr_semver_chunk(a_pr);
  const auto b_chunks = split_pr_semver_chunk(b_pr);
  const auto min_chunk_count = std::min(a_chunks.size(), b_chunks.size());

  for (std::size_t index = 0u; index < min_chunk_count; ++index) {
    if (const auto v = semver_compare_chunk(a_chunks[index], b_chunks[index]);
        v != 0) {
      return v;
    }
  }
  return a_chunks.size() - b_chunks.size();
}

} // namespace

semantic_version::semantic_version(
  std::size_t major,
  std::size_t minor,
  std::size_t patch,
  const std::optional<std::string>& pre_release,
  const std::optional<std::string>& build_meta)
  : _major(major)
  , _minor(minor)
  , _patch(patch)
  , _pre_release_version(pre_release)
  , _build_metadata(build_meta)
{
}

std::size_t
semantic_version::major() const noexcept
{
  return _major;
}

std::size_t
semantic_version::minor() const noexcept
{
  return _minor;
}

std::size_t
semantic_version::patch() const noexcept
{
  return _patch;
}

int
semantic_version::compare(const semantic_version& other) const noexcept
{
  if (const auto diff = major() - other.major(); diff != 0) {
    return diff;
  }
  if (const auto diff = minor() - other.minor(); diff != 0) {
    return diff;
  }
  if (const auto diff = patch() - other.patch(); diff != 0) {
    return diff;
  }

  return pre_release()
    ? (other.pre_release()
         ? semver_compare_pr(*pre_release(), *other.pre_release())
         : -1)
    : other.pre_release() ? 1
                          : 0;
}

bool
semantic_version::operator==(const semantic_version& other) const noexcept
{
  return compare(other) == 0;
}

bool
semantic_version::operator!=(const semantic_version& other) const noexcept
{
  return compare(other) != 0;
}

bool
semantic_version::operator>(const semantic_version& other) const noexcept
{
  return compare(other) > 0;
}

bool
semantic_version::operator<(const semantic_version& other) const noexcept
{
  return compare(other) < 0;
}

bool
semantic_version::operator<=(const semantic_version& other) const noexcept
{
  return compare(other) <= 0;
}

bool
semantic_version::operator>=(const semantic_version& other) const noexcept
{
  return compare(other) <= 0;
}

std::string
semantic_version::str() const
{
  std::stringstream ss;
  ss << _major << "." << _minor << "." << _patch;
  if (_pre_release_version) {
    ss << "-" << *_pre_release_version;
  }
  if (_build_metadata) {
    ss << "+" << *_build_metadata;
  }
  return ss.str();
}

std::optional<semantic_version>
semantic_version::try_parse(std::string_view text)
{
  std::cmatch match;
  // TODO - Values outside unsigned long range are valid semver, but this should
  // throw here as unacceptable input for implementation
  if (std::regex_match(text.begin(), text.end(), match, semver_regex)) {
    const auto major = std::stoull(match[1].str());
    const auto minor = std::stoull(match[2].str());
    const auto patch = std::stoull(match[3].str());
    std::optional<std::string> pre_release = std::nullopt;
    std::optional<std::string> build_meta = std::nullopt;
    if (match[4].matched) {
      pre_release = match[4].str();
    }
    if (match[5].matched) {
      build_meta = match[5].str();
    }

    return semantic_version{ major, minor, patch, pre_release, build_meta };
  }
  return std::nullopt;
}

const std::optional<std::string>&
semantic_version::pre_release() const noexcept
{
  return _pre_release_version;
}

const std::optional<std::string>&
semantic_version::build_metadata() const noexcept
{
  return _build_metadata;
}

} // namespace vn
