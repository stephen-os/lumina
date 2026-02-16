// Unit tests for input_layout

#include <catch2/catch_test_macros.hpp>

#include "graphics_test_fixture.h"
#include <lumina/graphics/input_layout.h>
#include <lumina/graphics/shader.h>

// Include compiled shader bytecode for testing
#include <lumina/graphics/shaders/quad_vs_dxil.h>
#include <lumina/graphics/shaders/quad_ps_dxil.h>
#include <lumina/graphics/shaders/quad_vs_spirv.h>
#include <lumina/graphics/shaders/quad_ps_spirv.h>

using namespace lumina::graphics;

TEST_CASE("input_layout_desc defaults", "[graphics][input_layout]")
{
    input_layout_desc desc;

    SECTION("default attributes empty")
    {
        REQUIRE(desc.attributes.empty());
    }

    SECTION("default stride is zero")
    {
        REQUIRE(desc.stride == 0);
    }
}

TEST_CASE("input_layout_desc builder", "[graphics][input_layout]")
{
    SECTION("add single attribute")
    {
        input_layout_desc desc;
        desc.add("POSITION", format::rgba32_float, vertex_semantic::position);

        REQUIRE(desc.attributes.size() == 1);
        REQUIRE(desc.attributes[0].name == "POSITION");
        REQUIRE(desc.attributes[0].attr_format == format::rgba32_float);
        REQUIRE(desc.attributes[0].semantic == vertex_semantic::position);
        REQUIRE(desc.stride == 16);  // 4 floats * 4 bytes
    }

    SECTION("add multiple attributes")
    {
        input_layout_desc desc;
        desc.add("POSITION", format::rgba32_float, vertex_semantic::position)
            .add("COLOR", format::rgba32_float, vertex_semantic::color)
            .add("TEXCOORD", format::rg32_float, vertex_semantic::texcoord);

        REQUIRE(desc.attributes.size() == 3);
        REQUIRE(desc.stride == 40);  // (4 + 4 + 2) * 4 bytes
    }

    SECTION("helper methods")
    {
        input_layout_desc desc;
        desc.add_position()
            .add_color()
            .add_texcoord();

        REQUIRE(desc.attributes.size() == 3);
        REQUIRE(desc.attributes[0].name == "POSITION");
        REQUIRE(desc.attributes[1].name == "COLOR");
        REQUIRE(desc.attributes[2].name == "TEXCOORD");
    }

    SECTION("offsets are calculated correctly")
    {
        input_layout_desc desc;
        desc.add("POSITION", format::rgba32_float, vertex_semantic::position)
            .add("COLOR", format::rgba32_float, vertex_semantic::color)
            .add("TEXCOORD", format::rg32_float, vertex_semantic::texcoord);

        REQUIRE(desc.attributes[0].offset == 0);
        REQUIRE(desc.attributes[1].offset == 16);
        REQUIRE(desc.attributes[2].offset == 32);
    }
}

TEST_CASE("predefined vertex layouts", "[graphics][input_layout]")
{
    SECTION("position only")
    {
        auto desc = vertex_layouts::position();

        REQUIRE(desc.attributes.size() == 1);
        REQUIRE(desc.attributes[0].name == "POSITION");
        REQUIRE(desc.stride == 16);
    }

    SECTION("position_color")
    {
        auto desc = vertex_layouts::position_color();

        REQUIRE(desc.attributes.size() == 2);
        REQUIRE(desc.stride == 32);
    }

    SECTION("position_texcoord")
    {
        auto desc = vertex_layouts::position_texcoord();

        REQUIRE(desc.attributes.size() == 2);
        REQUIRE(desc.stride == 24);
    }

    SECTION("position_color_texcoord")
    {
        auto desc = vertex_layouts::position_color_texcoord();

        REQUIRE(desc.attributes.size() == 3);
        REQUIRE(desc.stride == 40);
    }

    SECTION("position_normal_texcoord")
    {
        auto desc = vertex_layouts::position_normal_texcoord();

        REQUIRE(desc.attributes.size() == 3);
        REQUIRE(desc.stride == 40);
    }
}

