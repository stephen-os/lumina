#include "RenderTarget.h"

#include <lumina/core/Device.h>
#include "Texture.h"
#include "FormatUtils.h"

#include <lumina/core/Log.h>

namespace Lumina
{
    RenderTarget::~RenderTarget() = default;

    Ref<RenderTarget> RenderTarget::Create(Core::Device& dev, uint32_t width, uint32_t height, Format colorFormat, Format depthFormat, uint32_t sampleCount)
    {
        // Validate sample count (must be 1, 2, 4, or 8)
        if (sampleCount != 1 && sampleCount != 2 && sampleCount != 4 && sampleCount != 8)
        {
            LUMINA_LOG_WARN("Invalid MSAA sample count {}, defaulting to 1", sampleCount);
            sampleCount = 1;
        }

        auto rt = Ref<RenderTarget>(new RenderTarget(dev, width, height, colorFormat, depthFormat, sampleCount));

        if (!rt->CreateResources())
        {
            LUMINA_LOG_ERROR("Failed to create render target resources");
            return nullptr;
        }

        return rt;
    }

    void RenderTarget::Resize(uint32_t width, uint32_t height)
    {
        if (width == m_Width && height == m_Height)
            return;

        m_Width = width;
        m_Height = height;

        DestroyResources();
        if (!CreateResources())
        {
            LUMINA_LOG_ERROR("Failed to resize render target to {}x{}", width, height);
        }
    }

    void RenderTarget::Resolve(nvrhi::ICommandList* cmdList)
    {
        if (m_SampleCount <= 1 || !m_ResolvedColorHandle || !m_ColorTextureHandle)
            return;

        // Resolve MSAA texture to non-MSAA texture
        cmdList->resolveTexture(m_ResolvedColorHandle, nvrhi::TextureSubresourceSet{},
                                  m_ColorTextureHandle, nvrhi::TextureSubresourceSet{});
    }

    bool RenderTarget::CreateResources()
    {
        auto* nvrhiDevice = m_Device.GetNvrhiDevice();
        if (!nvrhiDevice)
            return false;

        const bool useMsaa = m_SampleCount > 1;

        // Create color texture (MSAA if sample_count > 1)
        nvrhi::TextureDesc colorDesc;
        colorDesc.width = m_Width;
        colorDesc.height = m_Height;
        colorDesc.format = ToNvrhiFormat(m_ColorFormat);
        colorDesc.dimension = useMsaa ? nvrhi::TextureDimension::Texture2DMS : nvrhi::TextureDimension::Texture2D;
        colorDesc.isShaderResource = true;  // Allow shader resource for all textures
        colorDesc.isRenderTarget = true;
        colorDesc.sampleCount = m_SampleCount;
        colorDesc.mipLevels = 1;
        colorDesc.arraySize = 1;
        colorDesc.debugName = useMsaa ? "Lumina RT Color MSAA" : "Lumina RT Color";
        colorDesc.initialState = nvrhi::ResourceStates::RenderTarget;
        colorDesc.keepInitialState = true;

        m_ColorTextureHandle = nvrhiDevice->createTexture(colorDesc);
        if (!m_ColorTextureHandle)
        {
            LUMINA_LOG_ERROR("Failed to create render target color texture");
            return false;
        }

        // Create resolved color texture for MSAA (non-MSAA, for sampling)
        if (useMsaa)
        {
            nvrhi::TextureDesc resolveDesc;
            resolveDesc.width = m_Width;
            resolveDesc.height = m_Height;
            resolveDesc.format = ToNvrhiFormat(m_ColorFormat);
            resolveDesc.dimension = nvrhi::TextureDimension::Texture2D;
            resolveDesc.isShaderResource = true;
            resolveDesc.isRenderTarget = false;
            resolveDesc.sampleCount = 1;
            resolveDesc.debugName = "Lumina RT Color Resolved";
            resolveDesc.initialState = nvrhi::ResourceStates::ResolveDest;
            resolveDesc.keepInitialState = true;

            m_ResolvedColorHandle = nvrhiDevice->createTexture(resolveDesc);
            if (!m_ResolvedColorHandle)
            {
                LUMINA_LOG_ERROR("Failed to create MSAA resolve texture");
                return false;
            }
        }

        // Create depth texture if requested (MSAA if sample_count > 1)
        if (m_DepthFormat != Format::Unknown)
        {
            nvrhi::TextureDesc depthDesc;
            depthDesc.width = m_Width;
            depthDesc.height = m_Height;
            depthDesc.format = ToNvrhiFormat(m_DepthFormat);
            depthDesc.dimension = useMsaa ? nvrhi::TextureDimension::Texture2DMS : nvrhi::TextureDimension::Texture2D;
            depthDesc.isShaderResource = true;  // Allow shader resource for all textures
            depthDesc.isRenderTarget = true;
            depthDesc.sampleCount = m_SampleCount;
            depthDesc.mipLevels = 1;
            depthDesc.arraySize = 1;
            depthDesc.debugName = useMsaa ? "Lumina RT Depth MSAA" : "Lumina RT Depth";
            depthDesc.initialState = nvrhi::ResourceStates::DepthWrite;
            depthDesc.keepInitialState = true;

            m_DepthTextureHandle = nvrhiDevice->createTexture(depthDesc);
            if (!m_DepthTextureHandle)
            {
                LUMINA_LOG_ERROR("Failed to create render target depth texture");
                return false;
            }
        }

        // Create framebuffer (with MSAA textures if applicable)
        nvrhi::FramebufferDesc fbDesc;
        fbDesc.addColorAttachment(m_ColorTextureHandle);
        if (m_DepthTextureHandle)
        {
            fbDesc.setDepthAttachment(m_DepthTextureHandle);
        }

        m_Framebuffer = nvrhiDevice->createFramebuffer(fbDesc);
        if (!m_Framebuffer)
        {
            LUMINA_LOG_ERROR("Failed to create render target framebuffer");
            return false;
        }

        // Create texture wrappers
        // For MSAA, the color wrapper points to the MSAA texture (for use as render target)
        m_ColorTexture = Texture::Wrap(m_Device, m_ColorTextureHandle.Get(), m_Width, m_Height, m_ColorFormat);

        // For MSAA, create wrapper for resolved texture (for sampling)
        if (useMsaa && m_ResolvedColorHandle)
        {
            m_ResolvedColorTexture = Texture::Wrap(m_Device, m_ResolvedColorHandle.Get(), m_Width, m_Height, m_ColorFormat);
        }

        if (m_DepthTextureHandle)
        {
            m_DepthTexture = Texture::Wrap(m_Device, m_DepthTextureHandle.Get(), m_Width, m_Height, m_DepthFormat);
        }

        if (useMsaa)
        {
            LUMINA_LOG_INFO("Created {}x{} render target with {}x MSAA", m_Width, m_Height, m_SampleCount);
        }

        return true;
    }

    void RenderTarget::DestroyResources()
    {
        // Reset texture wrappers first (they reference the handles)
        m_ColorTexture.reset();
        m_DepthTexture.reset();
        m_ResolvedColorTexture.reset();

        // Reset handles - RAII handles cleanup
        m_Framebuffer.Reset();
        m_ColorTextureHandle.Reset();
        m_DepthTextureHandle.Reset();
        m_ResolvedColorHandle.Reset();
    }
}
