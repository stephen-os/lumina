#include "Context.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "UniformBuffer.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "BindingLayout.h"
#include "Pipeline.h"
#include "FormatUtils.h"

#include <lumina/core/Device.h>
#include <lumina/core/Log.h>

#include <nvrhi/nvrhi.h>
#include <nvrhi/utils.h>

namespace Lumina
{
    Context::Context(Core::Device& dev)
        : m_Device(dev)
    {
    }

    Context::~Context() = default;

    void Context::BeginFrame()
    {
        // Reset per-frame state
        m_StateDirty = true;
    }

    void Context::EndFrame()
    {
        // Clear references
        m_CurrentRenderTarget = nullptr;
        m_CurrentPipeline = nullptr;
        m_CurrentBindingSet = nullptr;
        m_CurrentVertexBuffer = nullptr;
        m_CurrentIndexBuffer = nullptr;
        m_StateDirty = true;
    }

    void Context::SetCommandList(nvrhi::ICommandList* cmdList)
    {
        m_CommandList = cmdList;
    }

    void Context::SetRenderTarget(Ref<RenderTarget> target)
    {
        m_CurrentRenderTarget = target;
        m_StateDirty = true;
    }

    void Context::SetDefaultRenderTarget()
    {
        m_CurrentRenderTarget = nullptr;
        m_StateDirty = true;
    }

    void Context::SetSwapchainFramebuffer(nvrhi::IFramebuffer* framebuffer)
    {
        m_SwapchainFramebuffer = framebuffer;
        m_CurrentRenderTarget = nullptr;
        m_StateDirty = true;
    }

    void Context::Clear(const glm::vec4& color)
    {
        Clear(ClearColor(color.r, color.g, color.b, color.a));
    }

    void Context::Clear(const ClearColor& color)
    {
        if (!m_CommandList)
            return;

        nvrhi::IFramebuffer* fb = nullptr;
        if (m_CurrentRenderTarget)
        {
            fb = m_CurrentRenderTarget->GetFramebuffer();
        }
        else if (m_SwapchainFramebuffer)
        {
            fb = m_SwapchainFramebuffer;
        }

        if (fb)
        {
            nvrhi::utils::ClearColorAttachment(m_CommandList, fb, 0, nvrhi::Color(color.r, color.g, color.b, color.a));
        }
    }

    void Context::ClearDepth(float depth, uint8_t stencil)
    {
        if (!m_CommandList)
            return;

        nvrhi::IFramebuffer* fb = nullptr;
        if (m_CurrentRenderTarget)
        {
            fb = m_CurrentRenderTarget->GetFramebuffer();
        }
        else if (m_SwapchainFramebuffer)
        {
            fb = m_SwapchainFramebuffer;
        }

        if (fb && fb->getDesc().depthAttachment.texture)
        {
            m_CommandList->clearDepthStencilTexture(fb->getDesc().depthAttachment.texture, nvrhi::AllSubresources, true, depth, true, stencil);
        }
    }

    void Context::SetViewport(float x, float y, float width, float height)
    {
        Viewport vp;
        vp.x = x;
        vp.y = y;
        vp.width = width;
        vp.height = height;
        SetViewport(vp);
    }

    void Context::SetViewport(const Viewport& vp)
    {
        m_CurrentViewport = vp;
        m_StateDirty = true;
    }

    void Context::SetScissor(int32_t x, int32_t y, int32_t width, int32_t height)
    {
        ScissorRect rect;
        rect.x = x;
        rect.y = y;
        rect.width = width;
        rect.height = height;
        SetScissor(rect);
    }

    void Context::SetScissor(const ScissorRect& rect)
    {
        m_CurrentScissor = rect;
        m_StateDirty = true;
    }

    void Context::SetPipeline(Ref<Pipeline> pso)
    {
        m_CurrentPipeline = pso;
        m_StateDirty = true;
    }

    void Context::SetBindingSet(Ref<BindingSet> bindings)
    {
        m_CurrentBindingSet = bindings;
        m_StateDirty = true;
    }

    void Context::SetVertexBuffer(Ref<VertexBuffer> buffer)
    {
        m_CurrentVertexBuffer = buffer;
        m_StateDirty = true;
    }

    void Context::SetIndexBuffer(Ref<IndexBuffer> buffer)
    {
        m_CurrentIndexBuffer = buffer;
        m_StateDirty = true;
    }

