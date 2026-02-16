// Unit tests for pipeline

#include <catch2/catch_test_macros.hpp>

#include "graphics_test_fixture.h"
#include <lumina/core/base.h>
#include <lumina/graphics/pipeline.h>
#include <lumina/graphics/shader.h>
#include <lumina/graphics/input_layout.h>
#include <lumina/graphics/binding_layout.h>

// Include compiled shader bytecode for testing
#include <lumina/graphics/shaders/quad_vs_dxil.h>
#include <lumina/graphics/shaders/quad_ps_dxil.h>
#include <lumina/graphics/shaders/quad_vs_spirv.h>
#include <lumina/graphics/shaders/quad_ps_spirv.h>

using namespace lumina;
using namespace lumina::graphics;

// Helper to create test shader
static ref<shader> create_test_shader(core::device& dev)
{
    shader_desc desc;
    if (dev.get_api() == core::graphics_api::d3d12)
    {
        desc.vertex_blob = g_quad_vs_dxil;
        desc.vertex_size = sizeof(g_quad_vs_dxil);
        desc.pixel_blob = g_quad_ps_dxil;
        desc.pixel_size = sizeof(g_quad_ps_dxil);
    }
    else
    {
        desc.vertex_blob = g_quad_vs_spirv;
        desc.vertex_size = sizeof(g_quad_vs_spirv);
        desc.pixel_blob = g_quad_ps_spirv;
        desc.pixel_size = sizeof(g_quad_ps_spirv);
    }
    desc.vertex_entry = "VSMain";
    desc.pixel_entry = "PSMain";
    return shader::create(dev, desc);
}

// Helper to create test input layout matching quad shader
static ref<input_layout> create_test_input_layout(core::device& dev, ref<shader> shdr)
{
    input_layout_desc desc;
    desc.add("POSITION", format::rgba32_float, vertex_semantic::position)
        .add("COLOR", format::rgba32_float, vertex_semantic::color)
        .add("TEXCOORD", format::rg32_float, vertex_semantic::texcoord)
        .add("TEXINDEX", format::r32_float, vertex_semantic::custom)
        .add("ZINDEX", format::r32_float, vertex_semantic::custom);
    return input_layout::create(dev, desc, shdr);
}

TEST_CASE("pipeline_desc defaults", "[graphics][pipeline]")
{
    pipeline_desc desc;

    SECTION("default shader is null")
    {
        REQUIRE(desc.shader_program == nullptr);
    }

    SECTION("default layout is null")
    {
        REQUIRE(desc.vertex_layout == nullptr);
    }

    SECTION("default binding layouts empty")
    {
        REQUIRE(desc.binding_layouts.empty());
    }

    SECTION("default color format")
    {
        REQUIRE(desc.color_format == format::rgba8_unorm);
    }

    SECTION("default depth format")
    {
        REQUIRE(desc.depth_format == format::unknown);
    }
}

TEST_CASE("render_state defaults", "[graphics][pipeline]")
{
    render_state state;

    SECTION("default blend mode")
    {
        REQUIRE(state.blend == blend_mode::alpha);
    }

    SECTION("default depth mode")
    {
        REQUIRE(state.depth == depth_mode::none);
    }

    SECTION("default cull mode")
    {
        REQUIRE(state.cull == cull_mode::none);
    }

    SECTION("default topology")
    {
        REQUIRE(state.primitive == topology::triangles);
    }
}

TEST_CASE("pipeline creation", "[graphics][pipeline]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    auto shdr = create_test_shader(dev);
    REQUIRE(shdr != nullptr);

    auto layout = create_test_input_layout(dev, shdr);
    REQUIRE(layout != nullptr);

    // Create binding layout matching quad shader
    binding_layout_desc binding_desc;
    binding_desc.add_constant_buffer(0)
                .add_texture_array(0, 32)
                .add_sampler(0);
    auto bind_layout = binding_layout::create(dev, binding_desc);
    REQUIRE(bind_layout != nullptr);

    SECTION("create basic pipeline")
    {
        pipeline_desc desc;
        desc.shader_program = shdr;
        desc.vertex_layout = layout;
        desc.binding_layouts.push_back(bind_layout);
        desc.color_format = format::rgba8_unorm;

        auto pipe = pipeline::create(dev, desc);

        REQUIRE(pipe != nullptr);
        REQUIRE(pipe->get_pipeline() != nullptr);
    }

    SECTION("create with depth")
    {
        pipeline_desc desc;
        desc.shader_program = shdr;
        desc.vertex_layout = layout;
        desc.binding_layouts.push_back(bind_layout);
        desc.color_format = format::rgba8_unorm;
        desc.depth_format = format::d32_float;
        desc.state.depth = depth_mode::read_write;

        auto pipe = pipeline::create(dev, desc);

        REQUIRE(pipe != nullptr);
        REQUIRE(pipe->get_desc().depth_format == format::d32_float);
    }

    SECTION("create with alpha blend")
    {
        pipeline_desc desc;
        desc.shader_program = shdr;
        desc.vertex_layout = layout;
        desc.binding_layouts.push_back(bind_layout);
        desc.state.blend = blend_mode::alpha;

        auto pipe = pipeline::create(dev, desc);

        REQUIRE(pipe != nullptr);
        REQUIRE(pipe->get_desc().state.blend == blend_mode::alpha);
    }

    SECTION("create with additive blend")
    {
        pipeline_desc desc;
        desc.shader_program = shdr;
        desc.vertex_layout = layout;
        desc.binding_layouts.push_back(bind_layout);
        desc.state.blend = blend_mode::additive;

        auto pipe = pipeline::create(dev, desc);

        REQUIRE(pipe != nullptr);
    }

    SECTION("create with back-face culling")
    {
        pipeline_desc desc;
        desc.shader_program = shdr;
        desc.vertex_layout = layout;
        desc.binding_layouts.push_back(bind_layout);
        desc.state.cull = cull_mode::back;

        auto pipe = pipeline::create(dev, desc);

        REQUIRE(pipe != nullptr);
        REQUIRE(pipe->get_desc().state.cull == cull_mode::back);
    }
}

