//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     Iwd data = nlohmann::json::parse(jsonString);

#pragma once

#include <nlohmann/json.hpp>

#include <optional>
#include <regex>
#include <stdexcept>

#ifndef NLOHMANN_OPT_HELPER
#define NLOHMANN_OPT_HELPER
namespace nlohmann {
template<typename T>
struct adl_serializer<std::shared_ptr<T>>
{
  static void to_json(json& j, const std::shared_ptr<T>& opt)
  {
    if (!opt)
      j = nullptr;
    else
      j = *opt;
  }

  static std::shared_ptr<T> from_json(const json& j)
  {
    if (j.is_null())
      return std::unique_ptr<T>();
    else
      return std::unique_ptr<T>(new T(j.get<T>()));
  }
};
} // namespace nlohmann
#endif

namespace iwd {
namespace quicktype {
using nlohmann::json;

inline json
get_untyped(const json& j, const char* property)
{
  if (j.find(property) != j.end()) {
    return j.at(property).get<json>();
  }
  return json();
}

inline json
get_untyped(const json& j, std::string property)
{
  return get_untyped(j, property.data());
}

template<typename T>
inline std::shared_ptr<T>
get_optional(const json& j, const char* property)
{
  if (j.find(property) != j.end()) {
    return j.at(property).get<std::shared_ptr<T>>();
  }
  return std::shared_ptr<T>();
}

template<typename T>
inline std::shared_ptr<T>
get_optional(const json& j, std::string property)
{
  return get_optional<T>(j, property.data());
}

class Copy
{
public:
  Copy() = default;
  virtual ~Copy() = default;

private:
  std::string destination;
  std::shared_ptr<bool> keep_paths;
  std::shared_ptr<std::string> source_directory;
  std::string sources;

public:
  /**
   * Relative path to the iwd install directory, defines where are the found
   * files copied
   */
  const std::string& get_destination() const { return destination; }
  std::string& get_mutable_destination() { return destination; }
  void set_destination(const std::string& value) { this->destination = value; }

  /**
   * Controls whether to keep paths when copying found files
   */
  std::shared_ptr<bool> get_keep_paths() const { return keep_paths; }
  void set_keep_paths(std::shared_ptr<bool> value) { this->keep_paths = value; }

  /**
   * Provide a path to the directory, which will be treated as a root directory
   * for any expression and path resolving. If the value is not provided, if
   * defaults to the dependency source directory.
   */
  std::shared_ptr<std::string> get_source_directory() const
  {
    return source_directory;
  }
  void set_source_directory(std::shared_ptr<std::string> value)
  {
    this->source_directory = value;
  }

  /**
   * python glob expression , controls what kind of files should be copied
   */
  const std::string& get_sources() const { return sources; }
  std::string& get_mutable_sources() { return sources; }
  void set_sources(const std::string& value) { this->sources = value; }
};

/**
 * Describes what kind of patch command is this
 */
enum class Type : int
{
  append,
  create,
  replace
};

class Patch
{
public:
  Patch() = default;
  virtual ~Patch() = default;

private:
  std::string file;
  std::shared_ptr<std::string> pattern;
  std::vector<std::string> text;
  std::shared_ptr<Type> type;

public:
  /**
   * Describes the relative path from source directory to the file on which the
   * operation should be executed.
   */
  const std::string& get_file() const { return file; }
  std::string& get_mutable_file() { return file; }
  void set_file(const std::string& value) { this->file = value; }

  /**
   * Required only for replace command. Regex pattern that will serve as a
   * target for replacement.
   */
  std::shared_ptr<std::string> get_pattern() const { return pattern; }
  void set_pattern(std::shared_ptr<std::string> value)
  {
    this->pattern = value;
  }

  /**
   * Lines, which will be added to the file
   */
  const std::vector<std::string>& get_text() const { return text; }
  std::vector<std::string>& get_mutable_text() { return text; }
  void set_text(const std::vector<std::string>& value) { this->text = value; }

