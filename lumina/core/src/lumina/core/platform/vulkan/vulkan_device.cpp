#include "vulkan_device.h"

#include "../../log.h"
#include "../../assert.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.hpp>

#include <nvrhi/validation.h>

#include <vector>
#include <set>
#include <cstring>

namespace lumina::core::platform::vulkan
{
    // Validation layer name
    static const char* s_validation_layer = "VK_LAYER_KHRONOS_validation";

    // Required device extensions
    static const std::vector<const char*> s_device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
        void* user_data)
    {
        if (message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
            LUMINA_LOG_ERROR("[Vulkan] {}", callback_data->pMessage);
        else if (message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
            LUMINA_LOG_WARN("[Vulkan] {}", callback_data->pMessage);
        else
            LUMINA_LOG_INFO("[Vulkan] {}", callback_data->pMessage);

        return VK_FALSE;
    }

    void vulkan_message_callback::message(nvrhi::MessageSeverity severity, const char* message_text)
    {
        switch (severity)
        {
        case nvrhi::MessageSeverity::Info:
            LUMINA_LOG_INFO("[NVRHI-VK] {}", message_text);
            break;
        case nvrhi::MessageSeverity::Warning:
            LUMINA_LOG_WARN("[NVRHI-VK] {}", message_text);
            break;
        case nvrhi::MessageSeverity::Error:
            LUMINA_LOG_ERROR("[NVRHI-VK] {}", message_text);
            break;
        case nvrhi::MessageSeverity::Fatal:
            LUMINA_LOG_CRITICAL("[NVRHI-VK] {}", message_text);
            break;
        }
    }

    vulkan_device::~vulkan_device()
    {
        shutdown();
    }

    bool vulkan_device::init(const device_desc& desc)
    {
        m_window = desc.window;
        m_width = desc.width;
        m_height = desc.height;
        m_backbuffer_count = desc.backbuffer_count;
        m_vsync = desc.vsync;

        // Initialize Vulkan HPP dispatcher (for pre-instance functions like vkCreateInstance)
        VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

        if (!create_instance())
        {
            LUMINA_LOG_ERROR("Failed to create Vulkan instance");
            return false;
        }

        // Create window surface
        if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) != VK_SUCCESS)
        {
            LUMINA_LOG_ERROR("Failed to create Vulkan window surface");
            return false;
        }

        if (!create_device())
        {
            LUMINA_LOG_ERROR("Failed to create Vulkan device");
            return false;
        }

        if (!create_swapchain())
        {
            LUMINA_LOG_ERROR("Failed to create Vulkan swapchain");
            return false;
        }

        // Create NVRHI device wrapper
        nvrhi::vulkan::DeviceDesc nvrhi_desc;
        nvrhi_desc.errorCB = &m_message_callback;
        nvrhi_desc.instance = m_instance;
        nvrhi_desc.physicalDevice = m_physical_device;
        nvrhi_desc.device = m_device;
        nvrhi_desc.graphicsQueue = m_graphics_queue;
        nvrhi_desc.graphicsQueueIndex = static_cast<int>(m_graphics_queue_family);
        nvrhi_desc.deviceExtensions = const_cast<const char**>(s_device_extensions.data());
        nvrhi_desc.numDeviceExtensions = s_device_extensions.size();

        m_nvrhi_device = nvrhi::vulkan::createDevice(nvrhi_desc);
        if (!m_nvrhi_device)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI Vulkan device");
            return false;
        }

#ifdef LUMINA_DEBUG
        // Wrap with validation layer in debug builds
        nvrhi::DeviceHandle validation_layer = nvrhi::validation::createValidationLayer(m_nvrhi_device);
        m_nvrhi_device = validation_layer;
