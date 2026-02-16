#include "binding_layout.h"

#include <lumina/core/device.h>
#include "texture.h"
#include "sampler.h"
#include "uniform_buffer.h"

#include <lumina/core/log.h>

#include <nvrhi/nvrhi.h>

#include <utility>

namespace lumina::graphics
{
    static nvrhi::ResourceType to_nvrhi_resource_type(binding_type type)
    {
        switch (type)
        {
            case binding_type::texture:         return nvrhi::ResourceType::Texture_SRV;
            case binding_type::sampler:         return nvrhi::ResourceType::Sampler;
            case binding_type::constant_buffer: return nvrhi::ResourceType::ConstantBuffer;
            case binding_type::storage_texture: return nvrhi::ResourceType::Texture_UAV;
            case binding_type::storage_buffer:  return nvrhi::ResourceType::StructuredBuffer_UAV;
            default:                            return nvrhi::ResourceType::None;
        }
    }

    // --- binding_set_desc helpers ---

    binding_set_desc& binding_set_desc::add_texture(uint32_t slot, ref<texture> tex)
    {
        bindings.push_back({ slot, binding_type::texture, tex ? tex->get_texture() : nullptr, 0 });
        return *this;
    }

    binding_set_desc& binding_set_desc::add_texture_array_element(uint32_t slot, uint32_t array_index, ref<texture> tex)
    {
        bindings.push_back({ slot, binding_type::texture, tex ? tex->get_texture() : nullptr, array_index });
        return *this;
    }

    binding_set_desc& binding_set_desc::add_sampler(uint32_t slot, ref<sampler> samp)
    {
        bindings.push_back({ slot, binding_type::sampler, samp ? samp->get_sampler() : nullptr });
        return *this;
    }

    binding_set_desc& binding_set_desc::add_constant_buffer(uint32_t slot, ref<uniform_buffer> ubo)
    {
        bindings.push_back({ slot, binding_type::constant_buffer, ubo ? ubo->get_buffer() : nullptr });
        return *this;
    }

    // --- binding_layout ---

    binding_layout::~binding_layout() = default;

    ref<binding_layout> binding_layout::create(core::device& dev, const binding_layout_desc& desc)
    {
        auto* nvrhi_device = dev.get_nvrhi_device();
        if (!nvrhi_device)
        {
            LUMINA_LOG_ERROR("Failed to create binding layout: no device");
            return nullptr;
        }

        // Determine shader visibility
        nvrhi::ShaderType visibility = nvrhi::ShaderType::None;
        if (desc.vertex_shader_visible)
            visibility = visibility | nvrhi::ShaderType::Vertex;
        if (desc.pixel_shader_visible)
            visibility = visibility | nvrhi::ShaderType::Pixel;

        nvrhi::BindingLayoutDesc nvrhi_desc;
        nvrhi_desc.visibility = visibility;

        // Binding offsets must match DXC SPIR-V compilation flags
        // Leave room for multiple textures before samplers start
        nvrhi_desc.bindingOffsets.shaderResource = 0;
        nvrhi_desc.bindingOffsets.sampler = 32;     // Space for up to 32 textures
        nvrhi_desc.bindingOffsets.constantBuffer = 64;
        nvrhi_desc.bindingOffsets.unorderedAccess = 128;

        for (const auto& item : desc.bindings)
        {
            nvrhi::BindingLayoutItem nvrhi_item;
            nvrhi_item.slot = item.slot;
            nvrhi_item.type = to_nvrhi_resource_type(item.type);
            nvrhi_item.size = item.array_size;
            nvrhi_desc.bindings.push_back(nvrhi_item);
        }

        nvrhi::BindingLayoutHandle layout = nvrhi_device->createBindingLayout(nvrhi_desc);
        if (!layout)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI binding layout");
            return nullptr;
        }

        return ref<binding_layout>(new binding_layout(dev, std::move(layout), desc));
    }

    // --- binding_set ---

    binding_set::~binding_set() = default;

    ref<binding_set> binding_set::create(core::device& dev, const binding_set_desc& desc)
    {
        auto* nvrhi_device = dev.get_nvrhi_device();
        if (!nvrhi_device)
        {
            LUMINA_LOG_ERROR("Failed to create binding set: no device");
            return nullptr;
        }

        if (!desc.layout)
        {
            LUMINA_LOG_ERROR("Failed to create binding set: layout required");
            return nullptr;
        }

        nvrhi::BindingSetDesc nvrhi_desc;

        for (const auto& item : desc.bindings)
        {
            switch (item.type)
            {
                case binding_type::texture:
                {
                    auto* tex = static_cast<nvrhi::ITexture*>(item.resource);
                    auto binding = nvrhi::BindingSetItem::Texture_SRV(item.slot, tex);
                    binding.arrayElement = item.array_index;
                    nvrhi_desc.bindings.push_back(binding);
                    break;
                }
                case binding_type::storage_texture:
                {
                    auto* tex = static_cast<nvrhi::ITexture*>(item.resource);
                    nvrhi_desc.bindings.push_back(nvrhi::BindingSetItem::Texture_UAV(item.slot, tex));
                    break;
                }
                case binding_type::constant_buffer:
                {
                    auto* buf = static_cast<nvrhi::IBuffer*>(item.resource);
                    nvrhi_desc.bindings.push_back(nvrhi::BindingSetItem::ConstantBuffer(item.slot, buf));
                    break;
                }
                case binding_type::sampler:
                {
                    auto* samp = static_cast<nvrhi::ISampler*>(item.resource);
                    nvrhi_desc.bindings.push_back(nvrhi::BindingSetItem::Sampler(item.slot, samp));
                    break;
                }
                case binding_type::storage_buffer:
                {
                    auto* buf = static_cast<nvrhi::IBuffer*>(item.resource);
                    nvrhi_desc.bindings.push_back(nvrhi::BindingSetItem::StructuredBuffer_UAV(item.slot, buf));
                    break;
                }
                default:
                    break;
            }
        }

        nvrhi::BindingSetHandle set = nvrhi_device->createBindingSet(nvrhi_desc, desc.layout->get_layout());
        if (!set)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI binding set");
            return nullptr;
        }

        return ref<binding_set>(new binding_set(dev, std::move(set), desc.layout));
    }
}
