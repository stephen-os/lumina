#include "Texture.h"

#include <Lumina/Core/Device.h>
#include "FormatUtils.h"

#include <Lumina/Core/Log.h>

#include <stb_image.h>

#include <utility>

namespace Lumina
{
    Texture::~Texture() = default;

    Ref<Texture> Texture::Wrap(Device& dev, nvrhi::ITexture* handle, uint32_t width, uint32_t height, Format fmt)
    {
        if (!handle)
            return nullptr;

        // TextureHandle constructor AddRefs automatically, no explicit AddRef needed
        return Ref<Texture>(new Texture(dev, nvrhi::TextureHandle(handle), width, height, fmt));
    }

    Ref<Texture> Texture::Create(Device& dev, uint32_t width, uint32_t height, Format fmt, const void* data, std::string_view debugName)
    {
        TextureDesc desc;
        desc.Width = width;
        desc.Height = height;
        desc.PixelFormat = fmt;
        return Create(dev, desc, data, debugName);
    }

    Ref<Texture> Texture::Create(Device& dev, const TextureDesc& desc, const void* data, std::string_view debugName)
    {
        auto* nvrhiDevice = dev.GetNvrhiDevice();
        if (!nvrhiDevice)
        {
            LUMINA_LOG_ERROR("Failed to create texture: no device");
            return nullptr;
        }

        if (desc.Width == 0)
        {
            LUMINA_LOG_ERROR("Failed to create texture: width cannot be zero");
            return nullptr;
        }

        if (desc.Height == 0)
        {
            LUMINA_LOG_ERROR("Failed to create texture: height cannot be zero");
            return nullptr;
        }

        nvrhi::TextureDesc texDesc;
        texDesc.width = desc.Width;
        texDesc.height = desc.Height;
        texDesc.format = ToNvrhiFormat(desc.PixelFormat);
        texDesc.dimension = nvrhi::TextureDimension::Texture2D;
        texDesc.isShaderResource = true;
        texDesc.debugName = std::string(debugName);
        texDesc.initialState = nvrhi::ResourceStates::ShaderResource;
        texDesc.keepInitialState = true;

        if (desc.GenerateMips)
        {
            uint32_t maxDim = std::max(desc.Width, desc.Height);
            texDesc.mipLevels = static_cast<uint32_t>(std::floor(std::log2(maxDim))) + 1;
            texDesc.isUAV = true;
        }

        if (desc.AllowUAV)
        {
            texDesc.isUAV = true;
            texDesc.initialState = nvrhi::ResourceStates::UnorderedAccess;
        }

        nvrhi::TextureHandle tex = nvrhiDevice->createTexture(texDesc);
        if (!tex)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI texture");
            return nullptr;
        }

        if (data)
        {
            size_t rowPitch = desc.Width * FormatBytesPerPixel(desc.PixelFormat);

            nvrhi::CommandListHandle cmd = nvrhiDevice->createCommandList();
            cmd->open();
            cmd->writeTexture(tex, 0, 0, data, rowPitch);
            cmd->close();
            nvrhiDevice->executeCommandList(cmd);
            nvrhiDevice->waitForIdle();
        }

        return Ref<Texture>(new Texture(dev, std::move(tex), desc.Width, desc.Height, desc.PixelFormat));
    }

    Ref<Texture> Texture::CreateStorage(
        Device& dev,
        uint32_t width,
        uint32_t height,
        Format fmt,
        std::string_view debugName)
    {
        TextureDesc desc;
        desc.Width = width;
        desc.Height = height;
        desc.PixelFormat = fmt;
        desc.AllowUAV = true;
        return Create(dev, desc, nullptr, debugName);
    }

    Ref<Texture> Texture::LoadFromFile(Device& dev, const std::string& path, std::string_view debugName)
    {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(false);
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);  // Force RGBA

        if (!data)
        {
            LUMINA_LOG_ERROR("Failed to load texture: {}", path);
            return nullptr;
        }

        // Use path as debug name if none provided
        std::string_view effectiveDebugName = debugName.empty() ? std::string_view(path) : debugName;

        auto tex = Create(dev, static_cast<uint32_t>(width), static_cast<uint32_t>(height),
                          Format::RGBA8Unorm, data, effectiveDebugName);

        stbi_image_free(data);

        if (!tex)
        {
            LUMINA_LOG_ERROR("Failed to create texture from image: {}", path);
            return nullptr;
        }

        LUMINA_LOG_INFO("Loaded texture: {} ({}x{})", path, width, height);
        return tex;
    }
}
