#pragma once

#include <cstdint>
#include <cstddef>

namespace Lumina
{
    // --- Backend ---

    enum class Backend
    {
        D3D12,
        Vulkan
    };

    // --- Texture formats ---

    enum class Format
    {
        Unknown,

        // 8-bit
        R8_UNORM,
        R8_SNORM,
        R8_UINT,
        R8_SINT,

        // 16-bit
        R16_Float,
        R16_UNORM,
        R16_UINT,
        R16_SINT,
        RG8_UNORM,
        RG8_SNORM,

        // 32-bit
        R32_Float,
        R32_UINT,
        R32_SINT,
        RG16_Float,
        RG16_UNORM,
        RGBA8_UNORM,
        RGBA8_UNORM_SRGB,
        RGBA8_SNORM,
        BGRA8_UNORM,
        BGRA8_UNORM_SRGB,

        // 64-bit
        RG32_Float,
        RGBA16_Float,
        RGBA16_UNORM,

        // 128-bit
        RGBA32_Float,

        // Depth/stencil
        D16_UNORM,
        D24_UNORM_S8_UINT,
        D32_Float,
        D32_Float_S8_UINT,
    };

    // --- Blend modes ---

    enum class BlendMode
    {
        Opaque,      // No blending
        Alpha,       // Standard alpha blending: src * src_alpha + dst * (1 - src_alpha)
        Additive,    // Additive: src + dst
        Multiply     // Multiply: src * dst
    };

    // --- Depth modes ---

    enum class DepthMode
    {
        None,        // No depth testing or writing
        ReadOnly,    // Depth test enabled, no writing
        ReadWrite    // Depth test and write enabled
    };

    // --- Cull modes ---

    enum class CullMode
    {
        None,
        Back,
        Front
    };

    // --- Primitive topology ---

    enum class Topology
    {
        Triangles,
        TriangleStrip,
        Lines,
        LineStrip,
        Points
    };

    // --- Buffer usage hints ---

    enum class BufferUsage
    {
        Immutable,   // Data set once at creation, never updated
        Dynamic      // Data updated frequently (per-frame)
    };

    // --- Texture usage flags ---

    enum class TextureUsage : uint32_t
    {
        None            = 0,
        ShaderResource  = 1 << 0,   // Can be sampled in shaders
        RenderTarget    = 1 << 1,   // Can be used as render target
        DepthStencil    = 1 << 2,   // Can be used as depth buffer
    };

    [[nodiscard]] inline TextureUsage operator|(TextureUsage a, TextureUsage b) noexcept
    {
        return static_cast<TextureUsage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    [[nodiscard]] inline TextureUsage operator&(TextureUsage a, TextureUsage b) noexcept
    {
        return static_cast<TextureUsage>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    [[nodiscard]] inline bool HasFlag(TextureUsage value, TextureUsage flag) noexcept
    {
        return (static_cast<uint32_t>(value) & static_cast<uint32_t>(flag)) != 0;
    }

    // --- Filter modes ---

    enum class FilterMode
    {
        Point,       // Nearest neighbor
        Linear,      // Bilinear filtering
        Anisotropic  // Anisotropic filtering
    };

    // --- Address modes ---

    enum class AddressMode
    {
        Wrap,
        Clamp,
        Mirror,
        Border
    };

    // --- Shader stage ---

    enum class ShaderStage
    {
        Vertex,
        Pixel,
        Compute
    };

    // --- Render state ---

    struct RenderState
    {
        BlendMode Blend = BlendMode::Alpha;
        DepthMode Depth = DepthMode::None;
        CullMode Cull = CullMode::None;
        Topology Primitive = Topology::Triangles;
    };

    // --- Viewport ---

    struct Viewport
    {
        float X = 0.0f;
        float Y = 0.0f;
        float Width = 0.0f;
        float Height = 0.0f;
        float MinDepth = 0.0f;
        float MaxDepth = 1.0f;
    };

    // --- Scissor rect ---

    struct ScissorRect
    {
        int32_t X = 0;
        int32_t Y = 0;
        int32_t Width = 0;
        int32_t Height = 0;
    };

    // --- Clear values ---

    struct ClearColor
    {
        float R = 0.0f;
        float G = 0.0f;
        float B = 0.0f;
        float A = 1.0f;

        ClearColor() = default;
        ClearColor(float r, float g, float b, float a = 1.0f) : R(r), G(g), B(b), A(a) {}
    };

    struct ClearDepthStencil
    {
        float Depth = 1.0f;
        uint8_t Stencil = 0;
    };

    // --- Utility functions ---

    constexpr size_t FormatBytesPerPixel(Format fmt)
    {
        switch (fmt)
        {
            case Format::R8_UNORM:
            case Format::R8_SNORM:
            case Format::R8_UINT:
            case Format::R8_SINT:
                return 1;

            case Format::R16_Float:
            case Format::R16_UNORM:
            case Format::R16_UINT:
            case Format::R16_SINT:
            case Format::RG8_UNORM:
            case Format::RG8_SNORM:
            case Format::D16_UNORM:
                return 2;

            case Format::R32_Float:
            case Format::R32_UINT:
            case Format::R32_SINT:
            case Format::RG16_Float:
            case Format::RG16_UNORM:
            case Format::RGBA8_UNORM:
            case Format::RGBA8_UNORM_SRGB:
            case Format::RGBA8_SNORM:
            case Format::BGRA8_UNORM:
            case Format::BGRA8_UNORM_SRGB:
            case Format::D24_UNORM_S8_UINT:
            case Format::D32_Float:
                return 4;

            case Format::RG32_Float:
            case Format::RGBA16_Float:
            case Format::RGBA16_UNORM:
            case Format::D32_Float_S8_UINT:
                return 8;

            case Format::RGBA32_Float:
                return 16;

            default:
                return 0;
        }
    }

    constexpr bool FormatIsDepth(Format fmt)
    {
        switch (fmt)
        {
            case Format::D16_UNORM:
            case Format::D24_UNORM_S8_UINT:
            case Format::D32_Float:
            case Format::D32_Float_S8_UINT:
                return true;
            default:
                return false;
        }
    }

    constexpr bool FormatHasStencil(Format fmt)
    {
        switch (fmt)
        {
            case Format::D24_UNORM_S8_UINT:
            case Format::D32_Float_S8_UINT:
                return true;
            default:
                return false;
        }
    }
}
