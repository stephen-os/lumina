#pragma once

#include "types.h"
#include "pipeline.h"

#include <lumina/core/base.h>

#include <glm/glm.hpp>

namespace nvrhi
{
    class ICommandList;
    class IFramebuffer;
}
namespace Lumina { class Device; }

namespace Lumina
{
    class VertexBuffer;
    class IndexBuffer;
    class UniformBuffer;
    class Texture;
    class RenderTarget;
    class Shader;
    class InputLayout;
    class BindingLayout;
    class BindingSet;
    class Sampler;
    class Pipeline;

    /// Graphics rendering context managing state and draw commands.
    /// Wraps NVRHI command list with higher-level state management.
    class Context
    {
    public:
        explicit Context(Device& dev);
        ~Context();

        Context(const Context&) = delete;
        Context& operator=(const Context&) = delete;

        /// Frame management
        void BeginFrame();
        void EndFrame();

        /// Render target management
        void SetRenderTarget(Ref<RenderTarget> target);
        void SetDefaultRenderTarget();
        void SetSwapchainFramebuffer(nvrhi::IFramebuffer* framebuffer);
        void Clear(const glm::vec4& color);
        void Clear(const ClearColor& color);
        void ClearDepth(float depth = 1.0f, uint8_t stencil = 0);

        /// Viewport and scissor
        void SetViewport(float x, float y, float width, float height);
        void SetViewport(const Viewport& vp);
        void SetScissor(int32_t x, int32_t y, int32_t width, int32_t height);
        void SetScissor(const ScissorRect& rect);

        /// Pipeline binding
        void SetPipeline(Ref<Pipeline> pso);

        /// Resource binding
        void SetBindingSet(Ref<BindingSet> bindings);
        void SetVertexBuffer(Ref<VertexBuffer> buffer);
        void SetIndexBuffer(Ref<IndexBuffer> buffer);

        /// Draw commands
        void Draw(uint32_t vertexCount, uint32_t startVertex = 0);
        void DrawIndexed(uint32_t indexCount, uint32_t startIndex = 0, int32_t baseVertex = 0);
        void DrawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertex = 0, uint32_t startInstance = 0);
        void DrawIndexedInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t startIndex = 0, int32_t baseVertex = 0, uint32_t startInstance = 0);

        /// State access
        [[nodiscard]] Device& GetDevice() noexcept { return m_Device; }
        [[nodiscard]] nvrhi::ICommandList* GetCommandList() const noexcept { return m_CommandList; }
        [[nodiscard]] bool HasFramebuffer() const noexcept { return m_SwapchainFramebuffer != nullptr || m_CurrentRenderTarget != nullptr; }

        /// Sets the command list from core device (called by application).
        void SetCommandList(nvrhi::ICommandList* cmdList);

    private:
        void ApplyState();

        Device& m_Device;

        // Current state
        Ref<RenderTarget> m_CurrentRenderTarget;
        Ref<Pipeline> m_CurrentPipeline;
        Ref<BindingSet> m_CurrentBindingSet;
        Ref<VertexBuffer> m_CurrentVertexBuffer;
        Ref<IndexBuffer> m_CurrentIndexBuffer;

        Viewport m_CurrentViewport;
        ScissorRect m_CurrentScissor;

        // State tracking
        bool m_StateDirty = true;

        // NVRHI command list and swapchain
        nvrhi::ICommandList* m_CommandList = nullptr;
        nvrhi::IFramebuffer* m_SwapchainFramebuffer = nullptr;
    };
}
