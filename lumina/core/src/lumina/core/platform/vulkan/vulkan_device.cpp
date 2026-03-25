#include "vulkan_device.h"

#include "../../Log.h"
#include "../../Assert.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.hpp>

#include <nvrhi/validation.h>

#include <vector>
#include <set>
#include <cstring>

namespace Lumina
{
    // Validation layer name
    static const char* s_ValidationLayer = "VK_LAYER_KHRONOS_validation";

    // Required device extensions
    static const std::vector<const char*> s_DeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
        void* userData)
    {
        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
            LUMINA_LOG_ERROR("[Vulkan] {}", callbackData->pMessage);
        else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
            LUMINA_LOG_WARN("[Vulkan] {}", callbackData->pMessage);
        else
            LUMINA_LOG_INFO("[Vulkan] {}", callbackData->pMessage);

        return VK_FALSE;
    }

    void VulkanMessageCallback::message(nvrhi::MessageSeverity severity, const char* messageText)
    {
        switch (severity)
        {
        case nvrhi::MessageSeverity::Info:
            LUMINA_LOG_INFO("[NVRHI-VK] {}", messageText);
            break;
        case nvrhi::MessageSeverity::Warning:
            LUMINA_LOG_WARN("[NVRHI-VK] {}", messageText);
            break;
        case nvrhi::MessageSeverity::Error:
            LUMINA_LOG_ERROR("[NVRHI-VK] {}", messageText);
            break;
        case nvrhi::MessageSeverity::Fatal:
            LUMINA_LOG_CRITICAL("[NVRHI-VK] {}", messageText);
            break;
        }
    }

    VulkanDevice::~VulkanDevice()
    {
        Shutdown();
    }

    bool VulkanDevice::Init(const DeviceDesc& desc)
    {
        m_Window = desc.Window;
        m_Width = desc.Width;
        m_Height = desc.Height;
        m_BackbufferCount = desc.BackbufferCount;
        m_VSync = desc.VSync;

        // Initialize Vulkan HPP dispatcher (for pre-instance functions like vkCreateInstance)
        VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

        if (!CreateInstance())
        {
            LUMINA_LOG_ERROR("Failed to create Vulkan instance");
            return false;
        }

        // Create window surface
        if (glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface) != VK_SUCCESS)
        {
            LUMINA_LOG_ERROR("Failed to create Vulkan window surface");
            return false;
        }

        if (!CreateDevice())
        {
            LUMINA_LOG_ERROR("Failed to create Vulkan device");
            return false;
        }

        if (!CreateSwapchain())
        {
            LUMINA_LOG_ERROR("Failed to create Vulkan swapchain");
            return false;
        }

        // Create NVRHI device wrapper
        nvrhi::vulkan::DeviceDesc nvrhiDesc;
        nvrhiDesc.errorCB = &m_MessageCallback;
        nvrhiDesc.instance = m_Instance;
        nvrhiDesc.physicalDevice = m_PhysicalDevice;
        nvrhiDesc.device = m_Device;
        nvrhiDesc.graphicsQueue = m_GraphicsQueue;
        nvrhiDesc.graphicsQueueIndex = static_cast<int>(m_GraphicsQueueFamily);
        nvrhiDesc.deviceExtensions = const_cast<const char**>(s_DeviceExtensions.data());
        nvrhiDesc.numDeviceExtensions = s_DeviceExtensions.size();

        m_NvrhiDevice = nvrhi::vulkan::createDevice(nvrhiDesc);
        if (!m_NvrhiDevice)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI Vulkan device");
            return false;
        }

#ifdef LUMINA_DEBUG
        // Wrap with validation layer in debug builds
        nvrhi::DeviceHandle validationLayer = nvrhi::validation::createValidationLayer(m_NvrhiDevice);
        m_NvrhiDevice = validationLayer;
