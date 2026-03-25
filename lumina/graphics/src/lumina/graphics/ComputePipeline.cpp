#include "ComputePipeline.h"

#include "ComputeShader.h"
#include "BindingLayout.h"

#include <lumina/core/Device.h>
#include <lumina/core/Log.h>

#include <utility>

namespace Lumina
{
    // Hash combining helper
    static void HashCombine(size_t& seed, size_t value)
    {
        seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    size_t ComputePipelineDesc::Hash() const
    {
        size_t h = 0;

        HashCombine(h, reinterpret_cast<size_t>(Shader.get()));

        for (const auto& bl : BindingLayouts)
        {
            HashCombine(h, reinterpret_cast<size_t>(bl.get()));
        }

        return h;
    }

    // --- ComputePipeline ---

    ComputePipeline::~ComputePipeline() = default;

    Ref<ComputePipeline> ComputePipeline::Create(Device& dev, const ComputePipelineDesc& desc)
    {
        auto* nvrhiDevice = dev.GetNvrhiDevice();
        if (!nvrhiDevice)
        {
            LUMINA_LOG_ERROR("Failed to create compute pipeline: no device");
            return nullptr;
        }

        if (!desc.Shader)
        {
            LUMINA_LOG_ERROR("Failed to create compute pipeline: shader required");
            return nullptr;
        }

        nvrhi::ComputePipelineDesc psoDesc;
        psoDesc.CS = desc.Shader->GetShader();

        for (const auto& bl : desc.BindingLayouts)
        {
            if (bl)
            {
                psoDesc.bindingLayouts.push_back(bl->GetLayout());
            }
        }

        nvrhi::ComputePipelineHandle pso = nvrhiDevice->createComputePipeline(psoDesc);
        if (!pso)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI compute pipeline");
            return nullptr;
        }

        return Ref<ComputePipeline>(new ComputePipeline(dev, std::move(pso), desc));
    }

    Ref<ComputePipeline> ComputePipeline::Create(
        Device& dev,
        Ref<ComputeShader> shader,
        Ref<BindingLayout> layout)
    {
        ComputePipelineDesc desc;
        desc.Shader = shader;
        if (layout)
        {
            desc.BindingLayouts.push_back(layout);
        }
        return Create(dev, desc);
    }

    void ComputePipeline::Dispatch(
        nvrhi::ICommandList* cmdList,
        Ref<BindingSet> bindings,
        uint32_t groupsX,
        uint32_t groupsY,
        uint32_t groupsZ)
    {
        if (!cmdList)
        {
            LUMINA_LOG_ERROR("ComputePipeline::Dispatch: command list is null");
            return;
        }

        if (!m_Handle)
        {
            LUMINA_LOG_ERROR("ComputePipeline::Dispatch: pipeline is null");
            return;
        }

        nvrhi::ComputeState state;
        state.pipeline = m_Handle.Get();

        if (bindings)
        {
            state.bindings = { bindings->GetBindingSet() };
        }

        cmdList->setComputeState(state);
        cmdList->dispatch(groupsX, groupsY, groupsZ);
    }

    void ComputePipeline::UAVBarrier(nvrhi::ICommandList* cmdList)
    {
        if (cmdList)
        {
            cmdList->commitBarriers();
        }
    }

    // --- ComputePipelineCache ---

    ComputePipelineCache::ComputePipelineCache(Device& dev)
        : m_Device(dev)
    {
    }

    ComputePipelineCache::~ComputePipelineCache()
    {
        Clear();
    }

    Ref<ComputePipeline> ComputePipelineCache::GetOrCreate(const ComputePipelineDesc& desc)
    {
        size_t h = desc.Hash();

        auto it = m_Pipelines.find(h);
        if (it != m_Pipelines.end())
        {
            return it->second;
        }

        auto pso = ComputePipeline::Create(m_Device, desc);
        if (pso)
        {
            m_Pipelines[h] = pso;
        }

        return pso;
    }

    void ComputePipelineCache::Clear()
    {
        m_Pipelines.clear();
    }
}
