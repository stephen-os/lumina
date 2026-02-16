#pragma once

#include "types.h"

#include <lumina/core/base.h>

#include <nvrhi/nvrhi.h>

#include <cstdint>
#include <string>
#include <string_view>

namespace lumina::core { class device; }

namespace lumina::graphics
{
    /// Configuration for texture creation.
    /// Note: filter and address modes are sampler properties in modern graphics APIs.
    /// Use the sampler class to configure how textures are sampled.
    struct texture_desc
    {
        uint32_t width = 1;
        uint32_t height = 1;
        format pixel_format = format::rgba8_unorm;
        bool generate_mips = false;
    };

    /// GPU texture for storing 2D image data.
    /// Textures hold the actual pixel data; use sampler for filtering/addressing.
    class texture
    {
    public:
        ~texture();

        texture(const texture&) = delete;
        texture& operator=(const texture&) = delete;

        /// Creates a texture with the specified dimensions and format. Returns nullptr on failure.
        [[nodiscard]] static ref<texture> create(
            core::device& dev,
            uint32_t width,
            uint32_t height,
            format fmt,
            const void* data = nullptr,
            std::string_view debug_name = "Lumina Texture");

        /// Creates a texture from a descriptor. Returns nullptr on failure.
        [[nodiscard]] static ref<texture> create(
            core::device& dev,
            const texture_desc& desc,
            const void* data = nullptr,
            std::string_view debug_name = "Lumina Texture");

        /// Loads a texture from an image file (PNG, JPG, BMP, etc.). Returns nullptr on failure.
        [[nodiscard]] static ref<texture> load_from_file(
            core::device& dev,
            const std::string& path,
            std::string_view debug_name = "");

        /// Wraps an existing NVRHI texture handle. The texture does NOT take ownership.
        [[nodiscard]] static ref<texture> wrap(
            core::device& dev,
            nvrhi::ITexture* handle,
            uint32_t width,
            uint32_t height,
            format fmt);

        [[nodiscard]] uint32_t get_width() const noexcept { return m_width; }
        [[nodiscard]] uint32_t get_height() const noexcept { return m_height; }
        [[nodiscard]] format get_format() const noexcept { return m_format; }
        [[nodiscard]] nvrhi::ITexture* get_texture() const noexcept { return m_handle.Get(); }

    private:
        texture(core::device& dev, nvrhi::TextureHandle handle, uint32_t width, uint32_t height, format fmt)
            : m_device(dev)
            , m_handle(std::move(handle))
            , m_width(width)
            , m_height(height)
            , m_format(fmt)
        {}

        core::device& m_device;
        nvrhi::TextureHandle m_handle;
        uint32_t m_width;
        uint32_t m_height;
        format m_format;
    };
}
