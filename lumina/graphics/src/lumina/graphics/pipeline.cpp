#include "pipeline.h"

#include <lumina/core/device.h>
#include "shader.h"
#include "input_layout.h"
#include "binding_layout.h"
#include "render_target.h"
#include "format_utils.h"

#include <lumina/core/log.h>

#include <nvrhi/nvrhi.h>

#include <functional>
#include <utility>

namespace lumina::graphics
{
    // Hash combining helper
    static void hash_combine(size_t& seed, size_t value)
    {
        seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    size_t pipeline_desc::hash() const
    {
        size_t h = 0;

        // Hash shader pointers
        hash_combine(h, reinterpret_cast<size_t>(shader_program.get()));
        hash_combine(h, reinterpret_cast<size_t>(vertex_layout.get()));

        // Hash binding layouts
        for (const auto& bl : binding_layouts)
        {
            hash_combine(h, reinterpret_cast<size_t>(bl.get()));
        }

        // Hash render state
        hash_combine(h, static_cast<size_t>(state.blend));
        hash_combine(h, static_cast<size_t>(state.depth));
        hash_combine(h, static_cast<size_t>(state.cull));
        hash_combine(h, static_cast<size_t>(state.primitive));

        // Hash formats
        hash_combine(h, static_cast<size_t>(color_format));
        hash_combine(h, static_cast<size_t>(depth_format));

        // Hash MSAA sample count
        hash_combine(h, static_cast<size_t>(sample_count));

        return h;
    }

    // Convert Lumina blend mode to NVRHI
    static nvrhi::BlendState to_nvrhi_blend_state(blend_mode mode)
    {
        nvrhi::BlendState bs;
        bs.targets[0].blendEnable = true;

        switch (mode)
        {
            case blend_mode::opaque:
                bs.targets[0].blendEnable = false;
                break;

            case blend_mode::alpha:
                bs.targets[0].srcBlend = nvrhi::BlendFactor::SrcAlpha;
                bs.targets[0].destBlend = nvrhi::BlendFactor::InvSrcAlpha;
                bs.targets[0].srcBlendAlpha = nvrhi::BlendFactor::One;
                bs.targets[0].destBlendAlpha = nvrhi::BlendFactor::InvSrcAlpha;
                break;

            case blend_mode::additive:
                bs.targets[0].srcBlend = nvrhi::BlendFactor::SrcAlpha;
                bs.targets[0].destBlend = nvrhi::BlendFactor::One;
                bs.targets[0].srcBlendAlpha = nvrhi::BlendFactor::One;
                bs.targets[0].destBlendAlpha = nvrhi::BlendFactor::One;
                break;

            case blend_mode::multiply:
                bs.targets[0].srcBlend = nvrhi::BlendFactor::DstColor;
                bs.targets[0].destBlend = nvrhi::BlendFactor::Zero;
                bs.targets[0].srcBlendAlpha = nvrhi::BlendFactor::DstAlpha;
                bs.targets[0].destBlendAlpha = nvrhi::BlendFactor::Zero;
                break;
        }

        return bs;
    }

    // Convert Lumina depth mode to NVRHI
    static nvrhi::DepthStencilState to_nvrhi_depth_state(depth_mode mode)
    {
        nvrhi::DepthStencilState ds;

        switch (mode)
        {
            case depth_mode::none:
                ds.depthTestEnable = false;
                ds.depthWriteEnable = false;
                break;

            case depth_mode::read_only:
                ds.depthTestEnable = true;
                ds.depthWriteEnable = false;
                ds.depthFunc = nvrhi::ComparisonFunc::LessOrEqual;
                break;

            case depth_mode::read_write:
                ds.depthTestEnable = true;
                ds.depthWriteEnable = true;
                ds.depthFunc = nvrhi::ComparisonFunc::LessOrEqual;
                break;
        }

        return ds;
    }

    // Convert Lumina cull mode to NVRHI
    static nvrhi::RasterState to_nvrhi_raster_state(cull_mode mode)
    {
        nvrhi::RasterState rs;

        switch (mode)
        {
            case cull_mode::none:
                rs.cullMode = nvrhi::RasterCullMode::None;
                break;
            case cull_mode::back:
                rs.cullMode = nvrhi::RasterCullMode::Back;
                break;
            case cull_mode::front:
                rs.cullMode = nvrhi::RasterCullMode::Front;
                break;
        }

        rs.frontCounterClockwise = true;

        return rs;
    }

    // Convert Lumina topology to NVRHI
    static nvrhi::PrimitiveType to_nvrhi_primitive_type(topology topo)
    {
        switch (topo)
        {
            case topology::triangles:       return nvrhi::PrimitiveType::TriangleList;
            case topology::triangle_strip:  return nvrhi::PrimitiveType::TriangleStrip;
            case topology::lines:           return nvrhi::PrimitiveType::LineList;
            case topology::line_strip:      return nvrhi::PrimitiveType::LineStrip;
            case topology::points:          return nvrhi::PrimitiveType::PointList;
            default:                        return nvrhi::PrimitiveType::TriangleList;
        }
    }

    // --- pipeline ---

    pipeline::~pipeline() = default;

    ref<pipeline> pipeline::create(core::device& dev, const pipeline_desc& desc)
    {
        auto* nvrhi_device = dev.get_nvrhi_device();
        if (!nvrhi_device)
        {
            LUMINA_LOG_ERROR("Failed to create pipeline: no device");
            return nullptr;
        }

        if (!desc.shader_program)
        {
            LUMINA_LOG_ERROR("Failed to create pipeline: shader required");
            return nullptr;
        }

        if (!desc.vertex_layout)
        {
            LUMINA_LOG_ERROR("Failed to create pipeline: vertex layout required");
            return nullptr;
        }

        // Build NVRHI pipeline description
        nvrhi::GraphicsPipelineDesc pso_desc;

        // Shaders
        pso_desc.VS = desc.shader_program->get_vertex_shader();
        pso_desc.PS = desc.shader_program->get_pixel_shader();

        // Input layout
        pso_desc.inputLayout = desc.vertex_layout->get_layout();

        // Primitive type
        pso_desc.primType = to_nvrhi_primitive_type(desc.state.primitive);

        // Render state
        pso_desc.renderState.blendState = to_nvrhi_blend_state(desc.state.blend);
        pso_desc.renderState.depthStencilState = to_nvrhi_depth_state(desc.state.depth);
        pso_desc.renderState.rasterState = to_nvrhi_raster_state(desc.state.cull);

        // Binding layouts
        for (const auto& bl : desc.binding_layouts)
        {
            if (bl)
            {
                pso_desc.bindingLayouts.push_back(bl->get_layout());
            }
        }

        // Framebuffer info
        nvrhi::FramebufferInfo fb_info;
        fb_info.colorFormats.push_back(to_nvrhi_format(desc.color_format));
        if (desc.depth_format != format::unknown)
        {
            fb_info.depthFormat = to_nvrhi_format(desc.depth_format);
        }
        fb_info.sampleCount = desc.sample_count;

        nvrhi::GraphicsPipelineHandle pso = nvrhi_device->createGraphicsPipeline(pso_desc, fb_info);
        if (!pso)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI graphics pipeline");
            return nullptr;
        }

        return ref<pipeline>(new pipeline(dev, std::move(pso), desc));
    }

    // --- pipeline_cache ---

    pipeline_cache::pipeline_cache(core::device& dev)
        : m_device(dev)
    {
    }

    pipeline_cache::~pipeline_cache()
    {
        clear();
    }

    ref<pipeline> pipeline_cache::get_or_create(const pipeline_desc& desc)
    {
        size_t h = desc.hash();

        auto it = m_pipelines.find(h);
        if (it != m_pipelines.end())
        {
            return it->second;
        }

        // Create new pipeline
        auto pso = pipeline::create(m_device, desc);
        if (pso)
        {
            m_pipelines[h] = pso;
        }

        return pso;
    }

    void pipeline_cache::clear()
    {
        m_pipelines.clear();
    }
}
