// Unit tests for graphics types and enums

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <Lumina/Graphics/types.h>
#include <glm/glm.hpp>

using namespace lumina::graphics;
using Catch::Approx;

TEST_CASE("viewport structure", "[graphics][types]")
{
    SECTION("default construction")
    {
        viewport vp;

        REQUIRE(vp.x == Approx(0.0f));
        REQUIRE(vp.y == Approx(0.0f));
        REQUIRE(vp.width == Approx(0.0f));
        REQUIRE(vp.height == Approx(0.0f));
        REQUIRE(vp.min_depth == Approx(0.0f));
        REQUIRE(vp.max_depth == Approx(1.0f));
    }

    SECTION("custom values")
    {
        viewport vp;
        vp.x = 100.0f;
        vp.y = 50.0f;
        vp.width = 1920.0f;
        vp.height = 1080.0f;

        REQUIRE(vp.x == Approx(100.0f));
        REQUIRE(vp.y == Approx(50.0f));
        REQUIRE(vp.width == Approx(1920.0f));
        REQUIRE(vp.height == Approx(1080.0f));
    }

    SECTION("aspect ratio calculation")
    {
        viewport vp;
        vp.width = 1920.0f;
        vp.height = 1080.0f;

        float aspect = vp.width / vp.height;
        REQUIRE(aspect == Approx(16.0f / 9.0f).margin(0.01f));
    }
}

TEST_CASE("scissor_rect structure", "[graphics][types]")
{
    SECTION("default construction")
    {
        scissor_rect rect;

        REQUIRE(rect.x == 0);
        REQUIRE(rect.y == 0);
        REQUIRE(rect.width == 0);
        REQUIRE(rect.height == 0);
    }

    SECTION("custom values")
    {
        scissor_rect rect;
        rect.x = 100;
        rect.y = 200;
        rect.width = 400;
        rect.height = 300;

        REQUIRE(rect.x == 100);
        REQUIRE(rect.y == 200);
        REQUIRE(rect.width == 400);
        REQUIRE(rect.height == 300);
    }

    SECTION("intersection calculation")
    {
        scissor_rect a, b;

        // Rect A: 0,0 to 100,100
        a.x = 0; a.y = 0; a.width = 100; a.height = 100;

        // Rect B: 50,50 to 150,150
        b.x = 50; b.y = 50; b.width = 100; b.height = 100;

        // Calculate intersection
        int32_t x1 = std::max(a.x, b.x);
        int32_t y1 = std::max(a.y, b.y);
        int32_t x2 = std::min(a.x + a.width, b.x + b.width);
        int32_t y2 = std::min(a.y + a.height, b.y + b.height);

        scissor_rect intersection;
        intersection.x = x1;
        intersection.y = y1;
        intersection.width = x2 - x1;
        intersection.height = y2 - y1;

        REQUIRE(intersection.x == 50);
        REQUIRE(intersection.y == 50);
        REQUIRE(intersection.width == 50);
        REQUIRE(intersection.height == 50);
    }

    SECTION("no intersection")
    {
        scissor_rect a, b;

        // Rect A: 0,0 to 50,50
        a.x = 0; a.y = 0; a.width = 50; a.height = 50;

        // Rect B: 100,100 to 200,200 (no overlap)
        b.x = 100; b.y = 100; b.width = 100; b.height = 100;

        int32_t x1 = std::max(a.x, b.x);
        int32_t y1 = std::max(a.y, b.y);
        int32_t x2 = std::min(a.x + a.width, b.x + b.width);
        int32_t y2 = std::min(a.y + a.height, b.y + b.height);

        // No intersection when x2 <= x1 or y2 <= y1
        bool has_intersection = (x2 > x1) && (y2 > y1);
        REQUIRE_FALSE(has_intersection);
    }
}

TEST_CASE("clear_color structure", "[graphics][types]")
{
    SECTION("default construction")
    {
        clear_color cc;

        REQUIRE(cc.r == Approx(0.0f));
        REQUIRE(cc.g == Approx(0.0f));
        REQUIRE(cc.b == Approx(0.0f));
        REQUIRE(cc.a == Approx(1.0f));
    }

    SECTION("construction with values")
    {
        clear_color cc(0.5f, 0.6f, 0.7f, 0.8f);

        REQUIRE(cc.r == Approx(0.5f));
        REQUIRE(cc.g == Approx(0.6f));
        REQUIRE(cc.b == Approx(0.7f));
        REQUIRE(cc.a == Approx(0.8f));
    }
}

