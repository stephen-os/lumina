// NVRHI Platform Configuration Header
// This file is force-included before all NVRHI source files to ensure
// platform defines are set before Vulkan headers are included.

#pragma once

#ifdef _WIN32
    #define VK_USE_PLATFORM_WIN32_KHR
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
#endif
