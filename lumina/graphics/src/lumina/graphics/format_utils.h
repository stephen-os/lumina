#pragma once

#include "types.h"

#include <nvrhi/nvrhi.h>

namespace lumina::graphics
{
    inline nvrhi::Format to_nvrhi_format(format fmt)
    {
        switch (fmt)
        {
            case format::unknown:           return nvrhi::Format::UNKNOWN;

            case format::r8_unorm:          return nvrhi::Format::R8_UNORM;
            case format::r8_snorm:          return nvrhi::Format::R8_SNORM;
            case format::r8_uint:           return nvrhi::Format::R8_UINT;
            case format::r8_sint:           return nvrhi::Format::R8_SINT;

            case format::r16_float:         return nvrhi::Format::R16_FLOAT;
            case format::r16_unorm:         return nvrhi::Format::R16_UNORM;
            case format::r16_uint:          return nvrhi::Format::R16_UINT;
            case format::r16_sint:          return nvrhi::Format::R16_SINT;
            case format::rg8_unorm:         return nvrhi::Format::RG8_UNORM;
            case format::rg8_snorm:         return nvrhi::Format::RG8_SNORM;

            case format::r32_float:         return nvrhi::Format::R32_FLOAT;
            case format::r32_uint:          return nvrhi::Format::R32_UINT;
            case format::r32_sint:          return nvrhi::Format::R32_SINT;
            case format::rg16_float:        return nvrhi::Format::RG16_FLOAT;
            case format::rg16_unorm:        return nvrhi::Format::RG16_UNORM;
            case format::rgba8_unorm:       return nvrhi::Format::RGBA8_UNORM;
            case format::rgba8_unorm_srgb:  return nvrhi::Format::SRGBA8_UNORM;
            case format::rgba8_snorm:       return nvrhi::Format::RGBA8_SNORM;
            case format::bgra8_unorm:       return nvrhi::Format::BGRA8_UNORM;
            case format::bgra8_unorm_srgb:  return nvrhi::Format::SBGRA8_UNORM;

            case format::rg32_float:        return nvrhi::Format::RG32_FLOAT;
            case format::rgba16_float:      return nvrhi::Format::RGBA16_FLOAT;
            case format::rgba16_unorm:      return nvrhi::Format::RGBA16_UNORM;

            case format::rgba32_float:      return nvrhi::Format::RGBA32_FLOAT;

            case format::d16_unorm:         return nvrhi::Format::D16;
            case format::d24_unorm_s8_uint: return nvrhi::Format::D24S8;
            case format::d32_float:         return nvrhi::Format::D32;
            case format::d32_float_s8_uint: return nvrhi::Format::D32S8;

            default:                        return nvrhi::Format::UNKNOWN;
        }
    }

    inline format from_nvrhi_format(nvrhi::Format fmt)
    {
        switch (fmt)
        {
            case nvrhi::Format::UNKNOWN:        return format::unknown;

            case nvrhi::Format::R8_UNORM:       return format::r8_unorm;
            case nvrhi::Format::R8_SNORM:       return format::r8_snorm;
            case nvrhi::Format::R8_UINT:        return format::r8_uint;
            case nvrhi::Format::R8_SINT:        return format::r8_sint;

            case nvrhi::Format::R16_FLOAT:      return format::r16_float;
            case nvrhi::Format::R16_UNORM:      return format::r16_unorm;
            case nvrhi::Format::R16_UINT:       return format::r16_uint;
            case nvrhi::Format::R16_SINT:       return format::r16_sint;
            case nvrhi::Format::RG8_UNORM:      return format::rg8_unorm;
            case nvrhi::Format::RG8_SNORM:      return format::rg8_snorm;

            case nvrhi::Format::R32_FLOAT:      return format::r32_float;
            case nvrhi::Format::R32_UINT:       return format::r32_uint;
            case nvrhi::Format::R32_SINT:       return format::r32_sint;
            case nvrhi::Format::RG16_FLOAT:     return format::rg16_float;
            case nvrhi::Format::RG16_UNORM:     return format::rg16_unorm;
            case nvrhi::Format::RGBA8_UNORM:    return format::rgba8_unorm;
            case nvrhi::Format::SRGBA8_UNORM:   return format::rgba8_unorm_srgb;
            case nvrhi::Format::RGBA8_SNORM:    return format::rgba8_snorm;
            case nvrhi::Format::BGRA8_UNORM:    return format::bgra8_unorm;
            case nvrhi::Format::SBGRA8_UNORM:   return format::bgra8_unorm_srgb;

            case nvrhi::Format::RG32_FLOAT:     return format::rg32_float;
            case nvrhi::Format::RGBA16_FLOAT:   return format::rgba16_float;
            case nvrhi::Format::RGBA16_UNORM:   return format::rgba16_unorm;

            case nvrhi::Format::RGBA32_FLOAT:   return format::rgba32_float;

            case nvrhi::Format::D16:            return format::d16_unorm;
            case nvrhi::Format::D24S8:          return format::d24_unorm_s8_uint;
            case nvrhi::Format::D32:            return format::d32_float;
            case nvrhi::Format::D32S8:          return format::d32_float_s8_uint;

            default:                            return format::unknown;
        }
    }
}
