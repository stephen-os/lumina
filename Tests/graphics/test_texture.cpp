// Unit tests for texture

#include <catch2/catch_test_macros.hpp>

#include "graphics_test_fixture.h"
#include <Lumina/Graphics/texture.h>

#include <array>
#include <vector>

using namespace lumina::graphics;

TEST_CASE("texture creation", "[graphics][texture]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("create empty texture")
    {
        auto tex = texture::create(dev, 64, 64, format::rgba8_unorm, nullptr);

        REQUIRE(tex != nullptr);
        REQUIRE(tex->get_width() == 64);
        REQUIRE(tex->get_height() == 64);
        REQUIRE(tex->get_format() == format::rgba8_unorm);
        REQUIRE(tex->get_texture() != nullptr);
    }

    SECTION("create texture with data")
    {
        // 4x4 RGBA texture
        std::array<uint8_t, 64> pixels;  // 4 * 4 * 4 bytes
        pixels.fill(255);  // White

        auto tex = texture::create(dev, 4, 4, format::rgba8_unorm, pixels.data());

        REQUIRE(tex != nullptr);
        REQUIRE(tex->get_width() == 4);
        REQUIRE(tex->get_height() == 4);
    }

    SECTION("create with texture_desc")
    {
        texture_desc desc;
        desc.width = 128;
        desc.height = 64;
        desc.pixel_format = format::rgba8_unorm;

        auto tex = texture::create(dev, desc, nullptr);

        REQUIRE(tex != nullptr);
        REQUIRE(tex->get_width() == 128);
        REQUIRE(tex->get_height() == 64);
        REQUIRE(tex->get_format() == format::rgba8_unorm);
    }
}

TEST_CASE("texture formats", "[graphics][texture]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("RGBA8 format")
    {
        auto tex = texture::create(dev, 16, 16, format::rgba8_unorm);

        REQUIRE(tex != nullptr);
        REQUIRE(tex->get_format() == format::rgba8_unorm);
    }

    SECTION("BGRA8 format")
    {
        auto tex = texture::create(dev, 16, 16, format::bgra8_unorm);

        REQUIRE(tex != nullptr);
        REQUIRE(tex->get_format() == format::bgra8_unorm);
    }

    SECTION("R32 float format")
    {
        auto tex = texture::create(dev, 16, 16, format::r32_float);

        REQUIRE(tex != nullptr);
        REQUIRE(tex->get_format() == format::r32_float);
    }

    SECTION("RGBA32 float format")
    {
        auto tex = texture::create(dev, 16, 16, format::rgba32_float);

        REQUIRE(tex != nullptr);
        REQUIRE(tex->get_format() == format::rgba32_float);
    }
}

TEST_CASE("texture sizes", "[graphics][texture]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("1x1 texture (smallest)")
    {
        uint32_t pixel = 0xFFFFFFFF;  // White RGBA
        auto tex = texture::create(dev, 1, 1, format::rgba8_unorm, &pixel);

        REQUIRE(tex != nullptr);
        REQUIRE(tex->get_width() == 1);
        REQUIRE(tex->get_height() == 1);
    }

    SECTION("power of 2 texture")
    {
        auto tex = texture::create(dev, 256, 256, format::rgba8_unorm);

        REQUIRE(tex != nullptr);
        REQUIRE(tex->get_width() == 256);
        REQUIRE(tex->get_height() == 256);
    }

    SECTION("non-power of 2 texture")
    {
        auto tex = texture::create(dev, 100, 75, format::rgba8_unorm);

        REQUIRE(tex != nullptr);
        REQUIRE(tex->get_width() == 100);
        REQUIRE(tex->get_height() == 75);
    }

    SECTION("non-square texture")
    {
        auto tex = texture::create(dev, 512, 128, format::rgba8_unorm);

        REQUIRE(tex != nullptr);
        REQUIRE(tex->get_width() == 512);
        REQUIRE(tex->get_height() == 128);
    }

    SECTION("large texture")
    {
        auto tex = texture::create(dev, 2048, 2048, format::rgba8_unorm);

        REQUIRE(tex != nullptr);
        REQUIRE(tex->get_width() == 2048);
        REQUIRE(tex->get_height() == 2048);
    }
}

TEST_CASE("texture_desc defaults", "[graphics][texture]")
{
    texture_desc desc;

    SECTION("default width")
    {
        REQUIRE(desc.width == 1);
    }

    SECTION("default height")
    {
        REQUIRE(desc.height == 1);
    }

    SECTION("default format")
    {
        REQUIRE(desc.pixel_format == format::rgba8_unorm);
    }

    SECTION("default mip generation")
    {
        REQUIRE(desc.generate_mips == false);
    }
}

TEST_CASE("texture validation", "[graphics][texture]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("create with zero width fails")
    {
        auto tex = texture::create(dev, 0, 64, format::rgba8_unorm);

        REQUIRE(tex == nullptr);
    }

    SECTION("create with zero height fails")
    {
        auto tex = texture::create(dev, 64, 0, format::rgba8_unorm);

        REQUIRE(tex == nullptr);
    }

    SECTION("create with custom debug name")
    {
        auto tex = texture::create(dev, 32, 32, format::rgba8_unorm, nullptr, "My Custom Texture");

        REQUIRE(tex != nullptr);
    }
}
