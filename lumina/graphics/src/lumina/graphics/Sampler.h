#pragma once

#include "Types.h"

#include <lumina/core/Base.h>

#include <nvrhi/nvrhi.h>

namespace Lumina { class Device; }

namespace Lumina
{
    /// Configuration for how textures are sampled.
    /// Controls filtering (point/linear/anisotropic) and addressing (wrap/clamp/mirror).
    struct SamplerDesc
    {
        FilterMode Filter = FilterMode::Linear;
        AddressMode AddressU = AddressMode::Clamp;
        AddressMode AddressV = AddressMode::Clamp;
        AddressMode AddressW = AddressMode::Clamp;
        float MaxAnisotropy = 1.0f;
        float MipLodBias = 0.0f;
        float MinLod = 0.0f;
        float MaxLod = 1000.0f;

        SamplerDesc& SetFilter(FilterMode mode) noexcept { Filter = mode; return *this; }
        SamplerDesc& SetAddress(AddressMode mode) noexcept { AddressU = AddressV = AddressW = mode; return *this; }
        SamplerDesc& SetAnisotropy(float value) noexcept { MaxAnisotropy = value; Filter = FilterMode::Anisotropic; return *this; }
    };

    /// GPU sampler for controlling texture sampling behavior.
    /// Samplers are immutable; create a new one to change settings.
    class Sampler
    {
    public:
        ~Sampler();

        Sampler(const Sampler&) = delete;
        Sampler& operator=(const Sampler&) = delete;

        /// Creates a sampler with the specified settings. Returns nullptr on failure.
        [[nodiscard]] static Ref<Sampler> Create(Device& dev, const SamplerDesc& desc = {});

        [[nodiscard]] const SamplerDesc& GetDesc() const noexcept { return m_Desc; }
        [[nodiscard]] nvrhi::ISampler* GetSampler() const noexcept { return m_Handle.Get(); }

    private:
        Sampler(Device& dev, nvrhi::SamplerHandle handle, const SamplerDesc& desc)
            : m_Device(dev)
            , m_Handle(std::move(handle))
            , m_Desc(desc)
        {}

        Device& m_Device;
        nvrhi::SamplerHandle m_Handle;
        SamplerDesc m_Desc;
    };

    /// Common predefined sampler configurations.
    namespace Samplers
    {
        [[nodiscard]] inline SamplerDesc PointClamp() noexcept
        {
            SamplerDesc desc;
            desc.Filter = FilterMode::Point;
            desc.AddressU = desc.AddressV = desc.AddressW = AddressMode::Clamp;
            return desc;
        }

        [[nodiscard]] inline SamplerDesc LinearClamp() noexcept
        {
            SamplerDesc desc;
            desc.Filter = FilterMode::Linear;
            desc.AddressU = desc.AddressV = desc.AddressW = AddressMode::Clamp;
            return desc;
        }

        [[nodiscard]] inline SamplerDesc LinearWrap() noexcept
        {
            SamplerDesc desc;
            desc.Filter = FilterMode::Linear;
            desc.AddressU = desc.AddressV = desc.AddressW = AddressMode::Wrap;
            return desc;
        }

        [[nodiscard]] inline SamplerDesc AnisotropicClamp(float anisotropy = 16.0f) noexcept
        {
            SamplerDesc desc;
            desc.Filter = FilterMode::Anisotropic;
            desc.MaxAnisotropy = anisotropy;
            desc.AddressU = desc.AddressV = desc.AddressW = AddressMode::Clamp;
            return desc;
        }
    }
}
