#include <catch2/catch.hpp>
#include <vn/string_utils.hpp>

TEST_CASE("Replace all", "[string_utils]")
{
  REQUIRE(vn::replace_all("ala ma kota", "ala", "kota") == "kota ma kota");
  REQUIRE(vn::replace_all("aaaabbaabb", "aabb", "c") == "aacc");
  REQUIRE(vn::replace_all("", "aabb", "c") == "");
  REQUIRE(vn::replace_all("aabb", "", "c") == "aabb");
  REQUIRE(vn::replace_all("aabb", "", "") == "aabb");
}

TEST_CASE("join", "[string_utils]")
{
  REQUIRE(vn::join(",", "ala", "ma", "kota") == "ala,ma,kota");
  REQUIRE(vn::join(",", 1, 2, 3, 4, 5) == "1,2,3,4,5");
  REQUIRE(vn::join("", 1, 2, 3, 4, 5) == "12345");
}

TEST_CASE("starts with", "[string_utils]")
{
  REQUIRE(vn::starts_with("ala ma kota", "ala") == true);
  REQUIRE(vn::starts_with("ala", "ala") == true);
  REQUIRE(vn::starts_with("zxc ma kota", "ala") == false);
  REQUIRE(vn::starts_with("", "ala") == false);
  REQUIRE(vn::starts_with("ala ma kota", "") == true);
}

TEST_CASE("ends with", "[string_utils]")
{
  REQUIRE(vn::ends_with("ala ma kota", "kota") == true);
  REQUIRE(vn::ends_with("kota", "kota") == true);
  REQUIRE(vn::ends_with("zxc ma psa", "kota") == false);
  REQUIRE(vn::ends_with("", "ala") == false);
  REQUIRE(vn::ends_with("ala ma kota", "") == true);
}

TEST_CASE("string split", "[string_utils]")
{
  REQUIRE_THAT(
    vn::split("abcd", ","), Catch::Equals<std::string_view>({ "abcd" }));
  REQUIRE_THAT(
    vn::split("a,b,c,d", ","),
    Catch::Equals<std::string_view>({ "a", "b", "c", "d" }));
  REQUIRE_THAT(
    vn::split("a,,b,,c,,d", ",,"),
    Catch::Equals<std::string_view>({ "a", "b", "c", "d" }));
  REQUIRE_THAT(
    vn::split(",a,b,c,d", ","),
    Catch::Equals<std::string_view>({ "", "a", "b", "c", "d" }));
  REQUIRE_THAT(
    vn::split(",a,,b,c,d", ","),
    Catch::Equals<std::string_view>({ "", "a", "", "b", "c", "d" }));
  REQUIRE_THAT(vn::split(",", ","), Catch::Equals<std::string_view>({ "" }));
  REQUIRE_THAT(
    vn::split(",a,,b,c,d", ",", false),
    Catch::Equals<std::string_view>({ "a", "b", "c", "d" }));
}

TEST_CASE("has_digits_only", "[string_utils]")
{
  REQUIRE(vn::has_digits_only("0123") == true);
  REQUIRE(vn::has_digits_only("") == true);
  REQUIRE(vn::has_digits_only("ab0123") == false);
  REQUIRE(vn::has_digits_only("ab") == false);
}