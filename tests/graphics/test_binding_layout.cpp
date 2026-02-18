// Unit tests for binding_layout

#include <catch2/catch_test_macros.hpp>

#include "graphics_test_fixture.h"
#include <lumina/graphics/binding_layout.h>

using namespace lumina::graphics;

TEST_CASE("binding_item creation", "[graphics][binding_layout]")
{
    SECTION("default construction")
    {
        binding_item item;

        REQUIRE(item.slot == 0);
        REQUIRE(item.type == binding_type::texture);
        REQUIRE(item.array_size == 1);
    }

    SECTION("texture helper")
    {
        auto item = binding_item::texture(0);

        REQUIRE(item.slot == 0);
        REQUIRE(item.type == binding_type::texture);
        REQUIRE(item.array_size == 1);
    }

    SECTION("texture_array helper")
    {
        auto item = binding_item::texture_array(0, 32);

        REQUIRE(item.slot == 0);
        REQUIRE(item.type == binding_type::texture);
        REQUIRE(item.array_size == 32);
    }

    SECTION("sampler helper")
    {
        auto item = binding_item::sampler(1);

        REQUIRE(item.slot == 1);
        REQUIRE(item.type == binding_type::sampler);
        REQUIRE(item.array_size == 1);
    }

    SECTION("constant_buffer helper")
    {
        auto item = binding_item::constant_buffer(0);

        REQUIRE(item.slot == 0);
        REQUIRE(item.type == binding_type::constant_buffer);
        REQUIRE(item.array_size == 1);
    }
}

TEST_CASE("binding_layout_desc defaults", "[graphics][binding_layout]")
{
    binding_layout_desc desc;

    SECTION("default bindings empty")
    {
        REQUIRE(desc.bindings.empty());
    }

    SECTION("default visibility")
    {
        REQUIRE(desc.vertex_shader_visible == true);
        REQUIRE(desc.pixel_shader_visible == true);
    }
}

TEST_CASE("binding_layout_desc builder", "[graphics][binding_layout]")
{
    SECTION("add single texture")
    {
        binding_layout_desc desc;
        desc.add_texture(0);

        REQUIRE(desc.bindings.size() == 1);
        REQUIRE(desc.bindings[0].slot == 0);
        REQUIRE(desc.bindings[0].type == binding_type::texture);
    }

    SECTION("add texture array")
    {
        binding_layout_desc desc;
        desc.add_texture_array(0, 16);

        REQUIRE(desc.bindings.size() == 1);
        REQUIRE(desc.bindings[0].slot == 0);
        REQUIRE(desc.bindings[0].type == binding_type::texture);
        REQUIRE(desc.bindings[0].array_size == 16);
    }

    SECTION("add sampler")
    {
        binding_layout_desc desc;
        desc.add_sampler(0);

        REQUIRE(desc.bindings.size() == 1);
        REQUIRE(desc.bindings[0].type == binding_type::sampler);
    }

    SECTION("add constant buffer")
    {
        binding_layout_desc desc;
        desc.add_constant_buffer(0);

        REQUIRE(desc.bindings.size() == 1);
        REQUIRE(desc.bindings[0].type == binding_type::constant_buffer);
    }

    SECTION("chained calls")
    {
        binding_layout_desc desc;
        desc.add_constant_buffer(0)
            .add_texture_array(0, 32)
            .add_sampler(0);

        REQUIRE(desc.bindings.size() == 3);
    }
}

TEST_CASE("predefined binding layouts", "[graphics][binding_layout]")
{
    SECTION("texture_sampler")
    {
        auto desc = binding_layouts::texture_sampler();

        REQUIRE(desc.bindings.size() == 2);

        // Find texture and sampler
        bool has_texture = false;
        bool has_sampler = false;
        for (const auto& binding : desc.bindings)
        {
            if (binding.type == binding_type::texture) has_texture = true;
            if (binding.type == binding_type::sampler) has_sampler = true;
        }
        REQUIRE(has_texture);
        REQUIRE(has_sampler);
    }

    SECTION("constant_buffer")
    {
        auto desc = binding_layouts::constant_buffer();

        REQUIRE(desc.bindings.size() == 1);
        REQUIRE(desc.bindings[0].type == binding_type::constant_buffer);
    }

    SECTION("standard_2d")
    {
        auto desc = binding_layouts::standard_2d();

        REQUIRE(desc.bindings.size() == 3);

        bool has_cb = false;
        bool has_texture = false;
        bool has_sampler = false;
        for (const auto& binding : desc.bindings)
        {
            if (binding.type == binding_type::constant_buffer) has_cb = true;
            if (binding.type == binding_type::texture) has_texture = true;
            if (binding.type == binding_type::sampler) has_sampler = true;
        }
        REQUIRE(has_cb);
        REQUIRE(has_texture);
        REQUIRE(has_sampler);
    }
}

TEST_CASE("binding_layout creation", "[graphics][binding_layout]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("create simple layout")
    {
        binding_layout_desc desc;
        desc.add_texture(0)
            .add_sampler(0);

        auto layout = binding_layout::create(dev, desc);

        REQUIRE(layout != nullptr);
        REQUIRE(layout->get_layout() != nullptr);
        REQUIRE(layout->get_desc().bindings.size() == 2);
    }

    SECTION("create with constant buffer")
    {
        binding_layout_desc desc;
        desc.add_constant_buffer(0);

        auto layout = binding_layout::create(dev, desc);

        REQUIRE(layout != nullptr);
    }

    SECTION("create with texture array")
    {
        binding_layout_desc desc;
        desc.add_constant_buffer(0)
            .add_texture_array(0, 32)
            .add_sampler(0);

        auto layout = binding_layout::create(dev, desc);

        REQUIRE(layout != nullptr);
        REQUIRE(layout->get_desc().bindings.size() == 3);
    }

    SECTION("create multiple layouts")
    {
        auto layout1 = binding_layout::create(dev, binding_layouts::texture_sampler());
        auto layout2 = binding_layout::create(dev, binding_layouts::constant_buffer());
        auto layout3 = binding_layout::create(dev, binding_layouts::standard_2d());

        REQUIRE(layout1 != nullptr);
        REQUIRE(layout2 != nullptr);
        REQUIRE(layout3 != nullptr);
    }
}
