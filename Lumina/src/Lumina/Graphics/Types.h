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
        R8Unorm,
        R8Snorm,
        R8Uint,
        R8Sint,

        // 16-bit
        R16Float,
        R16Unorm,
        R16Uint,
        R16Sint,
        RG8Unorm,
        RG8Snorm,

        // 32-bit
        R32Float,
        R32Uint,
        R32Sint,
        RG16Float,
        RG16Unorm,
        RGBA8Unorm,
        RGBA8UnormSrgb,
        RGBA8Snorm,
        BGRA8Unorm,
        BGRA8UnormSrgb,

        // 64-bit
        RG32Float,
        RGBA16Float,
        RGBA16Unorm,

        // 128-bit
        RGBA32Float,

        // Depth/stencil
        D16Unorm,
        D24UnormS8Uint,
        D32Float,
        D32FloatS8Uint,
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
            case Format::R8Unorm:
            case Format::R8Snorm:
            case Format::R8Uint:
            case Format::R8Sint:
                return 1;

            case Format::R16Float:
            case Format::R16Unorm:
            case Format::R16Uint:
            case Format::R16Sint:
            case Format::RG8Unorm:
            case Format::RG8Snorm:
            case Format::D16Unorm:
                return 2;

            case Format::R32Float:
            case Format::R32Uint:
            case Format::R32Sint:
            case Format::RG16Float:
            case Format::RG16Unorm:
            case Format::RGBA8Unorm:
            case Format::RGBA8UnormSrgb:
            case Format::RGBA8Snorm:
            case Format::BGRA8Unorm:
            case Format::BGRA8UnormSrgb:
            case Format::D24UnormS8Uint:
            case Format::D32Float:
                return 4;

            case Format::RG32Float:
            case Format::RGBA16Float:
            case Format::RGBA16Unorm:
            case Format::D32FloatS8Uint:
                return 8;

            case Format::RGBA32Float:
                return 16;

            default:
                return 0;
        }
    }

    constexpr bool FormatIsDepth(Format fmt)
    {
        switch (fmt)
        {
            case Format::D16Unorm:
            case Format::D24UnormS8Uint:
            case Format::D32Float:
            case Format::D32FloatS8Uint:
                return true;
            default:
                return false;
        }
    }

    constexpr bool FormatHasStencil(Format fmt)
    {
        switch (fmt)
        {
            case Format::D24UnormS8Uint:
            case Format::D32FloatS8Uint:
                return true;
            default:
                return false;
        }
    }
}
