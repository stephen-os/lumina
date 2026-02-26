// Vulkan HPP Dynamic Dispatcher Storage
// This file provides the storage for the default dynamic dispatch loader
// Required when using VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1

#ifdef _WIN32
    #define VK_USE_PLATFORM_WIN32_KHR
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
#endif

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
