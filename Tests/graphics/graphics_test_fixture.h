#pragma once

// Graphics test fixture - provides shared NVRHI device for GPU tests
//
// Usage:
//   #include "graphics_test_fixture.h"
//   TEST_CASE("my test", "[graphics]") {
//       auto& fixture = graphics_fixture::get();
//       if (!fixture.is_available()) {
//           SKIP("Graphics device not available");
//       }
//       auto& dev = fixture.device();
//       // ... use dev to create buffers, textures, etc.
//   }

#include <Lumina/Core/device.h>

#include <memory>

struct GLFWwindow;

class graphics_fixture
{
public:
    static graphics_fixture& get();

    // Initialize the fixture (call once at test startup)
    bool init();

    // Shutdown (call once at test end)
    void shutdown();

    // Check if device is available
    bool is_available() const { return m_device != nullptr && m_initialized; }

    // Get the device
    lumina::core::device& device() { return *m_device; }

    // Get NVRHI device directly
    nvrhi::IDevice* nvrhi_device() { return m_device ? m_device->get_nvrhi_device() : nullptr; }

    // Begin/end frame for tests that need command list access
    void begin_frame();
    void end_frame();

    // Get command list (only valid between begin_frame/end_frame)
    nvrhi::ICommandList* command_list() { return m_device ? m_device->get_command_list() : nullptr; }

private:
    graphics_fixture() = default;
    ~graphics_fixture() = default;

    GLFWwindow* m_window = nullptr;
    std::unique_ptr<lumina::core::device> m_device;
    bool m_initialized = false;
};

// Macro to skip test if graphics not available
#define REQUIRE_GRAPHICS() \
    do { \
        auto& fixture = graphics_fixture::get(); \
        if (!fixture.is_available()) { \
            SKIP("Graphics device not available"); \
        } \
    } while(0)