TEST_CASE("pipeline_cache", "[graphics][pipeline]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    auto shdr = create_test_shader(dev);
    REQUIRE(shdr != nullptr);

    auto layout = create_test_input_layout(dev, shdr);
    REQUIRE(layout != nullptr);

    binding_layout_desc binding_desc;
    binding_desc.add_constant_buffer(0)
                .add_texture_array(0, 32)
                .add_sampler(0);
    auto bind_layout = binding_layout::create(dev, binding_desc);
    REQUIRE(bind_layout != nullptr);

    pipeline_cache cache(dev);

    SECTION("initial count is zero")
    {
        REQUIRE(cache.get_pipeline_count() == 0);
    }

    SECTION("get_or_create creates new pipeline")
    {
        pipeline_desc desc;
        desc.shader_program = shdr;
        desc.vertex_layout = layout;
        desc.binding_layouts.push_back(bind_layout);

        auto pipe = cache.get_or_create(desc);

        REQUIRE(pipe != nullptr);
        REQUIRE(cache.get_pipeline_count() == 1);
    }

    SECTION("same desc returns cached pipeline")
    {
        pipeline_desc desc;
        desc.shader_program = shdr;
        desc.vertex_layout = layout;
        desc.binding_layouts.push_back(bind_layout);

        auto pipe1 = cache.get_or_create(desc);
        auto pipe2 = cache.get_or_create(desc);

        REQUIRE(pipe1 == pipe2);  // Same pointer
        REQUIRE(cache.get_pipeline_count() == 1);
    }

    SECTION("different blend mode creates new pipeline")
    {
        pipeline_desc desc1;
        desc1.shader_program = shdr;
        desc1.vertex_layout = layout;
        desc1.binding_layouts.push_back(bind_layout);
        desc1.state.blend = blend_mode::opaque;

        pipeline_desc desc2;
        desc2.shader_program = shdr;
        desc2.vertex_layout = layout;
        desc2.binding_layouts.push_back(bind_layout);
        desc2.state.blend = blend_mode::alpha;

        auto pipe1 = cache.get_or_create(desc1);
        auto pipe2 = cache.get_or_create(desc2);

        REQUIRE(pipe1 != pipe2);
        REQUIRE(cache.get_pipeline_count() == 2);
    }

    SECTION("clear removes all pipelines")
    {
        pipeline_desc desc;
        desc.shader_program = shdr;
        desc.vertex_layout = layout;
        desc.binding_layouts.push_back(bind_layout);

        auto pso = cache.get_or_create(desc);
        REQUIRE(pso != nullptr);
        REQUIRE(cache.get_pipeline_count() == 1);

        cache.clear();
        REQUIRE(cache.get_pipeline_count() == 0);
    }
}

TEST_CASE("pipeline_desc hash", "[graphics][pipeline]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    auto shdr = create_test_shader(dev);
    REQUIRE(shdr != nullptr);

    auto layout = create_test_input_layout(dev, shdr);
    REQUIRE(layout != nullptr);

    binding_layout_desc binding_desc;
    binding_desc.add_constant_buffer(0);
    auto bind_layout = binding_layout::create(dev, binding_desc);

    SECTION("identical descs have same hash")
    {
        pipeline_desc desc1;
        desc1.shader_program = shdr;
        desc1.vertex_layout = layout;
        desc1.binding_layouts.push_back(bind_layout);
        desc1.state.blend = blend_mode::alpha;

        pipeline_desc desc2;
        desc2.shader_program = shdr;
        desc2.vertex_layout = layout;
        desc2.binding_layouts.push_back(bind_layout);
        desc2.state.blend = blend_mode::alpha;

        REQUIRE(desc1.hash() == desc2.hash());
    }

    SECTION("different blend mode produces different hash")
    {
        pipeline_desc desc1;
        desc1.shader_program = shdr;
        desc1.vertex_layout = layout;
        desc1.state.blend = blend_mode::opaque;

        pipeline_desc desc2;
        desc2.shader_program = shdr;
        desc2.vertex_layout = layout;
        desc2.state.blend = blend_mode::alpha;

        REQUIRE(desc1.hash() != desc2.hash());
    }
}
