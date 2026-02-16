#include "sampler.h"

#include <lumina/core/device.h>

#include <lumina/core/log.h>

#include <utility>

namespace lumina::graphics
{
    static nvrhi::SamplerAddressMode to_nvrhi_address_mode(address_mode mode)
    {
        switch (mode)
        {
            case address_mode::wrap:   return nvrhi::SamplerAddressMode::Repeat;
            case address_mode::clamp:  return nvrhi::SamplerAddressMode::ClampToEdge;
            case address_mode::mirror: return nvrhi::SamplerAddressMode::MirroredRepeat;
            case address_mode::border: return nvrhi::SamplerAddressMode::ClampToBorder;
            default:                   return nvrhi::SamplerAddressMode::ClampToEdge;
        }
    }

    sampler::~sampler() = default;

    ref<sampler> sampler::create(core::device& dev, const sampler_desc& desc)
    {
        auto* nvrhi_device = dev.get_nvrhi_device();
        if (!nvrhi_device)
        {
            LUMINA_LOG_ERROR("Failed to create sampler: no device");
            return nullptr;
        }

        nvrhi::SamplerDesc nvrhi_desc;
        nvrhi_desc.addressU = to_nvrhi_address_mode(desc.address_u);
        nvrhi_desc.addressV = to_nvrhi_address_mode(desc.address_v);
        nvrhi_desc.addressW = to_nvrhi_address_mode(desc.address_w);
        nvrhi_desc.mipBias = desc.mip_lod_bias;

        switch (desc.filter)
        {
            case filter_mode::point:
                nvrhi_desc.minFilter = false;
                nvrhi_desc.magFilter = false;
                nvrhi_desc.mipFilter = false;
                break;

            case filter_mode::linear:
                nvrhi_desc.minFilter = true;
                nvrhi_desc.magFilter = true;
                nvrhi_desc.mipFilter = true;
                break;

            case filter_mode::anisotropic:
                nvrhi_desc.minFilter = true;
                nvrhi_desc.magFilter = true;
                nvrhi_desc.mipFilter = true;
                nvrhi_desc.maxAnisotropy = desc.max_anisotropy;
                break;
        }

        nvrhi::SamplerHandle samp = nvrhi_device->createSampler(nvrhi_desc);
        if (!samp)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI sampler");
            return nullptr;
        }

        return ref<sampler>(new sampler(dev, std::move(samp), desc));
    }
}
