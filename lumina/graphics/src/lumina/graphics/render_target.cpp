#include "render_target.h"

#include <lumina/core/device.h>
#include "texture.h"
#include "format_utils.h"

#include <lumina/core/log.h>

namespace lumina::graphics
{
    render_target::~render_target() = default;

    ref<render_target> render_target::create(core::device& dev, uint32_t width, uint32_t height, format color_format, format depth_format, uint32_t sample_count)
    {
        // Validate sample count (must be 1, 2, 4, or 8)
        if (sample_count != 1 && sample_count != 2 && sample_count != 4 && sample_count != 8)
        {
            LUMINA_LOG_WARN("Invalid MSAA sample count {}, defaulting to 1", sample_count);
            sample_count = 1;
        }

        auto rt = ref<render_target>(new render_target(dev, width, height, color_format, depth_format, sample_count));

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

    void render_target::resolve(nvrhi::ICommandList* cmd_list)
    {
        if (m_sample_count <= 1 || !m_resolved_color_handle || !m_color_texture_handle)
            return;

        // Resolve MSAA texture to non-MSAA texture
        cmd_list->resolveTexture(m_resolved_color_handle, nvrhi::TextureSubresourceSet{},
                                  m_color_texture_handle, nvrhi::TextureSubresourceSet{});
    }

    bool render_target::create_resources()
    {
        auto* nvrhi_device = m_device.get_nvrhi_device();
        if (!nvrhi_device)
            return false;

        const bool use_msaa = m_sample_count > 1;

        // Create color texture (MSAA if sample_count > 1)
        nvrhi::TextureDesc color_desc;
        color_desc.width = m_width;
        color_desc.height = m_height;
        color_desc.format = to_nvrhi_format(m_color_format);
        color_desc.dimension = use_msaa ? nvrhi::TextureDimension::Texture2DMS : nvrhi::TextureDimension::Texture2D;
        color_desc.isShaderResource = true;  // Allow shader resource for all textures
        color_desc.isRenderTarget = true;
        color_desc.sampleCount = m_sample_count;
        color_desc.mipLevels = 1;
        color_desc.arraySize = 1;
        color_desc.debugName = use_msaa ? "Lumina RT Color MSAA" : "Lumina RT Color";
        color_desc.initialState = nvrhi::ResourceStates::RenderTarget;
        color_desc.keepInitialState = true;

        m_color_texture_handle = nvrhi_device->createTexture(color_desc);
        if (!m_color_texture_handle)
        {
            LUMINA_LOG_ERROR("Failed to create render target color texture");
            return false;
        }

        // Create resolved color texture for MSAA (non-MSAA, for sampling)
        if (use_msaa)
        {
            nvrhi::TextureDesc resolve_desc;
            resolve_desc.width = m_width;
            resolve_desc.height = m_height;
            resolve_desc.format = to_nvrhi_format(m_color_format);
            resolve_desc.dimension = nvrhi::TextureDimension::Texture2D;
            resolve_desc.isShaderResource = true;
            resolve_desc.isRenderTarget = false;
            resolve_desc.sampleCount = 1;
            resolve_desc.debugName = "Lumina RT Color Resolved";
            resolve_desc.initialState = nvrhi::ResourceStates::ResolveDest;
            resolve_desc.keepInitialState = true;

            m_resolved_color_handle = nvrhi_device->createTexture(resolve_desc);
            if (!m_resolved_color_handle)
            {
                LUMINA_LOG_ERROR("Failed to create MSAA resolve texture");
                return false;
            }
        }

        // Create depth texture if requested (MSAA if sample_count > 1)
        if (m_depth_format != format::unknown)
        {
            nvrhi::TextureDesc depth_desc;
            depth_desc.width = m_width;
            depth_desc.height = m_height;
            depth_desc.format = to_nvrhi_format(m_depth_format);
            depth_desc.dimension = use_msaa ? nvrhi::TextureDimension::Texture2DMS : nvrhi::TextureDimension::Texture2D;
            depth_desc.isShaderResource = true;  // Allow shader resource for all textures
            depth_desc.isRenderTarget = true;
            depth_desc.sampleCount = m_sample_count;
            depth_desc.mipLevels = 1;
            depth_desc.arraySize = 1;
            depth_desc.debugName = use_msaa ? "Lumina RT Depth MSAA" : "Lumina RT Depth";
            depth_desc.initialState = nvrhi::ResourceStates::DepthWrite;
            depth_desc.keepInitialState = true;

            m_depth_texture_handle = nvrhi_device->createTexture(depth_desc);
            if (!m_depth_texture_handle)
            {
                LUMINA_LOG_ERROR("Failed to create render target depth texture");
                return false;
            }
        }

        // Create framebuffer (with MSAA textures if applicable)
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

        // Create texture wrappers
        // For MSAA, the color wrapper points to the MSAA texture (for use as render target)
        m_color_texture = texture::wrap(m_device, m_color_texture_handle.Get(), m_width, m_height, m_color_format);

        // For MSAA, create wrapper for resolved texture (for sampling)
        if (use_msaa && m_resolved_color_handle)
        {
            m_resolved_color_texture = texture::wrap(m_device, m_resolved_color_handle.Get(), m_width, m_height, m_color_format);
        }

        if (m_depth_texture_handle)
        {
            m_depth_texture = texture::wrap(m_device, m_depth_texture_handle.Get(), m_width, m_height, m_depth_format);
        }

        if (use_msaa)
        {
            LUMINA_LOG_INFO("Created {}x{} render target with {}x MSAA", m_width, m_height, m_sample_count);
        }

        return true;
    }

    void render_target::destroy_resources()
    {
        // Reset texture wrappers first (they reference the handles)
        m_color_texture.reset();
        m_depth_texture.reset();
        m_resolved_color_texture.reset();

        // Reset handles - RAII handles cleanup
        m_framebuffer.Reset();
        m_color_texture_handle.Reset();
        m_depth_texture_handle.Reset();
        m_resolved_color_handle.Reset();
    }
}
