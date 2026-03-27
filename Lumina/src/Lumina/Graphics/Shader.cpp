#include "Shader.h"

#include <Lumina/Core/Device.h>

#include <Lumina/Core/Log.h>

#include <utility>

namespace Lumina
{
    Shader::~Shader() = default;

    Ref<Shader> Shader::Create(Core::Device& dev, const void* vertexBlob, size_t vertexSize, const void* pixelBlob, size_t pixelSize)
    {
        ShaderDesc desc;
        desc.VertexBlob = vertexBlob;
        desc.VertexSize = vertexSize;
        desc.PixelBlob = pixelBlob;
        desc.PixelSize = pixelSize;
        return Create(dev, desc);
    }

    Ref<Shader> Shader::Create(Core::Device& dev, const ShaderDesc& desc)
    {
        auto* nvrhiDevice = dev.GetNvrhiDevice();
        if (!nvrhiDevice)
        {
            LUMINA_LOG_ERROR("Failed to create shader: no device");
            return nullptr;
        }

        if (!desc.VertexBlob || desc.VertexSize == 0)
        {
            LUMINA_LOG_ERROR("Failed to create shader: vertex shader blob is empty");
            return nullptr;
        }

        if (!desc.PixelBlob || desc.PixelSize == 0)
        {
            LUMINA_LOG_ERROR("Failed to create shader: pixel shader blob is empty");
            return nullptr;
        }

        // Create vertex shader
        nvrhi::ShaderDesc vsDesc;
        vsDesc.shaderType = nvrhi::ShaderType::Vertex;
        vsDesc.debugName = desc.DebugName + " VS";
        vsDesc.entryName = desc.VertexEntry;

        nvrhi::ShaderHandle vertexShader = nvrhiDevice->createShader(vsDesc, desc.VertexBlob, desc.VertexSize);
        if (!vertexShader)
        {
            LUMINA_LOG_ERROR("Failed to create vertex shader");
            return nullptr;
        }

        // Create pixel shader
        nvrhi::ShaderDesc psDesc;
        psDesc.shaderType = nvrhi::ShaderType::Pixel;
        psDesc.debugName = desc.DebugName + " PS";
        psDesc.entryName = desc.PixelEntry;

        nvrhi::ShaderHandle pixelShader = nvrhiDevice->createShader(psDesc, desc.PixelBlob, desc.PixelSize);
        if (!pixelShader)
        {
            LUMINA_LOG_ERROR("Failed to create pixel shader");
            return nullptr;
        }

        return Ref<Shader>(new Shader(dev, std::move(vertexShader), std::move(pixelShader)));
    }
}
