#pragma once

#include "types.h"

#include <lumina/core/base.h>

#include <nvrhi/nvrhi.h>

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace Lumina { class Device; }

namespace Lumina
{
    class ComputeShader;
    class BindingLayout;
    class BindingSet;

    /// Configuration for compute pipeline creation.
    struct ComputePipelineDesc
    {
        Ref<ComputeShader> Shader;
        std::vector<Ref<BindingLayout>> BindingLayouts;

        /// Generates a hash for pipeline caching.
        [[nodiscard]] size_t Hash() const;
    };

    /// GPU compute pipeline for executing compute shaders.
    /// Combines compute shader with resource binding layouts.
    class ComputePipeline
    {
    public:
        ~ComputePipeline();

        ComputePipeline(const ComputePipeline&) = delete;
        ComputePipeline& operator=(const ComputePipeline&) = delete;

        /// Creates a compute pipeline. Returns nullptr on failure.
        [[nodiscard]] static Ref<ComputePipeline> Create(Device& dev, const ComputePipelineDesc& desc);

        /// Creates a compute pipeline from a shader with a single binding layout.
        [[nodiscard]] static Ref<ComputePipeline> Create(
            Device& dev,
            Ref<ComputeShader> shader,
            Ref<BindingLayout> layout);

        [[nodiscard]] const ComputePipelineDesc& GetDesc() const noexcept { return m_Desc; }
        [[nodiscard]] nvrhi::IComputePipeline* GetPipeline() const noexcept { return m_Handle.Get(); }

        /// Dispatches compute work with the specified thread group counts.
        /// Must be called within an active command list context.
        void Dispatch(
            nvrhi::ICommandList* cmdList,
            Ref<BindingSet> bindings,
            uint32_t groupsX,
            uint32_t groupsY = 1,
            uint32_t groupsZ = 1);

        /// Inserts a UAV barrier to ensure compute writes are visible.
        static void UAVBarrier(nvrhi::ICommandList* cmdList);

    private:
        ComputePipeline(Device& dev, nvrhi::ComputePipelineHandle handle, const ComputePipelineDesc& desc)
            : m_Device(dev)
            , m_Handle(std::move(handle))
            , m_Desc(desc)
        {}

        Device& m_Device;
        nvrhi::ComputePipelineHandle m_Handle;
        ComputePipelineDesc m_Desc;
    };

    /// Manages and reuses compute pipelines based on their configuration.
    class ComputePipelineCache
    {
    public:
        explicit ComputePipelineCache(Device& dev);
        ~ComputePipelineCache();

        ComputePipelineCache(const ComputePipelineCache&) = delete;
        ComputePipelineCache& operator=(const ComputePipelineCache&) = delete;

        /// Gets an existing pipeline or creates a new one matching the description.
        [[nodiscard]] Ref<ComputePipeline> GetOrCreate(const ComputePipelineDesc& desc);

        /// Clears all cached pipelines.
        void Clear();

        [[nodiscard]] size_t GetPipelineCount() const noexcept { return m_Pipelines.size(); }

    private:
        Device& m_Device;
        std::unordered_map<size_t, Ref<ComputePipeline>> m_Pipelines;
    };
}