TEST_CASE("input_layout creation", "[graphics][input_layout]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    // Create shader first (needed for input layout validation)
    shader_desc shdr_desc;
    if (dev.get_api() == lumina::core::graphics_api::d3d12)
    {
        shdr_desc.vertex_blob = g_quad_vs_dxil;
        shdr_desc.vertex_size = sizeof(g_quad_vs_dxil);
        shdr_desc.pixel_blob = g_quad_ps_dxil;
        shdr_desc.pixel_size = sizeof(g_quad_ps_dxil);
    }
    else
    {
        shdr_desc.vertex_blob = g_quad_vs_spirv;
        shdr_desc.vertex_size = sizeof(g_quad_vs_spirv);
        shdr_desc.pixel_blob = g_quad_ps_spirv;
        shdr_desc.pixel_size = sizeof(g_quad_ps_spirv);
    }
    shdr_desc.vertex_entry = "VSMain";
    shdr_desc.pixel_entry = "PSMain";

    auto shdr = shader::create(dev, shdr_desc);
    REQUIRE(shdr != nullptr);

    SECTION("create with quad layout")
    {
        // Match the quad shader's expected input
        input_layout_desc layout_desc;
        layout_desc.add("POSITION", format::rgba32_float, vertex_semantic::position)
                   .add("COLOR", format::rgba32_float, vertex_semantic::color)
                   .add("TEXCOORD", format::rg32_float, vertex_semantic::texcoord)
                   .add("TEXINDEX", format::r32_float, vertex_semantic::custom)
                   .add("ZINDEX", format::r32_float, vertex_semantic::custom);

        auto layout = input_layout::create(dev, layout_desc, shdr);

        REQUIRE(layout != nullptr);
        REQUIRE(layout->get_layout() != nullptr);
        REQUIRE(layout->get_stride() == layout_desc.stride);
        REQUIRE(layout->get_attribute_count() == 5);
    }
}

TEST_CASE("vertex_attribute construction", "[graphics][input_layout]")
{
    SECTION("default construction")
    {
        vertex_attribute attr;

        REQUIRE(attr.name.empty());
        REQUIRE(attr.offset == 0);
        REQUIRE(attr.semantic == vertex_semantic::custom);
    }

    SECTION("parameterized construction")
    {
        vertex_attribute attr("POSITION", format::rgba32_float, 0, vertex_semantic::position);

        REQUIRE(attr.name == "POSITION");
        REQUIRE(attr.attr_format == format::rgba32_float);
        REQUIRE(attr.offset == 0);
        REQUIRE(attr.semantic == vertex_semantic::position);
    }
}

// =============================================================================
// RIGOROUS MATHEMATICAL VERIFICATION TESTS
// =============================================================================

