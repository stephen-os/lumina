#pragma once

#include "base.h"

#include <nvrhi/nvrhi.h>

#include <cstdint>
#include <string>

struct GLFWwindow;

namespace lumina::core
{
    enum class graphics_api
    {
        d3d12,
        vulkan
    };

    struct device_desc
    {
        GLFWwindow* window = nullptr;
        uint32_t width = 1600;
        uint32_t height = 900;
        uint32_t backbuffer_count = 2;
        bool vsync = true;
        bool enable_debug_layer = false;
        std::string app_name = "Lumina Application";
    };

    // Native handle structs for viewport support (void* to avoid leaking API headers)
    struct vulkan_native_handles
    {
        void* instance = nullptr;           // VkInstance
        void* physical_device = nullptr;    // VkPhysicalDevice
        void* device = nullptr;             // VkDevice
        void* graphics_queue = nullptr;     // VkQueue
        uint32_t graphics_queue_family = 0;
    };

    struct d3d12_native_handles
    {
        void* device = nullptr;             // ID3D12Device*
        void* command_queue = nullptr;      // ID3D12CommandQueue*
        void* dxgi_factory = nullptr;       // IDXGIFactory6*
    };

    class device
    {
    public:
        virtual ~device() = default;

        virtual bool init(const device_desc& desc) = 0;
        virtual void shutdown() = 0;

        virtual void begin_frame() = 0;
        virtual void present() = 0;

        virtual void resize(uint32_t width, uint32_t height) = 0;

        virtual nvrhi::IDevice* get_nvrhi_device() const = 0;
        virtual nvrhi::ICommandList* get_command_list() const = 0;
        virtual nvrhi::IFramebuffer* get_current_framebuffer() const = 0;

        virtual uint32_t get_width() const = 0;
        virtual uint32_t get_height() const = 0;
        virtual uint32_t get_frame_index() const = 0;

        virtual graphics_api get_api() const = 0;
        virtual nvrhi::Format get_swapchain_format() const = 0;

        virtual vulkan_native_handles get_vulkan_handles() const { return {}; }
        virtual d3d12_native_handles get_d3d12_handles() const { return {}; }

        static scope<device> create(graphics_api api);
    };
}
