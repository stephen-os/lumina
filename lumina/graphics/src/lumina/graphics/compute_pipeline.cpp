#include "compute_pipeline.h"

#include "compute_shader.h"
#include "binding_layout.h"

#include <lumina/core/device.h>
#include <lumina/core/log.h>

#include <utility>

namespace lumina::graphics
{
    // Hash combining helper
    static void hash_combine(size_t& seed, size_t value)
    {
        seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    size_t compute_pipeline_desc::hash() const
    {
        size_t h = 0;

        hash_combine(h, reinterpret_cast<size_t>(shader.get()));

        for (const auto& bl : binding_layouts)
        {
            hash_combine(h, reinterpret_cast<size_t>(bl.get()));
        }

        return h;
    }

    // --- compute_pipeline ---

    compute_pipeline::~compute_pipeline() = default;

    ref<compute_pipeline> compute_pipeline::create(core::device& dev, const compute_pipeline_desc& desc)
    {
        auto* nvrhi_device = dev.get_nvrhi_device();
        if (!nvrhi_device)
        {
            LUMINA_LOG_ERROR("Failed to create compute pipeline: no device");
            return nullptr;
        }

        if (!desc.shader)
        {
            LUMINA_LOG_ERROR("Failed to create compute pipeline: shader required");
            return nullptr;
        }

        nvrhi::ComputePipelineDesc pso_desc;
        pso_desc.CS = desc.shader->get_shader();

        for (const auto& bl : desc.binding_layouts)
        {
            if (bl)
            {
                pso_desc.bindingLayouts.push_back(bl->get_layout());
            }
        }

        nvrhi::ComputePipelineHandle pso = nvrhi_device->createComputePipeline(pso_desc);
        if (!pso)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI compute pipeline");
            return nullptr;
        }

        return ref<compute_pipeline>(new compute_pipeline(dev, std::move(pso), desc));
    }

    ref<compute_pipeline> compute_pipeline::create(
        core::device& dev,
        ref<compute_shader> shader,
        ref<binding_layout> layout)
    {
        compute_pipeline_desc desc;
        desc.shader = shader;
        if (layout)
        {
            desc.binding_layouts.push_back(layout);
        }
        return create(dev, desc);
    }

    void compute_pipeline::dispatch(
        nvrhi::ICommandList* cmd_list,
        ref<binding_set> bindings,
        uint32_t groups_x,
        uint32_t groups_y,
        uint32_t groups_z)
    {
        if (!cmd_list)
        {
            LUMINA_LOG_ERROR("compute_pipeline::dispatch: command list is null");
            return;
        }

        if (!m_handle)
        {
            LUMINA_LOG_ERROR("compute_pipeline::dispatch: pipeline is null");
            return;
        }

        nvrhi::ComputeState state;
        state.pipeline = m_handle.Get();

        if (bindings)
        {
            state.bindings = { bindings->get_binding_set() };
        }

        cmd_list->setComputeState(state);
        cmd_list->dispatch(groups_x, groups_y, groups_z);
    }

    void compute_pipeline::uav_barrier(nvrhi::ICommandList* cmd_list)
    {
        if (cmd_list)
        {
            cmd_list->commitBarriers();
        }
    }

    // --- compute_pipeline_cache ---

    compute_pipeline_cache::compute_pipeline_cache(core::device& dev)
        : m_device(dev)
    {
    }

    compute_pipeline_cache::~compute_pipeline_cache()
    {
        clear();
    }

    ref<compute_pipeline> compute_pipeline_cache::get_or_create(const compute_pipeline_desc& desc)
    {
        size_t h = desc.hash();

        auto it = m_pipelines.find(h);
        if (it != m_pipelines.end())
        {
            return it->second;
        }

        auto pso = compute_pipeline::create(m_device, desc);
        if (pso)
        {
            m_pipelines[h] = pso;
        }

        return pso;
    }

    void compute_pipeline_cache::clear()
    {
        m_pipelines.clear();
    }
}
