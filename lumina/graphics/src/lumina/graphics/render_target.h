#pragma once

#include "types.h"

#include <lumina/core/base.h>

#include <cstdint>

namespace nvrhi { class IFramebuffer; }
namespace lumina::core { class device; }

namespace lumina::graphics
{
    class texture;

    class render_target
    {
    public:
        ~render_target();

        render_target(const render_target&) = delete;
        render_target& operator=(const render_target&) = delete;

        static ref<render_target> create(core::device& dev, uint32_t width, uint32_t height, format color_format, format depth_format = format::unknown);

        void resize(uint32_t width, uint32_t height);

        uint32_t get_width() const { return m_width; }
        uint32_t get_height() const { return m_height; }
        format get_color_format() const { return m_color_format; }
        format get_depth_format() const { return m_depth_format; }
        bool has_depth() const { return m_depth_format != format::unknown; }

        ref<texture> get_color_texture() const { return m_color_texture; }
        ref<texture> get_depth_texture() const { return m_depth_texture; }

        nvrhi::IFramebuffer* get_framebuffer() const { return m_framebuffer; }

    private:
        render_target(core::device& dev, uint32_t width, uint32_t height, format color_format, format depth_format)
            : m_device(dev)
            , m_framebuffer(nullptr)
            , m_width(width)
            , m_height(height)
            , m_color_format(color_format)
            , m_depth_format(depth_format)
        {}

        bool create_resources();
        void destroy_resources();

        core::device& m_device;
        nvrhi::IFramebuffer* m_framebuffer;
        ref<texture> m_color_texture;
        ref<texture> m_depth_texture;
        uint32_t m_width;
        uint32_t m_height;
        format m_color_format;
        format m_depth_format;
    };
}
