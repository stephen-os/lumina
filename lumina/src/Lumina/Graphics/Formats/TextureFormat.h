#pragma once

#include <cstdint>

namespace Lumina
{
    enum class TextureFormat : uint8_t
    {
        None = 0,
        R8, RG8, RGB8, RGBA8,
        R16F, RG16F, RGB16F, RGBA16F,
        R32F, RG32F, RGB32F, RGBA32F
    };

    struct TextureFormatInfo
    {
        uint32_t InternalFormat;
        uint32_t DataFormat;
        uint32_t DataType;
        uint8_t ComponentCount;
        uint8_t BytesPerPixel;
        const char* Name;
        bool IsFloat;
    };

    class TextureFormats
    {
    public:
        static const TextureFormatInfo& GetInfo(TextureFormat format);

        static uint32_t GetInternalFormat(TextureFormat format) { return GetInfo(format).InternalFormat; }
        static uint32_t GetDataFormat(TextureFormat format) { return GetInfo(format).DataFormat; }
        static uint32_t GetDataType(TextureFormat format) { return GetInfo(format).DataType; }
        static uint8_t GetComponentCount(TextureFormat format) { return GetInfo(format).ComponentCount; }
        static uint8_t GetBytesPerPixel(TextureFormat format) { return GetInfo(format).BytesPerPixel; }
        static const char* GetName(TextureFormat format) { return GetInfo(format).Name; }
        static bool IsFloat(TextureFormat format) { return GetInfo(format).IsFloat; }

        static TextureFormat FromComponentCount(int components);
        static bool IsValidFormat(TextureFormat format);
        static size_t CalculateImageSize(TextureFormat format, uint32_t width, uint32_t height);

    private:
        static const TextureFormatInfo s_FormatTable[];
    };
}