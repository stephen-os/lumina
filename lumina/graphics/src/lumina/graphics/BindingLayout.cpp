#include "BindingLayout.h"

#include <lumina/core/Device.h>
#include "Texture.h"
#include "Sampler.h"
#include "UniformBuffer.h"
#include "StorageBuffer.h"

#include <lumina/core/Log.h>

#include <nvrhi/nvrhi.h>

#include <utility>

namespace Lumina
{
    static nvrhi::ResourceType ToNvrhiResourceType(BindingType type)
    {
        switch (type)
        {
            case BindingType::Texture:          return nvrhi::ResourceType::Texture_SRV;
            case BindingType::Sampler:          return nvrhi::ResourceType::Sampler;
            case BindingType::ConstantBuffer:   return nvrhi::ResourceType::ConstantBuffer;
            case BindingType::StorageTexture:   return nvrhi::ResourceType::Texture_UAV;
            case BindingType::StorageBuffer:    return nvrhi::ResourceType::StructuredBuffer_UAV;
            case BindingType::StructuredBuffer: return nvrhi::ResourceType::StructuredBuffer_SRV;
            default:                            return nvrhi::ResourceType::None;
        }
    }

    // --- BindingSetDesc helpers ---

    BindingSetDesc& BindingSetDesc::AddTexture(uint32_t slot, Ref<Texture> tex)
    {
        Bindings.push_back({ slot, BindingType::Texture, tex ? tex->GetTexture() : nullptr, 0 });
        return *this;
    }

    BindingSetDesc& BindingSetDesc::AddTextureArrayElement(uint32_t slot, uint32_t arrayIndex, Ref<Texture> tex)
    {
        Bindings.push_back({ slot, BindingType::Texture, tex ? tex->GetTexture() : nullptr, arrayIndex });
        return *this;
    }

    BindingSetDesc& BindingSetDesc::AddSampler(uint32_t slot, Ref<Sampler> samp)
    {
        Bindings.push_back({ slot, BindingType::Sampler, samp ? samp->GetSampler() : nullptr });
        return *this;
    }

    BindingSetDesc& BindingSetDesc::AddConstantBuffer(uint32_t slot, Ref<UniformBuffer> ubo)
    {
        Bindings.push_back({ slot, BindingType::ConstantBuffer, ubo ? ubo->GetBuffer() : nullptr });
        return *this;
    }

    BindingSetDesc& BindingSetDesc::AddStorageTexture(uint32_t slot, Ref<Texture> tex)
    {
        Bindings.push_back({ slot, BindingType::StorageTexture, tex ? tex->GetTexture() : nullptr, 0 });
        return *this;
    }

    BindingSetDesc& BindingSetDesc::AddStorageBuffer(uint32_t slot, Ref<StorageBuffer> buf)
    {
        Bindings.push_back({ slot, BindingType::StorageBuffer, buf ? buf->GetBuffer() : nullptr, 0 });
        return *this;
    }

    BindingSetDesc& BindingSetDesc::AddStructuredBuffer(uint32_t slot, Ref<StorageBuffer> buf)
    {
        Bindings.push_back({ slot, BindingType::StructuredBuffer, buf ? buf->GetBuffer() : nullptr, 0 });
        return *this;
    }

    // --- BindingLayout ---

    BindingLayout::~BindingLayout() = default;

