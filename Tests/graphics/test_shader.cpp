// Unit tests for shader

#include <catch2/catch_test_macros.hpp>

#include "graphics_test_fixture.h"
#include <Lumina/Graphics/shader.h>

// Include compiled shader bytecode for testing
#include <Lumina/Graphics/shaders/quad_vs_dxil.h>
#include <Lumina/Graphics/shaders/quad_ps_dxil.h>
#include <Lumina/Graphics/shaders/quad_vs_spirv.h>
#include <Lumina/Graphics/shaders/quad_ps_spirv.h>

using namespace lumina::graphics;

TEST_CASE("shader_desc defaults", "[graphics][shader]")
{
    shader_desc desc;

    SECTION("default vertex blob is null")
    {
        REQUIRE(desc.vertex_blob == nullptr);
        REQUIRE(desc.vertex_size == 0);
    }

    SECTION("default pixel blob is null")
    {
        REQUIRE(desc.pixel_blob == nullptr);
        REQUIRE(desc.pixel_size == 0);
    }

    SECTION("default entry points")
    {
        REQUIRE(desc.vertex_entry == "main");
        REQUIRE(desc.pixel_entry == "main");
    }

    SECTION("default debug name")
    {
        REQUIRE(desc.debug_name == "Lumina Shader");
    }
}

TEST_CASE("shader creation", "[graphics][shader]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("create with shader_desc")
    {
        shader_desc desc;

        // Use appropriate bytecode based on graphics API
        if (dev.get_api() == lumina::core::graphics_api::d3d12)
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
        desc.debug_name = "Test Quad Shader";

        auto shdr = shader::create(dev, desc);

        REQUIRE(shdr != nullptr);
        REQUIRE(shdr->is_valid());
        REQUIRE(shdr->get_vertex_shader() != nullptr);
        REQUIRE(shdr->get_pixel_shader() != nullptr);
    }

    SECTION("create with raw pointers")
    {
        const void* vs_blob;
        size_t vs_size;
        const void* ps_blob;
        size_t ps_size;

        if (dev.get_api() == lumina::core::graphics_api::d3d12)
        {
            vs_blob = g_quad_vs_dxil;
            vs_size = sizeof(g_quad_vs_dxil);
            ps_blob = g_quad_ps_dxil;
            ps_size = sizeof(g_quad_ps_dxil);
        }
        else
        {
            vs_blob = g_quad_vs_spirv;
            vs_size = sizeof(g_quad_vs_spirv);
            ps_blob = g_quad_ps_spirv;
            ps_size = sizeof(g_quad_ps_spirv);
        }

        auto shdr = shader::create(dev, vs_blob, vs_size, ps_blob, ps_size);

        REQUIRE(shdr != nullptr);
        REQUIRE(shdr->is_valid());
    }
}

TEST_CASE("shader invalid creation", "[graphics][shader]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("null vertex shader")
    {
        shader_desc desc;
        desc.vertex_blob = nullptr;
        desc.vertex_size = 0;

        if (dev.get_api() == lumina::core::graphics_api::d3d12)
        {
            desc.pixel_blob = g_quad_ps_dxil;
            desc.pixel_size = sizeof(g_quad_ps_dxil);
        }
        else
        {
            desc.pixel_blob = g_quad_ps_spirv;
            desc.pixel_size = sizeof(g_quad_ps_spirv);
        }

        auto shdr = shader::create(dev, desc);

        // Should fail or return invalid shader
        if (shdr)
        {
            REQUIRE_FALSE(shdr->is_valid());
        }
    }

    SECTION("null pixel shader")
    {
        shader_desc desc;

        if (dev.get_api() == lumina::core::graphics_api::d3d12)
        {
            desc.vertex_blob = g_quad_vs_dxil;
            desc.vertex_size = sizeof(g_quad_vs_dxil);
        }
        else
        {
            desc.vertex_blob = g_quad_vs_spirv;
            desc.vertex_size = sizeof(g_quad_vs_spirv);
        }

        desc.pixel_blob = nullptr;
        desc.pixel_size = 0;

        auto shdr = shader::create(dev, desc);

        // Should fail or return invalid shader
        if (shdr)
        {
            REQUIRE_FALSE(shdr->is_valid());
        }
    }
}

TEST_CASE("shader bytecode verification", "[graphics][shader]")
{
    SECTION("DXIL bytecode exists")
    {
        REQUIRE(sizeof(g_quad_vs_dxil) > 0);
        REQUIRE(sizeof(g_quad_ps_dxil) > 0);
    }

    SECTION("SPIRV bytecode exists")
    {
        REQUIRE(sizeof(g_quad_vs_spirv) > 0);
        REQUIRE(sizeof(g_quad_ps_spirv) > 0);
    }
}