  /**
   * Describes what kind of patch command is this
   */
  std::shared_ptr<Type> get_type() const { return type; }
  void set_type(std::shared_ptr<Type> value) { this->type = value; }
};

class Requirement
{
public:
  Requirement() = default;
  virtual ~Requirement() = default;

private:
  std::shared_ptr<bool> cmake_build;
  std::shared_ptr<std::string> cmake_directory;
  std::shared_ptr<std::map<std::string, nlohmann::json>> configuration;
  std::shared_ptr<std::vector<Copy>> copy;
  std::string name;
  std::shared_ptr<std::vector<Patch>> patch;
  std::string url;
  std::string version;

public:
  /**
   * Controls whether the project should be build and installed with cmake.
   * Defaults to true. Set it to false when some library does not provide
   * CMakelists.txt or is header-only.
   */
  std::shared_ptr<bool> get_cmake_build() const { return cmake_build; }
  void set_cmake_build(std::shared_ptr<bool> value)
  {
    this->cmake_build = value;
  }

  /**
   * Some of the projects have CMakeLists.txt inside non project-root directory.
   * If that's the case provide relative path to the project root that points to
   * the directory containing CMakeLists
   */
  std::shared_ptr<std::string> get_cmake_directory() const
  {
    return cmake_directory;
  }
  void set_cmake_directory(std::shared_ptr<std::string> value)
  {
    this->cmake_directory = value;
  }

  /**
   * Dictionary of key value pairs, where keys are
   * cmake variable names.
   * Those values strictly correspond to the -D{key}={value} args provided in
   * the command line when configuring with cmake
   */
  std::shared_ptr<std::map<std::string, nlohmann::json>> get_configuration()
    const
  {
    return configuration;
  }
  void set_configuration(
    std::shared_ptr<std::map<std::string, nlohmann::json>> value)
  {
    this->configuration = value;
  }

  /**
   * After sourcing the file, provide a list of copy objects
   * that describe what to copy to the install directory
   */
  std::shared_ptr<std::vector<Copy>> get_copy() const { return copy; }
  void set_copy(std::shared_ptr<std::vector<Copy>> value)
  {
    this->copy = value;
  }

  /**
   * Name of the library that will be used
   */
  const std::string& get_name() const { return name; }
  std::string& get_mutable_name() { return name; }
  void set_name(const std::string& value) { this->name = value; }

  std::shared_ptr<std::vector<Patch>> get_patch() const { return patch; }
  void set_patch(std::shared_ptr<std::vector<Patch>> value)
  {
    this->patch = value;
  }

  /**
   * Target url that points to either git repository or
   * tar gz file with the sources
   */
  const std::string& get_url() const { return url; }
  std::string& get_mutable_url() { return url; }
  void set_url(const std::string& value) { this->url = value; }

  /**
   * Target version of the library that should be used.
   * When url points to git repository, the version should match the release tag
   * of the repository
   */
  const std::string& get_version() const { return version; }
  std::string& get_mutable_version() { return version; }
  void set_version(const std::string& value) { this->version = value; }
};

class Iwd
{
public:
  Iwd() = default;
  virtual ~Iwd() = default;

private:
  std::string iwd_version;
  std::vector<Requirement> requirements;

public:
  /**
   * Minimal iwd version required
   */
  const std::string& get_iwd_version() const { return iwd_version; }
  std::string& get_mutable_iwd_version() { return iwd_version; }
  void set_iwd_version(const std::string& value) { this->iwd_version = value; }

  /**
   * List of requirements
   */
  const std::vector<Requirement>& get_requirements() const
  {
    return requirements;
  }
  std::vector<Requirement>& get_mutable_requirements() { return requirements; }
  void set_requirements(const std::vector<Requirement>& value)
  {
    this->requirements = value;
  }
};
} // namespace quicktype
} // namespace iwd

