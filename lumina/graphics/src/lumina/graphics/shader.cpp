#include "shader.h"

#include <lumina/core/device.h>

#include <lumina/core/log.h>

#include <utility>

namespace lumina::graphics
{
    shader::~shader() = default;

    ref<shader> shader::create(core::device& dev, const void* vertex_blob, size_t vertex_size, const void* pixel_blob, size_t pixel_size)
    {
        shader_desc desc;
        desc.vertex_blob = vertex_blob;
        desc.vertex_size = vertex_size;
        desc.pixel_blob = pixel_blob;
        desc.pixel_size = pixel_size;
        return create(dev, desc);
    }

    ref<shader> shader::create(core::device& dev, const shader_desc& desc)
    {
        auto* nvrhi_device = dev.get_nvrhi_device();
        if (!nvrhi_device)
        {
            LUMINA_LOG_ERROR("Failed to create shader: no device");
            return nullptr;
        }

        if (!desc.vertex_blob || desc.vertex_size == 0)
        {
            LUMINA_LOG_ERROR("Failed to create shader: vertex shader blob is empty");
            return nullptr;
        }

        if (!desc.pixel_blob || desc.pixel_size == 0)
        {
            LUMINA_LOG_ERROR("Failed to create shader: pixel shader blob is empty");
            return nullptr;
        }

        // Create vertex shader
        nvrhi::ShaderDesc vs_desc;
        vs_desc.shaderType = nvrhi::ShaderType::Vertex;
        vs_desc.debugName = desc.debug_name + " VS";
        vs_desc.entryName = desc.vertex_entry;

        nvrhi::ShaderHandle vertex_shader = nvrhi_device->createShader(vs_desc, desc.vertex_blob, desc.vertex_size);
        if (!vertex_shader)
        {
            LUMINA_LOG_ERROR("Failed to create vertex shader");
            return nullptr;
        }

        // Create pixel shader
        nvrhi::ShaderDesc ps_desc;
        ps_desc.shaderType = nvrhi::ShaderType::Pixel;
        ps_desc.debugName = desc.debug_name + " PS";
        ps_desc.entryName = desc.pixel_entry;

        nvrhi::ShaderHandle pixel_shader = nvrhi_device->createShader(ps_desc, desc.pixel_blob, desc.pixel_size);
        if (!pixel_shader)
        {
            LUMINA_LOG_ERROR("Failed to create pixel shader");
            return nullptr;
        }

        return ref<shader>(new shader(dev, std::move(vertex_shader), std::move(pixel_shader)));
    }
}
