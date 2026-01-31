#pragma once

#include <cstdint>
#include <cstddef>

namespace lumina::graphics
{
    // --- Backend ---

    enum class backend
    {
        d3d12,
        vulkan
    };

    // --- Texture formats ---

    enum class format
    {
        unknown,

        // 8-bit
        r8_unorm,
        r8_snorm,
        r8_uint,
        r8_sint,

        // 16-bit
        r16_float,
        r16_unorm,
        r16_uint,
        r16_sint,
        rg8_unorm,
        rg8_snorm,

        // 32-bit
        r32_float,
        r32_uint,
        r32_sint,
        rg16_float,
        rg16_unorm,
        rgba8_unorm,
        rgba8_unorm_srgb,
        rgba8_snorm,
        bgra8_unorm,
        bgra8_unorm_srgb,

        // 64-bit
        rg32_float,
        rgba16_float,
        rgba16_unorm,

        // 128-bit
        rgba32_float,

        // Depth/stencil
        d16_unorm,
        d24_unorm_s8_uint,
        d32_float,
        d32_float_s8_uint,
    };

    // --- Blend modes ---

    enum class blend_mode
    {
        opaque,      // No blending
        alpha,       // Standard alpha blending: src * src_alpha + dst * (1 - src_alpha)
        additive,    // Additive: src + dst
        multiply     // Multiply: src * dst
    };

    // --- Depth modes ---

    enum class depth_mode
    {
        none,        // No depth testing or writing
        read_only,   // Depth test enabled, no writing
        read_write   // Depth test and write enabled
    };

    // --- Cull modes ---

    enum class cull_mode
    {
        none,
        back,
        front
    };

    // --- Primitive topology ---

    enum class topology
    {
        triangles,
        triangle_strip,
        lines,
        line_strip,
        points
    };

    // --- Buffer usage hints ---

    enum class buffer_usage
    {
        immutable,   // Data set once at creation, never updated
        dynamic      // Data updated frequently (per-frame)
    };

    // --- Texture usage flags ---

    enum class texture_usage : uint32_t
    {
        none            = 0,
        shader_resource = 1 << 0,   // Can be sampled in shaders
        render_target   = 1 << 1,   // Can be used as render target
        depth_stencil   = 1 << 2,   // Can be used as depth buffer
    };

    inline texture_usage operator|(texture_usage a, texture_usage b)
    {
        return static_cast<texture_usage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline texture_usage operator&(texture_usage a, texture_usage b)
    {
        return static_cast<texture_usage>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    inline bool has_flag(texture_usage value, texture_usage flag)
    {
        return (static_cast<uint32_t>(value) & static_cast<uint32_t>(flag)) != 0;
    }

    // --- Filter modes ---

    enum class filter_mode
    {
        point,       // Nearest neighbor
        linear,      // Bilinear filtering
        anisotropic  // Anisotropic filtering
    };

    // --- Address modes ---

    enum class address_mode
    {
        wrap,
        clamp,
        mirror,
        border
    };

    // --- Shader stage ---

    enum class shader_stage
    {
        vertex,
        pixel,
        compute
    };

    // --- Render state ---

    struct render_state
    {
        blend_mode blend = blend_mode::alpha;
        depth_mode depth = depth_mode::none;
        cull_mode cull = cull_mode::none;
        topology primitive = topology::triangles;
    };

    // --- Viewport ---

    struct viewport
    {
        float x = 0.0f;
        float y = 0.0f;
        float width = 0.0f;
        float height = 0.0f;
        float min_depth = 0.0f;
        float max_depth = 1.0f;
    };

    // --- Scissor rect ---

    struct scissor_rect
    {
        int32_t x = 0;
        int32_t y = 0;
        int32_t width = 0;
        int32_t height = 0;
    };

    // --- Clear values ---

    struct clear_color
    {
        float r = 0.0f;
        float g = 0.0f;
        float b = 0.0f;
        float a = 1.0f;

        clear_color() = default;
        clear_color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
    };

    struct clear_depth_stencil
    {
        float depth = 1.0f;
        uint8_t stencil = 0;
    };

    // --- Utility functions ---

    constexpr size_t format_bytes_per_pixel(format fmt)
    {
        switch (fmt)
        {
            case format::r8_unorm:
            case format::r8_snorm:
            case format::r8_uint:
            case format::r8_sint:
                return 1;

            case format::r16_float:
            case format::r16_unorm:
            case format::r16_uint:
            case format::r16_sint:
            case format::rg8_unorm:
            case format::rg8_snorm:
            case format::d16_unorm:
                return 2;

            case format::r32_float:
            case format::r32_uint:
            case format::r32_sint:
            case format::rg16_float:
            case format::rg16_unorm:
            case format::rgba8_unorm:
            case format::rgba8_unorm_srgb:
            case format::rgba8_snorm:
            case format::bgra8_unorm:
            case format::bgra8_unorm_srgb:
            case format::d24_unorm_s8_uint:
            case format::d32_float:
                return 4;

            case format::rg32_float:
            case format::rgba16_float:
            case format::rgba16_unorm:
            case format::d32_float_s8_uint:
                return 8;

            case format::rgba32_float:
                return 16;

            default:
                return 0;
        }
    }

    constexpr bool format_is_depth(format fmt)
    {
        switch (fmt)
        {
            case format::d16_unorm:
            case format::d24_unorm_s8_uint:
            case format::d32_float:
            case format::d32_float_s8_uint:
                return true;
            default:
                return false;
        }
    }

    constexpr bool format_has_stencil(format fmt)
    {
        switch (fmt)
        {
            case format::d24_unorm_s8_uint:
            case format::d32_float_s8_uint:
                return true;
            default:
                return false;
        }
    }
}
