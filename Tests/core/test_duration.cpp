#include <catch2/catch_all.hpp>
#include <Lumina/Core/duration.h>

using namespace lumina::core;

TEST_CASE("Duration from milliseconds", "[duration]")
{
    auto d = duration::from_milliseconds(500.0f);

    REQUIRE(d.milliseconds() == 500.0f);
    REQUIRE(d.seconds() == 0.5f);
}

TEST_CASE("Duration from seconds", "[duration]")
{
    auto d = duration::from_seconds(2.5f);

    REQUIRE(d.seconds() == 2.5f);
    REQUIRE(d.milliseconds() == 2500.0f);
}

TEST_CASE("Duration from minutes", "[duration]")
{
    auto d = duration::from_minutes(1.0f);

    REQUIRE(d.milliseconds() == 60000.0f);
    REQUIRE(d.seconds() == 60.0f);
}

TEST_CASE("Duration from hours", "[duration]")
{
    auto d = duration::from_hours(1.0f);

    REQUIRE(d.milliseconds() == 3600000.0f);
    REQUIRE(d.seconds() == 3600.0f);
}

TEST_CASE("Duration comparison operators", "[duration]")
{
    auto d1 = duration::from_seconds(1.0f);
    auto d2 = duration::from_seconds(2.0f);
    auto d3 = duration::from_milliseconds(1000.0f);

    REQUIRE(d1 == d3);
    REQUIRE(d1 != d2);
    REQUIRE(d1 < d2);
    REQUIRE(d2 > d1);
    REQUIRE(d1 <= d3);
    REQUIRE(d1 >= d3);
}

TEST_CASE("Duration arithmetic operators", "[duration]")
{
    auto d1 = duration::from_seconds(1.0f);
    auto d2 = duration::from_seconds(2.0f);

    auto sum = d1 + d2;
    REQUIRE(sum.seconds() == 3.0f);

    auto diff = d2 - d1;
    REQUIRE(diff.seconds() == 1.0f);

    auto scaled = d1 * 3.0f;
    REQUIRE(scaled.seconds() == 3.0f);

    auto divided = d2 / 2.0f;
    REQUIRE(divided.seconds() == 1.0f);
}

TEST_CASE("Duration compound assignment operators", "[duration]")
{
    auto d = duration::from_seconds(1.0f);

    d += duration::from_seconds(0.5f);
    REQUIRE(d.seconds() == 1.5f);

    d -= duration::from_milliseconds(500.0f);
    REQUIRE(d.seconds() == 1.0f);

    d *= 2.0f;
    REQUIRE(d.seconds() == 2.0f);

    d /= 4.0f;
    REQUIRE(d.seconds() == 0.5f);
}

TEST_CASE("Duration literals - milliseconds", "[duration]")
{
    auto d1 = 100_ms;
    REQUIRE(d1.milliseconds() == 100.0f);

    auto d2 = 50.5_ms;
    REQUIRE(d2.milliseconds() == Catch::Approx(50.5f));
}

TEST_CASE("Duration literals - seconds", "[duration]")
{
    auto d1 = 5_s;
    REQUIRE(d1.seconds() == 5.0f);

    auto d2 = 1.5_s;
    REQUIRE(d2.seconds() == Catch::Approx(1.5f));
}

TEST_CASE("Duration literals - minutes", "[duration]")
{
    auto d1 = 2_min;
    REQUIRE(d1.seconds() == 120.0f);

    auto d2 = 0.5_min;
    REQUIRE(d2.seconds() == 30.0f);
}

TEST_CASE("Duration literals - hours", "[duration]")
{
    auto d1 = 1_h;
    REQUIRE(d1.seconds() == 3600.0f);

    auto d2 = 0.5_h;
    REQUIRE(d2.seconds() == 1800.0f);
}

TEST_CASE("Duration to chrono conversion", "[duration]")
{
    auto d = duration::from_milliseconds(1500.0f);

    auto chrono_ms = d.to_chrono_milliseconds();
    REQUIRE(chrono_ms.count() == 1500);

    auto chrono_s = d.to_chrono_seconds();
    REQUIRE(chrono_s.count() == Catch::Approx(1.5f));
}
