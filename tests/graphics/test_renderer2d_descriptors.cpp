// Unit tests for renderer2d descriptor structures

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <lumina/graphics/renderer2d.h>
#include <glm/glm.hpp>

using namespace lumina::graphics;
using Catch::Approx;

TEST_CASE("render_layer enum", "[graphics][renderer2d]")
{
    SECTION("predefined layers have correct order")
    {
        REQUIRE(static_cast<uint32_t>(render_layer::background) < static_cast<uint32_t>(render_layer::world));
        REQUIRE(static_cast<uint32_t>(render_layer::world) < static_cast<uint32_t>(render_layer::sprites));
        REQUIRE(static_cast<uint32_t>(render_layer::sprites) < static_cast<uint32_t>(render_layer::effects));
        REQUIRE(static_cast<uint32_t>(render_layer::effects) < static_cast<uint32_t>(render_layer::ui));
        REQUIRE(static_cast<uint32_t>(render_layer::ui) < static_cast<uint32_t>(render_layer::overlay));
    }

    SECTION("make_layer creates custom layer")
    {
        auto custom = make_layer(150);
        REQUIRE(static_cast<uint32_t>(custom) == 150);

        // Custom layer between world(100) and sprites(200)
        REQUIRE(static_cast<uint32_t>(custom) > static_cast<uint32_t>(render_layer::world));
        REQUIRE(static_cast<uint32_t>(custom) < static_cast<uint32_t>(render_layer::sprites));
    }

    SECTION("default layer is world")
    {
        REQUIRE(render_layer::default_layer == render_layer::world);
    }
}

TEST_CASE("quad_desc defaults", "[graphics][renderer2d]")
{
    quad_desc desc;

    SECTION("default position")
    {
        REQUIRE(desc.position == glm::vec3(0, 0, 0));
    }

    SECTION("default size")
    {
        REQUIRE(desc.size == glm::vec2(1, 1));
    }

    SECTION("default color")
    {
        REQUIRE(desc.color == glm::vec4(1, 1, 1, 1));
    }

    SECTION("default rotation")
    {
        REQUIRE(desc.rotation == Approx(0.0f));
    }

    SECTION("default origin (center)")
    {
        REQUIRE(desc.origin == glm::vec2(0.5f, 0.5f));
    }

    SECTION("default texture is null")
    {
        REQUIRE(desc.texture == nullptr);
    }

    SECTION("default UVs cover full texture")
    {
        REQUIRE(desc.uv_min == glm::vec2(0, 0));
        REQUIRE(desc.uv_max == glm::vec2(1, 1));
    }

    SECTION("default layer")
    {
        REQUIRE(desc.layer == render_layer::default_layer);
    }

    SECTION("default z-index")
    {
        REQUIRE(desc.z == Approx(0.0f));
    }

    SECTION("default blend mode")
    {
        REQUIRE(desc.blend == blend_mode::alpha);
    }
}

TEST_CASE("circle_desc defaults", "[graphics][renderer2d]")
{
    circle_desc desc;

    SECTION("default position")
    {
        REQUIRE(desc.position == glm::vec3(0, 0, 0));
    }

    SECTION("default radius")
    {
        REQUIRE(desc.radius == glm::vec2(1, 1));
    }

    SECTION("default color")
    {
        REQUIRE(desc.color == glm::vec4(1, 1, 1, 1));
    }

    SECTION("default thickness (filled)")
    {
        REQUIRE(desc.thickness == Approx(1.0f));
    }

    SECTION("default fade")
    {
        REQUIRE(desc.fade == Approx(0.005f));
    }
}

TEST_CASE("line_desc defaults", "[graphics][renderer2d]")
{
    line_desc desc;

    SECTION("default start")
    {
        REQUIRE(desc.start == glm::vec3(0, 0, 0));
    }

    SECTION("default end")
    {
        REQUIRE(desc.end == glm::vec3(1, 0, 0));
    }

    SECTION("default color")
    {
        REQUIRE(desc.color == glm::vec4(1, 1, 1, 1));
    }

    SECTION("default thickness")
    {
        REQUIRE(desc.thickness == Approx(1.0f));
    }
}

TEST_CASE("text_desc defaults", "[graphics][renderer2d]")
{
    text_desc desc;

    SECTION("default position")
    {
        REQUIRE(desc.position == glm::vec3(0, 0, 0));
    }

    SECTION("default scale")
    {
        REQUIRE(desc.scale == Approx(1.0f));
    }

    SECTION("default color")
    {
        REQUIRE(desc.color == glm::vec4(1, 1, 1, 1));
    }

    SECTION("default alignment")
    {
        REQUIRE(desc.alignment == text_alignment::left);
    }

    SECTION("default font is null")
    {
        REQUIRE(desc.font == nullptr);
    }
}

TEST_CASE("triangle_desc defaults", "[graphics][renderer2d]")
{
    triangle_desc desc;

    SECTION("default vertices")
    {
        REQUIRE(desc.p0 == glm::vec3(0, 0, 0));
        REQUIRE(desc.p1 == glm::vec3(1, 0, 0));
        REQUIRE(desc.p2 == glm::vec3(0.5f, 1, 0));
    }

    SECTION("default color")
    {
        REQUIRE(desc.color == glm::vec4(1, 1, 1, 1));
    }

    SECTION("default UVs")
    {
        REQUIRE(desc.uv0 == glm::vec2(0, 1));
        REQUIRE(desc.uv1 == glm::vec2(1, 1));
        REQUIRE(desc.uv2 == glm::vec2(0.5f, 0));
    }
}