#endif

        // Create command list
        m_command_list = m_nvrhi_device->createCommandList();
        if (!m_command_list)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI command list");
            return false;
        }

        if (!create_framebuffers())
        {
            LUMINA_LOG_ERROR("Failed to create framebuffers");
            return false;
        }

        LUMINA_LOG_INFO("Vulkan graphics device initialized successfully");
        return true;
    }

    void vulkan_device::shutdown()
    {
        if (m_device)
        {
            vkDeviceWaitIdle(m_device);
        }

        destroy_framebuffers();
        destroy_swapchain();

        m_command_list = nullptr;
        m_nvrhi_device = nullptr;

        if (m_device)
        {
            vkDestroyDevice(m_device, nullptr);
            m_device = VK_NULL_HANDLE;
        }

        if (m_surface)
        {
            vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
            m_surface = VK_NULL_HANDLE;
        }

#ifdef LUMINA_DEBUG
        if (m_debug_messenger)
        {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT");
            if (func)
                func(m_instance, m_debug_messenger, nullptr);
            m_debug_messenger = VK_NULL_HANDLE;
        }
#endif

        if (m_instance)
        {
            vkDestroyInstance(m_instance, nullptr);
            m_instance = VK_NULL_HANDLE;
        }

        LUMINA_LOG_INFO("Vulkan graphics device shutdown");
    }

    bool vulkan_device::create_instance()
    {
        VkApplicationInfo app_info{};
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName = "Lumina Application";
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName = "Lumina";
        app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion = VK_API_VERSION_1_3;

        // Get required extensions from GLFW
        uint32_t glfw_extension_count = 0;
        const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

        std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

#ifdef LUMINA_DEBUG
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        VkInstanceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &app_info;
        create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        create_info.ppEnabledExtensionNames = extensions.data();

#ifdef LUMINA_DEBUG
        // Enable validation layers in debug mode
        create_info.enabledLayerCount = 1;
        create_info.ppEnabledLayerNames = &s_validation_layer;

        VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
        debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debug_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debug_create_info.pfnUserCallback = debug_callback;

        create_info.pNext = &debug_create_info;
#else
        create_info.enabledLayerCount = 0;
#endif

        if (vkCreateInstance(&create_info, nullptr, &m_instance) != VK_SUCCESS)
        {
            LUMINA_LOG_ERROR("Failed to create Vulkan instance");
            return false;
        }

        // Initialize Vulkan HPP dispatcher with instance
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_instance, vkGetInstanceProcAddr);

#ifdef LUMINA_DEBUG
        // Create debug messenger
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");
        if (func)
        {
            VkDebugUtilsMessengerCreateInfoEXT messenger_info{};
            messenger_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            messenger_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            messenger_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            messenger_info.pfnUserCallback = debug_callback;

            func(m_instance, &messenger_info, nullptr, &m_debug_messenger);
        }
        LUMINA_LOG_INFO("Vulkan validation layers enabled");
