#include <catch2/catch.hpp>
#include <vn/semantic_version.hpp>

TEST_CASE("Semantic version parsing legal", "[semantic_version]")
{
  auto version_string = GENERATE(
    "0.0.4",
    "1.2.3",
    "10.20.30",
    "1.1.2-prerelease+meta",
    "1.1.2+meta",
    "1.1.2+meta-valid",
    "1.0.0-alpha",
    "1.0.0-beta",
    "1.0.0-alpha.beta",
    "1.0.0-alpha.beta.1",
    "1.0.0-alpha.1",
    "1.0.0-alpha0.valid",
    "1.0.0-alpha.0valid",
    "1.0.0-alpha-a.b-c-somethinglong+build.1-aef.1-its-okay",
    "1.0.0-rc.1+build.1",
    "2.0.0-rc.1+build.123",
    "1.2.3-beta",
    "10.2.3-DEV-SNAPSHOT",
    "1.2.3-SNAPSHOT-123",
    "1.0.0",
    "2.0.0",
    "1.1.7",
    "2.0.0+build.1848",
    "2.0.1-alpha.1227",
    "1.0.0-alpha+beta",
    "1.2.3----RC-SNAPSHOT.12.9.1--.12+788",
    "1.2.3----R-S.12.9.1--.12+meta",
    "1.2.3----RC-SNAPSHOT.12.9.1--.12",
    "1.0.0+0.build.1-rc.10000aaa-kk-0.1",
    // TODO - This case is illegal, but should throw with appropeate information
    //"99999999999999999999999.999999999999999999.99999999999999999",
    "1.0.0-0A.is.legal");
  auto semver = vn::semantic_version::try_parse(version_string);
  REQUIRE(semver.has_value());
  REQUIRE(semver->str() == version_string);
}

TEST_CASE("Version comparsion tests", "[semantic_version]")
{
  REQUIRE(
    vn::semantic_version::try_parse("1.0.0-alpha").value() ==
    vn::semantic_version(1, 0, 0, "alpha"));
  REQUIRE(
    vn::semantic_version::try_parse("1.0.0-alpha").value() <
    vn::semantic_version(1, 0, 0, "alpha.1"));
  REQUIRE(
    vn::semantic_version::try_parse("1.0.0-alpha").value() <
    vn::semantic_version(1, 0, 0, "alpha.beta"));
  REQUIRE(
    vn::semantic_version::try_parse("1.0.0-alpha").value() <
    vn::semantic_version(1, 0, 0, "beta"));
  REQUIRE(
    vn::semantic_version::try_parse("1.0.0-alpha").value() <
    vn::semantic_version(1, 0, 0, "beta.2"));
  REQUIRE(
    vn::semantic_version::try_parse("1.0.0-alpha").value() <
    vn::semantic_version(1, 0, 0, "beta.11"));
  REQUIRE(
    vn::semantic_version::try_parse("1.0.0-alpha").value() <
    vn::semantic_version(1, 0, 0, "rc.1"));
  REQUIRE(
    vn::semantic_version::try_parse("1.0.0-alpha").value() <
    vn::semantic_version(1, 0, 0));
  REQUIRE(
    vn::semantic_version::try_parse("1.0.0").value() ==
    vn::semantic_version(1, 0, 0));
  REQUIRE(
    vn::semantic_version::try_parse("1.1.0").value() ==
    vn::semantic_version(1, 1, 0));
  REQUIRE(
    vn::semantic_version::try_parse("1.1.0").value() >
    vn::semantic_version(1, 0, 0));
}