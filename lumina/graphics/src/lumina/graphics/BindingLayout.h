#pragma once

#include "Types.h"

#include <lumina/core/Base.h>

#include <nvrhi/nvrhi.h>

#include <utility>
#include <vector>

namespace Lumina { class Device; }

namespace Lumina
{
    class Texture;
    class UniformBuffer;
    class StorageBuffer;

    /// Types of shader resource bindings.
    enum class BindingType
    {
        Texture,            // Shader resource view (SRV) for textures
        Sampler,            // Texture sampler
        ConstantBuffer,     // Uniform/constant buffer
        StorageTexture,     // Read-write texture (UAV)
        StorageBuffer,      // Read-write buffer (UAV)
        StructuredBuffer    // Read-only structured buffer (SRV)
    };

    /// Describes a single binding slot in a binding layout.
    struct BindingItem
    {
        uint32_t Slot = 0;
        BindingType Type = BindingType::Texture;
        uint32_t ArraySize = 1;

        BindingItem() = default;
        BindingItem(uint32_t slot, BindingType type, uint32_t size = 1) : Slot(slot), Type(type), ArraySize(size) {}

        [[nodiscard]] static BindingItem Texture(uint32_t slot) noexcept { return { slot, BindingType::Texture, 1 }; }
        [[nodiscard]] static BindingItem TextureArray(uint32_t slot, uint32_t count) noexcept { return { slot, BindingType::Texture, count }; }
        [[nodiscard]] static BindingItem Sampler(uint32_t slot) noexcept { return { slot, BindingType::Sampler, 1 }; }
        [[nodiscard]] static BindingItem ConstantBuffer(uint32_t slot) noexcept { return { slot, BindingType::ConstantBuffer, 1 }; }
        [[nodiscard]] static BindingItem StorageTexture(uint32_t slot) noexcept { return { slot, BindingType::StorageTexture, 1 }; }
        [[nodiscard]] static BindingItem StorageBuffer(uint32_t slot) noexcept { return { slot, BindingType::StorageBuffer, 1 }; }
        [[nodiscard]] static BindingItem StructuredBuffer(uint32_t slot) noexcept { return { slot, BindingType::StructuredBuffer, 1 }; }
    };

    /// Configuration for binding layout creation.
    struct BindingLayoutDesc
    {
        std::vector<BindingItem> Bindings;
        bool VertexShaderVisible = true;
        bool PixelShaderVisible = true;
        bool ComputeShaderVisible = false;

        BindingLayoutDesc& AddTexture(uint32_t slot)
        {
            Bindings.push_back(BindingItem::Texture(slot));
            return *this;
        }

        BindingLayoutDesc& AddTextureArray(uint32_t slot, uint32_t count)
        {
            Bindings.push_back(BindingItem::TextureArray(slot, count));
            return *this;
        }

        BindingLayoutDesc& AddSampler(uint32_t slot)
        {
            Bindings.push_back(BindingItem::Sampler(slot));
            return *this;
        }

        BindingLayoutDesc& AddConstantBuffer(uint32_t slot)
        {
            Bindings.push_back(BindingItem::ConstantBuffer(slot));
            return *this;
        }

        BindingLayoutDesc& AddStorageTexture(uint32_t slot)
        {
            Bindings.push_back(BindingItem::StorageTexture(slot));
            return *this;
        }

        BindingLayoutDesc& AddStorageBuffer(uint32_t slot)
        {
            Bindings.push_back(BindingItem::StorageBuffer(slot));
            return *this;
        }

        BindingLayoutDesc& AddStructuredBuffer(uint32_t slot)
        {
            Bindings.push_back(BindingItem::StructuredBuffer(slot));
            return *this;
        }

        /// Sets visibility for compute shaders only.
        BindingLayoutDesc& ForCompute()
        {
            VertexShaderVisible = false;
            PixelShaderVisible = false;
            ComputeShaderVisible = true;
            return *this;
        }

        /// Sets visibility for all shader stages.
        BindingLayoutDesc& ForAllStages()
        {
            VertexShaderVisible = true;
            PixelShaderVisible = true;
            ComputeShaderVisible = true;
            return *this;
        }
    };

    /// GPU binding layout describing shader resource binding slots.
    /// Defines the structure of resource bindings without actual resources.
    class BindingLayout
    {
    public:
        ~BindingLayout();

