#include "Pipeline.h"

#include <lumina/core/Device.h>
#include "Shader.h"
#include "InputLayout.h"
#include "BindingLayout.h"
#include "RenderTarget.h"
#include "FormatUtils.h"

#include <lumina/core/Log.h>

#include <nvrhi/nvrhi.h>

#include <functional>
#include <utility>

namespace Lumina
{
    // Hash combining helper
    static void HashCombine(size_t& seed, size_t value)
    {
        seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    size_t PipelineDesc::Hash() const
    {
        size_t h = 0;

        // Hash shader pointers
        HashCombine(h, reinterpret_cast<size_t>(ShaderProgram.get()));
        HashCombine(h, reinterpret_cast<size_t>(VertexLayout.get()));

        // Hash binding layouts
        for (const auto& bl : BindingLayouts)
        {
            HashCombine(h, reinterpret_cast<size_t>(bl.get()));
        }

        // Hash render state
        HashCombine(h, static_cast<size_t>(State.Blend));
        HashCombine(h, static_cast<size_t>(State.Depth));
        HashCombine(h, static_cast<size_t>(State.Cull));
        HashCombine(h, static_cast<size_t>(State.Primitive));

        // Hash formats
        HashCombine(h, static_cast<size_t>(ColorFormat));
        HashCombine(h, static_cast<size_t>(DepthFormat));

        // Hash MSAA sample count
        HashCombine(h, static_cast<size_t>(SampleCount));

        return h;
    }

    // Convert Lumina blend mode to NVRHI
    static nvrhi::BlendState ToNvrhiBlendState(BlendMode mode)
    {
        nvrhi::BlendState bs;
        bs.targets[0].blendEnable = true;

        switch (mode)
        {
            case BlendMode::Opaque:
                bs.targets[0].blendEnable = false;
                break;

            case BlendMode::Alpha:
                bs.targets[0].srcBlend = nvrhi::BlendFactor::SrcAlpha;
                bs.targets[0].destBlend = nvrhi::BlendFactor::InvSrcAlpha;
                bs.targets[0].srcBlendAlpha = nvrhi::BlendFactor::One;
                bs.targets[0].destBlendAlpha = nvrhi::BlendFactor::InvSrcAlpha;
                break;

            case BlendMode::Additive:
                bs.targets[0].srcBlend = nvrhi::BlendFactor::SrcAlpha;
                bs.targets[0].destBlend = nvrhi::BlendFactor::One;
                bs.targets[0].srcBlendAlpha = nvrhi::BlendFactor::One;
                bs.targets[0].destBlendAlpha = nvrhi::BlendFactor::One;
                break;

            case BlendMode::Multiply:
                bs.targets[0].srcBlend = nvrhi::BlendFactor::DstColor;
                bs.targets[0].destBlend = nvrhi::BlendFactor::Zero;
                bs.targets[0].srcBlendAlpha = nvrhi::BlendFactor::DstAlpha;
                bs.targets[0].destBlendAlpha = nvrhi::BlendFactor::Zero;
                break;
        }

        return bs;
    }

    // Convert Lumina depth mode to NVRHI
    static nvrhi::DepthStencilState ToNvrhiDepthState(DepthMode mode)
    {
        nvrhi::DepthStencilState ds;

        switch (mode)
        {
            case DepthMode::None:
                ds.depthTestEnable = false;
                ds.depthWriteEnable = false;
                break;

            case DepthMode::ReadOnly:
                ds.depthTestEnable = true;
                ds.depthWriteEnable = false;
                ds.depthFunc = nvrhi::ComparisonFunc::LessOrEqual;
                break;

            case DepthMode::ReadWrite:
                ds.depthTestEnable = true;
                ds.depthWriteEnable = true;
                ds.depthFunc = nvrhi::ComparisonFunc::LessOrEqual;
                break;
        }

        return ds;
    }

    // Convert Lumina cull mode to NVRHI
    static nvrhi::RasterState ToNvrhiRasterState(CullMode mode)
    {
        nvrhi::RasterState rs;

        switch (mode)
        {
            case CullMode::None:
                rs.cullMode = nvrhi::RasterCullMode::None;
                break;
            case CullMode::Back:
                rs.cullMode = nvrhi::RasterCullMode::Back;
                break;
            case CullMode::Front:
                rs.cullMode = nvrhi::RasterCullMode::Front;
                break;
        }

        rs.frontCounterClockwise = true;

        return rs;
    }

    // Convert Lumina topology to NVRHI
    static nvrhi::PrimitiveType ToNvrhiPrimitiveType(Topology topo)
    {
        switch (topo)
        {
            case Topology::Triangles:       return nvrhi::PrimitiveType::TriangleList;
            case Topology::TriangleStrip:   return nvrhi::PrimitiveType::TriangleStrip;
            case Topology::Lines:           return nvrhi::PrimitiveType::LineList;
            case Topology::LineStrip:       return nvrhi::PrimitiveType::LineStrip;
            case Topology::Points:          return nvrhi::PrimitiveType::PointList;
            default:                        return nvrhi::PrimitiveType::TriangleList;
        }
    }

    // --- Pipeline ---

    Pipeline::~Pipeline() = default;

    Ref<Pipeline> Pipeline::Create(Core::Device& dev, const PipelineDesc& desc)
    {
        auto* nvrhiDevice = dev.GetNvrhiDevice();
        if (!nvrhiDevice)
        {
            LUMINA_LOG_ERROR("Failed to create pipeline: no device");
            return nullptr;
        }

        if (!desc.ShaderProgram)
        {
            LUMINA_LOG_ERROR("Failed to create pipeline: shader required");
            return nullptr;
        }

        if (!desc.VertexLayout)
        {
            LUMINA_LOG_ERROR("Failed to create pipeline: vertex layout required");
            return nullptr;
        }

        // Build NVRHI pipeline description
        nvrhi::GraphicsPipelineDesc psoDesc;

        // Shaders
        psoDesc.VS = desc.ShaderProgram->GetVertexShader();
        psoDesc.PS = desc.ShaderProgram->GetPixelShader();

        // Input layout
        psoDesc.inputLayout = desc.VertexLayout->GetLayout();

        // Primitive type
        psoDesc.primType = ToNvrhiPrimitiveType(desc.State.Primitive);

        // Render state
        psoDesc.renderState.blendState = ToNvrhiBlendState(desc.State.Blend);
        psoDesc.renderState.depthStencilState = ToNvrhiDepthState(desc.State.Depth);
        psoDesc.renderState.rasterState = ToNvrhiRasterState(desc.State.Cull);

        // Binding layouts
        for (const auto& bl : desc.BindingLayouts)
        {
            if (bl)
            {
                psoDesc.bindingLayouts.push_back(bl->GetLayout());
            }
        }

        // Framebuffer info
        nvrhi::FramebufferInfo fbInfo;
        fbInfo.colorFormats.push_back(ToNvrhiFormat(desc.ColorFormat));
        if (desc.DepthFormat != Format::Unknown)
        {
            fbInfo.depthFormat = ToNvrhiFormat(desc.DepthFormat);
        }
        fbInfo.sampleCount = desc.SampleCount;

        nvrhi::GraphicsPipelineHandle pso = nvrhiDevice->createGraphicsPipeline(psoDesc, fbInfo);
        if (!pso)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI graphics pipeline");
            return nullptr;
        }

        return Ref<Pipeline>(new Pipeline(dev, std::move(pso), desc));
    }

    // --- PipelineCache ---

    PipelineCache::PipelineCache(Core::Device& dev)
        : m_Device(dev)
    {
    }

    PipelineCache::~PipelineCache()
    {
        Clear();
    }

    Ref<Pipeline> PipelineCache::GetOrCreate(const PipelineDesc& desc)
    {
        size_t h = desc.Hash();

        auto it = m_Pipelines.find(h);
        if (it != m_Pipelines.end())
        {
            return it->second;
        }

        // Create new pipeline
        auto pso = Pipeline::Create(m_Device, desc);
        if (pso)
        {
            m_Pipelines[h] = pso;
        }

        return pso;
    }

    void PipelineCache::Clear()
    {
        m_Pipelines.clear();
    }
}
