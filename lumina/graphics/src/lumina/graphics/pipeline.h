#pragma once

#include "types.h"

#include <lumina/core/base.h>

#include <nvrhi/nvrhi.h>

#include <unordered_map>
#include <utility>
#include <vector>

namespace Lumina { class Device; }

namespace Lumina
{
    class Shader;
    class InputLayout;
    class BindingLayout;
    class RenderTarget;

    /// Complete graphics pipeline configuration.
    /// Combines shader, vertex layout, bindings, and render state.
    struct PipelineDesc
    {
        Ref<Shader> ShaderProgram;
        Ref<InputLayout> VertexLayout;
        std::vector<Ref<BindingLayout>> BindingLayouts;
        RenderState State;
        Format ColorFormat = Format::RGBA8Unorm;
        Format DepthFormat = Format::Unknown;
        uint32_t SampleCount = 1;  // MSAA sample count (1 = no MSAA)

        /// Generates a hash for pipeline caching.
        [[nodiscard]] size_t Hash() const;
    };

    /// GPU graphics pipeline encapsulating all rendering state.
    /// Pipelines are immutable; use PipelineCache to manage them efficiently.
    class Pipeline
    {
    public:
        ~Pipeline();

        Pipeline(const Pipeline&) = delete;
        Pipeline& operator=(const Pipeline&) = delete;

        /// Creates a graphics pipeline. Returns nullptr on failure.
        [[nodiscard]] static Ref<Pipeline> Create(Device& dev, const PipelineDesc& desc);

        [[nodiscard]] const PipelineDesc& GetDesc() const noexcept { return m_Desc; }
        [[nodiscard]] nvrhi::IGraphicsPipeline* GetPipeline() const noexcept { return m_Handle.Get(); }

    private:
        Pipeline(Device& dev, nvrhi::GraphicsPipelineHandle handle, const PipelineDesc& desc)
            : m_Device(dev)
            , m_Handle(std::move(handle))
            , m_Desc(desc)
        {}

        Device& m_Device;
        nvrhi::GraphicsPipelineHandle m_Handle;
        PipelineDesc m_Desc;
    };

    /// Manages and reuses pipelines based on their configuration.
    /// Avoids creating duplicate pipelines for the same settings.
    class PipelineCache
    {
    public:
        explicit PipelineCache(Device& dev);
        ~PipelineCache();

        PipelineCache(const PipelineCache&) = delete;
        PipelineCache& operator=(const PipelineCache&) = delete;

        /// Gets an existing pipeline or creates a new one matching the description.
        [[nodiscard]] Ref<Pipeline> GetOrCreate(const PipelineDesc& desc);

        /// Clears all cached pipelines.
        void Clear();

        [[nodiscard]] size_t GetPipelineCount() const noexcept { return m_Pipelines.size(); }

    private:
        Device& m_Device;
        std::unordered_map<size_t, Ref<Pipeline>> m_Pipelines;
    };
}