TEST_CASE("format_bytes_per_pixel - mathematical verification", "[graphics][input_layout][math]")
{
    // Verify that format_bytes_per_pixel returns the mathematically correct
    // byte count based on the format's component count and bit depth

    SECTION("8-bit single channel formats = 1 byte")
    {
        // 1 component * 8 bits / 8 bits per byte = 1 byte
        REQUIRE(format_bytes_per_pixel(format::r8_unorm) == 1);
        REQUIRE(format_bytes_per_pixel(format::r8_snorm) == 1);
        REQUIRE(format_bytes_per_pixel(format::r8_uint) == 1);
        REQUIRE(format_bytes_per_pixel(format::r8_sint) == 1);
    }

    SECTION("16-bit single channel formats = 2 bytes")
    {
        // 1 component * 16 bits / 8 bits per byte = 2 bytes
        REQUIRE(format_bytes_per_pixel(format::r16_float) == 2);
        REQUIRE(format_bytes_per_pixel(format::r16_unorm) == 2);
        REQUIRE(format_bytes_per_pixel(format::r16_uint) == 2);
        REQUIRE(format_bytes_per_pixel(format::r16_sint) == 2);
    }

    SECTION("8-bit dual channel formats = 2 bytes")
    {
        // 2 components * 8 bits / 8 bits per byte = 2 bytes
        REQUIRE(format_bytes_per_pixel(format::rg8_unorm) == 2);
        REQUIRE(format_bytes_per_pixel(format::rg8_snorm) == 2);
    }

    SECTION("32-bit single channel formats = 4 bytes")
    {
        // 1 component * 32 bits / 8 bits per byte = 4 bytes
        REQUIRE(format_bytes_per_pixel(format::r32_float) == 4);
        REQUIRE(format_bytes_per_pixel(format::r32_uint) == 4);
        REQUIRE(format_bytes_per_pixel(format::r32_sint) == 4);
    }

    SECTION("16-bit dual channel formats = 4 bytes")
    {
        // 2 components * 16 bits / 8 bits per byte = 4 bytes
        REQUIRE(format_bytes_per_pixel(format::rg16_float) == 4);
        REQUIRE(format_bytes_per_pixel(format::rg16_unorm) == 4);
    }

    SECTION("8-bit quad channel formats = 4 bytes")
    {
        // 4 components * 8 bits / 8 bits per byte = 4 bytes
        REQUIRE(format_bytes_per_pixel(format::rgba8_unorm) == 4);
        REQUIRE(format_bytes_per_pixel(format::rgba8_unorm_srgb) == 4);
        REQUIRE(format_bytes_per_pixel(format::rgba8_snorm) == 4);
        REQUIRE(format_bytes_per_pixel(format::bgra8_unorm) == 4);
        REQUIRE(format_bytes_per_pixel(format::bgra8_unorm_srgb) == 4);
    }

    SECTION("32-bit dual channel formats = 8 bytes")
    {
        // 2 components * 32 bits / 8 bits per byte = 8 bytes
        REQUIRE(format_bytes_per_pixel(format::rg32_float) == 8);
    }

    SECTION("16-bit quad channel formats = 8 bytes")
    {
        // 4 components * 16 bits / 8 bits per byte = 8 bytes
        REQUIRE(format_bytes_per_pixel(format::rgba16_float) == 8);
        REQUIRE(format_bytes_per_pixel(format::rgba16_unorm) == 8);
    }

    SECTION("32-bit quad channel formats = 16 bytes")
    {
        // 4 components * 32 bits / 8 bits per byte = 16 bytes
        REQUIRE(format_bytes_per_pixel(format::rgba32_float) == 16);
    }

    SECTION("depth formats")
    {
        // D16 = 16 bits = 2 bytes
        REQUIRE(format_bytes_per_pixel(format::d16_unorm) == 2);

        // D24S8 = 24 bits depth + 8 bits stencil = 32 bits = 4 bytes
        REQUIRE(format_bytes_per_pixel(format::d24_unorm_s8_uint) == 4);

        // D32 = 32 bits = 4 bytes
        REQUIRE(format_bytes_per_pixel(format::d32_float) == 4);

        // D32S8 = 32 bits depth + 8 bits stencil (padded to 64) = 8 bytes
        REQUIRE(format_bytes_per_pixel(format::d32_float_s8_uint) == 8);
    }
}

