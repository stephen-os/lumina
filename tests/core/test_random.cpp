#include <catch2/catch_all.hpp>
#include <lumina/core/random.h>

using namespace lumina::core;

TEST_CASE("Random with seed produces deterministic results", "[random]")
{
    random rng1(12345);
    random rng2(12345);

    for (int i = 0; i < 10; ++i)
    {
        REQUIRE(rng1.next_uint32() == rng2.next_uint32());
    }
}

TEST_CASE("Random different seeds produce different results", "[random]")
{
    random rng1(12345);
    random rng2(54321);

    // Very unlikely to be equal with different seeds
    REQUIRE(rng1.next_uint32() != rng2.next_uint32());
}

TEST_CASE("Random set_seed resets sequence", "[random]")
{
    random rng(12345);
    uint32_t first = rng.next_uint32();
    rng.next_uint32();
    rng.next_uint32();

    rng.set_seed(12345);
    REQUIRE(rng.next_uint32() == first);
}

TEST_CASE("Random int32 range", "[random]")
{
    random rng(42);

    for (int i = 0; i < 100; ++i)
    {
        int32_t val = rng.int32(10, 20);
        REQUIRE(val >= 10);
        REQUIRE(val <= 20);
    }
}

TEST_CASE("Random int32 with max", "[random]")
{
    random rng(42);

    for (int i = 0; i < 100; ++i)
    {
        int32_t val = rng.int32(50);
        REQUIRE(val >= 0);
        REQUIRE(val <= 50);
    }
}

TEST_CASE("Random floating range 0-1", "[random]")
{
    random rng(42);

    for (int i = 0; i < 100; ++i)
    {
        float val = rng.floating();
        REQUIRE(val >= 0.0f);
        REQUIRE(val <= 1.0f);
    }
}

TEST_CASE("Random floating with max", "[random]")
{
    random rng(42);

    for (int i = 0; i < 100; ++i)
    {
        float val = rng.floating(10.0f);
        REQUIRE(val >= 0.0f);
        REQUIRE(val <= 10.0f);
    }
}

TEST_CASE("Random floating range", "[random]")
{
    random rng(42);

    for (int i = 0; i < 100; ++i)
    {
        float val = rng.floating(5.0f, 15.0f);
        REQUIRE(val >= 5.0f);
        REQUIRE(val <= 15.0f);
    }
}

TEST_CASE("Random real (double) range", "[random]")
{
    random rng(42);

    for (int i = 0; i < 100; ++i)
    {
        double val = rng.real();
        REQUIRE(val >= 0.0);
        REQUIRE(val <= 1.0);
    }
}

TEST_CASE("Random boolean", "[random]")
{
    random rng(42);

    int true_count = 0;
    int false_count = 0;

    for (int i = 0; i < 1000; ++i)
    {
        if (rng.boolean())
            ++true_count;
        else
            ++false_count;
    }

    // Should have a reasonable distribution (not all one value)
    REQUIRE(true_count > 100);
    REQUIRE(false_count > 100);
}

TEST_CASE("Random boolean with probability", "[random]")
{
    random rng(42);

    // With 0% probability, should always be false
    for (int i = 0; i < 100; ++i)
    {
        REQUIRE(rng.boolean(0.0f) == false);
    }

    // With 100% probability, should always be true
    for (int i = 0; i < 100; ++i)
    {
        REQUIRE(rng.boolean(1.0f) == true);
    }
}

TEST_CASE("Random vec2", "[random]")
{
    random rng(42);

    auto v = rng.vec2();
    REQUIRE(v.x >= 0.0f);
    REQUIRE(v.x <= 1.0f);
    REQUIRE(v.y >= 0.0f);
    REQUIRE(v.y <= 1.0f);
}

TEST_CASE("Random vec3 with range", "[random]")
{
    random rng(42);

    auto v = rng.vec3(10.0f, 20.0f);
    REQUIRE(v.x >= 10.0f);
    REQUIRE(v.x <= 20.0f);
    REQUIRE(v.y >= 10.0f);
    REQUIRE(v.y <= 20.0f);
    REQUIRE(v.z >= 10.0f);
    REQUIRE(v.z <= 20.0f);
}

TEST_CASE("Random vec4", "[random]")
{
    random rng(42);

    auto v = rng.vec4();
    REQUIRE(v.x >= 0.0f);
    REQUIRE(v.x <= 1.0f);
    REQUIRE(v.w >= 0.0f);
    REQUIRE(v.w <= 1.0f);
}

TEST_CASE("Random unit_circle produces unit vectors", "[random]")
{
    random rng(42);

    for (int i = 0; i < 100; ++i)
    {
        auto v = rng.unit_circle();
        float length = glm::length(v);
        REQUIRE(length == Catch::Approx(1.0f).margin(0.0001f));
    }
}

TEST_CASE("Random inside_circle within radius", "[random]")
{
    random rng(42);

    for (int i = 0; i < 100; ++i)
    {
        auto v = rng.inside_circle();
        float length = glm::length(v);
        REQUIRE(length <= 1.0f);
    }
}

TEST_CASE("Random inside_circle with custom radius", "[random]")
{
    random rng(42);

    for (int i = 0; i < 100; ++i)
    {
        auto v = rng.inside_circle(5.0f);
        float length = glm::length(v);
        REQUIRE(length <= 5.0f);
    }
}

TEST_CASE("Random angle", "[random]")
{
    random rng(42);

    for (int i = 0; i < 100; ++i)
    {
        float angle = rng.angle();
        REQUIRE(angle >= 0.0f);
        REQUIRE(angle <= glm::two_pi<float>());
    }
}

TEST_CASE("Random angle_degrees", "[random]")
{
    random rng(42);

    for (int i = 0; i < 100; ++i)
    {
        float angle = rng.angle_degrees();
        REQUIRE(angle >= 0.0f);
        REQUIRE(angle <= 360.0f);
    }
}

TEST_CASE("Random choose from vector", "[random]")
{
    random rng(42);
    std::vector<int> items = {10, 20, 30, 40, 50};

    for (int i = 0; i < 100; ++i)
    {
        int chosen = rng.choose(items);
        REQUIRE((chosen == 10 || chosen == 20 || chosen == 30 || chosen == 40 || chosen == 50));
    }
}

TEST_CASE("Random choose from initializer list", "[random]")
{
    random rng(42);

    for (int i = 0; i < 100; ++i)
    {
        int chosen = rng.choose({1, 2, 3});
        REQUIRE((chosen == 1 || chosen == 2 || chosen == 3));
    }
}

TEST_CASE("Random color", "[random]")
{
    random rng(42);

    auto c = rng.color();
    REQUIRE(c.r >= 0.0f);
    REQUIRE(c.r <= 1.0f);
    REQUIRE(c.g >= 0.0f);
    REQUIRE(c.g <= 1.0f);
    REQUIRE(c.b >= 0.0f);
    REQUIRE(c.b <= 1.0f);
}

TEST_CASE("Random color_with_alpha", "[random]")
{
    random rng(42);

    auto c = rng.color_with_alpha();
    REQUIRE(c.r >= 0.0f);
    REQUIRE(c.r <= 1.0f);
    REQUIRE(c.a >= 0.0f);
    REQUIRE(c.a <= 1.0f);
}