#endif

        // Create command list
        m_CommandList = m_NvrhiDevice->createCommandList();
        if (!m_CommandList)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI command list");
            return false;
        }

        if (!CreateFramebuffers())
        {
            LUMINA_LOG_ERROR("Failed to create framebuffers");
            return false;
        }

#ifdef TRACY_ENABLE
        // Create command pool for Tracy
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = m_GraphicsQueueFamily;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        if (vkCreateCommandPool(m_Device, &poolInfo, nullptr, &m_TracyCommandPool) == VK_SUCCESS)
        {
            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandPool = m_TracyCommandPool;
            allocInfo.commandBufferCount = 1;

            VkCommandBuffer tracyCmd;
            if (vkAllocateCommandBuffers(m_Device, &allocInfo, &tracyCmd) == VK_SUCCESS)
            {
                m_TracyCtx = LUMINA_PROFILE_GPU_VK_CONTEXT(m_PhysicalDevice, m_Device, m_GraphicsQueue, tracyCmd);
                LUMINA_PROFILE_GPU_VK_CONTEXT_NAME(m_TracyCtx, "Vulkan Main Queue");
            }
        }
#endif

        LUMINA_LOG_INFO("Vulkan graphics device initialized successfully");
        return true;
    }

    void VulkanDevice::Shutdown()
    {
        if (m_Device)
        {
            vkDeviceWaitIdle(m_Device);
        }

#ifdef TRACY_ENABLE
        if (m_TracyCtx)
        {
            LUMINA_PROFILE_GPU_VK_DESTROY(m_TracyCtx);
            m_TracyCtx = nullptr;
        }
        if (m_TracyCommandPool)
        {
            vkDestroyCommandPool(m_Device, m_TracyCommandPool, nullptr);
            m_TracyCommandPool = VK_NULL_HANDLE;
        }
#endif

        DestroyFramebuffers();
        DestroySwapchain();

        m_CommandList = nullptr;
        m_NvrhiDevice = nullptr;

        if (m_Device)
        {
            vkDestroyDevice(m_Device, nullptr);
            m_Device = VK_NULL_HANDLE;
        }

        if (m_Surface)
        {
            vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
            m_Surface = VK_NULL_HANDLE;
        }

#ifdef LUMINA_DEBUG
        if (m_DebugMessenger)
        {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");
            if (func)
                func(m_Instance, m_DebugMessenger, nullptr);
            m_DebugMessenger = VK_NULL_HANDLE;
        }
#endif

        if (m_Instance)
        {
            vkDestroyInstance(m_Instance, nullptr);
            m_Instance = VK_NULL_HANDLE;
        }

        LUMINA_LOG_INFO("Vulkan graphics device shutdown");
    }

    bool VulkanDevice::CreateInstance()
    {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Lumina Application";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Lumina";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        // Get required extensions from GLFW
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef LUMINA_DEBUG
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

#ifdef LUMINA_DEBUG
        // Enable validation layers in debug mode
        createInfo.enabledLayerCount = 1;
        createInfo.ppEnabledLayerNames = &s_ValidationLayer;

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = DebugCallback;

        createInfo.pNext = &debugCreateInfo;
#else
        createInfo.enabledLayerCount = 0;
#endif

        if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS)
        {
            LUMINA_LOG_ERROR("Failed to create Vulkan instance");
            return false;
        }

        // Initialize Vulkan HPP dispatcher with instance
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Instance, vkGetInstanceProcAddr);

#ifdef LUMINA_DEBUG
        // Create debug messenger
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
        if (func)
        {
            VkDebugUtilsMessengerCreateInfoEXT messengerInfo{};
            messengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            messengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            messengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            messengerInfo.pfnUserCallback = DebugCallback;

            func(m_Instance, &messengerInfo, nullptr, &m_DebugMessenger);
        }
        LUMINA_LOG_INFO("Vulkan validation layers enabled");
