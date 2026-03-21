#include "compute_shader.h"

#include <lumina/core/device.h>
#include <lumina/core/log.h>

#include <utility>

namespace lumina::graphics
{
    compute_shader::~compute_shader() = default;

    ref<compute_shader> compute_shader::create(
        core::device& dev,
        const void* bytecode,
        size_t bytecode_size,
        std::string_view debug_name)
    {
        compute_shader_desc desc;
        desc.bytecode = bytecode;
        desc.bytecode_size = bytecode_size;
        desc.debug_name = std::string(debug_name);
        return create(dev, desc);
    }

    ref<compute_shader> compute_shader::create(core::device& dev, const compute_shader_desc& desc)
    {
        auto* nvrhi_device = dev.get_nvrhi_device();
        if (!nvrhi_device)
        {
            LUMINA_LOG_ERROR("Failed to create compute shader: no device");
            return nullptr;
        }

        if (!desc.bytecode || desc.bytecode_size == 0)
        {
            LUMINA_LOG_ERROR("Failed to create compute shader: bytecode is empty");
            return nullptr;
        }

        nvrhi::ShaderDesc shader_desc;
        shader_desc.shaderType = nvrhi::ShaderType::Compute;
        shader_desc.debugName = desc.debug_name;
        shader_desc.entryName = desc.entry_point;

        nvrhi::ShaderHandle shader = nvrhi_device->createShader(shader_desc, desc.bytecode, desc.bytecode_size);
        if (!shader)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI compute shader");
            return nullptr;
        }

        return ref<compute_shader>(new compute_shader(dev, std::move(shader)));
    }
}
