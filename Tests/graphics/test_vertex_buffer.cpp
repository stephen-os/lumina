// Unit tests for vertex_buffer

#include <catch2/catch_test_macros.hpp>

#include "graphics_test_fixture.h"
#include <Lumina/Graphics/vertex_buffer.h>

#include <array>
#include <vector>

using namespace lumina::graphics;

TEST_CASE("vertex_buffer creation", "[graphics][vertex_buffer]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("create immutable buffer with data")
    {
        std::array<float, 12> vertices = {
            0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            0.5f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f
        };

        auto buffer = vertex_buffer::create(dev, vertices.data(), sizeof(vertices), sizeof(float) * 3, buffer_usage::immutable);

        REQUIRE(buffer != nullptr);
        REQUIRE(buffer->get_size() == sizeof(vertices));
        REQUIRE(buffer->get_stride() == sizeof(float) * 3);
        REQUIRE(buffer->get_vertex_count() == 4);
        REQUIRE(buffer->get_usage() == buffer_usage::immutable);
        REQUIRE_FALSE(buffer->is_dynamic());
        REQUIRE(buffer->get_buffer() != nullptr);
    }

    SECTION("create dynamic buffer")
    {
        std::array<float, 9> vertices = {
            0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            0.5f, 1.0f, 0.0f
        };

        auto buffer = vertex_buffer::create(dev, vertices.data(), sizeof(vertices), sizeof(float) * 3, buffer_usage::dynamic);

        REQUIRE(buffer != nullptr);
        REQUIRE(buffer->get_size() == sizeof(vertices));
        REQUIRE(buffer->get_stride() == sizeof(float) * 3);
        REQUIRE(buffer->get_vertex_count() == 3);
        REQUIRE(buffer->get_usage() == buffer_usage::dynamic);
        REQUIRE(buffer->is_dynamic());
    }

    SECTION("create with span")
    {
        struct Vertex { float x, y, z; };
        const std::vector<Vertex> vertices = {
            {0.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f},
            {0.5f, 1.0f, 0.0f}
        };

        auto buffer = vertex_buffer::create(dev, std::span<const Vertex>{vertices}, buffer_usage::immutable);

        REQUIRE(buffer != nullptr);
        REQUIRE(buffer->get_size() == sizeof(Vertex) * 3);
        REQUIRE(buffer->get_stride() == sizeof(Vertex));
        REQUIRE(buffer->get_vertex_count() == 3);
    }

    SECTION("create empty buffer")
    {
        auto buffer = vertex_buffer::create(dev, nullptr, 1024, sizeof(float) * 3, buffer_usage::dynamic);

        REQUIRE(buffer != nullptr);
        REQUIRE(buffer->get_size() == 1024);
    }
}

TEST_CASE("vertex_buffer update", "[graphics][vertex_buffer]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("update dynamic buffer with CPU mapping")
    {
        std::array<float, 9> initial = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
        auto buffer = vertex_buffer::create(dev, initial.data(), sizeof(initial), sizeof(float) * 3, buffer_usage::dynamic);

        REQUIRE(buffer != nullptr);

        std::array<float, 9> updated = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f};
        buffer->update(updated.data(), sizeof(updated));

        // Buffer should still be valid after update
        REQUIRE(buffer->get_size() == sizeof(updated));
    }

    SECTION("update with command list")
    {
        fixture.begin_frame();

        std::array<float, 9> initial = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
        auto buffer = vertex_buffer::create(dev, initial.data(), sizeof(initial), sizeof(float) * 3, buffer_usage::dynamic);

        REQUIRE(buffer != nullptr);

        std::array<float, 9> updated = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f};
        buffer->update(updated.data(), sizeof(updated), fixture.command_list());

        fixture.end_frame();

        REQUIRE(buffer->get_size() == sizeof(updated));
    }

    SECTION("update at offset")
    {
        fixture.begin_frame();

        std::array<float, 12> initial = {
            1.0f, 1.0f, 1.0f,
            2.0f, 2.0f, 2.0f,
            3.0f, 3.0f, 3.0f,
            4.0f, 4.0f, 4.0f
        };
        auto buffer = vertex_buffer::create(dev, initial.data(), sizeof(initial), sizeof(float) * 3, buffer_usage::dynamic);

        REQUIRE(buffer != nullptr);

        // Update second vertex only
        std::array<float, 3> new_vertex = {9.0f, 9.0f, 9.0f};
        size_t offset = sizeof(float) * 3;  // Skip first vertex
        buffer->update_at_offset(new_vertex.data(), sizeof(new_vertex), offset, fixture.command_list());

        fixture.end_frame();

        REQUIRE(buffer->get_size() == sizeof(initial));
    }
}

