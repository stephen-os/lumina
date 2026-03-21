#pragma once

#include "../../device.h"

// Platform defines must be set before including Vulkan headers
#ifdef _WIN32
    #ifndef VK_USE_PLATFORM_WIN32_KHR
        #define VK_USE_PLATFORM_WIN32_KHR
    #endif
#endif

#ifndef VULKAN_HPP_DISPATCH_LOADER_DYNAMIC
    #define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#endif

#include <nvrhi/vulkan.h>

// Include profiler and Tracy Vulkan GPU header
#include "../../profiler.h"
#ifdef TRACY_ENABLE
    #include <tracy/TracyVulkan.hpp>
#endif

#include <vector>

namespace lumina::core::platform::vulkan
{
    class vulkan_message_callback : public nvrhi::IMessageCallback
    {
    public:
        void message(nvrhi::MessageSeverity severity, const char* message_text) override;
    };

    class vulkan_device : public device
    {
    public:
        vulkan_device() = default;
        ~vulkan_device() override;

        bool init(const device_desc& desc) override;
        void shutdown() override;

        void begin_frame() override;
        void present() override;

        void resize(uint32_t width, uint32_t height) override;

        nvrhi::IDevice* get_nvrhi_device() const override { return m_nvrhi_device.Get(); }
        nvrhi::ICommandList* get_command_list() const override { return m_command_list.Get(); }
        nvrhi::IFramebuffer* get_current_framebuffer() const override;

        uint32_t get_width() const override { return m_width; }
        uint32_t get_height() const override { return m_height; }
        uint32_t get_frame_index() const override { return m_frame_index; }

        graphics_api get_api() const override { return graphics_api::vulkan; }
        nvrhi::Format get_swapchain_format() const override;

        vulkan_native_handles get_vulkan_handles() const override
        {
            return { m_instance, m_physical_device, m_device, m_graphics_queue, m_graphics_queue_family };
        }

    private:
        bool create_instance();
        bool create_device();
        bool create_swapchain();
        bool create_framebuffers();
        void destroy_framebuffers();
        void destroy_swapchain();

        void wait_for_gpu();

        GLFWwindow* m_window = nullptr;
        uint32_t m_width = 0;
        uint32_t m_height = 0;
        uint32_t m_backbuffer_count = 2;
        bool m_vsync = true;

        // Vulkan objects
        VkInstance m_instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT m_debug_messenger = VK_NULL_HANDLE;
        VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
        VkDevice m_device = VK_NULL_HANDLE;
        VkQueue m_graphics_queue = VK_NULL_HANDLE;
        uint32_t m_graphics_queue_family = 0;
        VkSurfaceKHR m_surface = VK_NULL_HANDLE;
        VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
        VkFormat m_swapchain_format = VK_FORMAT_UNDEFINED;

        std::vector<VkImage> m_swapchain_images;
        std::vector<VkSemaphore> m_image_available_semaphores;
        std::vector<VkSemaphore> m_render_finished_semaphores;
        std::vector<VkFence> m_in_flight_fences;
        uint32_t m_frame_index = 0;
        uint32_t m_image_index = 0;

        // NVRHI objects
        vulkan_message_callback m_message_callback;
        nvrhi::DeviceHandle m_nvrhi_device;
        nvrhi::CommandListHandle m_command_list;

        // Swapchain textures and framebuffers
        std::vector<nvrhi::TextureHandle> m_swapchain_textures;
        std::vector<nvrhi::FramebufferHandle> m_swapchain_framebuffers;

#ifdef TRACY_ENABLE
        TracyVkCtx m_tracy_ctx = nullptr;
        VkCommandPool m_tracy_command_pool = VK_NULL_HANDLE;
#endif
    };
}
