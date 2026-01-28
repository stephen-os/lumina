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

    struct graphics_device_desc
    {
        GLFWwindow* window = nullptr;
        uint32_t width = 1600;
        uint32_t height = 900;
        uint32_t backbuffer_count = 2;
        bool vsync = true;
        bool enable_debug_layer = false;
        std::string app_name = "Lumina Application";
    };

    class graphics_device
    {
    public:
        virtual ~graphics_device() = default;

        virtual bool init(const graphics_device_desc& desc) = 0;
        virtual void shutdown() = 0;

        virtual void begin_frame() = 0;
        virtual void present() = 0;

        virtual void resize(uint32_t width, uint32_t height) = 0;

        virtual nvrhi::IDevice* get_device() const = 0;
        virtual nvrhi::ICommandList* get_command_list() const = 0;
        virtual nvrhi::IFramebuffer* get_current_framebuffer() const = 0;

        virtual uint32_t get_width() const = 0;
        virtual uint32_t get_height() const = 0;
        virtual uint32_t get_frame_index() const = 0;

        virtual graphics_api get_api() const = 0;
        virtual nvrhi::Format get_swapchain_format() const = 0;

        static scope<graphics_device> create(graphics_api api);
    };
}
