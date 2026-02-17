// Unit tests for render_target

#include <catch2/catch_test_macros.hpp>

#include "graphics_test_fixture.h"
#include <lumina/graphics/render_target.h>
#include <lumina/graphics/texture.h>

using namespace lumina::graphics;

TEST_CASE("render_target creation", "[graphics][render_target]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("create color only")
    {
        auto rt = render_target::create(dev, 256, 256, format::rgba8_unorm);

        REQUIRE(rt != nullptr);
        REQUIRE(rt->get_width() == 256);
        REQUIRE(rt->get_height() == 256);
        REQUIRE(rt->get_color_format() == format::rgba8_unorm);
        REQUIRE(rt->get_depth_format() == format::unknown);
        REQUIRE_FALSE(rt->has_depth());
        REQUIRE(rt->get_framebuffer() != nullptr);
    }

    SECTION("create with depth")
    {
        auto rt = render_target::create(dev, 512, 512, format::rgba8_unorm, format::d32_float);

        REQUIRE(rt != nullptr);
        REQUIRE(rt->get_width() == 512);
        REQUIRE(rt->get_height() == 512);
        REQUIRE(rt->get_color_format() == format::rgba8_unorm);
        REQUIRE(rt->get_depth_format() == format::d32_float);
        REQUIRE(rt->has_depth());
    }

    SECTION("create non-square")
    {
        auto rt = render_target::create(dev, 1920, 1080, format::rgba8_unorm);

        REQUIRE(rt != nullptr);
        REQUIRE(rt->get_width() == 1920);
        REQUIRE(rt->get_height() == 1080);
    }

    SECTION("create small")
    {
        auto rt = render_target::create(dev, 64, 64, format::rgba8_unorm);

        REQUIRE(rt != nullptr);
        REQUIRE(rt->get_width() == 64);
        REQUIRE(rt->get_height() == 64);
    }
}

TEST_CASE("render_target formats", "[graphics][render_target]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("RGBA8 format")
    {
        auto rt = render_target::create(dev, 256, 256, format::rgba8_unorm);

        REQUIRE(rt != nullptr);
        REQUIRE(rt->get_color_format() == format::rgba8_unorm);
    }

    SECTION("BGRA8 format")
    {
        auto rt = render_target::create(dev, 256, 256, format::bgra8_unorm);

        REQUIRE(rt != nullptr);
        REQUIRE(rt->get_color_format() == format::bgra8_unorm);
    }

    SECTION("RGBA32F format (HDR)")
    {
        auto rt = render_target::create(dev, 256, 256, format::rgba32_float);

        REQUIRE(rt != nullptr);
        REQUIRE(rt->get_color_format() == format::rgba32_float);
    }
}

TEST_CASE("render_target depth formats", "[graphics][render_target]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("D32 float depth")
    {
        auto rt = render_target::create(dev, 256, 256, format::rgba8_unorm, format::d32_float);

        REQUIRE(rt != nullptr);
        REQUIRE(rt->has_depth());
        REQUIRE(rt->get_depth_format() == format::d32_float);
    }

    SECTION("D24S8 depth-stencil")
    {
        auto rt = render_target::create(dev, 256, 256, format::rgba8_unorm, format::d24_unorm_s8_uint);

        REQUIRE(rt != nullptr);
        REQUIRE(rt->has_depth());
        REQUIRE(rt->get_depth_format() == format::d24_unorm_s8_uint);
    }
}

TEST_CASE("render_target textures", "[graphics][render_target]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("color texture accessible")
    {
        auto rt = render_target::create(dev, 256, 256, format::rgba8_unorm);

        REQUIRE(rt != nullptr);
        REQUIRE(rt->get_color_texture() != nullptr);
        REQUIRE(rt->get_color_texture()->get_width() == 256);
        REQUIRE(rt->get_color_texture()->get_height() == 256);
    }

    SECTION("depth texture accessible when present")
    {
        auto rt = render_target::create(dev, 256, 256, format::rgba8_unorm, format::d32_float);

        REQUIRE(rt != nullptr);
        REQUIRE(rt->get_depth_texture() != nullptr);
        REQUIRE(rt->get_depth_texture()->get_width() == 256);
        REQUIRE(rt->get_depth_texture()->get_height() == 256);
    }

    SECTION("depth texture null when not present")
    {
        auto rt = render_target::create(dev, 256, 256, format::rgba8_unorm);

        REQUIRE(rt != nullptr);
        REQUIRE(rt->get_depth_texture() == nullptr);
    }
}

