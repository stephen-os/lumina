#include "TextureFormat.h"

#include <glad/glad.h>

namespace Lumina
{
    const TextureFormatInfo TextureFormats::s_FormatTable[] = 
    {
        // Format       DataFormat  DataType            Components  Bytes   Name        IsFloat
        { 0,            0,          0,                  0,          0,      "None",     false },  // None
        { GL_R8,        GL_RED,     GL_UNSIGNED_BYTE,   1,          1,      "R8",       false },  // R8
        { GL_RG8,       GL_RG,      GL_UNSIGNED_BYTE,   2,          2,      "RG8",      false },  // RG8
        { GL_RGB8,      GL_RGB,     GL_UNSIGNED_BYTE,   3,          3,      "RGB8",     false },  // RGB8
        { GL_RGBA8,     GL_RGBA,    GL_UNSIGNED_BYTE,   4,          4,      "RGBA8",    false },  // RGBA8
        { GL_R16F,      GL_RED,     GL_FLOAT,           1,          2,      "R16F",     true  },  // R16F
        { GL_RG16F,     GL_RG,      GL_FLOAT,           2,          4,      "RG16F",    true  },  // RG16F
        { GL_RGB16F,    GL_RGB,     GL_FLOAT,           3,          6,      "RGB16F",   true  },  // RGB16F
        { GL_RGBA16F,   GL_RGBA,    GL_FLOAT,           4,          8,      "RGBA16F",  true  },  // RGBA16F
        { GL_R32F,      GL_RED,     GL_FLOAT,           1,          4,      "R32F",     true  },  // R32F
        { GL_RG32F,     GL_RG,      GL_FLOAT,           2,          8,      "RG32F",    true  },  // RG32F
        { GL_RGB32F,    GL_RGB,     GL_FLOAT,           3,          12,     "RGB32F",   true  },  // RGB32F
        { GL_RGBA32F,   GL_RGBA,    GL_FLOAT,           4,          16,     "RGBA32F",  true  },  // RGBA32F
    };

    const TextureFormatInfo& TextureFormats::GetInfo(TextureFormat format)
    {
        uint8_t index = static_cast<uint8_t>(format);

        if (index >= sizeof(s_FormatTable) / sizeof(s_FormatTable[0]))
            return s_FormatTable[0];
        
        return s_FormatTable[index];
    }

    TextureFormat TextureFormats::FromComponentCount(int components)
    {
        switch (components)
        {
        case 1: return TextureFormat::R8;
        case 2: return TextureFormat::RG8;
        case 3: return TextureFormat::RGB8;
        case 4: return TextureFormat::RGBA8;
        default: return TextureFormat::None;
        }
    }

    bool TextureFormats::IsValidFormat(TextureFormat format)
    {
        return format != TextureFormat::None && static_cast<uint8_t>(format) < (sizeof(s_FormatTable) / sizeof(s_FormatTable[0]));
    }

    size_t TextureFormats::CalculateImageSize(TextureFormat format, uint32_t width, uint32_t height)
    {
        return static_cast<size_t>(width) * height * GetBytesPerPixel(format);
    }
}