// Unit tests for sampler

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "graphics_test_fixture.h"
#include <Lumina/Graphics/sampler.h>

using namespace lumina::graphics;
using Catch::Approx;

TEST_CASE("sampler creation", "[graphics][sampler]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("create with defaults")
    {
        auto samp = sampler::create(dev);

        REQUIRE(samp != nullptr);
        REQUIRE(samp->get_sampler() != nullptr);
        REQUIRE(samp->get_desc().filter == filter_mode::linear);
        REQUIRE(samp->get_desc().address_u == address_mode::clamp);
    }

    SECTION("create with custom desc")
    {
        sampler_desc desc;
        desc.filter = filter_mode::point;
        desc.address_u = address_mode::wrap;
        desc.address_v = address_mode::wrap;

        auto samp = sampler::create(dev, desc);

        REQUIRE(samp != nullptr);
        REQUIRE(samp->get_desc().filter == filter_mode::point);
        REQUIRE(samp->get_desc().address_u == address_mode::wrap);
        REQUIRE(samp->get_desc().address_v == address_mode::wrap);
    }

    SECTION("create multiple samplers")
    {
        auto samp1 = sampler::create(dev, samplers::point_clamp());
        auto samp2 = sampler::create(dev, samplers::linear_clamp());
        auto samp3 = sampler::create(dev, samplers::linear_wrap());

        REQUIRE(samp1 != nullptr);
        REQUIRE(samp2 != nullptr);
        REQUIRE(samp3 != nullptr);
    }
}

TEST_CASE("sampler_desc defaults", "[graphics][sampler]")
{
    sampler_desc desc;

    SECTION("default filter")
    {
        REQUIRE(desc.filter == filter_mode::linear);
    }

    SECTION("default address modes")
    {
        REQUIRE(desc.address_u == address_mode::clamp);
        REQUIRE(desc.address_v == address_mode::clamp);
        REQUIRE(desc.address_w == address_mode::clamp);
    }

    SECTION("default anisotropy")
    {
        REQUIRE(desc.max_anisotropy == Approx(1.0f));
    }

    SECTION("default LOD settings")
    {
        REQUIRE(desc.mip_lod_bias == Approx(0.0f));
        REQUIRE(desc.min_lod == Approx(0.0f));
        REQUIRE(desc.max_lod == Approx(1000.0f));
    }
}

TEST_CASE("sampler_desc builder pattern", "[graphics][sampler]")
{
    SECTION("set_filter")
    {
        sampler_desc desc;
        desc.set_filter(filter_mode::point);

        REQUIRE(desc.filter == filter_mode::point);
    }

    SECTION("set_address")
    {
        sampler_desc desc;
        desc.set_address(address_mode::wrap);

        REQUIRE(desc.address_u == address_mode::wrap);
        REQUIRE(desc.address_v == address_mode::wrap);
        REQUIRE(desc.address_w == address_mode::wrap);
    }

    SECTION("set_anisotropy")
    {
        sampler_desc desc;
        desc.set_anisotropy(8.0f);

        REQUIRE(desc.max_anisotropy == Approx(8.0f));
        REQUIRE(desc.filter == filter_mode::anisotropic);
    }

    SECTION("chained calls")
    {
        sampler_desc desc;
        desc.set_filter(filter_mode::linear)
            .set_address(address_mode::mirror);

        REQUIRE(desc.filter == filter_mode::linear);
        REQUIRE(desc.address_u == address_mode::mirror);
    }
}

TEST_CASE("predefined samplers", "[graphics][sampler]")
{
    SECTION("point_clamp")
    {
        auto desc = samplers::point_clamp();

        REQUIRE(desc.filter == filter_mode::point);
        REQUIRE(desc.address_u == address_mode::clamp);
        REQUIRE(desc.address_v == address_mode::clamp);
        REQUIRE(desc.address_w == address_mode::clamp);
    }

    SECTION("linear_clamp")
    {
        auto desc = samplers::linear_clamp();

        REQUIRE(desc.filter == filter_mode::linear);
        REQUIRE(desc.address_u == address_mode::clamp);
        REQUIRE(desc.address_v == address_mode::clamp);
        REQUIRE(desc.address_w == address_mode::clamp);
    }

    SECTION("linear_wrap")
    {
        auto desc = samplers::linear_wrap();

        REQUIRE(desc.filter == filter_mode::linear);
        REQUIRE(desc.address_u == address_mode::wrap);
        REQUIRE(desc.address_v == address_mode::wrap);
        REQUIRE(desc.address_w == address_mode::wrap);
    }

    SECTION("anisotropic_clamp default")
    {
        auto desc = samplers::anisotropic_clamp();

        REQUIRE(desc.filter == filter_mode::anisotropic);
        REQUIRE(desc.max_anisotropy == Approx(16.0f));
        REQUIRE(desc.address_u == address_mode::clamp);
    }

    SECTION("anisotropic_clamp custom")
    {
        auto desc = samplers::anisotropic_clamp(4.0f);

        REQUIRE(desc.max_anisotropy == Approx(4.0f));
    }
}

TEST_CASE("sampler filter modes", "[graphics][sampler]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("point filter")
    {
        sampler_desc desc;
        desc.filter = filter_mode::point;

        auto samp = sampler::create(dev, desc);

        REQUIRE(samp != nullptr);
        REQUIRE(samp->get_desc().filter == filter_mode::point);
    }

    SECTION("linear filter")
    {
        sampler_desc desc;
        desc.filter = filter_mode::linear;

        auto samp = sampler::create(dev, desc);

        REQUIRE(samp != nullptr);
        REQUIRE(samp->get_desc().filter == filter_mode::linear);
    }

    SECTION("anisotropic filter")
    {
        sampler_desc desc;
        desc.filter = filter_mode::anisotropic;
        desc.max_anisotropy = 8.0f;

        auto samp = sampler::create(dev, desc);

        REQUIRE(samp != nullptr);
        REQUIRE(samp->get_desc().filter == filter_mode::anisotropic);
    }
}

TEST_CASE("sampler address modes", "[graphics][sampler]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("clamp mode")
    {
        sampler_desc desc;
        desc.address_u = address_mode::clamp;
        desc.address_v = address_mode::clamp;

        auto samp = sampler::create(dev, desc);

        REQUIRE(samp != nullptr);
    }

    SECTION("wrap mode")
    {
        sampler_desc desc;
        desc.address_u = address_mode::wrap;
        desc.address_v = address_mode::wrap;

        auto samp = sampler::create(dev, desc);

        REQUIRE(samp != nullptr);
    }

    SECTION("mirror mode")
    {
        sampler_desc desc;
        desc.address_u = address_mode::mirror;
        desc.address_v = address_mode::mirror;

        auto samp = sampler::create(dev, desc);

        REQUIRE(samp != nullptr);
    }

    SECTION("mixed address modes")
    {
        sampler_desc desc;
        desc.address_u = address_mode::wrap;
        desc.address_v = address_mode::clamp;
        desc.address_w = address_mode::mirror;

        auto samp = sampler::create(dev, desc);

        REQUIRE(samp != nullptr);
        REQUIRE(samp->get_desc().address_u == address_mode::wrap);
        REQUIRE(samp->get_desc().address_v == address_mode::clamp);
        REQUIRE(samp->get_desc().address_w == address_mode::mirror);
    }
}