    void Context::ApplyState()
    {
        if (!m_StateDirty)
            return;

        if (!m_CommandList)
            return;

        // Build graphics state
        nvrhi::GraphicsState state;

        // Pipeline
        if (m_CurrentPipeline)
        {
            state.pipeline = m_CurrentPipeline->GetPipeline();
        }

        // Framebuffer
        if (m_CurrentRenderTarget)
        {
            state.framebuffer = m_CurrentRenderTarget->GetFramebuffer();
        }
        else if (m_SwapchainFramebuffer)
        {
            state.framebuffer = m_SwapchainFramebuffer;
        }

        // Viewport
        state.viewport.addViewport(nvrhi::Viewport(
            m_CurrentViewport.x,
            m_CurrentViewport.x + m_CurrentViewport.width,
            m_CurrentViewport.y,
            m_CurrentViewport.y + m_CurrentViewport.height,
            m_CurrentViewport.minDepth,
            m_CurrentViewport.maxDepth
        ));

        // Scissor (use viewport as default if not set)
        if (m_CurrentScissor.width > 0 && m_CurrentScissor.height > 0)
        {
            state.viewport.addScissorRect(nvrhi::Rect(
                m_CurrentScissor.x,
                m_CurrentScissor.x + m_CurrentScissor.width,
                m_CurrentScissor.y,
                m_CurrentScissor.y + m_CurrentScissor.height
            ));
        }
        else
        {
            state.viewport.addScissorRect(nvrhi::Rect(
                static_cast<int>(m_CurrentViewport.x),
                static_cast<int>(m_CurrentViewport.x + m_CurrentViewport.width),
                static_cast<int>(m_CurrentViewport.y),
                static_cast<int>(m_CurrentViewport.y + m_CurrentViewport.height)
            ));
        }

        // Binding set
        if (m_CurrentBindingSet)
        {
            state.bindings.push_back(m_CurrentBindingSet->GetBindingSet());
        }

        // Vertex buffer
        if (m_CurrentVertexBuffer)
        {
            nvrhi::VertexBufferBinding vb;
            vb.buffer = m_CurrentVertexBuffer->GetBuffer();
            vb.slot = 0;
            vb.offset = 0;
            state.vertexBuffers.push_back(vb);
        }

        // Index buffer
        if (m_CurrentIndexBuffer)
        {
            state.indexBuffer.buffer = m_CurrentIndexBuffer->GetBuffer();
            state.indexBuffer.format = m_CurrentIndexBuffer->Is32Bit() ? nvrhi::Format::R32_UINT : nvrhi::Format::R16_UINT;
            state.indexBuffer.offset = 0;
        }

        m_CommandList->setGraphicsState(state);
        m_StateDirty = false;
    }

    void Context::Draw(uint32_t vertexCount, uint32_t startVertex)
    {
        if (!m_CommandList || !m_CurrentPipeline)
            return;

        ApplyState();

        nvrhi::DrawArguments args;
        args.vertexCount = vertexCount;
        args.startVertexLocation = startVertex;
        args.instanceCount = 1;
        args.startInstanceLocation = 0;

        m_CommandList->draw(args);
    }

    void Context::DrawIndexed(uint32_t indexCount, uint32_t startIndex, int32_t baseVertex)
    {
        if (!m_CommandList || !m_CurrentPipeline || !m_CurrentIndexBuffer)
            return;

        ApplyState();

        nvrhi::DrawArguments args;
        args.vertexCount = indexCount;           // Number of indices to draw
        args.startIndexLocation = startIndex;    // Offset into index buffer
        args.startVertexLocation = baseVertex;   // Base vertex added to each index
        args.instanceCount = 1;
        args.startInstanceLocation = 0;

        m_CommandList->drawIndexed(args);
    }

    void Context::DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex, uint32_t startInstance)
    {
        if (!m_CommandList || !m_CurrentPipeline)
            return;

        ApplyState();

        nvrhi::DrawArguments args;
        args.vertexCount = vertexCount;
        args.startVertexLocation = startVertex;
        args.instanceCount = instanceCount;
        args.startInstanceLocation = startInstance;

        m_CommandList->draw(args);
    }

    void Context::DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex, int32_t baseVertex, uint32_t startInstance)
    {
        if (!m_CommandList || !m_CurrentPipeline || !m_CurrentIndexBuffer)
            return;

        ApplyState();

        nvrhi::DrawArguments args;
        args.vertexCount = indexCount;
        args.startIndexLocation = startIndex;
        args.startVertexLocation = baseVertex;
        args.instanceCount = instanceCount;
        args.startInstanceLocation = startInstance;

        m_CommandList->drawIndexed(args);
    }
}