TEST_CASE("render_target resize", "[graphics][render_target]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("resize maintains format")
    {
        auto rt = render_target::create(dev, 256, 256, format::rgba8_unorm, format::d32_float);

        REQUIRE(rt != nullptr);
        REQUIRE(rt->get_width() == 256);
        REQUIRE(rt->get_height() == 256);

        rt->resize(512, 512);

        REQUIRE(rt->get_width() == 512);
        REQUIRE(rt->get_height() == 512);
        REQUIRE(rt->get_color_format() == format::rgba8_unorm);
        REQUIRE(rt->get_depth_format() == format::d32_float);
        REQUIRE(rt->has_depth());
    }

    SECTION("resize to smaller")
    {
        auto rt = render_target::create(dev, 512, 512, format::rgba8_unorm);

        rt->resize(128, 128);

        REQUIRE(rt->get_width() == 128);
        REQUIRE(rt->get_height() == 128);
    }

    SECTION("resize to larger")
    {
        auto rt = render_target::create(dev, 128, 128, format::rgba8_unorm);

        rt->resize(1024, 1024);

        REQUIRE(rt->get_width() == 1024);
        REQUIRE(rt->get_height() == 1024);
    }

    SECTION("resize updates textures")
    {
        auto rt = render_target::create(dev, 256, 256, format::rgba8_unorm);

        rt->resize(512, 384);

        REQUIRE(rt->get_color_texture()->get_width() == 512);
        REQUIRE(rt->get_color_texture()->get_height() == 384);
    }
}

TEST_CASE("render_target multiple instances", "[graphics][render_target]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("create multiple render targets")
    {
        auto rt1 = render_target::create(dev, 256, 256, format::rgba8_unorm);
        auto rt2 = render_target::create(dev, 512, 512, format::rgba8_unorm);
        auto rt3 = render_target::create(dev, 128, 128, format::rgba8_unorm, format::d32_float);

        REQUIRE(rt1 != nullptr);
        REQUIRE(rt2 != nullptr);
        REQUIRE(rt3 != nullptr);

        // Each should have unique framebuffers
        REQUIRE(rt1->get_framebuffer() != rt2->get_framebuffer());
        REQUIRE(rt2->get_framebuffer() != rt3->get_framebuffer());
    }
}

TEST_CASE("render_target MSAA", "[graphics][render_target]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("no MSAA by default")
    {
        auto rt = render_target::create(dev, 256, 256, format::rgba8_unorm);

        REQUIRE(rt != nullptr);
        REQUIRE_FALSE(rt->is_msaa());
        REQUIRE(rt->get_sample_count() == 1);
    }

    SECTION("create with 4x MSAA")
    {
        auto rt = render_target::create(dev, 256, 256, format::rgba8_unorm, format::unknown, 4);

        REQUIRE(rt != nullptr);
        REQUIRE(rt->is_msaa());
        REQUIRE(rt->get_sample_count() == 4);
        REQUIRE(rt->get_width() == 256);
        REQUIRE(rt->get_height() == 256);
    }

    SECTION("create with 8x MSAA")
    {
        auto rt = render_target::create(dev, 256, 256, format::rgba8_unorm, format::unknown, 8);

        REQUIRE(rt != nullptr);
        REQUIRE(rt->is_msaa());
        REQUIRE(rt->get_sample_count() == 8);
    }

    SECTION("MSAA with depth buffer")
    {
        auto rt = render_target::create(dev, 256, 256, format::rgba8_unorm, format::d32_float, 4);

        REQUIRE(rt != nullptr);
        REQUIRE(rt->is_msaa());
        REQUIRE(rt->get_sample_count() == 4);
        REQUIRE(rt->has_depth());
        REQUIRE(rt->get_depth_format() == format::d32_float);
    }

    SECTION("MSAA color texture accessible for sampling")
    {
        auto rt = render_target::create(dev, 256, 256, format::rgba8_unorm, format::unknown, 4);

        REQUIRE(rt != nullptr);
        // get_color_texture should return the resolved texture for MSAA
        REQUIRE(rt->get_color_texture() != nullptr);
        REQUIRE(rt->get_color_texture()->get_width() == 256);
        REQUIRE(rt->get_color_texture()->get_height() == 256);
    }

    SECTION("resize maintains MSAA sample count")
    {
        auto rt = render_target::create(dev, 256, 256, format::rgba8_unorm, format::unknown, 4);

        REQUIRE(rt != nullptr);
        REQUIRE(rt->get_sample_count() == 4);

        rt->resize(512, 512);

        REQUIRE(rt->get_width() == 512);
        REQUIRE(rt->get_height() == 512);
        REQUIRE(rt->is_msaa());
        REQUIRE(rt->get_sample_count() == 4);
    }
}
