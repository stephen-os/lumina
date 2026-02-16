#pragma once

#include "types.h"

#include <lumina/core/base.h>

#include <unordered_map>
#include <vector>

namespace nvrhi { class IGraphicsPipeline; }
namespace lumina::core { class device; }

namespace lumina::graphics
{
    class shader;
    class input_layout;
    class binding_layout;
    class render_target;

    /// Complete graphics pipeline configuration.
    /// Combines shader, vertex layout, bindings, and render state.
    struct pipeline_desc
    {
        ref<shader> shader_program;
        ref<input_layout> vertex_layout;
        std::vector<ref<binding_layout>> binding_layouts;
        render_state state;
        format color_format = format::rgba8_unorm;
        format depth_format = format::unknown;

        /// Generates a hash for pipeline caching.
        [[nodiscard]] size_t hash() const;
    };

    /// GPU graphics pipeline encapsulating all rendering state.
    /// Pipelines are immutable; use pipeline_cache to manage them efficiently.
    class pipeline
    {
    public:
        ~pipeline();

        pipeline(const pipeline&) = delete;
        pipeline& operator=(const pipeline&) = delete;

        /// Creates a graphics pipeline. Returns nullptr on failure.
        [[nodiscard]] static ref<pipeline> create(core::device& dev, const pipeline_desc& desc);

        [[nodiscard]] const pipeline_desc& get_desc() const noexcept { return m_desc; }
        [[nodiscard]] nvrhi::IGraphicsPipeline* get_pipeline() const noexcept { return m_handle; }

    private:
        pipeline(core::device& dev, nvrhi::IGraphicsPipeline* handle, const pipeline_desc& desc)
            : m_device(dev)
            , m_handle(handle)
            , m_desc(desc)
        {}

        core::device& m_device;
        nvrhi::IGraphicsPipeline* m_handle;
        pipeline_desc m_desc;
    };

    /// Manages and reuses pipelines based on their configuration.
    /// Avoids creating duplicate pipelines for the same settings.
    class pipeline_cache
    {
    public:
        explicit pipeline_cache(core::device& dev);
        ~pipeline_cache();

        pipeline_cache(const pipeline_cache&) = delete;
        pipeline_cache& operator=(const pipeline_cache&) = delete;

        /// Gets an existing pipeline or creates a new one matching the description.
        [[nodiscard]] ref<pipeline> get_or_create(const pipeline_desc& desc);

        /// Clears all cached pipelines.
        void clear();

        [[nodiscard]] size_t get_pipeline_count() const noexcept { return m_pipelines.size(); }

    private:
        core::device& m_device;
        std::unordered_map<size_t, ref<pipeline>> m_pipelines;
    };
}
