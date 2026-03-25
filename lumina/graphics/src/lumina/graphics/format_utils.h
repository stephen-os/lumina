#pragma once

#include "types.h"

#include <nvrhi/nvrhi.h>

namespace Lumina
{
    [[nodiscard]] inline nvrhi::Format ToNvrhiFormat(Format fmt) noexcept
    {
        switch (fmt)
        {
            case Format::Unknown:           return nvrhi::Format::UNKNOWN;

            case Format::R8Unorm:           return nvrhi::Format::R8_UNORM;
            case Format::R8Snorm:           return nvrhi::Format::R8_SNORM;
            case Format::R8Uint:            return nvrhi::Format::R8_UINT;
            case Format::R8Sint:            return nvrhi::Format::R8_SINT;

            case Format::R16Float:          return nvrhi::Format::R16_FLOAT;
            case Format::R16Unorm:          return nvrhi::Format::R16_UNORM;
            case Format::R16Uint:           return nvrhi::Format::R16_UINT;
            case Format::R16Sint:           return nvrhi::Format::R16_SINT;
            case Format::RG8Unorm:          return nvrhi::Format::RG8_UNORM;
            case Format::RG8Snorm:          return nvrhi::Format::RG8_SNORM;

            case Format::R32Float:          return nvrhi::Format::R32_FLOAT;
            case Format::R32Uint:           return nvrhi::Format::R32_UINT;
            case Format::R32Sint:           return nvrhi::Format::R32_SINT;
            case Format::RG16Float:         return nvrhi::Format::RG16_FLOAT;
            case Format::RG16Unorm:         return nvrhi::Format::RG16_UNORM;
            case Format::RGBA8Unorm:        return nvrhi::Format::RGBA8_UNORM;
            case Format::RGBA8UnormSrgb:    return nvrhi::Format::SRGBA8_UNORM;
            case Format::RGBA8Snorm:        return nvrhi::Format::RGBA8_SNORM;
            case Format::BGRA8Unorm:        return nvrhi::Format::BGRA8_UNORM;
            case Format::BGRA8UnormSrgb:    return nvrhi::Format::SBGRA8_UNORM;

            case Format::RG32Float:         return nvrhi::Format::RG32_FLOAT;
            case Format::RGBA16Float:       return nvrhi::Format::RGBA16_FLOAT;
            case Format::RGBA16Unorm:       return nvrhi::Format::RGBA16_UNORM;

            case Format::RGBA32Float:       return nvrhi::Format::RGBA32_FLOAT;

            case Format::D16Unorm:          return nvrhi::Format::D16;
            case Format::D24UnormS8Uint:    return nvrhi::Format::D24S8;
            case Format::D32Float:          return nvrhi::Format::D32;
            case Format::D32FloatS8Uint:    return nvrhi::Format::D32S8;

            default:                        return nvrhi::Format::UNKNOWN;
        }
    }

    [[nodiscard]] inline Format FromNvrhiFormat(nvrhi::Format fmt) noexcept
    {
        switch (fmt)
        {
            case nvrhi::Format::UNKNOWN:        return Format::Unknown;

            case nvrhi::Format::R8_UNORM:       return Format::R8Unorm;
            case nvrhi::Format::R8_SNORM:       return Format::R8Snorm;
            case nvrhi::Format::R8_UINT:        return Format::R8Uint;
            case nvrhi::Format::R8_SINT:        return Format::R8Sint;

            case nvrhi::Format::R16_FLOAT:      return Format::R16Float;
            case nvrhi::Format::R16_UNORM:      return Format::R16Unorm;
            case nvrhi::Format::R16_UINT:       return Format::R16Uint;
            case nvrhi::Format::R16_SINT:       return Format::R16Sint;
            case nvrhi::Format::RG8_UNORM:      return Format::RG8Unorm;
            case nvrhi::Format::RG8_SNORM:      return Format::RG8Snorm;

            case nvrhi::Format::R32_FLOAT:      return Format::R32Float;
            case nvrhi::Format::R32_UINT:       return Format::R32Uint;
            case nvrhi::Format::R32_SINT:       return Format::R32Sint;
            case nvrhi::Format::RG16_FLOAT:     return Format::RG16Float;
            case nvrhi::Format::RG16_UNORM:     return Format::RG16Unorm;
            case nvrhi::Format::RGBA8_UNORM:    return Format::RGBA8Unorm;
            case nvrhi::Format::SRGBA8_UNORM:   return Format::RGBA8UnormSrgb;
            case nvrhi::Format::RGBA8_SNORM:    return Format::RGBA8Snorm;
            case nvrhi::Format::BGRA8_UNORM:    return Format::BGRA8Unorm;
            case nvrhi::Format::SBGRA8_UNORM:   return Format::BGRA8UnormSrgb;

            case nvrhi::Format::RG32_FLOAT:     return Format::RG32Float;
            case nvrhi::Format::RGBA16_FLOAT:   return Format::RGBA16Float;
            case nvrhi::Format::RGBA16_UNORM:   return Format::RGBA16Unorm;

            case nvrhi::Format::RGBA32_FLOAT:   return Format::RGBA32Float;

            case nvrhi::Format::D16:            return Format::D16Unorm;
            case nvrhi::Format::D24S8:          return Format::D24UnormS8Uint;
            case nvrhi::Format::D32:            return Format::D32Float;
            case nvrhi::Format::D32S8:          return Format::D32FloatS8Uint;

            default:                            return Format::Unknown;
        }
    }
}
