#include "graphics_device.h"
#include "log.h"

#ifdef LUMINA_PLATFORM_WINDOWS
#include "platform/d3d12/d3d12_device.h"
#endif

#include "platform/vulkan/vulkan_device.h"

namespace lumina::core
{
    scope<graphics_device> graphics_device::create(graphics_api api)
    {
        switch (api)
        {
        case graphics_api::d3d12:
#ifdef LUMINA_PLATFORM_WINDOWS
            return make_scope<platform::d3d12::d3d12_device>();
#else
            LUMINA_LOG_ERROR("D3D12 is only supported on Windows");
            return nullptr;
#endif

        case graphics_api::vulkan:
            return make_scope<platform::vulkan::vulkan_device>();

        default:
            LUMINA_LOG_ERROR("Unknown graphics API");
            return nullptr;
        }
    }
}
