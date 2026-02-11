#pragma once

#include "types.h"

#include <lumina/core/base.h>

#include <cstdint>
#include <string>

namespace nvrhi { class ITexture; }
namespace lumina::core { class device; }

namespace lumina::graphics
{

    struct texture_desc
    {
        uint32_t width = 1;
        uint32_t height = 1;
        format pixel_format = format::rgba8_unorm;
        filter_mode filter = filter_mode::linear;
        address_mode address = address_mode::clamp;
        bool generate_mips = false;
    };

    class texture
    {
    public:
        ~texture();

        texture(const texture&) = delete;
        texture& operator=(const texture&) = delete;

        static ref<texture> create(core::device& dev, uint32_t width, uint32_t height, format fmt, const void* data = nullptr);
        static ref<texture> create(core::device& dev, const texture_desc& desc, const void* data = nullptr);

        // Load texture from image file (PNG, JPG, BMP, etc.)
        static ref<texture> load_from_file(core::device& dev, const std::string& path);

        static ref<texture> wrap(core::device& dev, nvrhi::ITexture* handle, uint32_t width, uint32_t height, format fmt);

        uint32_t get_width() const { return m_width; }
        uint32_t get_height() const { return m_height; }
        format get_format() const { return m_format; }

        nvrhi::ITexture* get_texture() const { return m_handle; }

    private:
        texture(core::device& dev, nvrhi::ITexture* handle, uint32_t width, uint32_t height, format fmt, bool owns_handle = true)
            : m_device(dev)
            , m_handle(handle)
            , m_width(width)
            , m_height(height)
            , m_format(fmt)
            , m_owns_handle(owns_handle)
        {}

        core::device& m_device;
        nvrhi::ITexture* m_handle;
        uint32_t m_width;
        uint32_t m_height;
        format m_format;
        bool m_owns_handle = true;
    };
}