TEST_CASE("input_layout stride calculation - mathematical verification", "[graphics][input_layout][math]")
{
    // Stride = sum of all attribute sizes
    // Each attribute size = format_bytes_per_pixel(attribute.format)

    SECTION("single attribute stride equals format size")
    {
        input_layout_desc desc;
        desc.add("POSITION", format::rgba32_float, vertex_semantic::position);

        // rgba32_float = 16 bytes
        REQUIRE(desc.stride == 16);
        REQUIRE(desc.stride == format_bytes_per_pixel(format::rgba32_float));
    }

    SECTION("position + color stride")
    {
        input_layout_desc desc;
        desc.add("POSITION", format::rgba32_float, vertex_semantic::position)  // 16 bytes
            .add("COLOR", format::rgba32_float, vertex_semantic::color);       // 16 bytes

        // Total = 16 + 16 = 32 bytes
        REQUIRE(desc.stride == 32);
        REQUIRE(desc.stride ==
            format_bytes_per_pixel(format::rgba32_float) +
            format_bytes_per_pixel(format::rgba32_float));
    }

    SECTION("position + texcoord stride")
    {
        input_layout_desc desc;
        desc.add("POSITION", format::rgba32_float, vertex_semantic::position)  // 16 bytes
            .add("TEXCOORD", format::rg32_float, vertex_semantic::texcoord);   // 8 bytes

        // Total = 16 + 8 = 24 bytes
        REQUIRE(desc.stride == 24);
        REQUIRE(desc.stride ==
            format_bytes_per_pixel(format::rgba32_float) +
            format_bytes_per_pixel(format::rg32_float));
    }

    SECTION("full vertex with 5 attributes")
    {
        input_layout_desc desc;
        desc.add("POSITION", format::rgba32_float, vertex_semantic::position)  // 16 bytes
            .add("COLOR", format::rgba32_float, vertex_semantic::color)        // 16 bytes
            .add("TEXCOORD", format::rg32_float, vertex_semantic::texcoord)    // 8 bytes
            .add("TEXINDEX", format::r32_float, vertex_semantic::custom)       // 4 bytes
            .add("ZINDEX", format::r32_float, vertex_semantic::custom);        // 4 bytes

        // Total = 16 + 16 + 8 + 4 + 4 = 48 bytes
        size_t expected_stride =
            format_bytes_per_pixel(format::rgba32_float) +  // POSITION
            format_bytes_per_pixel(format::rgba32_float) +  // COLOR
            format_bytes_per_pixel(format::rg32_float) +    // TEXCOORD
            format_bytes_per_pixel(format::r32_float) +     // TEXINDEX
            format_bytes_per_pixel(format::r32_float);      // ZINDEX

        REQUIRE(desc.stride == expected_stride);
        REQUIRE(desc.stride == 48);
    }

    SECTION("compact vertex with small formats")
    {
        input_layout_desc desc;
        desc.add("POSITION", format::rgba16_float, vertex_semantic::position)  // 8 bytes
            .add("COLOR", format::rgba8_unorm, vertex_semantic::color)         // 4 bytes
            .add("TEXCOORD", format::rg16_float, vertex_semantic::texcoord);   // 4 bytes

        // Total = 8 + 4 + 4 = 16 bytes
        size_t expected_stride =
            format_bytes_per_pixel(format::rgba16_float) +
            format_bytes_per_pixel(format::rgba8_unorm) +
            format_bytes_per_pixel(format::rg16_float);

        REQUIRE(desc.stride == expected_stride);
        REQUIRE(desc.stride == 16);
    }
}

