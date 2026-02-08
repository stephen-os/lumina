#include "render_target.h"

#include <lumina/core/device.h>
#include "texture.h"
#include "format_utils.h"

#include <lumina/core/log.h>

#include <nvrhi/nvrhi.h>

namespace lumina::graphics
{
    render_target::~render_target()
    {
        destroy_resources();
    }

    ref<render_target> render_target::create(core::device& dev, uint32_t width, uint32_t height, format color_format, format depth_format)
    {
        auto rt = ref<render_target>(new render_target(dev, width, height, color_format, depth_format));

        if (!rt->create_resources())
        {
            LUMINA_LOG_ERROR("Failed to create render target resources");
            return nullptr;
        }

        return rt;
    }

    void render_target::resize(uint32_t width, uint32_t height)
    {
        if (width == m_width && height == m_height)
            return;

        m_width = width;
        m_height = height;

        destroy_resources();
        if (!create_resources())
        {
            LUMINA_LOG_ERROR("Failed to resize render target to {}x{}", width, height);
        }
    }

    bool render_target::create_resources()
    {
        auto* nvrhi_device = m_device.get_nvrhi_device();
        if (!nvrhi_device)
            return false;

        // Create color texture
        nvrhi::TextureDesc color_desc;
        color_desc.width = m_width;
        color_desc.height = m_height;
        color_desc.format = to_nvrhi_format(m_color_format);
        color_desc.dimension = nvrhi::TextureDimension::Texture2D;
        color_desc.isShaderResource = true;
        color_desc.isRenderTarget = true;
        color_desc.debugName = "Lumina RT Color";
        color_desc.initialState = nvrhi::ResourceStates::RenderTarget;
        color_desc.keepInitialState = true;

        nvrhi::TextureHandle color_tex = nvrhi_device->createTexture(color_desc);
        if (!color_tex)
        {
            LUMINA_LOG_ERROR("Failed to create render target color texture");
            return false;
        }

        // Create depth texture if requested
        nvrhi::TextureHandle depth_tex;
        if (m_depth_format != format::unknown)
        {
            nvrhi::TextureDesc depth_desc;
            depth_desc.width = m_width;
            depth_desc.height = m_height;
            depth_desc.format = to_nvrhi_format(m_depth_format);
            depth_desc.dimension = nvrhi::TextureDimension::Texture2D;
            depth_desc.isShaderResource = true;
            depth_desc.isRenderTarget = true;
            depth_desc.debugName = "Lumina RT Depth";
            depth_desc.initialState = nvrhi::ResourceStates::DepthWrite;
            depth_desc.keepInitialState = true;

            depth_tex = nvrhi_device->createTexture(depth_desc);
            if (!depth_tex)
            {
                LUMINA_LOG_ERROR("Failed to create render target depth texture");
                return false;
            }
        }

        // Create framebuffer
        nvrhi::FramebufferDesc fb_desc;
        fb_desc.addColorAttachment(color_tex);
        if (depth_tex)
        {
            fb_desc.setDepthAttachment(depth_tex);
        }

        nvrhi::FramebufferHandle framebuffer = nvrhi_device->createFramebuffer(fb_desc);
        if (!framebuffer)
        {
            LUMINA_LOG_ERROR("Failed to create render target framebuffer");
            return false;
        }

        // Store handles (raw pointers, add refs)
        color_tex->AddRef();
        if (depth_tex)
            depth_tex->AddRef();
        framebuffer->AddRef();

        m_framebuffer = framebuffer.Get();

        // Create non-owning texture wrappers for sampling
        m_color_texture = texture::wrap(m_device, color_tex.Get(), m_width, m_height, m_color_format);
        if (depth_tex)
        {
            m_depth_texture = texture::wrap(m_device, depth_tex.Get(), m_width, m_height, m_depth_format);
        }

        return true;
    }

    void render_target::destroy_resources()
    {
        if (m_framebuffer)
        {
            // Release the textures from the framebuffer
            auto info = m_framebuffer->getDesc();
            for (const auto& attachment : info.colorAttachments)
            {
                if (attachment.texture)
                    attachment.texture->Release();
            }
            if (info.depthAttachment.texture)
                info.depthAttachment.texture->Release();

            m_framebuffer->Release();
            m_framebuffer = nullptr;
        }

        m_color_texture = nullptr;
        m_depth_texture = nullptr;
    }
}