TEST_CASE("blend_mode enum", "[graphics][types]")
{
    SECTION("enum values exist")
    {
        blend_mode mode;

        mode = blend_mode::opaque;
        mode = blend_mode::alpha;
        mode = blend_mode::additive;
        mode = blend_mode::multiply;

        REQUIRE(true);  // Just checking compilation
    }

    SECTION("enum casting")
    {
        REQUIRE(static_cast<int>(blend_mode::opaque) != static_cast<int>(blend_mode::alpha));
        REQUIRE(static_cast<int>(blend_mode::alpha) != static_cast<int>(blend_mode::additive));
        REQUIRE(static_cast<int>(blend_mode::additive) != static_cast<int>(blend_mode::multiply));
    }
}

TEST_CASE("depth_mode enum", "[graphics][types]")
{
    SECTION("enum values exist")
    {
        depth_mode mode;

        mode = depth_mode::none;
        mode = depth_mode::read_only;
        mode = depth_mode::read_write;

        REQUIRE(true);
    }
}

TEST_CASE("cull_mode enum", "[graphics][types]")
{
    SECTION("enum values exist")
    {
        cull_mode mode;

        mode = cull_mode::none;
        mode = cull_mode::front;
        mode = cull_mode::back;

        REQUIRE(true);
    }
}

TEST_CASE("format enum", "[graphics][types]")
{
    SECTION("common formats exist")
    {
        format fmt;

        fmt = format::unknown;
        fmt = format::rgba8_unorm;
        fmt = format::bgra8_unorm;
        fmt = format::r32_float;
        fmt = format::rg32_float;
        fmt = format::rgba32_float;
        fmt = format::d32_float;
        fmt = format::d24_unorm_s8_uint;

        REQUIRE(true);
    }
}

TEST_CASE("buffer_usage enum", "[graphics][types]")
{
    SECTION("enum values exist")
    {
        buffer_usage usage;

        usage = buffer_usage::immutable;
        usage = buffer_usage::dynamic;

        REQUIRE(static_cast<int>(buffer_usage::immutable) != static_cast<int>(buffer_usage::dynamic));
    }
}

TEST_CASE("filter_mode enum", "[graphics][types]")
{
    SECTION("enum values exist")
    {
        filter_mode mode;

        mode = filter_mode::point;
        mode = filter_mode::linear;

        REQUIRE(true);
    }
}

TEST_CASE("topology enum", "[graphics][types]")
{
    SECTION("enum values exist")
    {
        topology topo;

        topo = topology::triangles;
        topo = topology::lines;
        topo = topology::points;

        REQUIRE(true);
    }
}

TEST_CASE("color conversions", "[graphics][types]")
{
    SECTION("glm::vec4 color range")
    {
        glm::vec4 color(0.5f, 0.25f, 0.75f, 1.0f);

        // Convert to 8-bit values
        uint8_t r = static_cast<uint8_t>(color.r * 255.0f);
        uint8_t g = static_cast<uint8_t>(color.g * 255.0f);
        uint8_t b = static_cast<uint8_t>(color.b * 255.0f);
        uint8_t a = static_cast<uint8_t>(color.a * 255.0f);

        REQUIRE(r == 127);  // 0.5 * 255
        REQUIRE(g == 63);   // 0.25 * 255
        REQUIRE(b == 191);  // 0.75 * 255
        REQUIRE(a == 255);  // 1.0 * 255
    }

    SECTION("8-bit to float")
    {
        uint8_t r = 128, g = 64, b = 192, a = 255;

        float fr = r / 255.0f;
        float fg = g / 255.0f;
        float fb = b / 255.0f;
        float fa = a / 255.0f;

        REQUIRE(fr == Approx(0.502f).margin(0.01f));
        REQUIRE(fg == Approx(0.251f).margin(0.01f));
        REQUIRE(fb == Approx(0.753f).margin(0.01f));
        REQUIRE(fa == Approx(1.0f));
    }

    SECTION("packed RGBA")
    {
        uint8_t r = 255, g = 128, b = 64, a = 255;

        uint32_t packed =
            (static_cast<uint32_t>(r) << 0) |
            (static_cast<uint32_t>(g) << 8) |
            (static_cast<uint32_t>(b) << 16) |
            (static_cast<uint32_t>(a) << 24);

        // Unpack
        uint8_t ur = (packed >> 0) & 0xFF;
        uint8_t ug = (packed >> 8) & 0xFF;
        uint8_t ub = (packed >> 16) & 0xFF;
        uint8_t ua = (packed >> 24) & 0xFF;

        REQUIRE(ur == r);
        REQUIRE(ug == g);
        REQUIRE(ub == b);
        REQUIRE(ua == a);
    }
}