TEST_CASE("vertex_buffer different vertex formats", "[graphics][vertex_buffer]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("position only (3 floats)")
    {
        struct PositionVertex { float x, y, z; };
        const std::vector<PositionVertex> vertices(100);

        auto buffer = vertex_buffer::create(dev, std::span<const PositionVertex>{vertices}, buffer_usage::immutable);

        REQUIRE(buffer->get_stride() == sizeof(PositionVertex));
        REQUIRE(buffer->get_vertex_count() == 100);
    }

    SECTION("position + color (7 floats)")
    {
        struct ColoredVertex {
            float x, y, z;
            float r, g, b, a;
        };
        const std::vector<ColoredVertex> vertices(50);

        auto buffer = vertex_buffer::create(dev, std::span<const ColoredVertex>{vertices}, buffer_usage::immutable);

        REQUIRE(buffer->get_stride() == sizeof(ColoredVertex));
        REQUIRE(buffer->get_vertex_count() == 50);
    }

    SECTION("position + texcoord + normal (8 floats)")
    {
        struct FullVertex {
            float px, py, pz;
            float u, v;
            float nx, ny, nz;
        };
        const std::vector<FullVertex> vertices(200);

        auto buffer = vertex_buffer::create(dev, std::span<const FullVertex>{vertices}, buffer_usage::immutable);

        REQUIRE(buffer->get_stride() == sizeof(FullVertex));
        REQUIRE(buffer->get_vertex_count() == 200);
    }
}

TEST_CASE("vertex_buffer move semantics", "[graphics][vertex_buffer]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("move constructor")
    {
        std::array<float, 9> vertices = {0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 1.0f, 0.0f};
        auto buffer1 = vertex_buffer::create(dev, vertices.data(), sizeof(vertices), sizeof(float) * 3, buffer_usage::dynamic);

        REQUIRE(buffer1 != nullptr);
        size_t original_size = buffer1->get_size();
        nvrhi::IBuffer* original_handle = buffer1->get_buffer();

        vertex_buffer buffer2 = std::move(*buffer1);

        REQUIRE(buffer2.get_size() == original_size);
        REQUIRE(buffer2.get_buffer() == original_handle);
    }
}

TEST_CASE("vertex_buffer validation", "[graphics][vertex_buffer]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("create with zero stride fails")
    {
        std::array<float, 9> vertices = {0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 1.0f, 0.0f};
        auto buffer = vertex_buffer::create(dev, vertices.data(), sizeof(vertices), 0, buffer_usage::dynamic);

        REQUIRE(buffer == nullptr);
    }

    SECTION("create with zero size fails")
    {
        auto buffer = vertex_buffer::create(dev, nullptr, 0, sizeof(float) * 3, buffer_usage::dynamic);

        REQUIRE(buffer == nullptr);
    }

    SECTION("create with custom debug name")
    {
        std::array<float, 9> vertices = {0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 1.0f, 0.0f};
        auto buffer = vertex_buffer::create(dev, vertices.data(), sizeof(vertices), sizeof(float) * 3,
            buffer_usage::dynamic, "Custom Debug Name");

        REQUIRE(buffer != nullptr);
    }
}
