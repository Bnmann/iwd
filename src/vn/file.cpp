#include <vn/file.hpp>

#include <fstream>
#include <streambuf>
#include <vn/error.hpp>

namespace vn {

std::string
read_whole_file(const std::filesystem::path& file_path)
{
  std::error_code ec;
  if (auto v = read_whole_file(file_path, ec); v) {
    return *v;
  }
  throw std::system_error(ec);
}

std::optional<std::string>
read_whole_file(
  const std::filesystem::path& file_path,
  std::error_code& ec) noexcept
{
  std::FILE* fp = std::fopen(file_path.c_str(), "rb");
  if (fp) {
    std::string contents;
    std::fseek(fp, 0, SEEK_END);
    contents.resize(std::ftell(fp));
    std::rewind(fp);
    std::fread(&contents[0], 1, contents.size(), fp);
    std::fclose(fp);
    return contents;
  }
  ec = vn::errno_as_error_code();
  return std::nullopt;
}

void
create_text_file(
  const std::filesystem::path& file_path,
  std::string_view contents)
{
  std::FILE* fp = std::fopen(file_path.c_str(), "w");
  if (fp) {
    const auto bytes_written =
      std::fwrite(contents.data(), sizeof(char), contents.size(), fp);
    std::fclose(fp);
    return;
  }
  throw std::system_error(vn::errno_as_error_code());
}

std::pair<std::optional<std::string>, std::error_code>
try_read_while_file(const std::filesystem::path& file_path) noexcept
{
  std::error_code ec{};
  return std::make_pair(read_whole_file(file_path, ec), ec);
}

} // namespace vn
