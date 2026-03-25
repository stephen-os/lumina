#pragma once

#include "Types.h"

#include <lumina/core/Base.h>

#include <nvrhi/nvrhi.h>

#include <cstdint>
#include <string>
#include <string_view>

namespace Lumina { class Device; }

namespace Lumina
{
    /// Configuration for texture creation.
    /// Note: filter and address modes are sampler properties in modern graphics APIs.
    /// Use the sampler class to configure how textures are sampled.
    struct TextureDesc
    {
        uint32_t Width = 1;
        uint32_t Height = 1;
        Format PixelFormat = Format::RGBA8_UNORM;
        bool GenerateMips = false;
        bool AllowUAV = false;     // Enable UAV access for compute shaders
    };

    /// GPU texture for storing 2D image data.
    /// Textures hold the actual pixel data; use sampler for filtering/addressing.
    class Texture
    {
    public:
        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        /// Creates a texture with the specified dimensions and format. Returns nullptr on failure.
        [[nodiscard]] static Ref<Texture> Create(
            Device& dev,
            uint32_t width,
            uint32_t height,
            Format fmt,
            const void* data = nullptr,
            std::string_view debugName = "Lumina Texture");

        /// Creates a texture from a descriptor. Returns nullptr on failure.
        [[nodiscard]] static Ref<Texture> Create(
            Device& dev,
            const TextureDesc& desc,
            const void* data = nullptr,
            std::string_view debugName = "Lumina Texture");

        /// Loads a texture from an image file (PNG, JPG, BMP, etc.). Returns nullptr on failure.
        [[nodiscard]] static Ref<Texture> LoadFromFile(
            Device& dev,
            const std::string& path,
            std::string_view debugName = "");

        /// Wraps an existing NVRHI texture handle. The texture does NOT take ownership.
        [[nodiscard]] static Ref<Texture> Wrap(
            Device& dev,
            nvrhi::ITexture* handle,
            uint32_t width,
            uint32_t height,
            Format fmt);

        /// Creates a texture that can be used as a storage texture (UAV) in compute shaders.
        [[nodiscard]] static Ref<Texture> CreateStorage(
            Device& dev,
            uint32_t width,
            uint32_t height,
            Format fmt = Format::RGBA8_UNORM,
            std::string_view debugName = "Lumina Storage Texture");

        [[nodiscard]] uint32_t GetWidth() const noexcept { return m_Width; }
        [[nodiscard]] uint32_t GetHeight() const noexcept { return m_Height; }
        [[nodiscard]] Format GetFormat() const noexcept { return m_Format; }
        [[nodiscard]] nvrhi::ITexture* GetTexture() const noexcept { return m_Handle.Get(); }

    private:
        Texture(Device& dev, nvrhi::TextureHandle handle, uint32_t width, uint32_t height, Format fmt)
            : m_Device(dev)
            , m_Handle(std::move(handle))
            , m_Width(width)
            , m_Height(height)
            , m_Format(fmt)
        {}

        Device& m_Device;
        nvrhi::TextureHandle m_Handle;
        uint32_t m_Width;
        uint32_t m_Height;
        Format m_Format;
    };
}
