#pragma once

// Tracy profiler integration for Lumina engine
// Provides LUMINA_PROFILE_* macros that wrap Tracy functionality
// These compile to nothing when TRACY_ENABLE is not defined

#ifdef TRACY_ENABLE
    #include <tracy/Tracy.hpp>

    // NOTE: GPU-specific Tracy headers (TracyD3D12.hpp, TracyVulkan.hpp) are NOT
    // included here. They must be included manually AFTER the respective graphics
    // API headers in files that use GPU profiling. See:
    //   - platform/d3d12/d3d12_device.h
    //   - platform/vulkan/vulkan_device.h
#endif

// =============================================================================
// CPU Profiling Macros
// =============================================================================

#ifdef TRACY_ENABLE
    // Basic scoped zone (uses function name automatically)
    #define LUMINA_PROFILE_SCOPE()                  ZoneScoped

    // Named scoped zone
    #define LUMINA_PROFILE_SCOPE_N(name)            ZoneScopedN(name)

    // Colored scoped zone
    #define LUMINA_PROFILE_SCOPE_C(color)           ZoneScopedC(color)

    // Named + colored scoped zone
    #define LUMINA_PROFILE_SCOPE_NC(name, color)    ZoneScopedNC(name, color)

    // Frame marker (call at end of main loop)
    #define LUMINA_PROFILE_FRAME()                  FrameMark

    // Named frame marker (for secondary frame loops)
    #define LUMINA_PROFILE_FRAME_N(name)            FrameMarkNamed(name)

    // Plot a value (for memory stats, draw calls, etc.)
    #define LUMINA_PROFILE_PLOT(name, value)        TracyPlot(name, value)

    // Configure a plot (call once at startup)
    #define LUMINA_PROFILE_PLOT_CONFIG(name, type, step, fill, color) \
        TracyPlotConfig(name, type, step, fill, color)

    // Message logging
    #define LUMINA_PROFILE_MESSAGE(text)            TracyMessageL(text)
    #define LUMINA_PROFILE_MESSAGE_C(text, color)   TracyMessageLC(text, color)

#else
    #define LUMINA_PROFILE_SCOPE()
    #define LUMINA_PROFILE_SCOPE_N(name)
    #define LUMINA_PROFILE_SCOPE_C(color)
    #define LUMINA_PROFILE_SCOPE_NC(name, color)
    #define LUMINA_PROFILE_FRAME()
    #define LUMINA_PROFILE_FRAME_N(name)
    #define LUMINA_PROFILE_PLOT(name, value)
    #define LUMINA_PROFILE_PLOT_CONFIG(name, type, step, fill, color)
    #define LUMINA_PROFILE_MESSAGE(text)
    #define LUMINA_PROFILE_MESSAGE_C(text, color)
#endif

// =============================================================================
// GPU Profiling Macros (D3D12)
// =============================================================================

#ifdef TRACY_ENABLE
    #define LUMINA_PROFILE_GPU_D3D12_CONTEXT(device, queue) \
        TracyD3D12Context(device, queue)

    #define LUMINA_PROFILE_GPU_D3D12_DESTROY(ctx) \
        TracyD3D12Destroy(ctx)

    #define LUMINA_PROFILE_GPU_D3D12_CONTEXT_NAME(ctx, name) \
        TracyD3D12ContextName(ctx, name, strlen(name))

    #define LUMINA_PROFILE_GPU_D3D12_ZONE(ctx, cmdList, name) \
        TracyD3D12Zone(ctx, cmdList, name)

    #define LUMINA_PROFILE_GPU_D3D12_ZONE_C(ctx, cmdList, name, color) \
        TracyD3D12ZoneC(ctx, cmdList, name, color)

    #define LUMINA_PROFILE_GPU_D3D12_COLLECT(ctx) \
        TracyD3D12Collect(ctx)

    #define LUMINA_PROFILE_GPU_D3D12_NEW_FRAME(ctx) \
        TracyD3D12NewFrame(ctx)
#else
    #define LUMINA_PROFILE_GPU_D3D12_CONTEXT(device, queue) nullptr
    #define LUMINA_PROFILE_GPU_D3D12_DESTROY(ctx)
    #define LUMINA_PROFILE_GPU_D3D12_CONTEXT_NAME(ctx, name)
    #define LUMINA_PROFILE_GPU_D3D12_ZONE(ctx, cmdList, name)
    #define LUMINA_PROFILE_GPU_D3D12_ZONE_C(ctx, cmdList, name, color)
    #define LUMINA_PROFILE_GPU_D3D12_COLLECT(ctx)
    #define LUMINA_PROFILE_GPU_D3D12_NEW_FRAME(ctx)
#endif

// =============================================================================
// GPU Profiling Macros (Vulkan)
// =============================================================================

#ifdef TRACY_ENABLE
    #define LUMINA_PROFILE_GPU_VK_CONTEXT(physDev, device, queue, cmdBuf) \
        TracyVkContext(physDev, device, queue, cmdBuf)

    #define LUMINA_PROFILE_GPU_VK_DESTROY(ctx) \
        TracyVkDestroy(ctx)

    #define LUMINA_PROFILE_GPU_VK_CONTEXT_NAME(ctx, name) \
        TracyVkContextName(ctx, name, strlen(name))

    #define LUMINA_PROFILE_GPU_VK_ZONE(ctx, cmdBuf, name) \
        TracyVkZone(ctx, cmdBuf, name)

    #define LUMINA_PROFILE_GPU_VK_ZONE_C(ctx, cmdBuf, name, color) \
        TracyVkZoneC(ctx, cmdBuf, name, color)

    #define LUMINA_PROFILE_GPU_VK_COLLECT(ctx, cmdBuf) \
        TracyVkCollect(ctx, cmdBuf)
#else
    #define LUMINA_PROFILE_GPU_VK_CONTEXT(physDev, device, queue, cmdBuf) nullptr
    #define LUMINA_PROFILE_GPU_VK_DESTROY(ctx)
    #define LUMINA_PROFILE_GPU_VK_CONTEXT_NAME(ctx, name)
    #define LUMINA_PROFILE_GPU_VK_ZONE(ctx, cmdBuf, name)
    #define LUMINA_PROFILE_GPU_VK_ZONE_C(ctx, cmdBuf, name, color)
    #define LUMINA_PROFILE_GPU_VK_COLLECT(ctx, cmdBuf)
#endif

// =============================================================================
// Profiler Color Constants
// =============================================================================

namespace lumina::profiler
{
    namespace colors
    {
#ifdef TRACY_ENABLE
        constexpr uint32_t frame    = 0x4444FF;  // Blue - frame boundaries
        constexpr uint32_t render   = 0x44FF44;  // Green - rendering operations
        constexpr uint32_t gpu      = 0xFF4444;  // Red - GPU operations
        constexpr uint32_t physics  = 0xFFFF44;  // Yellow - physics
        constexpr uint32_t ui       = 0xFF44FF;  // Magenta - UI/ImGui
        constexpr uint32_t audio    = 0x44FFFF;  // Cyan - audio
        constexpr uint32_t io       = 0xFF8844;  // Orange - I/O operations
        constexpr uint32_t memory   = 0x8844FF;  // Purple - memory operations
#else
        constexpr uint32_t frame    = 0;
        constexpr uint32_t render   = 0;
        constexpr uint32_t gpu      = 0;
        constexpr uint32_t physics  = 0;
        constexpr uint32_t ui       = 0;
        constexpr uint32_t audio    = 0;
        constexpr uint32_t io       = 0;
        constexpr uint32_t memory   = 0;
#endif
    }
}
