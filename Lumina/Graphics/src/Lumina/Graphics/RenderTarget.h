#pragma once

#include "Types.h"

#include <Lumina/Core/Base.h>

#include <nvrhi/nvrhi.h>

#include <cstdint>

namespace Lumina { class Device; }

namespace Lumina
{
    class Texture;

    /// Off-screen render target with color and optional depth attachments.
    /// Use for rendering to texture, post-processing, or multi-pass rendering.
    /// Supports MSAA with automatic resolve to non-MSAA texture for sampling.
    class RenderTarget
    {
    public:
        ~RenderTarget();

        RenderTarget(const RenderTarget&) = delete;
        RenderTarget& operator=(const RenderTarget&) = delete;

        /// Creates a render target. Returns nullptr on failure.
        /// @param sampleCount MSAA sample count (1 = no MSAA, 2/4/8 for MSAA)
        [[nodiscard]] static Ref<RenderTarget> Create(
            Device& dev,
            uint32_t width,
            uint32_t height,
            Format colorFormat,
            Format depthFormat = Format::Unknown,
            uint32_t sampleCount = 1);

        /// Resizes the render target, recreating internal resources.
        void Resize(uint32_t width, uint32_t height);

        /// Resolves MSAA texture to non-MSAA texture for sampling.
        /// Call this after rendering and before sampling the color texture.
        /// No-op if sample_count is 1.
        void Resolve(nvrhi::ICommandList* cmdList);

        [[nodiscard]] uint32_t GetWidth() const noexcept { return m_Width; }
        [[nodiscard]] uint32_t GetHeight() const noexcept { return m_Height; }
        [[nodiscard]] Format GetColorFormat() const noexcept { return m_ColorFormat; }
        [[nodiscard]] Format GetDepthFormat() const noexcept { return m_DepthFormat; }
        [[nodiscard]] uint32_t GetSampleCount() const noexcept { return m_SampleCount; }
        [[nodiscard]] bool HasDepth() const noexcept { return m_DepthFormat != Format::Unknown; }
        [[nodiscard]] bool IsMSAA() const noexcept { return m_SampleCount > 1; }

        /// Gets the resolved (non-MSAA) color texture for sampling.
        /// For MSAA targets, call Resolve() first.
        [[nodiscard]] Ref<Texture> GetColorTexture() const noexcept { return m_ResolvedColorTexture ? m_ResolvedColorTexture : m_ColorTexture; }
        [[nodiscard]] Ref<Texture> GetDepthTexture() const noexcept { return m_DepthTexture; }
        [[nodiscard]] nvrhi::IFramebuffer* GetFramebuffer() const noexcept { return m_Framebuffer.Get(); }

    private:
        RenderTarget(Device& dev, uint32_t width, uint32_t height, Format colorFormat, Format depthFormat, uint32_t sampleCount)
            : m_Device(dev)
            , m_Width(width)
            , m_Height(height)
            , m_ColorFormat(colorFormat)
            , m_DepthFormat(depthFormat)
            , m_SampleCount(sampleCount)
        {}

        bool CreateResources();
        void DestroyResources();

        Device& m_Device;
        nvrhi::FramebufferHandle m_Framebuffer;
        nvrhi::TextureHandle m_ColorTextureHandle;        // MSAA or non-MSAA depending on sample_count
        nvrhi::TextureHandle m_DepthTextureHandle;        // MSAA or non-MSAA depending on sample_count
        nvrhi::TextureHandle m_ResolvedColorHandle;       // Non-MSAA resolve target (only if MSAA)
        Ref<Texture> m_ColorTexture;                      // Wrapper for MSAA color (render target)
        Ref<Texture> m_DepthTexture;                      // Wrapper for depth
        Ref<Texture> m_ResolvedColorTexture;              // Wrapper for resolved color (sampling)
        uint32_t m_Width;
        uint32_t m_Height;
        Format m_ColorFormat;
        Format m_DepthFormat;
        uint32_t m_SampleCount = 1;
    };
}
