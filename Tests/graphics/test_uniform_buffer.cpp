// Unit tests for uniform_buffer

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "graphics_test_fixture.h"
#include <Lumina/Graphics/uniform_buffer.h>

#include <glm/glm.hpp>

using namespace lumina::graphics;
using Catch::Approx;

TEST_CASE("uniform_buffer creation", "[graphics][uniform_buffer]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("create with size")
    {
        auto buffer = uniform_buffer::create(dev, 256);

        REQUIRE(buffer != nullptr);
        REQUIRE(buffer->get_size() == 256);
        REQUIRE(buffer->get_aligned_size() >= 256);  // Aligned size >= requested size
        REQUIRE(buffer->get_buffer() != nullptr);
    }

    SECTION("create with type")
    {
        struct CameraData {
            glm::mat4 view_projection;
        };

        auto buffer = uniform_buffer::create<CameraData>(dev);

        REQUIRE(buffer != nullptr);
        REQUIRE(buffer->get_size() == sizeof(CameraData));
    }

    SECTION("create multiple buffers")
    {
        auto buffer1 = uniform_buffer::create(dev, 64);
        auto buffer2 = uniform_buffer::create(dev, 128);
        auto buffer3 = uniform_buffer::create(dev, 256);

        REQUIRE(buffer1 != nullptr);
        REQUIRE(buffer2 != nullptr);
        REQUIRE(buffer3 != nullptr);
        REQUIRE(buffer1->get_buffer() != buffer2->get_buffer());
        REQUIRE(buffer2->get_buffer() != buffer3->get_buffer());
    }
}

TEST_CASE("uniform_buffer sizes", "[graphics][uniform_buffer]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("small buffer")
    {
        auto buffer = uniform_buffer::create(dev, 16);

        REQUIRE(buffer != nullptr);
        REQUIRE(buffer->get_size() == 16);
    }

    SECTION("typical mvp buffer (64 bytes)")
    {
        // Single mat4
        auto buffer = uniform_buffer::create(dev, sizeof(glm::mat4));

        REQUIRE(buffer != nullptr);
        REQUIRE(buffer->get_size() == sizeof(glm::mat4));
        REQUIRE(buffer->get_size() == 64);
    }

    SECTION("large uniform buffer")
    {
        // 16KB is usually the minimum guaranteed by GPUs
        auto buffer = uniform_buffer::create(dev, 16384);

        REQUIRE(buffer != nullptr);
        REQUIRE(buffer->get_size() == 16384);
    }
}

TEST_CASE("uniform_buffer update", "[graphics][uniform_buffer]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("update with struct")
    {
        struct TestData {
            glm::vec4 color;
            float time;
            float padding[3];  // Align to 16 bytes
        };

        auto buffer = uniform_buffer::create<TestData>(dev);
        REQUIRE(buffer != nullptr);

        fixture.begin_frame();

        TestData data;
        data.color = glm::vec4(1.0f, 0.5f, 0.25f, 1.0f);
        data.time = 3.14f;

        buffer->update(data, fixture.command_list());

        fixture.end_frame();

        REQUIRE(buffer->get_size() == sizeof(TestData));
    }

    SECTION("update with raw data")
    {
        auto buffer = uniform_buffer::create(dev, 128);
        REQUIRE(buffer != nullptr);

        fixture.begin_frame();

        std::array<float, 32> data;
        data.fill(1.0f);

        buffer->update(data.data(), sizeof(data), fixture.command_list());

        fixture.end_frame();

        REQUIRE(buffer->get_size() == 128);
    }

    SECTION("update multiple times")
    {
        struct FrameData {
            glm::mat4 transform;
        };

        auto buffer = uniform_buffer::create<FrameData>(dev);
        REQUIRE(buffer != nullptr);

        for (int i = 0; i < 5; ++i)
        {
            fixture.begin_frame();

            FrameData data;
            data.transform = glm::mat4(static_cast<float>(i));

            buffer->update(data, fixture.command_list());

            fixture.end_frame();
        }
    }
}

TEST_CASE("uniform_buffer common use cases", "[graphics][uniform_buffer]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("camera uniform buffer")
    {
        struct CameraBuffer {
            glm::mat4 view;
            glm::mat4 projection;
            glm::mat4 view_projection;
        };

        auto buffer = uniform_buffer::create<CameraBuffer>(dev);

        REQUIRE(buffer != nullptr);
        REQUIRE(buffer->get_size() == sizeof(CameraBuffer));
        REQUIRE(buffer->get_size() == 192);  // 3 * 64 bytes
    }

    SECTION("per-object uniform buffer")
    {
        struct ObjectBuffer {
            glm::mat4 model;
            glm::vec4 color;
        };

        auto buffer = uniform_buffer::create<ObjectBuffer>(dev);

        REQUIRE(buffer != nullptr);
        REQUIRE(buffer->get_size() == sizeof(ObjectBuffer));
    }

    SECTION("lighting uniform buffer")
    {
        struct LightBuffer {
            glm::vec4 position;
            glm::vec4 color;
            float intensity;
            float radius;
            float falloff;
            float padding;
        };

        auto buffer = uniform_buffer::create<LightBuffer>(dev);

        REQUIRE(buffer != nullptr);
        REQUIRE(buffer->get_size() == sizeof(LightBuffer));
    }
}

TEST_CASE("uniform_buffer move semantics", "[graphics][uniform_buffer]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("move constructor")
    {
        auto buffer1 = uniform_buffer::create(dev, 256);

        REQUIRE(buffer1 != nullptr);
        size_t original_size = buffer1->get_size();
        nvrhi::IBuffer* original_handle = buffer1->get_buffer();

        uniform_buffer buffer2 = std::move(*buffer1);

        REQUIRE(buffer2.get_size() == original_size);
        REQUIRE(buffer2.get_buffer() == original_handle);
    }
}

TEST_CASE("uniform_buffer validation", "[graphics][uniform_buffer]")
{
    REQUIRE_GRAPHICS();
    auto& fixture = graphics_fixture::get();
    auto& dev = fixture.device();

    SECTION("create with zero size fails")
    {
        auto buffer = uniform_buffer::create(dev, 0);

        REQUIRE(buffer == nullptr);
    }

    SECTION("create with custom debug name")
    {
        auto buffer = uniform_buffer::create(dev, 64, "My Custom Uniform Buffer");

        REQUIRE(buffer != nullptr);
    }
}
