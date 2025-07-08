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
        uint32_t internalFormat;
        uint32_t dataFormat;
        uint32_t dataType;
        uint8_t componentCount;
        uint8_t bytesPerPixel;
        const char* name;
        bool isFloat;
    };

    class TextureFormats
    {
    public:
        static const TextureFormatInfo& GetInfo(TextureFormat format);

        // Convenience accessors
        static uint32_t GetInternalFormat(TextureFormat format) { return GetInfo(format).internalFormat; }
        static uint32_t GetDataFormat(TextureFormat format) { return GetInfo(format).dataFormat; }
        static uint32_t GetDataType(TextureFormat format) { return GetInfo(format).dataType; }
        static uint8_t GetComponentCount(TextureFormat format) { return GetInfo(format).componentCount; }
        static uint8_t GetBytesPerPixel(TextureFormat format) { return GetInfo(format).bytesPerPixel; }
        static const char* GetName(TextureFormat format) { return GetInfo(format).name; }
        static bool IsFloat(TextureFormat format) { return GetInfo(format).isFloat; }

        // Utility functions
        static TextureFormat FromComponentCount(int components);
        static bool IsValidFormat(TextureFormat format);
        static size_t CalculateImageSize(TextureFormat format, uint32_t width, uint32_t height);

    private:
        static const TextureFormatInfo s_FormatTable[];
    };
}