#include "input_layout.h"

#include <lumina/core/device.h>
#include "shader.h"
#include "format_utils.h"

#include <lumina/core/log.h>

#include <nvrhi/nvrhi.h>

#include <utility>

namespace Lumina
{
    InputLayout::~InputLayout() = default;

    Ref<InputLayout> InputLayout::Create(Core::Device& dev, const InputLayoutDesc& desc, Ref<Shader> vertexShader)
    {
        auto* nvrhiDevice = dev.GetNvrhiDevice();
        if (!nvrhiDevice)
        {
            LUMINA_LOG_ERROR("Failed to create input layout: no device");
            return nullptr;
        }

        if (!vertexShader)
        {
            LUMINA_LOG_ERROR("Failed to create input layout: vertex shader required");
            return nullptr;
        }

        std::vector<nvrhi::VertexAttributeDesc> nvrhiAttrs;
        nvrhiAttrs.reserve(desc.Attributes.size());

        for (const auto& attr : desc.Attributes)
        {
            nvrhi::VertexAttributeDesc nvrhiAttr;
            nvrhiAttr.name = attr.Name;
            nvrhiAttr.format = ToNvrhiFormat(attr.AttrFormat);
            nvrhiAttr.offset = attr.Offset;
            nvrhiAttr.elementStride = desc.Stride;
            nvrhiAttr.bufferIndex = 0;
            nvrhiAttr.isInstanced = false;
            nvrhiAttrs.push_back(nvrhiAttr);
        }

        nvrhi::InputLayoutHandle layout = nvrhiDevice->createInputLayout(
            nvrhiAttrs.data(),
            static_cast<uint32_t>(nvrhiAttrs.size()),
            vertexShader->GetVertexShader()
        );

        if (!layout)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI input layout");
            return nullptr;
        }

        return Ref<InputLayout>(new InputLayout(dev, std::move(layout), desc));
    }
}
