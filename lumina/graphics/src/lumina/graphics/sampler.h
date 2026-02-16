#pragma once

#include "types.h"

#include <lumina/core/base.h>

namespace nvrhi { class ISampler; }
namespace lumina::core { class device; }

namespace lumina::graphics
{
    /// Configuration for how textures are sampled.
    /// Controls filtering (point/linear/anisotropic) and addressing (wrap/clamp/mirror).
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

        sampler_desc& set_filter(filter_mode mode) noexcept { filter = mode; return *this; }
        sampler_desc& set_address(address_mode mode) noexcept { address_u = address_v = address_w = mode; return *this; }
        sampler_desc& set_anisotropy(float value) noexcept { max_anisotropy = value; filter = filter_mode::anisotropic; return *this; }
    };

    /// GPU sampler for controlling texture sampling behavior.
    /// Samplers are immutable; create a new one to change settings.
    class sampler
    {
    public:
        ~sampler();

        sampler(const sampler&) = delete;
        sampler& operator=(const sampler&) = delete;

        /// Creates a sampler with the specified settings. Returns nullptr on failure.
        [[nodiscard]] static ref<sampler> create(core::device& dev, const sampler_desc& desc = {});

        [[nodiscard]] const sampler_desc& get_desc() const noexcept { return m_desc; }
        [[nodiscard]] nvrhi::ISampler* get_sampler() const noexcept { return m_handle; }

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

    /// Common predefined sampler configurations.
    namespace samplers
    {
        [[nodiscard]] inline sampler_desc point_clamp() noexcept
        {
            sampler_desc desc;
            desc.filter = filter_mode::point;
            desc.address_u = desc.address_v = desc.address_w = address_mode::clamp;
            return desc;
        }

        [[nodiscard]] inline sampler_desc linear_clamp() noexcept
        {
            sampler_desc desc;
            desc.filter = filter_mode::linear;
            desc.address_u = desc.address_v = desc.address_w = address_mode::clamp;
            return desc;
        }

        [[nodiscard]] inline sampler_desc linear_wrap() noexcept
        {
            sampler_desc desc;
            desc.filter = filter_mode::linear;
            desc.address_u = desc.address_v = desc.address_w = address_mode::wrap;
            return desc;
        }

        [[nodiscard]] inline sampler_desc anisotropic_clamp(float anisotropy = 16.0f) noexcept
        {
            sampler_desc desc;
            desc.filter = filter_mode::anisotropic;
            desc.max_anisotropy = anisotropy;
            desc.address_u = desc.address_v = desc.address_w = address_mode::clamp;
            return desc;
        }
    }
}
