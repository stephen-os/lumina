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

    // Complete pipeline description
    struct pipeline_desc
    {
        ref<shader> shader_program;
        ref<input_layout> vertex_layout;
        std::vector<ref<binding_layout>> binding_layouts;
        render_state state;
        format color_format = format::rgba8_unorm;
        format depth_format = format::unknown;

        // Generate a hash for pipeline caching
        size_t hash() const;
    };

    class pipeline
    {
    public:
        ~pipeline();

        pipeline(const pipeline&) = delete;
        pipeline& operator=(const pipeline&) = delete;

        static ref<pipeline> create(core::device& dev, const pipeline_desc& desc);

        const pipeline_desc& get_desc() const { return m_desc; }

        nvrhi::IGraphicsPipeline* get_pipeline() const { return m_handle; }

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

    // Pipeline cache manages and reuses pipelines
    class pipeline_cache
    {
    public:
        explicit pipeline_cache(core::device& dev);
        ~pipeline_cache();

        pipeline_cache(const pipeline_cache&) = delete;
        pipeline_cache& operator=(const pipeline_cache&) = delete;

        // Get or create a pipeline matching the description
        ref<pipeline> get_or_create(const pipeline_desc& desc);

        // Clear all cached pipelines
        void clear();

        // Get cache statistics
        size_t get_pipeline_count() const { return m_pipelines.size(); }

    private:
        core::device& m_device;
        std::unordered_map<size_t, ref<pipeline>> m_pipelines;
    };
}
