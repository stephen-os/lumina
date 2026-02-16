#include "render_target.h"

#include <lumina/core/device.h>
#include "texture.h"
#include "format_utils.h"

#include <lumina/core/log.h>

namespace lumina::graphics
{
    render_target::~render_target() = default;

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

        m_color_texture_handle = nvrhi_device->createTexture(color_desc);
        if (!m_color_texture_handle)
        {
            LUMINA_LOG_ERROR("Failed to create render target color texture");
            return false;
        }

        // Create depth texture if requested
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

            m_depth_texture_handle = nvrhi_device->createTexture(depth_desc);
            if (!m_depth_texture_handle)
            {
                LUMINA_LOG_ERROR("Failed to create render target depth texture");
                return false;
            }
        }

        // Create framebuffer
        nvrhi::FramebufferDesc fb_desc;
        fb_desc.addColorAttachment(m_color_texture_handle);
        if (m_depth_texture_handle)
        {
            fb_desc.setDepthAttachment(m_depth_texture_handle);
        }

        m_framebuffer = nvrhi_device->createFramebuffer(fb_desc);
        if (!m_framebuffer)
        {
            LUMINA_LOG_ERROR("Failed to create render target framebuffer");
            return false;
        }

        // Create texture wrappers for sampling
        m_color_texture = texture::wrap(m_device, m_color_texture_handle.Get(), m_width, m_height, m_color_format);
        if (m_depth_texture_handle)
        {
            m_depth_texture = texture::wrap(m_device, m_depth_texture_handle.Get(), m_width, m_height, m_depth_format);
        }

        return true;
    }

    void render_target::destroy_resources()
    {
        // Reset texture wrappers first (they reference the handles)
        m_color_texture.reset();
        m_depth_texture.reset();

        // Reset handles - RAII handles cleanup
        m_framebuffer.Reset();
        m_color_texture_handle.Reset();
        m_depth_texture_handle.Reset();
    }
}