namespace nlohmann {
void
from_json(const json& j, iwd::quicktype::Copy& x);
void
to_json(json& j, const iwd::quicktype::Copy& x);

void
from_json(const json& j, iwd::quicktype::Patch& x);
void
to_json(json& j, const iwd::quicktype::Patch& x);

void
from_json(const json& j, iwd::quicktype::Requirement& x);
void
to_json(json& j, const iwd::quicktype::Requirement& x);

void
from_json(const json& j, iwd::quicktype::Iwd& x);
void
to_json(json& j, const iwd::quicktype::Iwd& x);

void
from_json(const json& j, iwd::quicktype::Type& x);
void
to_json(json& j, const iwd::quicktype::Type& x);

inline void
from_json(const json& j, iwd::quicktype::Copy& x)
{
  x.set_destination(j.at("destination").get<std::string>());
  x.set_keep_paths(iwd::quicktype::get_optional<bool>(j, "keep-paths"));
  x.set_source_directory(
    iwd::quicktype::get_optional<std::string>(j, "source-directory"));
  x.set_sources(j.at("sources").get<std::string>());
}

inline void
to_json(json& j, const iwd::quicktype::Copy& x)
{
  j = json::object();
  j["destination"] = x.get_destination();
  j["keep-paths"] = x.get_keep_paths();
  j["source-directory"] = x.get_source_directory();
  j["sources"] = x.get_sources();
}

inline void
from_json(const json& j, iwd::quicktype::Patch& x)
{
  x.set_file(j.at("file").get<std::string>());
  x.set_pattern(iwd::quicktype::get_optional<std::string>(j, "pattern"));
  x.set_text(j.at("text").get<std::vector<std::string>>());
  x.set_type(iwd::quicktype::get_optional<iwd::quicktype::Type>(j, "type"));
}

inline void
to_json(json& j, const iwd::quicktype::Patch& x)
{
  j = json::object();
  j["file"] = x.get_file();
  j["pattern"] = x.get_pattern();
  j["text"] = x.get_text();
  j["type"] = x.get_type();
}

inline void
from_json(const json& j, iwd::quicktype::Requirement& x)
{
  x.set_cmake_build(iwd::quicktype::get_optional<bool>(j, "cmake_build"));
  x.set_cmake_directory(
    iwd::quicktype::get_optional<std::string>(j, "cmake_directory"));
  x.set_configuration(iwd::quicktype::get_optional<std::map<std::string, json>>(
    j, "configuration"));
  x.set_copy(
    iwd::quicktype::get_optional<std::vector<iwd::quicktype::Copy>>(j, "copy"));
  x.set_name(j.at("name").get<std::string>());
  x.set_patch(iwd::quicktype::get_optional<std::vector<iwd::quicktype::Patch>>(
    j, "patch"));
  x.set_url(j.at("url").get<std::string>());
  x.set_version(j.at("version").get<std::string>());
}

inline void
to_json(json& j, const iwd::quicktype::Requirement& x)
{
  j = json::object();
  j["cmake_build"] = x.get_cmake_build();
  j["cmake_directory"] = x.get_cmake_directory();
  j["configuration"] = x.get_configuration();
  j["copy"] = x.get_copy();
  j["name"] = x.get_name();
  j["patch"] = x.get_patch();
  j["url"] = x.get_url();
  j["version"] = x.get_version();
}

inline void
from_json(const json& j, iwd::quicktype::Iwd& x)
{
  x.set_iwd_version(j.at("iwd_version").get<std::string>());
  x.set_requirements(
    j.at("requirements").get<std::vector<iwd::quicktype::Requirement>>());
}

inline void
to_json(json& j, const iwd::quicktype::Iwd& x)
{
  j = json::object();
  j["iwd_version"] = x.get_iwd_version();
  j["requirements"] = x.get_requirements();
}

inline void
from_json(const json& j, iwd::quicktype::Type& x)
{
  if (j == "append")
    x = iwd::quicktype::Type::append;
  else if (j == "create")
    x = iwd::quicktype::Type::create;
  else if (j == "replace")
    x = iwd::quicktype::Type::replace;
  else
    throw "Input JSON does not conform to schema";
}

inline void
to_json(json& j, const iwd::quicktype::Type& x)
{
  switch (x) {
    case iwd::quicktype::Type::append:
      j = "append";
      break;
    case iwd::quicktype::Type::create:
      j = "create";
      break;
    case iwd::quicktype::Type::replace:
      j = "replace";
      break;
    default:
      throw "This should not happen";
  }
}
} // namespace nlohmann