#endif

        return true;
    }

    bool vulkan_device::create_device()
    {
        // Enumerate physical devices
        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);

        if (device_count == 0)
        {
            LUMINA_LOG_ERROR("No Vulkan-capable GPU found");
            return false;
        }

        std::vector<VkPhysicalDevice> devices(device_count);
        vkEnumeratePhysicalDevices(m_instance, &device_count, devices.data());

        // Find a suitable device
        for (const auto& device : devices)
        {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(device, &properties);

            // Find queue families
            uint32_t queue_family_count = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

            std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

            int graphics_family = -1;

            for (uint32_t i = 0; i < queue_family_count; ++i)
            {
                if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                    VkBool32 present_support = false;
                    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &present_support);

                    if (present_support)
                    {
                        graphics_family = static_cast<int>(i);
                        break;
                    }
                }
            }

            if (graphics_family >= 0)
            {
                m_physical_device = device;
                m_graphics_queue_family = static_cast<uint32_t>(graphics_family);

                LUMINA_LOG_INFO("Using GPU: {}", properties.deviceName);
                break;
            }
        }

        if (!m_physical_device)
        {
            LUMINA_LOG_ERROR("No suitable GPU found");
            return false;
        }

        // Create logical device
        float queue_priority = 1.0f;

        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = m_graphics_queue_family;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;

        // Enable Vulkan 1.2 features required by NVRHI
        VkPhysicalDeviceVulkan12Features vulkan12_features{};
        vulkan12_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        vulkan12_features.timelineSemaphore = VK_TRUE;
        vulkan12_features.bufferDeviceAddress = VK_TRUE;
        vulkan12_features.descriptorIndexing = VK_TRUE;
        vulkan12_features.runtimeDescriptorArray = VK_TRUE;
        vulkan12_features.descriptorBindingPartiallyBound = VK_TRUE;
        vulkan12_features.descriptorBindingVariableDescriptorCount = VK_TRUE;
        vulkan12_features.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;

        // Enable Vulkan 1.3 features required by NVRHI
        VkPhysicalDeviceVulkan13Features vulkan13_features{};
        vulkan13_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        vulkan13_features.pNext = &vulkan12_features;
        vulkan13_features.synchronization2 = VK_TRUE;
        vulkan13_features.dynamicRendering = VK_TRUE;
        vulkan13_features.maintenance4 = VK_TRUE;

        VkPhysicalDeviceFeatures2 device_features2{};
        device_features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        device_features2.pNext = &vulkan13_features;

        VkDeviceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        create_info.pNext = &device_features2;
        create_info.queueCreateInfoCount = 1;
        create_info.pQueueCreateInfos = &queue_create_info;
        create_info.pEnabledFeatures = nullptr; // Using pNext chain instead
        create_info.enabledExtensionCount = static_cast<uint32_t>(s_device_extensions.size());
        create_info.ppEnabledExtensionNames = s_device_extensions.data();

        if (vkCreateDevice(m_physical_device, &create_info, nullptr, &m_device) != VK_SUCCESS)
        {
            LUMINA_LOG_ERROR("Failed to create Vulkan logical device");
            return false;
        }

        // Initialize Vulkan HPP dispatcher with device (full init with all handles)
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_instance, vkGetInstanceProcAddr, m_device, vkGetDeviceProcAddr);

        vkGetDeviceQueue(m_device, m_graphics_queue_family, 0, &m_graphics_queue);

        return true;
    }

    bool vulkan_device::create_swapchain()
    {
        // Query swapchain support
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physical_device, m_surface, &capabilities);

        uint32_t format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_physical_device, m_surface, &format_count, nullptr);
        std::vector<VkSurfaceFormatKHR> formats(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_physical_device, m_surface, &format_count, formats.data());

        uint32_t present_mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_physical_device, m_surface, &present_mode_count, nullptr);
        std::vector<VkPresentModeKHR> present_modes(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_physical_device, m_surface, &present_mode_count, present_modes.data());

        // Choose surface format - prefer UNORM since ImGui outputs pre-gamma-encoded colors
        // Using SRGB would double-encode and cause washed out colors
        VkSurfaceFormatKHR surface_format = formats[0];
        for (const auto& format : formats)
        {
            if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                surface_format = format;
                break;
            }
        }
        m_swapchain_format = surface_format.format;

        // Choose present mode
        VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
        if (!m_vsync)
        {
            for (const auto& mode : present_modes)
            {
                if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
                {
                    present_mode = mode;
                    break;
                }
                if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
                {
                    present_mode = mode;
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
            extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, m_width));
            extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, m_height));
        }
        m_width = extent.width;
        m_height = extent.height;

        uint32_t image_count = capabilities.minImageCount + 1;
        if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount)
        {
            image_count = capabilities.maxImageCount;
        }
        m_backbuffer_count = image_count;

        VkSwapchainCreateInfoKHR create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        create_info.surface = m_surface;
        create_info.minImageCount = image_count;
        create_info.imageFormat = surface_format.format;
        create_info.imageColorSpace = surface_format.colorSpace;
        create_info.imageExtent = extent;
        create_info.imageArrayLayers = 1;
        create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.preTransform = capabilities.currentTransform;
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.presentMode = present_mode;
        create_info.clipped = VK_TRUE;
        create_info.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(m_device, &create_info, nullptr, &m_swapchain) != VK_SUCCESS)
        {
            LUMINA_LOG_ERROR("Failed to create Vulkan swapchain");
            return false;
        }

        // Get swapchain images
        vkGetSwapchainImagesKHR(m_device, m_swapchain, &image_count, nullptr);
        m_swapchain_images.resize(image_count);
        vkGetSwapchainImagesKHR(m_device, m_swapchain, &image_count, m_swapchain_images.data());

        // Create synchronization objects
        m_image_available_semaphores.resize(m_backbuffer_count);
        m_render_finished_semaphores.resize(m_backbuffer_count);
        m_in_flight_fences.resize(m_backbuffer_count);

        VkSemaphoreCreateInfo semaphore_info{};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fence_info{};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (uint32_t i = 0; i < m_backbuffer_count; ++i)
        {
            if (vkCreateSemaphore(m_device, &semaphore_info, nullptr, &m_image_available_semaphores[i]) != VK_SUCCESS)
            {
                LUMINA_LOG_ERROR("Failed to create image available semaphore {}", i);
                return false;
            }
            if (vkCreateSemaphore(m_device, &semaphore_info, nullptr, &m_render_finished_semaphores[i]) != VK_SUCCESS)
            {
                LUMINA_LOG_ERROR("Failed to create render finished semaphore {}", i);
                return false;
            }
            if (vkCreateFence(m_device, &fence_info, nullptr, &m_in_flight_fences[i]) != VK_SUCCESS)
            {
                LUMINA_LOG_ERROR("Failed to create in-flight fence {}", i);
                return false;
            }
        }

        return true;
    }

    bool vulkan_device::create_framebuffers()
    {
        m_swapchain_textures.resize(m_swapchain_images.size());
        m_swapchain_framebuffers.resize(m_swapchain_images.size());

        nvrhi::Format nvrhi_format = nvrhi::Format::BGRA8_UNORM;
        if (m_swapchain_format == VK_FORMAT_R8G8B8A8_SRGB || m_swapchain_format == VK_FORMAT_B8G8R8A8_SRGB)
        {
            nvrhi_format = nvrhi::Format::SBGRA8_UNORM;
        }

        for (size_t i = 0; i < m_swapchain_images.size(); ++i)
        {
            nvrhi::TextureDesc texture_desc;
            texture_desc.dimension = nvrhi::TextureDimension::Texture2D;
            texture_desc.format = nvrhi_format;
            texture_desc.width = m_width;
            texture_desc.height = m_height;
            texture_desc.isRenderTarget = true;
            texture_desc.debugName = "Swapchain Texture " + std::to_string(i);
            texture_desc.initialState = nvrhi::ResourceStates::Present;
            texture_desc.keepInitialState = true;

            m_swapchain_textures[i] = m_nvrhi_device->createHandleForNativeTexture(
                nvrhi::ObjectTypes::VK_Image,
                nvrhi::Object(m_swapchain_images[i]),
                texture_desc);

            if (!m_swapchain_textures[i])
            {
                LUMINA_LOG_ERROR("Failed to create NVRHI texture for swapchain image {}", i);
                return false;
            }

            nvrhi::FramebufferDesc fb_desc;
            fb_desc.addColorAttachment(m_swapchain_textures[i]);

            m_swapchain_framebuffers[i] = m_nvrhi_device->createFramebuffer(fb_desc);
            if (!m_swapchain_framebuffers[i])
            {
                LUMINA_LOG_ERROR("Failed to create framebuffer for swapchain image {}", i);
                return false;
            }
        }

        return true;
    }

    void vulkan_device::destroy_framebuffers()
    {
        m_swapchain_framebuffers.clear();
        m_swapchain_textures.clear();
    }

    void vulkan_device::destroy_swapchain()
    {
        for (auto& semaphore : m_image_available_semaphores)
        {
            if (semaphore) vkDestroySemaphore(m_device, semaphore, nullptr);
        }
        m_image_available_semaphores.clear();

        for (auto& semaphore : m_render_finished_semaphores)
        {
            if (semaphore) vkDestroySemaphore(m_device, semaphore, nullptr);
        }
        m_render_finished_semaphores.clear();

        for (auto& fence : m_in_flight_fences)
        {
            if (fence) vkDestroyFence(m_device, fence, nullptr);
        }
        m_in_flight_fences.clear();

        if (m_swapchain)
        {
            vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
            m_swapchain = VK_NULL_HANDLE;
        }

        m_swapchain_images.clear();
    }

    void vulkan_device::begin_frame()
    {
        // Wait for any previous work to complete before acquiring
        vkQueueWaitIdle(m_graphics_queue);

        // Run garbage collection to free staging buffers from previous frames
        // This must be called after GPU sync to safely release resources
        m_nvrhi_device->runGarbageCollection();

        // Reset fence before using it for acquire
        vkResetFences(m_device, 1, &m_in_flight_fences[m_frame_index]);

        // Acquire the next swapchain image
        VkResult result = vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX,
            VK_NULL_HANDLE, m_in_flight_fences[m_frame_index], &m_image_index);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            // Handle resize - will be picked up by resize handler
            return;
        }

        // Wait for acquire to complete
        vkWaitForFences(m_device, 1, &m_in_flight_fences[m_frame_index], VK_TRUE, UINT64_MAX);

        m_command_list->open();
    }

    void vulkan_device::present()
    {
        m_command_list->close();
        m_nvrhi_device->executeCommandList(m_command_list);

        // Wait for GPU work to complete before presenting
        vkQueueWaitIdle(m_graphics_queue);

        VkPresentInfoKHR present_info{};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 0;
        present_info.pWaitSemaphores = nullptr;
        present_info.swapchainCount = 1;
        present_info.pSwapchains = &m_swapchain;
        present_info.pImageIndices = &m_image_index;

        VkResult result = vkQueuePresentKHR(m_graphics_queue, &present_info);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            // Swapchain needs to be recreated - will be handled by resize
        }

        m_frame_index = (m_frame_index + 1) % m_backbuffer_count;
    }

    void vulkan_device::resize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0)
            return;

        if (width == m_width && height == m_height)
            return;

        wait_for_gpu();

        destroy_framebuffers();
        destroy_swapchain();

        m_width = width;
        m_height = height;

        create_swapchain();
        create_framebuffers();

        LUMINA_LOG_INFO("Vulkan swapchain resized to {}x{}", width, height);
    }

    nvrhi::IFramebuffer* vulkan_device::get_current_framebuffer() const
    {
        return m_swapchain_framebuffers[m_image_index].Get();
    }

    void vulkan_device::wait_for_gpu()
    {
        vkDeviceWaitIdle(m_device);
    }

    nvrhi::Format vulkan_device::get_swapchain_format() const
    {
        switch (m_swapchain_format)
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