    Ref<BindingLayout> BindingLayout::Create(Device& dev, const BindingLayoutDesc& desc)
    {
        auto* nvrhiDevice = dev.GetNvrhiDevice();
        if (!nvrhiDevice)
        {
            LUMINA_LOG_ERROR("Failed to create binding layout: no device");
            return nullptr;
        }

        // Determine shader visibility
        nvrhi::ShaderType visibility = nvrhi::ShaderType::None;
        if (desc.VertexShaderVisible)
            visibility = visibility | nvrhi::ShaderType::Vertex;
        if (desc.PixelShaderVisible)
            visibility = visibility | nvrhi::ShaderType::Pixel;
        if (desc.ComputeShaderVisible)
            visibility = visibility | nvrhi::ShaderType::Compute;

        nvrhi::BindingLayoutDesc nvrhiDesc;
        nvrhiDesc.visibility = visibility;

        // Binding offsets must match DXC SPIR-V compilation flags
        // Leave room for multiple textures before samplers start
        nvrhiDesc.bindingOffsets.shaderResource = 0;
        nvrhiDesc.bindingOffsets.sampler = 32;     // Space for up to 32 textures
        nvrhiDesc.bindingOffsets.constantBuffer = 64;
        nvrhiDesc.bindingOffsets.unorderedAccess = 128;

        for (const auto& item : desc.Bindings)
        {
            nvrhi::BindingLayoutItem nvrhiItem;
            nvrhiItem.slot = item.Slot;
            nvrhiItem.type = ToNvrhiResourceType(item.Type);
            nvrhiItem.size = item.ArraySize;
            nvrhiDesc.bindings.push_back(nvrhiItem);
        }

        nvrhi::BindingLayoutHandle layout = nvrhiDevice->createBindingLayout(nvrhiDesc);
        if (!layout)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI binding layout");
            return nullptr;
        }

        return Ref<BindingLayout>(new BindingLayout(dev, std::move(layout), desc));
    }

    // --- BindingSet ---

    BindingSet::~BindingSet() = default;

    Ref<BindingSet> BindingSet::Create(Device& dev, const BindingSetDesc& desc)
    {
        auto* nvrhiDevice = dev.GetNvrhiDevice();
        if (!nvrhiDevice)
        {
            LUMINA_LOG_ERROR("Failed to create binding set: no device");
            return nullptr;
        }

        if (!desc.Layout)
        {
            LUMINA_LOG_ERROR("Failed to create binding set: layout required");
            return nullptr;
        }

        nvrhi::BindingSetDesc nvrhiDesc;

        for (const auto& item : desc.Bindings)
        {
            switch (item.Type)
            {
                case BindingType::Texture:
                {
                    auto* tex = static_cast<nvrhi::ITexture*>(item.Resource);
                    auto binding = nvrhi::BindingSetItem::Texture_SRV(item.Slot, tex);
                    binding.arrayElement = item.ArrayIndex;
                    nvrhiDesc.bindings.push_back(binding);
                    break;
                }
                case BindingType::StorageTexture:
                {
                    auto* tex = static_cast<nvrhi::ITexture*>(item.Resource);
                    nvrhiDesc.bindings.push_back(nvrhi::BindingSetItem::Texture_UAV(item.Slot, tex));
                    break;
                }
                case BindingType::ConstantBuffer:
                {
                    auto* buf = static_cast<nvrhi::IBuffer*>(item.Resource);
                    nvrhiDesc.bindings.push_back(nvrhi::BindingSetItem::ConstantBuffer(item.Slot, buf));
                    break;
                }
                case BindingType::Sampler:
                {
                    auto* samp = static_cast<nvrhi::ISampler*>(item.Resource);
                    nvrhiDesc.bindings.push_back(nvrhi::BindingSetItem::Sampler(item.Slot, samp));
                    break;
                }
                case BindingType::StorageBuffer:
                {
                    auto* buf = static_cast<nvrhi::IBuffer*>(item.Resource);
                    nvrhiDesc.bindings.push_back(nvrhi::BindingSetItem::StructuredBuffer_UAV(item.Slot, buf));
                    break;
                }
                case BindingType::StructuredBuffer:
                {
                    auto* buf = static_cast<nvrhi::IBuffer*>(item.Resource);
                    nvrhiDesc.bindings.push_back(nvrhi::BindingSetItem::StructuredBuffer_SRV(item.Slot, buf));
                    break;
                }
                default:
                    break;
            }
        }

        nvrhi::BindingSetHandle set = nvrhiDevice->createBindingSet(nvrhiDesc, desc.Layout->GetLayout());
        if (!set)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI binding set");
            return nullptr;
        }

        return Ref<BindingSet>(new BindingSet(dev, std::move(set), desc.Layout));
    }
}
