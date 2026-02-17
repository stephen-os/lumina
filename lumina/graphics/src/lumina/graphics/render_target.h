#pragma once

#include "types.h"

#include <lumina/core/base.h>

#include <nvrhi/nvrhi.h>

#include <cstdint>

namespace lumina::core { class device; }

namespace lumina::graphics
{
    class texture;

    /// Off-screen render target with color and optional depth attachments.
    /// Use for rendering to texture, post-processing, or multi-pass rendering.
    /// Supports MSAA with automatic resolve to non-MSAA texture for sampling.
    class render_target
    {
    public:
        ~render_target();

        render_target(const render_target&) = delete;
        render_target& operator=(const render_target&) = delete;

        /// Creates a render target. Returns nullptr on failure.
        /// @param sample_count MSAA sample count (1 = no MSAA, 2/4/8 for MSAA)
        [[nodiscard]] static ref<render_target> create(
            core::device& dev,
            uint32_t width,
            uint32_t height,
            format color_format,
            format depth_format = format::unknown,
            uint32_t sample_count = 1);

        /// Resizes the render target, recreating internal resources.
        void resize(uint32_t width, uint32_t height);

        /// Resolves MSAA texture to non-MSAA texture for sampling.
        /// Call this after rendering and before sampling the color texture.
        /// No-op if sample_count is 1.
        void resolve(nvrhi::ICommandList* cmd_list);

        [[nodiscard]] uint32_t get_width() const noexcept { return m_width; }
        [[nodiscard]] uint32_t get_height() const noexcept { return m_height; }
        [[nodiscard]] format get_color_format() const noexcept { return m_color_format; }
        [[nodiscard]] format get_depth_format() const noexcept { return m_depth_format; }
        [[nodiscard]] uint32_t get_sample_count() const noexcept { return m_sample_count; }
        [[nodiscard]] bool has_depth() const noexcept { return m_depth_format != format::unknown; }
        [[nodiscard]] bool is_msaa() const noexcept { return m_sample_count > 1; }

        /// Gets the resolved (non-MSAA) color texture for sampling.
        /// For MSAA targets, call resolve() first.
        [[nodiscard]] ref<texture> get_color_texture() const noexcept { return m_resolved_color_texture ? m_resolved_color_texture : m_color_texture; }
        [[nodiscard]] ref<texture> get_depth_texture() const noexcept { return m_depth_texture; }
        [[nodiscard]] nvrhi::IFramebuffer* get_framebuffer() const noexcept { return m_framebuffer.Get(); }

    private:
        render_target(core::device& dev, uint32_t width, uint32_t height, format color_format, format depth_format, uint32_t sample_count)
            : m_device(dev)
            , m_width(width)
            , m_height(height)
            , m_color_format(color_format)
            , m_depth_format(depth_format)
            , m_sample_count(sample_count)
        {}

        bool create_resources();
        void destroy_resources();

        core::device& m_device;
        nvrhi::FramebufferHandle m_framebuffer;
        nvrhi::TextureHandle m_color_texture_handle;        // MSAA or non-MSAA depending on sample_count
        nvrhi::TextureHandle m_depth_texture_handle;        // MSAA or non-MSAA depending on sample_count
        nvrhi::TextureHandle m_resolved_color_handle;       // Non-MSAA resolve target (only if MSAA)
        ref<texture> m_color_texture;                       // Wrapper for MSAA color (render target)
        ref<texture> m_depth_texture;                       // Wrapper for depth
        ref<texture> m_resolved_color_texture;              // Wrapper for resolved color (sampling)
        uint32_t m_width;
        uint32_t m_height;
        format m_color_format;
        format m_depth_format;
        uint32_t m_sample_count = 1;
    };
}
