#include "input_layout.h"

#include <lumina/core/device.h>
#include "shader.h"
#include "format_utils.h"

#include <lumina/core/log.h>

#include <nvrhi/nvrhi.h>

namespace lumina::graphics
{
    input_layout::~input_layout()
    {
        if (m_handle)
        {
            m_handle->Release();
            m_handle = nullptr;
        }
    }

    ref<input_layout> input_layout::create(core::device& dev, const input_layout_desc& desc, ref<shader> vertex_shader)
    {
        auto* nvrhi_device = dev.get_nvrhi_device();
        if (!nvrhi_device)
        {
            LUMINA_LOG_ERROR("Failed to create input layout: no device");
            return nullptr;
        }

        if (!vertex_shader)
        {
            LUMINA_LOG_ERROR("Failed to create input layout: vertex shader required");
            return nullptr;
        }

        std::vector<nvrhi::VertexAttributeDesc> nvrhi_attrs;
        nvrhi_attrs.reserve(desc.attributes.size());

        for (const auto& attr : desc.attributes)
        {
            nvrhi::VertexAttributeDesc nvrhi_attr;
            nvrhi_attr.name = attr.name;
            nvrhi_attr.format = to_nvrhi_format(attr.attr_format);
            nvrhi_attr.offset = attr.offset;
            nvrhi_attr.elementStride = desc.stride;
            nvrhi_attr.bufferIndex = 0;
            nvrhi_attr.isInstanced = false;
            nvrhi_attrs.push_back(nvrhi_attr);
        }

        nvrhi::InputLayoutHandle layout = nvrhi_device->createInputLayout(
            nvrhi_attrs.data(),
            static_cast<uint32_t>(nvrhi_attrs.size()),
            vertex_shader->get_vertex_shader()
        );

        if (!layout)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI input layout");
            return nullptr;
-        }

        layout->AddRef();

        return ref<input_layout>(new input_layout(dev, layout.Get(), desc));
    }
}