        BindingLayout(const BindingLayout&) = delete;
        BindingLayout& operator=(const BindingLayout&) = delete;

        /// Creates a binding layout. Returns nullptr on failure.
        [[nodiscard]] static Ref<BindingLayout> Create(Device& dev, const BindingLayoutDesc& desc);

        [[nodiscard]] const BindingLayoutDesc& GetDesc() const noexcept { return m_Desc; }
        [[nodiscard]] nvrhi::IBindingLayout* GetLayout() const noexcept { return m_Handle.Get(); }

    private:
        BindingLayout(Device& dev, nvrhi::BindingLayoutHandle handle, const BindingLayoutDesc& desc)
            : m_Device(dev)
            , m_Handle(std::move(handle))
            , m_Desc(desc)
        {}

        Device& m_Device;
        nvrhi::BindingLayoutHandle m_Handle;
        BindingLayoutDesc m_Desc;
    };

    /// Describes a resource bound to a specific slot.
    struct BindingSetItem
    {
        uint32_t Slot = 0;                          // Binding slot
        uint32_t ArrayIndex = 0;                    // Array index for array bindings
        BindingType Type = BindingType::Texture;    // Type of binding
        void* Resource = nullptr;                   // Pointer to resource (texture, sampler, buffer, etc.)

        BindingSetItem() = default;
        BindingSetItem(uint32_t slot, BindingType type, void* res, uint32_t arrIdx = 0)
            : Slot(slot), ArrayIndex(arrIdx), Type(type), Resource(res) {}
    };

    class Texture;
    class Sampler;

    /// Configuration for binding set creation.
    struct BindingSetDesc
    {
        Ref<BindingLayout> Layout;
        std::vector<BindingSetItem> Bindings;

        BindingSetDesc& AddTexture(uint32_t slot, Ref<Texture> tex);
        BindingSetDesc& AddTextureArrayElement(uint32_t slot, uint32_t arrayIndex, Ref<Texture> tex);
        BindingSetDesc& AddSampler(uint32_t slot, Ref<Sampler> samp);
        BindingSetDesc& AddConstantBuffer(uint32_t slot, Ref<UniformBuffer> ubo);
        BindingSetDesc& AddStorageTexture(uint32_t slot, Ref<Texture> tex);
        BindingSetDesc& AddStorageBuffer(uint32_t slot, Ref<StorageBuffer> buf);
        BindingSetDesc& AddStructuredBuffer(uint32_t slot, Ref<StorageBuffer> buf);
    };

    /// GPU binding set containing actual resource bindings.
    /// Associates concrete resources with binding layout slots.
    class BindingSet
    {
    public:
        ~BindingSet();

        BindingSet(const BindingSet&) = delete;
        BindingSet& operator=(const BindingSet&) = delete;

        /// Creates a binding set. Returns nullptr on failure.
        [[nodiscard]] static Ref<BindingSet> Create(Device& dev, const BindingSetDesc& desc);

        [[nodiscard]] nvrhi::IBindingSet* GetBindingSet() const noexcept { return m_Handle.Get(); }

    private:
        BindingSet(Device& dev, nvrhi::BindingSetHandle handle, Ref<BindingLayout> layout)
            : m_Device(dev)
            , m_Handle(std::move(handle))
            , m_Layout(layout)
        {}

        Device& m_Device;
        nvrhi::BindingSetHandle m_Handle;
        Ref<BindingLayout> m_Layout;
    };

    /// Common predefined binding layouts.
    namespace BindingLayouts
    {
        /// Single texture + sampler (typical 2D sprite shader)
        [[nodiscard]] inline BindingLayoutDesc TextureSampler() noexcept
        {
            BindingLayoutDesc desc;
            desc.AddTexture(0);
            desc.AddSampler(0);
            return desc;
        }

        /// Constant buffer only (transform matrices, etc.)
        [[nodiscard]] inline BindingLayoutDesc ConstantBuffer() noexcept
        {
            BindingLayoutDesc desc;
            desc.AddConstantBuffer(0);
            return desc;
        }

        /// Constant buffer + texture + sampler
        [[nodiscard]] inline BindingLayoutDesc Standard2D() noexcept
        {
            BindingLayoutDesc desc;
            desc.AddConstantBuffer(0);
            desc.AddTexture(0);
            desc.AddSampler(0);
            return desc;
        }
    }
}