#endif

        return true;
    }

    bool VulkanDevice::CreateDevice()
    {
        // Enumerate physical devices
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

        if (deviceCount == 0)
        {
            LUMINA_LOG_ERROR("No Vulkan-capable GPU found");
            return false;
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

        // Find a suitable device
        for (const auto& device : devices)
        {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(device, &properties);

            // Find queue families
            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

            std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

            int graphicsFamily = -1;

            for (uint32_t i = 0; i < queueFamilyCount; ++i)
            {
                if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                    VkBool32 presentSupport = false;
                    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);

                    if (presentSupport)
                    {
                        graphicsFamily = static_cast<int>(i);
                        break;
                    }
                }
            }

            if (graphicsFamily >= 0)
            {
                m_PhysicalDevice = device;
                m_GraphicsQueueFamily = static_cast<uint32_t>(graphicsFamily);

                LUMINA_LOG_INFO("Using GPU: {}", properties.deviceName);
                break;
            }
        }

        if (!m_PhysicalDevice)
        {
            LUMINA_LOG_ERROR("No suitable GPU found");
            return false;
        }

        // Create logical device
        float queuePriority = 1.0f;

        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = m_GraphicsQueueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        // Enable Vulkan 1.2 features required by NVRHI
        VkPhysicalDeviceVulkan12Features vulkan12Features{};
        vulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        vulkan12Features.timelineSemaphore = VK_TRUE;
        vulkan12Features.bufferDeviceAddress = VK_TRUE;
        vulkan12Features.descriptorIndexing = VK_TRUE;
        vulkan12Features.runtimeDescriptorArray = VK_TRUE;
        vulkan12Features.descriptorBindingPartiallyBound = VK_TRUE;
        vulkan12Features.descriptorBindingVariableDescriptorCount = VK_TRUE;
        vulkan12Features.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;

        // Enable Vulkan 1.3 features required by NVRHI
        VkPhysicalDeviceVulkan13Features vulkan13Features{};
        vulkan13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        vulkan13Features.pNext = &vulkan12Features;
        vulkan13Features.synchronization2 = VK_TRUE;
        vulkan13Features.dynamicRendering = VK_TRUE;
        vulkan13Features.maintenance4 = VK_TRUE;

        VkPhysicalDeviceFeatures2 deviceFeatures2{};
        deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        deviceFeatures2.pNext = &vulkan13Features;

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pNext = &deviceFeatures2;
        createInfo.queueCreateInfoCount = 1;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.pEnabledFeatures = nullptr; // Using pNext chain instead
        createInfo.enabledExtensionCount = static_cast<uint32_t>(s_DeviceExtensions.size());
        createInfo.ppEnabledExtensionNames = s_DeviceExtensions.data();

        if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
        {
            LUMINA_LOG_ERROR("Failed to create Vulkan logical device");
            return false;
        }

        // Initialize Vulkan HPP dispatcher with device (full init with all handles)
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Instance, vkGetInstanceProcAddr, m_Device, vkGetDeviceProcAddr);

        vkGetDeviceQueue(m_Device, m_GraphicsQueueFamily, 0, &m_GraphicsQueue);

        return true;
    }

    bool VulkanDevice::CreateSwapchain()
    {
        // Query swapchain support
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &formatCount, nullptr);
        std::vector<VkSurfaceFormatKHR> formats(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &formatCount, formats.data());

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &presentModeCount, nullptr);
        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &presentModeCount, presentModes.data());

        // Choose surface format - prefer UNORM since ImGui outputs pre-gamma-encoded colors
        // Using SRGB would double-encode and cause washed out colors
        VkSurfaceFormatKHR surfaceFormat = formats[0];
        for (const auto& format : formats)
        {
            if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                surfaceFormat = format;
                break;
            }
        }
        m_SwapchainFormat = surfaceFormat.format;

        // Choose present mode
        VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
        if (!m_VSync)
        {
            for (const auto& mode : presentModes)
            {
                if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
                {
                    presentMode = mode;
                    break;
                }
                if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
                {
                    presentMode = mode;
                }
            }
        }

        // Choose extent
        VkExtent2D extent;
        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            extent = capabilities.currentExtent;
        }
        else
        {
            extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, m_Width));
            extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, m_Height));
        }
        m_Width = extent.width;
        m_Height = extent.height;

        uint32_t imageCount = capabilities.minImageCount + 1;
        if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
        {
            imageCount = capabilities.maxImageCount;
        }
        m_BackbufferCount = imageCount;

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = m_Surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.preTransform = capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_Swapchain) != VK_SUCCESS)
        {
            LUMINA_LOG_ERROR("Failed to create Vulkan swapchain");
            return false;
        }

        // Get swapchain images
        vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &imageCount, nullptr);
        m_SwapchainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &imageCount, m_SwapchainImages.data());

        // Create synchronization objects
        m_ImageAvailableSemaphores.resize(m_BackbufferCount);
        m_RenderFinishedSemaphores.resize(m_BackbufferCount);
        m_InFlightFences.resize(m_BackbufferCount);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (uint32_t i = 0; i < m_BackbufferCount; ++i)
        {
            if (vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) != VK_SUCCESS)
            {
                LUMINA_LOG_ERROR("Failed to create image available semaphore {}", i);
                return false;
            }
            if (vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) != VK_SUCCESS)
            {
                LUMINA_LOG_ERROR("Failed to create render finished semaphore {}", i);
                return false;
            }
            if (vkCreateFence(m_Device, &fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS)
            {
                LUMINA_LOG_ERROR("Failed to create in-flight fence {}", i);
                return false;
            }
        }

        return true;
    }

    bool VulkanDevice::CreateFramebuffers()
    {
        m_SwapchainTextures.resize(m_SwapchainImages.size());
        m_SwapchainFramebuffers.resize(m_SwapchainImages.size());

        nvrhi::Format nvrhiFormat = nvrhi::Format::BGRA8_UNORM;
        if (m_SwapchainFormat == VK_FORMAT_R8G8B8A8_SRGB || m_SwapchainFormat == VK_FORMAT_B8G8R8A8_SRGB)
        {
            nvrhiFormat = nvrhi::Format::SBGRA8_UNORM;
        }

        for (size_t i = 0; i < m_SwapchainImages.size(); ++i)
        {
            nvrhi::TextureDesc textureDesc;
            textureDesc.dimension = nvrhi::TextureDimension::Texture2D;
            textureDesc.format = nvrhiFormat;
            textureDesc.width = m_Width;
            textureDesc.height = m_Height;
            textureDesc.isRenderTarget = true;
            textureDesc.debugName = "Swapchain Texture " + std::to_string(i);
            textureDesc.initialState = nvrhi::ResourceStates::Present;
            textureDesc.keepInitialState = true;

            m_SwapchainTextures[i] = m_NvrhiDevice->createHandleForNativeTexture(
                nvrhi::ObjectTypes::VK_Image,
                nvrhi::Object(m_SwapchainImages[i]),
                textureDesc);

            if (!m_SwapchainTextures[i])
            {
                LUMINA_LOG_ERROR("Failed to create NVRHI texture for swapchain image {}", i);
                return false;
            }

            nvrhi::FramebufferDesc fbDesc;
            fbDesc.addColorAttachment(m_SwapchainTextures[i]);

            m_SwapchainFramebuffers[i] = m_NvrhiDevice->createFramebuffer(fbDesc);
            if (!m_SwapchainFramebuffers[i])
            {
                LUMINA_LOG_ERROR("Failed to create framebuffer for swapchain image {}", i);
                return false;
            }
        }

        return true;
    }

    void VulkanDevice::DestroyFramebuffers()
    {
        m_SwapchainFramebuffers.clear();
        m_SwapchainTextures.clear();
    }

    void VulkanDevice::DestroySwapchain()
    {
        for (auto& semaphore : m_ImageAvailableSemaphores)
        {
            if (semaphore) vkDestroySemaphore(m_Device, semaphore, nullptr);
        }
        m_ImageAvailableSemaphores.clear();

        for (auto& semaphore : m_RenderFinishedSemaphores)
        {
            if (semaphore) vkDestroySemaphore(m_Device, semaphore, nullptr);
        }
        m_RenderFinishedSemaphores.clear();

        for (auto& fence : m_InFlightFences)
        {
            if (fence) vkDestroyFence(m_Device, fence, nullptr);
        }
        m_InFlightFences.clear();

        if (m_Swapchain)
        {
            vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
            m_Swapchain = VK_NULL_HANDLE;
        }

        m_SwapchainImages.clear();
    }

    void VulkanDevice::BeginFrame()
    {
        LUMINA_PROFILE_SCOPE_NC("Vulkan::BeginFrame", Profiler::Colors::GPU);

        // Wait for any previous work to complete before acquiring
        vkQueueWaitIdle(m_GraphicsQueue);

        // Run garbage collection to free staging buffers from previous frames
        // This must be called after GPU sync to safely release resources
        m_NvrhiDevice->runGarbageCollection();

        // Reset fence before using it for acquire
        vkResetFences(m_Device, 1, &m_InFlightFences[m_FrameIndex]);

        // Acquire the next swapchain image
        VkResult result = vkAcquireNextImageKHR(m_Device, m_Swapchain, UINT64_MAX,
            VK_NULL_HANDLE, m_InFlightFences[m_FrameIndex], &m_ImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            // Handle resize - will be picked up by resize handler
            return;
        }

        // Wait for acquire to complete
        vkWaitForFences(m_Device, 1, &m_InFlightFences[m_FrameIndex], VK_TRUE, UINT64_MAX);

        m_CommandList->open();
    }

    void VulkanDevice::Present()
    {
        LUMINA_PROFILE_SCOPE_NC("Vulkan::Present", Profiler::Colors::GPU);

        m_CommandList->close();
        m_NvrhiDevice->executeCommandList(m_CommandList);

        // Wait for GPU work to complete before presenting
        vkQueueWaitIdle(m_GraphicsQueue);

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 0;
        presentInfo.pWaitSemaphores = nullptr;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_Swapchain;
        presentInfo.pImageIndices = &m_ImageIndex;

        VkResult result = vkQueuePresentKHR(m_GraphicsQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            // Swapchain needs to be recreated - will be handled by resize
        }

        m_FrameIndex = (m_FrameIndex + 1) % m_BackbufferCount;
    }

    void VulkanDevice::Resize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0)
            return;

        if (width == m_Width && height == m_Height)
            return;

        WaitForGPU();

        DestroyFramebuffers();
        DestroySwapchain();

        m_Width = width;
        m_Height = height;

        CreateSwapchain();
        CreateFramebuffers();

        LUMINA_LOG_INFO("Vulkan swapchain resized to {}x{}", width, height);
    }

    nvrhi::IFramebuffer* VulkanDevice::GetCurrentFramebuffer() const
    {
        return m_SwapchainFramebuffers[m_ImageIndex].Get();
    }

    void VulkanDevice::WaitForGPU()
    {
        vkDeviceWaitIdle(m_Device);
    }

    nvrhi::Format VulkanDevice::GetSwapchainFormat() const
    {
        switch (m_SwapchainFormat)
        {
        case VK_FORMAT_B8G8R8A8_UNORM:
            return nvrhi::Format::BGRA8_UNORM;
        case VK_FORMAT_R8G8B8A8_UNORM:
            return nvrhi::Format::RGBA8_UNORM;
        case VK_FORMAT_B8G8R8A8_SRGB:
            return nvrhi::Format::SBGRA8_UNORM;
        case VK_FORMAT_R8G8B8A8_SRGB:
            return nvrhi::Format::SRGBA8_UNORM;
        default:
            return nvrhi::Format::BGRA8_UNORM;
        }
    }
}
