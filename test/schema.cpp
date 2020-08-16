class string
{};

template<typename... T>
class variant
{};

template<typename T>
class vector
{};

class patch_base
{
  // Describes the relative path from source directory
  // to the file on which the operation should be executed
  [[required]] string file;
};

class create_file
{
  // Lines, which the file will be created with
  [[required]] string text;
};

class replace_in_file : patch_base
{
  // Regex expression that serves as the pattern for replacement
  [[required]] string pattern;

  // Lines, which will be added to the file
  [[required]] string text;
};

class append_file : patch_base
{
  // Lines, which will be appended to the file
  [[required]] string text;
};

[[main]] class requirement
{

  vector<variant<create_file, replace_in_file, append_file>> patch;
};
