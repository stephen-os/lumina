// Unit tests for index_buffer

#include <catch2/catch_test_macros.hpp>

#include "graphics_test_fixture.h"
#include <lumina/graphics/index_buffer.h>

#include <array>
#include <vector>

using namespace lumina::graphics;

TEST_CASE("index_buffer creation", "[graphics][index_buffer]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("create 16-bit index buffer")
    {
        std::array<uint16_t, 6> indices = {0, 1, 2, 2, 3, 0};

        auto buffer = index_buffer::create(dev, std::span{indices});

        REQUIRE(buffer != nullptr);
        REQUIRE(buffer->get_count() == 6);
        REQUIRE(buffer->get_index_size() == sizeof(uint16_t));
        REQUIRE(buffer->get_byte_size() == sizeof(indices));
        REQUIRE(buffer->is_16bit());
        REQUIRE_FALSE(buffer->is_32bit());
        REQUIRE(buffer->get_buffer() != nullptr);
    }

    SECTION("create 32-bit index buffer")
    {
        std::array<uint32_t, 6> indices = {0, 1, 2, 2, 3, 0};

        auto buffer = index_buffer::create(dev, std::span{indices});

        REQUIRE(buffer != nullptr);
        REQUIRE(buffer->get_count() == 6);
        REQUIRE(buffer->get_index_size() == sizeof(uint32_t));
        REQUIRE(buffer->get_byte_size() == sizeof(indices));
        REQUIRE(buffer->is_32bit());
        REQUIRE_FALSE(buffer->is_16bit());
    }

    SECTION("create with raw data pointer - 16 bit")
    {
        std::vector<uint16_t> indices = {0, 1, 2, 3, 4, 5, 6, 7, 8};

        auto buffer = index_buffer::create(dev, indices.data(), indices.size(), sizeof(uint16_t));

        REQUIRE(buffer != nullptr);
        REQUIRE(buffer->get_count() == 9);
        REQUIRE(buffer->is_16bit());
    }

    SECTION("create with raw data pointer - 32 bit")
    {
        std::vector<uint32_t> indices = {0, 1, 2, 3, 4, 5};

        auto buffer = index_buffer::create(dev, indices.data(), indices.size(), sizeof(uint32_t));

        REQUIRE(buffer != nullptr);
        REQUIRE(buffer->get_count() == 6);
        REQUIRE(buffer->is_32bit());
    }
}

TEST_CASE("index_buffer large buffers", "[graphics][index_buffer]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("16-bit buffer near limit")
    {
        // 65535 is max value for uint16_t
        std::vector<uint16_t> indices(10000);
        for (size_t i = 0; i < indices.size(); ++i)
        {
            indices[i] = static_cast<uint16_t>(i % 65536);
        }

        auto buffer = index_buffer::create(dev, std::span{indices});

        REQUIRE(buffer != nullptr);
        REQUIRE(buffer->get_count() == 10000);
        REQUIRE(buffer->is_16bit());
    }

    SECTION("32-bit buffer for large meshes")
    {
        // For meshes with more than 65535 vertices
        std::vector<uint32_t> indices(100000);
        for (size_t i = 0; i < indices.size(); ++i)
        {
            indices[i] = static_cast<uint32_t>(i);
        }

        auto buffer = index_buffer::create(dev, std::span{indices});

        REQUIRE(buffer != nullptr);
        REQUIRE(buffer->get_count() == 100000);
        REQUIRE(buffer->is_32bit());
    }
}

TEST_CASE("index_buffer triangle patterns", "[graphics][index_buffer]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("single triangle")
    {
        std::array<uint16_t, 3> indices = {0, 1, 2};

        auto buffer = index_buffer::create(dev, std::span{indices});

        REQUIRE(buffer != nullptr);
        REQUIRE(buffer->get_count() == 3);
    }

    SECTION("quad (two triangles)")
    {
        std::array<uint16_t, 6> indices = {0, 1, 2, 2, 3, 0};

        auto buffer = index_buffer::create(dev, std::span{indices});

        REQUIRE(buffer != nullptr);
        REQUIRE(buffer->get_count() == 6);
    }

    SECTION("triangle strip pattern")
    {
        // Triangle strip: each new vertex forms a triangle with the previous two
        std::array<uint16_t, 8> indices = {0, 1, 2, 3, 4, 5, 6, 7};

        auto buffer = index_buffer::create(dev, std::span{indices});

        REQUIRE(buffer != nullptr);
        REQUIRE(buffer->get_count() == 8);
    }
}

TEST_CASE("index_buffer move semantics", "[graphics][index_buffer]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("move constructor")
    {
        std::array<uint16_t, 6> indices = {0, 1, 2, 2, 3, 0};
        auto buffer1 = index_buffer::create(dev, std::span{indices});

        REQUIRE(buffer1 != nullptr);
        size_t original_count = buffer1->get_count();
        nvrhi::IBuffer* original_handle = buffer1->get_buffer();

        index_buffer buffer2 = std::move(*buffer1);

        REQUIRE(buffer2.get_count() == original_count);
        REQUIRE(buffer2.get_buffer() == original_handle);
    }
}

TEST_CASE("index_buffer validation", "[graphics][index_buffer]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("create with zero count fails")
    {
        auto buffer = index_buffer::create(dev, nullptr, 0, sizeof(uint16_t));

        REQUIRE(buffer == nullptr);
    }

    SECTION("create with invalid index size fails")
    {
        std::array<uint8_t, 6> indices = {0, 1, 2, 2, 3, 0};

        // index_size of 1 is invalid (only 2 or 4 allowed)
        auto buffer = index_buffer::create(dev, indices.data(), indices.size(), sizeof(uint8_t));

        REQUIRE(buffer == nullptr);
    }

    SECTION("create with index size 3 fails")
    {
        auto buffer = index_buffer::create(dev, nullptr, 6, 3);

        REQUIRE(buffer == nullptr);
    }

    SECTION("create with custom debug name")
    {
        std::array<uint16_t, 6> indices = {0, 1, 2, 2, 3, 0};
        auto buffer = index_buffer::create(dev, std::span{indices}, "My Custom Index Buffer");

        REQUIRE(buffer != nullptr);
    }
}
