#ifndef VN__SCHEMA__TOKEN_HPP
#define VN__SCHEMA__TOKEN_HPP

#include <string_view>

namespace vn::schema {
struct token
{
  enum class type
  {
    class_decl,
    open_bracket,
    close_bracket,
    square_bracket_open,
    square_bracket_close,
    annotation_begin,
    annotation_end,
    string,
    question_mark,
    assignment,
    line_comment,
  };

  std::string_view _text;
  std::size_t _line;
  std::size_t _column;
};
} // namespace vn::schema

#endif
