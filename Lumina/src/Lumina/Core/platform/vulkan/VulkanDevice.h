#pragma once

#include "../../Device.h"

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
#include "../../Profiler.h"
#ifdef TRACY_ENABLE
    #include <tracy/TracyVulkan.hpp>
#endif

#include <vector>

namespace Lumina
{
    class VulkanMessageCallback : public nvrhi::IMessageCallback
    {
    public:
        void message(nvrhi::MessageSeverity severity, const char* messageText) override;
    };

    class VulkanDevice : public Device
    {
    public:
        VulkanDevice() = default;
        ~VulkanDevice() override;

        bool Init(const DeviceDesc& desc) override;
        void Shutdown() override;

        void BeginFrame() override;
        void Present() override;

        void Resize(uint32_t width, uint32_t height) override;

        nvrhi::IDevice* GetNvrhiDevice() const override { return m_NvrhiDevice.Get(); }
        nvrhi::ICommandList* GetCommandList() const override { return m_CommandList.Get(); }
        nvrhi::IFramebuffer* GetCurrentFramebuffer() const override;

        uint32_t GetWidth() const override { return m_Width; }
        uint32_t GetHeight() const override { return m_Height; }
        uint32_t GetFrameIndex() const override { return m_FrameIndex; }

        GraphicsAPI GetAPI() const override { return GraphicsAPI::Vulkan; }
        nvrhi::Format GetSwapchainFormat() const override;

        VulkanNativeHandles GetVulkanHandles() const override
        {
            return { m_Instance, m_PhysicalDevice, m_Device, m_GraphicsQueue, m_GraphicsQueueFamily };
        }

    private:
        bool CreateInstance();
        bool CreateDevice();
        bool CreateSwapchain();
        bool CreateFramebuffers();
        void DestroyFramebuffers();
        void DestroySwapchain();

        void WaitForGPU();

        GLFWwindow* m_Window = nullptr;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        uint32_t m_BackbufferCount = 2;
        bool m_VSync = true;

        // Vulkan objects
        VkInstance m_Instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice m_Device = VK_NULL_HANDLE;
        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
        uint32_t m_GraphicsQueueFamily = 0;
        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
        VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
        VkFormat m_SwapchainFormat = VK_FORMAT_UNDEFINED;

        std::vector<VkImage> m_SwapchainImages;
        std::vector<VkSemaphore> m_ImageAvailableSemaphores;
        std::vector<VkSemaphore> m_RenderFinishedSemaphores;
        std::vector<VkFence> m_InFlightFences;
        uint32_t m_FrameIndex = 0;
        uint32_t m_ImageIndex = 0;

        // NVRHI objects
        VulkanMessageCallback m_MessageCallback;
        nvrhi::DeviceHandle m_NvrhiDevice;
        nvrhi::CommandListHandle m_CommandList;

        // Swapchain textures and framebuffers
        std::vector<nvrhi::TextureHandle> m_SwapchainTextures;
        std::vector<nvrhi::FramebufferHandle> m_SwapchainFramebuffers;

#ifdef TRACY_ENABLE
        TracyVkCtx m_TracyCtx = nullptr;
        VkCommandPool m_TracyCommandPool = VK_NULL_HANDLE;
#endif
    };
}
