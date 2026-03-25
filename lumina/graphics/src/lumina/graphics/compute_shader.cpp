#include "compute_shader.h"

#include <lumina/core/device.h>
#include <lumina/core/log.h>

#include <utility>

namespace Lumina
{
    ComputeShader::~ComputeShader() = default;

    Ref<ComputeShader> ComputeShader::Create(
        Device& dev,
        const void* bytecode,
        size_t bytecodeSize,
        std::string_view debugName)
    {
        ComputeShaderDesc desc;
        desc.Bytecode = bytecode;
        desc.BytecodeSize = bytecodeSize;
        desc.DebugName = std::string(debugName);
        return Create(dev, desc);
    }

    Ref<ComputeShader> ComputeShader::Create(Device& dev, const ComputeShaderDesc& desc)
    {
        auto* nvrhiDevice = dev.GetNvrhiDevice();
        if (!nvrhiDevice)
        {
            LUMINA_LOG_ERROR("Failed to create compute shader: no device");
            return nullptr;
        }

        if (!desc.Bytecode || desc.BytecodeSize == 0)
        {
            LUMINA_LOG_ERROR("Failed to create compute shader: bytecode is empty");
            return nullptr;
        }

        nvrhi::ShaderDesc shaderDesc;
        shaderDesc.shaderType = nvrhi::ShaderType::Compute;
        shaderDesc.debugName = desc.DebugName;
        shaderDesc.entryName = desc.EntryPoint;

        nvrhi::ShaderHandle shader = nvrhiDevice->createShader(shaderDesc, desc.Bytecode, desc.BytecodeSize);
        if (!shader)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI compute shader");
            return nullptr;
        }

        return Ref<ComputeShader>(new ComputeShader(dev, std::move(shader)));
    }
}