TEST_CASE("input_layout offset calculation - mathematical verification", "[graphics][input_layout][math]")
{
    // Offset[n] = sum of sizes of attributes [0..n-1]
    // offset[0] = 0
    // offset[1] = size[0]
    // offset[2] = size[0] + size[1]
    // etc.

    SECTION("first attribute has offset 0")
    {
        input_layout_desc desc;
        desc.add("POSITION", format::rgba32_float, vertex_semantic::position);

        REQUIRE(desc.attributes[0].offset == 0);
    }

    SECTION("sequential offsets accumulate correctly")
    {
        input_layout_desc desc;
        desc.add("POSITION", format::rgba32_float, vertex_semantic::position)  // 16 bytes, offset 0
            .add("COLOR", format::rgba32_float, vertex_semantic::color)        // 16 bytes, offset 16
            .add("TEXCOORD", format::rg32_float, vertex_semantic::texcoord)    // 8 bytes, offset 32
            .add("NORMAL", format::rgba32_float, vertex_semantic::normal);     // 16 bytes, offset 40

        // Verify each offset
        REQUIRE(desc.attributes[0].offset == 0);   // POSITION at 0
        REQUIRE(desc.attributes[1].offset == 16);  // COLOR at 0 + 16
        REQUIRE(desc.attributes[2].offset == 32);  // TEXCOORD at 0 + 16 + 16
        REQUIRE(desc.attributes[3].offset == 40);  // NORMAL at 0 + 16 + 16 + 8

        // Verify the running sum formula
        size_t running_offset = 0;
        REQUIRE(desc.attributes[0].offset == running_offset);

        running_offset += format_bytes_per_pixel(format::rgba32_float);
        REQUIRE(desc.attributes[1].offset == running_offset);

        running_offset += format_bytes_per_pixel(format::rgba32_float);
        REQUIRE(desc.attributes[2].offset == running_offset);

        running_offset += format_bytes_per_pixel(format::rg32_float);
        REQUIRE(desc.attributes[3].offset == running_offset);
    }

    SECTION("mixed format sizes")
    {
        input_layout_desc desc;
        desc.add("A", format::r32_float, vertex_semantic::custom)      // 4 bytes, offset 0
            .add("B", format::rg32_float, vertex_semantic::custom)     // 8 bytes, offset 4
            .add("C", format::rgba32_float, vertex_semantic::custom)   // 16 bytes, offset 12
            .add("D", format::r32_float, vertex_semantic::custom);     // 4 bytes, offset 28

        REQUIRE(desc.attributes[0].offset == 0);
        REQUIRE(desc.attributes[1].offset == 4);
        REQUIRE(desc.attributes[2].offset == 12);
        REQUIRE(desc.attributes[3].offset == 28);

        // Final stride = 4 + 8 + 16 + 4 = 32
        REQUIRE(desc.stride == 32);
    }

    SECTION("last offset + last size = stride")
    {
        input_layout_desc desc;
        desc.add("POSITION", format::rgba32_float, vertex_semantic::position)
            .add("COLOR", format::rgba8_unorm, vertex_semantic::color)
            .add("TEXCOORD", format::rg16_float, vertex_semantic::texcoord);

        // Verify: last_offset + last_size == stride
        const auto& last_attr = desc.attributes.back();
        size_t last_size = format_bytes_per_pixel(last_attr.attr_format);
        REQUIRE(last_attr.offset + last_size == desc.stride);
    }
}

TEST_CASE("vertex data alignment - mathematical verification", "[graphics][input_layout][math]")
{
    // Given vertex count and stride, verify memory calculations

    SECTION("total buffer size = vertex_count * stride")
    {
        input_layout_desc desc;
        desc.add("POSITION", format::rgba32_float, vertex_semantic::position)
            .add("COLOR", format::rgba32_float, vertex_semantic::color);

        // stride = 32 bytes
        REQUIRE(desc.stride == 32);

        // For 100 vertices: 100 * 32 = 3200 bytes
        size_t vertex_count = 100;
        size_t expected_size = vertex_count * desc.stride;
        REQUIRE(expected_size == 3200);

        // For 1000 vertices: 1000 * 32 = 32000 bytes
        vertex_count = 1000;
        expected_size = vertex_count * desc.stride;
        REQUIRE(expected_size == 32000);
    }

    SECTION("attribute access offset = vertex_index * stride + attribute_offset")
    {
        input_layout_desc desc;
        desc.add("POSITION", format::rgba32_float, vertex_semantic::position)  // offset 0
            .add("COLOR", format::rgba32_float, vertex_semantic::color)        // offset 16
            .add("TEXCOORD", format::rg32_float, vertex_semantic::texcoord);   // offset 32

        // stride = 40 bytes
        REQUIRE(desc.stride == 40);

        // Access vertex 5's COLOR attribute
        size_t vertex_index = 5;
        size_t color_offset = desc.attributes[1].offset;
        size_t byte_offset = vertex_index * desc.stride + color_offset;

        // Expected: 5 * 40 + 16 = 200 + 16 = 216
        REQUIRE(byte_offset == 216);

        // Access vertex 10's TEXCOORD attribute
        vertex_index = 10;
        size_t texcoord_offset = desc.attributes[2].offset;
        byte_offset = vertex_index * desc.stride + texcoord_offset;

        // Expected: 10 * 40 + 32 = 400 + 32 = 432
        REQUIRE(byte_offset == 432);
    }
}
