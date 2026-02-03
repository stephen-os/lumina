#pragma once

#include "types.h"

#include <lumina/core/base.h>

namespace nvrhi { class ISampler; }
namespace lumina::core { class device; }

namespace lumina::graphics
{

    struct sampler_desc
    {
        filter_mode filter = filter_mode::linear;
        address_mode address_u = address_mode::clamp;
        address_mode address_v = address_mode::clamp;
        address_mode address_w = address_mode::clamp;
        float max_anisotropy = 1.0f;
        float mip_lod_bias = 0.0f;
        float min_lod = 0.0f;
        float max_lod = 1000.0f;

        sampler_desc& set_filter(filter_mode mode) { filter = mode; return *this; }
        sampler_desc& set_address(address_mode mode) { address_u = address_v = address_w = mode; return *this; }
        sampler_desc& set_anisotropy(float value) { max_anisotropy = value; filter = filter_mode::anisotropic; return *this; }
    };

    class sampler
    {
    public:
        ~sampler();

        sampler(const sampler&) = delete;
        sampler& operator=(const sampler&) = delete;

        static ref<sampler> create(core::device& dev, const sampler_desc& desc = {});

        const sampler_desc& get_desc() const { return m_desc; }

        nvrhi::ISampler* get_sampler() const { return m_handle; }

    private:
        sampler(core::device& dev, nvrhi::ISampler* handle, const sampler_desc& desc)
            : m_device(dev)
            , m_handle(handle)
            , m_desc(desc)
        {}

        core::device& m_device;
        nvrhi::ISampler* m_handle;
        sampler_desc m_desc;
    };

    // Common predefined samplers
    namespace samplers
    {
        inline sampler_desc point_clamp()
        {
            sampler_desc desc;
            desc.filter = filter_mode::point;
            desc.address_u = desc.address_v = desc.address_w = address_mode::clamp;
            return desc;
        }

        inline sampler_desc linear_clamp()
        {
            sampler_desc desc;
            desc.filter = filter_mode::linear;
            desc.address_u = desc.address_v = desc.address_w = address_mode::clamp;
            return desc;
        }

        inline sampler_desc linear_wrap()
        {
            sampler_desc desc;
            desc.filter = filter_mode::linear;
            desc.address_u = desc.address_v = desc.address_w = address_mode::wrap;
            return desc;
        }

        inline sampler_desc anisotropic_clamp(float anisotropy = 16.0f)
        {
            sampler_desc desc;
            desc.filter = filter_mode::anisotropic;
            desc.max_anisotropy = anisotropy;
            desc.address_u = desc.address_v = desc.address_w = address_mode::clamp;
            return desc;
        }
    }
}
