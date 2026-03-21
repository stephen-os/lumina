#include "texture.h"

#include <lumina/core/device.h>
#include "format_utils.h"

#include <lumina/core/log.h>

#include <stb_image.h>

#include <utility>

namespace lumina::graphics
{
    texture::~texture() = default;

    ref<texture> texture::wrap(core::device& dev, nvrhi::ITexture* handle, uint32_t width, uint32_t height, format fmt)
    {
        if (!handle)
            return nullptr;

        // TextureHandle constructor AddRefs automatically, no explicit AddRef needed
        return ref<texture>(new texture(dev, nvrhi::TextureHandle(handle), width, height, fmt));
    }

    ref<texture> texture::create(core::device& dev, uint32_t width, uint32_t height, format fmt, const void* data, std::string_view debug_name)
    {
        texture_desc desc;
        desc.width = width;
        desc.height = height;
        desc.pixel_format = fmt;
        return create(dev, desc, data, debug_name);
    }

    ref<texture> texture::create(core::device& dev, const texture_desc& desc, const void* data, std::string_view debug_name)
    {
        auto* nvrhi_device = dev.get_nvrhi_device();
        if (!nvrhi_device)
        {
            LUMINA_LOG_ERROR("Failed to create texture: no device");
            return nullptr;
        }

        if (desc.width == 0)
        {
            LUMINA_LOG_ERROR("Failed to create texture: width cannot be zero");
            return nullptr;
        }

        if (desc.height == 0)
        {
            LUMINA_LOG_ERROR("Failed to create texture: height cannot be zero");
            return nullptr;
        }

        nvrhi::TextureDesc tex_desc;
        tex_desc.width = desc.width;
        tex_desc.height = desc.height;
        tex_desc.format = to_nvrhi_format(desc.pixel_format);
        tex_desc.dimension = nvrhi::TextureDimension::Texture2D;
        tex_desc.isShaderResource = true;
        tex_desc.debugName = std::string(debug_name);
        tex_desc.initialState = nvrhi::ResourceStates::ShaderResource;
        tex_desc.keepInitialState = true;

        if (desc.generate_mips)
        {
            uint32_t max_dim = std::max(desc.width, desc.height);
            tex_desc.mipLevels = static_cast<uint32_t>(std::floor(std::log2(max_dim))) + 1;
            tex_desc.isUAV = true;
        }

        if (desc.allow_uav)
        {
            tex_desc.isUAV = true;
            tex_desc.initialState = nvrhi::ResourceStates::UnorderedAccess;
        }

        nvrhi::TextureHandle tex = nvrhi_device->createTexture(tex_desc);
        if (!tex)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI texture");
            return nullptr;
        }

        if (data)
        {
            size_t row_pitch = desc.width * format_bytes_per_pixel(desc.pixel_format);

            nvrhi::CommandListHandle cmd = nvrhi_device->createCommandList();
            cmd->open();
            cmd->writeTexture(tex, 0, 0, data, row_pitch);
            cmd->close();
            nvrhi_device->executeCommandList(cmd);
            nvrhi_device->waitForIdle();
        }

        return ref<texture>(new texture(dev, std::move(tex), desc.width, desc.height, desc.pixel_format));
    }

    ref<texture> texture::create_storage(
        core::device& dev,
        uint32_t width,
        uint32_t height,
        format fmt,
        std::string_view debug_name)
    {
        texture_desc desc;
        desc.width = width;
        desc.height = height;
        desc.pixel_format = fmt;
        desc.allow_uav = true;
        return create(dev, desc, nullptr, debug_name);
    }

    ref<texture> texture::load_from_file(core::device& dev, const std::string& path, std::string_view debug_name)
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
        std::string_view effective_debug_name = debug_name.empty() ? std::string_view(path) : debug_name;

        auto tex = create(dev, static_cast<uint32_t>(width), static_cast<uint32_t>(height),
                          format::rgba8_unorm, data, effective_debug_name);

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
