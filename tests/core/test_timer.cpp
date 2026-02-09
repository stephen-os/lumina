#include <catch2/catch_all.hpp>
#include <lumina/core/timer.h>
#include <thread>

using namespace lumina::core;

TEST_CASE("Timer starts at zero", "[timer]")
{
    timer t;

    // Should be very close to zero right after creation
    REQUIRE(t.elapsed() < 0.1f);
    REQUIRE(t.elapsed_millis() < 100.0f);
}

TEST_CASE("Timer measures elapsed time", "[timer]")
{
    timer t;

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    float elapsed = t.elapsed_millis();
    // Allow some tolerance for timing
    REQUIRE(elapsed >= 40.0f);
    REQUIRE(elapsed < 150.0f);
}

TEST_CASE("Timer reset resets elapsed time", "[timer]")
{
    timer t;

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    t.reset();

    // Should be close to zero after reset
    REQUIRE(t.elapsed_millis() < 50.0f);
}

TEST_CASE("Timer elapsed returns seconds", "[timer]")
{
    timer t;

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    float elapsed_sec = t.elapsed();
    float elapsed_ms = t.elapsed_millis();

    // elapsed() should be ~1000x smaller than elapsed_millis()
    REQUIRE(elapsed_ms == Catch::Approx(elapsed_sec * 1000.0f).margin(1.0f));
}
