#include "texture.h"

#include <lumina/core/device.h>
#include "format_utils.h"

#include <lumina/core/log.h>

#include <nvrhi/nvrhi.h>

namespace lumina::graphics
{
    texture::~texture()
    {
        if (m_handle)
        {
            m_handle->Release();
            m_handle = nullptr;
        }
    }

    ref<texture> texture::create(core::device& dev, uint32_t width, uint32_t height, format fmt, const void* data)
    {
        texture_desc desc;
        desc.width = width;
        desc.height = height;
        desc.pixel_format = fmt;
        return create(dev, desc, data);
    }

    ref<texture> texture::create(core::device& dev, const texture_desc& desc, const void* data)
    {
        auto* nvrhi_device = dev.get_nvrhi_device();
        if (!nvrhi_device)
        {
            LUMINA_LOG_ERROR("Failed to create texture: no device");
            return nullptr;
        }

        nvrhi::TextureDesc tex_desc;
        tex_desc.width = desc.width;
        tex_desc.height = desc.height;
        tex_desc.format = to_nvrhi_format(desc.pixel_format);
        tex_desc.dimension = nvrhi::TextureDimension::Texture2D;
        tex_desc.isShaderResource = true;
        tex_desc.debugName = "Lumina Texture";
        tex_desc.initialState = nvrhi::ResourceStates::ShaderResource;
        tex_desc.keepInitialState = true;

        if (desc.generate_mips)
        {
            uint32_t max_dim = std::max(desc.width, desc.height);
            tex_desc.mipLevels = static_cast<uint32_t>(std::floor(std::log2(max_dim))) + 1;
            tex_desc.isUAV = true;
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

        tex->AddRef();

        return ref<texture>(new texture(dev, tex.Get(), desc.width, desc.height, desc.pixel_format));
    }
}