TEST_CASE("pixel_desc defaults", "[graphics][renderer2d]")
{
    pixel_desc desc;

    SECTION("default position")
    {
        REQUIRE(desc.position == glm::vec3(0, 0, 0));
    }

    SECTION("default color")
    {
        REQUIRE(desc.color == glm::vec4(1, 1, 1, 1));
    }

    SECTION("default size")
    {
        REQUIRE(desc.size == Approx(1.0f));
    }
}

TEST_CASE("rect_desc defaults", "[graphics][renderer2d]")
{
    rect_desc desc;

    SECTION("default position")
    {
        REQUIRE(desc.position == glm::vec3(0, 0, 0));
    }

    SECTION("default size")
    {
        REQUIRE(desc.size == glm::vec2(1, 1));
    }

    SECTION("default color")
    {
        REQUIRE(desc.color == glm::vec4(1, 1, 1, 1));
    }

    SECTION("default thickness")
    {
        REQUIRE(desc.thickness == Approx(1.0f));
    }

    SECTION("default rotation")
    {
        REQUIRE(desc.rotation == Approx(0.0f));
    }
}

TEST_CASE("grid_desc defaults", "[graphics][renderer2d]")
{
    grid_desc desc;

    SECTION("default position")
    {
        REQUIRE(desc.position == glm::vec3(0, 0, 0));
    }

    SECTION("default size")
    {
        REQUIRE(desc.size == glm::vec2(10, 10));
    }

    SECTION("default cell size")
    {
        REQUIRE(desc.cell_size == Approx(1.0f));
    }

    SECTION("default line color")
    {
        REQUIRE(desc.line_color == glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
    }

    SECTION("default line width")
    {
        REQUIRE(desc.line_width == Approx(1.0f));
    }

    SECTION("checkerboard off by default")
    {
        REQUIRE(desc.checkerboard == false);
    }
}

TEST_CASE("sprite_desc defaults", "[graphics][renderer2d]")
{
    sprite_desc desc;

    SECTION("default position")
    {
        REQUIRE(desc.position == glm::vec3(0, 0, 0));
    }

    SECTION("default size (auto)")
    {
        REQUIRE(desc.size == glm::vec2(0, 0));
    }

    SECTION("default color")
    {
        REQUIRE(desc.color == glm::vec4(1, 1, 1, 1));
    }

    SECTION("default rotation")
    {
        REQUIRE(desc.rotation == Approx(0.0f));
    }

    SECTION("default origin (center)")
    {
        REQUIRE(desc.origin == glm::vec2(0.5f, 0.5f));
    }

    SECTION("flip flags off by default")
    {
        REQUIRE(desc.flip_x == false);
        REQUIRE(desc.flip_y == false);
    }
}

TEST_CASE("point_light_desc defaults", "[graphics][renderer2d]")
{
    point_light_desc desc;

    SECTION("default position")
    {
        REQUIRE(desc.position == glm::vec3(0, 0, 0));
    }

    SECTION("default color")
    {
        REQUIRE(desc.color == glm::vec3(1, 1, 1));
    }

    SECTION("default intensity")
    {
        REQUIRE(desc.intensity == Approx(1.0f));
    }

    SECTION("default radius")
    {
        REQUIRE(desc.radius == Approx(10.0f));
    }

    SECTION("default blend mode")
    {
        REQUIRE(desc.blend == light_blend_mode::additive);
    }

    SECTION("default attenuation")
    {
        REQUIRE(desc.attenuation == attenuation_model::quadratic);
    }

    SECTION("default layer is effects")
    {
        REQUIRE(desc.layer == render_layer::effects);
    }
}

TEST_CASE("renderer2d_stats", "[graphics][renderer2d]")
{
    renderer2d_stats stats;

    SECTION("default values are zero")
    {
        REQUIRE(stats.draw_calls == 0);
        REQUIRE(stats.quad_count == 0);
        REQUIRE(stats.circle_count == 0);
        REQUIRE(stats.line_count == 0);
        REQUIRE(stats.text_char_count == 0);
        REQUIRE(stats.triangle_count == 0);
        REQUIRE(stats.pixel_count == 0);
        REQUIRE(stats.grid_count == 0);
        REQUIRE(stats.point_light_count == 0);
        REQUIRE(stats.texture_binds == 0);
    }

    SECTION("get_total_primitives")
    {
        stats.quad_count = 100;
        stats.circle_count = 50;
        stats.line_count = 25;
        stats.triangle_count = 10;
        stats.pixel_count = 5;
        stats.grid_count = 2;

        REQUIRE(stats.get_total_primitives() == 192);
    }

    SECTION("reset clears all values")
    {
        stats.draw_calls = 10;
        stats.quad_count = 100;
        stats.circle_count = 50;
        stats.texture_binds = 5;

        stats.reset();

        REQUIRE(stats.draw_calls == 0);
        REQUIRE(stats.quad_count == 0);
        REQUIRE(stats.circle_count == 0);
        REQUIRE(stats.texture_binds == 0);
    }
}

TEST_CASE("renderer2d_config defaults", "[graphics][renderer2d]")
{
    renderer2d_config config;

    SECTION("default max values")
    {
        REQUIRE(config.max_quads == 10000);
        REQUIRE(config.max_circles == 10000);
        REQUIRE(config.max_lines == 10000);
        REQUIRE(config.max_triangles == 10000);
        REQUIRE(config.max_text_chars == 10000);
        REQUIRE(config.max_pixels == 50000);
        REQUIRE(config.max_grids == 1000);
        REQUIRE(config.max_point_lights == 32);
        REQUIRE(config.max_textures == 32);
    }
}
