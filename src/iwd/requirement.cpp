#include "iwd/requirement.hpp"
#include "nlohmann/json.hpp"

namespace iwd::json {

json_object::json_object(nlohmann::json j)
  : json_(std::move(j))
{}

bool
build_object::cmake_build() const
{
  return try_retrieve<bool>("cmake_build").value_or(true);
}

std::optional<std::string>
build_object::cmake_directory() const
{
  return try_retrieve<std::string>("cmake_directory");
}

bool
copy_object::keep_paths() const
{
  return try_retrieve<bool>("keep-paths").value_or(true);
}

std::optional<std::string>
copy_object::source_directory() const
{
  return try_retrieve<std::string>("source-directory");
}

std::string
copy_object::sources() const
{
  return retrieve<std::string>("sources");
}

std::string
copy_object::destination() const
{
  return retrieve<std::string>("destination");
}

patch_object::patch_type
patch_object::type() const
{
  static std::map<std::string, patch_type> kNameToEnumMapping = {
    { "replace", patch_type::replace },
    { "append", patch_type::append },
    { "create", patch_type::create },
    { "apply", patch_type::apply },
  };
  return kNameToEnumMapping.at(retrieve<std::string>("type"));
}
std::string
patch_object::file() const
{
  return retrieve<std::string>("file");
}
std::vector<std::string>
patch_object::text() const
{
  return retrieve<std::vector<std::string>>("text");
}
std::optional<std::string>
patch_object::pattern() const
{
  return try_retrieve<std::string>("pattern");
}

std::string
requirement::name() const
{
  return retrieve<std::string>("name");
}

std::string
requirement::version() const
{
  return retrieve<std::string>("version");
}

std::string
requirement::url() const
{
  return retrieve<std::string>("url");
}

build_object
requirement::build() const
{
  return build_object(try_retrieve<nlohmann::json::object_t>("build").value_or(
    nlohmann::json::object_t{}));
}

std::optional<std::vector<copy_object>>
requirement::copy() const
{
  return try_retrieve_array<copy_object>("copy");
}

std::optional<std::vector<patch_object>>
requirement::patch() const
{
  return try_retrieve_array<patch_object>("patch");
}

std::optional<std::map<std::string, std::string>>
requirement::configuration() const
{
  return try_retrieve<std::map<std::string, std::string>>("configuration");
}

std::string
specification::version()
{
  return retrieve<std::string>("version");
}

std::vector<requirement>
specification::requirements() const
{
  return retrieve_array<requirement>("requirements");
}

} // namespace iwd::json
