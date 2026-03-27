#include "Sampler.h"

#include <Lumina/Core/Device.h>

#include <Lumina/Core/Log.h>

#include <utility>

namespace Lumina
{
    static nvrhi::SamplerAddressMode ToNvrhiAddressMode(AddressMode mode)
    {
        switch (mode)
        {
            case AddressMode::Wrap:   return nvrhi::SamplerAddressMode::Repeat;
            case AddressMode::Clamp:  return nvrhi::SamplerAddressMode::ClampToEdge;
            case AddressMode::Mirror: return nvrhi::SamplerAddressMode::MirroredRepeat;
            case AddressMode::Border: return nvrhi::SamplerAddressMode::ClampToBorder;
            default:                  return nvrhi::SamplerAddressMode::ClampToEdge;
        }
    }

    Sampler::~Sampler() = default;

    Ref<Sampler> Sampler::Create(Core::Device& dev, const SamplerDesc& desc)
    {
        auto* nvrhiDevice = dev.GetNvrhiDevice();
        if (!nvrhiDevice)
        {
            LUMINA_LOG_ERROR("Failed to create sampler: no device");
            return nullptr;
        }

        nvrhi::SamplerDesc nvrhiDesc;
        nvrhiDesc.addressU = ToNvrhiAddressMode(desc.AddressU);
        nvrhiDesc.addressV = ToNvrhiAddressMode(desc.AddressV);
        nvrhiDesc.addressW = ToNvrhiAddressMode(desc.AddressW);
        nvrhiDesc.mipBias = desc.MipLodBias;

        switch (desc.Filter)
        {
            case FilterMode::Point:
                nvrhiDesc.minFilter = false;
                nvrhiDesc.magFilter = false;
                nvrhiDesc.mipFilter = false;
                break;

            case FilterMode::Linear:
                nvrhiDesc.minFilter = true;
                nvrhiDesc.magFilter = true;
                nvrhiDesc.mipFilter = true;
                break;

            case FilterMode::Anisotropic:
                nvrhiDesc.minFilter = true;
                nvrhiDesc.magFilter = true;
                nvrhiDesc.mipFilter = true;
                nvrhiDesc.maxAnisotropy = desc.MaxAnisotropy;
                break;
        }

        nvrhi::SamplerHandle samp = nvrhiDevice->createSampler(nvrhiDesc);
        if (!samp)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI sampler");
            return nullptr;
        }

        return Ref<Sampler>(new Sampler(dev, std::move(samp), desc));
    }
}
