#pragma once

#include "types.h"

#include <lumina/core/base.h>

#include <nvrhi/nvrhi.h>

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace lumina::core { class device; }

namespace lumina::graphics
{
    class compute_shader;
    class binding_layout;
    class binding_set;

    /// Configuration for compute pipeline creation.
    struct compute_pipeline_desc
    {
        ref<compute_shader> shader;
        std::vector<ref<binding_layout>> binding_layouts;

        /// Generates a hash for pipeline caching.
        [[nodiscard]] size_t hash() const;
    };

    /// GPU compute pipeline for executing compute shaders.
    /// Combines compute shader with resource binding layouts.
    class compute_pipeline
    {
    public:
        ~compute_pipeline();

        compute_pipeline(const compute_pipeline&) = delete;
        compute_pipeline& operator=(const compute_pipeline&) = delete;

        /// Creates a compute pipeline. Returns nullptr on failure.
        [[nodiscard]] static ref<compute_pipeline> create(core::device& dev, const compute_pipeline_desc& desc);

        /// Creates a compute pipeline from a shader with a single binding layout.
        [[nodiscard]] static ref<compute_pipeline> create(
            core::device& dev,
            ref<compute_shader> shader,
            ref<binding_layout> layout);

        [[nodiscard]] const compute_pipeline_desc& get_desc() const noexcept { return m_desc; }
        [[nodiscard]] nvrhi::IComputePipeline* get_pipeline() const noexcept { return m_handle.Get(); }

        /// Dispatches compute work with the specified thread group counts.
        /// Must be called within an active command list context.
        void dispatch(
            nvrhi::ICommandList* cmd_list,
            ref<binding_set> bindings,
            uint32_t groups_x,
            uint32_t groups_y = 1,
            uint32_t groups_z = 1);

        /// Inserts a UAV barrier to ensure compute writes are visible.
        static void uav_barrier(nvrhi::ICommandList* cmd_list);

    private:
        compute_pipeline(core::device& dev, nvrhi::ComputePipelineHandle handle, const compute_pipeline_desc& desc)
            : m_device(dev)
            , m_handle(std::move(handle))
            , m_desc(desc)
        {}

        core::device& m_device;
        nvrhi::ComputePipelineHandle m_handle;
        compute_pipeline_desc m_desc;
    };

    /// Manages and reuses compute pipelines based on their configuration.
    class compute_pipeline_cache
    {
    public:
        explicit compute_pipeline_cache(core::device& dev);
        ~compute_pipeline_cache();

        compute_pipeline_cache(const compute_pipeline_cache&) = delete;
        compute_pipeline_cache& operator=(const compute_pipeline_cache&) = delete;

        /// Gets an existing pipeline or creates a new one matching the description.
        [[nodiscard]] ref<compute_pipeline> get_or_create(const compute_pipeline_desc& desc);

        /// Clears all cached pipelines.
        void clear();

        [[nodiscard]] size_t get_pipeline_count() const noexcept { return m_pipelines.size(); }

    private:
        core::device& m_device;
        std::unordered_map<size_t, ref<compute_pipeline>> m_pipelines;
    };
}
