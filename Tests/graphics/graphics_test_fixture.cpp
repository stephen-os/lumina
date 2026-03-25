#include "graphics_test_fixture.h"

#include <Lumina/Core/log.h>

#include <GLFW/glfw3.h>

static graphics_fixture* s_instance = nullptr;

graphics_fixture& graphics_fixture::get()
{
    if (!s_instance)
    {
        s_instance = new graphics_fixture();
    }
    return *s_instance;
}

bool graphics_fixture::init()
{
    if (m_initialized)
        return true;

    // Initialize logging first
    lumina::core::log::init("Graphics Tests");

    // Initialize GLFW
    if (!glfwInit())
    {
        return false;
    }

    // Create a hidden window for the graphics device
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    m_window = glfwCreateWindow(64, 64, "Test Window", nullptr, nullptr);
    if (!m_window)
    {
        glfwTerminate();
        return false;
    }

    // Create device - try D3D12 first on Windows, then Vulkan
#ifdef LUMINA_PLATFORM_WINDOWS
    m_device = lumina::core::device::create(lumina::core::graphics_api::d3d12);
#else
    m_device = lumina::core::device::create(lumina::core::graphics_api::vulkan);
#endif

    if (!m_device)
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
        return false;
    }

    lumina::core::device_desc desc;
    desc.window = m_window;
    desc.width = 64;
    desc.height = 64;
    desc.vsync = false;
    desc.enable_debug_layer = true;
    desc.app_name = "Graphics Tests";

    if (!m_device->init(desc))
    {
        m_device.reset();
        glfwDestroyWindow(m_window);
        glfwTerminate();
        return false;
    }

    m_initialized = true;
    return true;
}

void graphics_fixture::shutdown()
{
    if (!m_initialized)
        return;

    if (m_device)
    {
        m_device->shutdown();
        m_device.reset();
    }

    if (m_window)
    {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }

    glfwTerminate();

    lumina::core::log::shutdown();

    m_initialized = false;

    delete s_instance;
    s_instance = nullptr;
}

void graphics_fixture::begin_frame()
{
    if (m_device)
    {
        m_device->begin_frame();
    }
}

void graphics_fixture::end_frame()
{
    if (m_device)
    {
        m_device->present();
    }
}
