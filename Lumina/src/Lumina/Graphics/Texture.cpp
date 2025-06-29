#include "Texture.h"
#include "RendererDebug.h"

#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>

#include "../Core/Assert.h"
#include "../Core/Log.h"

namespace Lumina
{
    // Static factory methods
    Ref<Texture> Texture::Create(const std::string& source)
    {
        return Ref<Texture>::Create(source);
    }

    Ref<Texture> Texture::Create(uint32_t width, uint32_t height, TextureFormat format)
    {
        return Ref<Texture>::Create(width, height, format);
    }

    Ref<Texture> Texture::CreateFromData(const void* data, uint32_t width, uint32_t height, int components)
    {
        TextureFormat format = ComponentCountToTextureFormat(components);
        if (format == TextureFormat::None)
        {
            LUMINA_LOG_ERROR("Unsupported component count: {0}", components);
            return nullptr;
        }

        auto texture = Ref<Texture>::Create(width, height, format);
        if (data)
        {
            texture->SetData(data, width, height, format);
        }
        return texture;
    }

    Ref<Texture> Texture::CreateFromData(const void* data, uint32_t width, uint32_t height, TextureFormat format)
    {
        auto texture = Ref<Texture>::Create(width, height, format);
        if (data)
        {
            texture->SetData(data, width, height, format);
        }
        return texture;
    }

    Ref<Texture> Texture::CreateCubemap(const std::vector<std::string>& faces)
    {
        if (faces.size() != 6)
        {
            LUMINA_LOG_ERROR("Cubemap requires exactly 6 face textures, got {0}", faces.size());
            return nullptr;
        }

        auto texture = Ref<Texture>::Create(0, 0, TextureFormat::RGBA8);
        texture->m_IsCubemap = true;

        // Create cubemap texture using DSA
        GLCALL(glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &texture->m_BufferID));

        // Load each face
        int width = 0, height = 0;
        for (int i = 0; i < 6; ++i)
        {
            int channels;
            int faceWidth, faceHeight;
            unsigned char* data = stbi_load(faces[i].c_str(), &faceWidth, &faceHeight, &channels, 0);

            if (!data)
            {
                LUMINA_LOG_ERROR("Failed to load cubemap face: {0}", faces[i]);
                return nullptr;
            }

            // Set dimensions from first face
            if (i == 0)
            {
                width = faceWidth;
                height = faceHeight;
                texture->m_Width = width;
                texture->m_Height = height;

                // Allocate storage for the entire cubemap
                GLCALL(glTextureStorage2D(texture->m_BufferID, 1, GL_RGBA8, width, height));
            }
            else if (faceWidth != width || faceHeight != height)
            {
                LUMINA_LOG_ERROR("All cubemap faces must have the same dimensions");
                stbi_image_free(data);
                return nullptr;
            }

            GLenum format = (channels == 4) ? GL_RGBA : (channels == 3) ? GL_RGB : GL_RED;

            // Upload face data
            GLCALL(glTextureSubImage3D(texture->m_BufferID, 0, 0, 0, i, width, height, 1, format, GL_UNSIGNED_BYTE, data));

            stbi_image_free(data);
        }

        // Set cubemap parameters
        GLCALL(glTextureParameteri(texture->m_BufferID, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCALL(glTextureParameteri(texture->m_BufferID, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCALL(glTextureParameteri(texture->m_BufferID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCALL(glTextureParameteri(texture->m_BufferID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCALL(glTextureParameteri(texture->m_BufferID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

        return texture;
    }

    Ref<Texture> Texture::CreateCubemap(uint32_t width, uint32_t height, const void* data)
    {
        auto texture = Ref<Texture>::Create(width, height, TextureFormat::RGBA8);
        texture->CreateCubemapTexture(width, height, data);
        return texture;
    }

    // Constructors
    Texture::Texture(const std::string& source)
    {
        LoadFromFile(source);
    }

    Texture::Texture(uint32_t width, uint32_t height, TextureFormat format)
        : m_Width(width), m_Height(height), m_Format(format)
    {
        CreateTexture(width, height, format);
    }

    Texture::~Texture()
    {
        GLCALL(glDeleteTextures(1, &m_BufferID));
    }

    void Texture::Bind(uint32_t slot) const
    {
        GLCALL(glBindTextureUnit(slot, m_BufferID));
    }

    void Texture::Unbind() const
    {
        // With DSA, explicit unbinding is less necessary
    }

    bool Texture::SetResolution(uint32_t width, uint32_t height)
    {
        LUMINA_ASSERT(width > 0 && height > 0, "Invalid resolution: {0}, {1}", width, height);

        m_Width = width;
        m_Height = height;

        // Reallocate texture storage using DSA
        GLCALL(glDeleteTextures(1, &m_BufferID));
        CreateTexture(width, height, m_Format);

        return true;
    }

    void Texture::SetData(const void* data, uint32_t size)
    {
        LUMINA_ASSERT(data != nullptr, "SetData called with null data pointer");

        uint32_t expectedSize = m_Width * m_Height * GetComponentCount();
        LUMINA_ASSERT(size == expectedSize, "Texture::SetData - Data size mismatch. Expected: {0}, got: {1}", expectedSize, size);

        uint32_t dataFormat = TextureFormatToOpenGLDataFormat(m_Format);
        uint32_t dataType = TextureFormatToOpenGLDataType(m_Format);

        GLCALL(glTextureSubImage2D(m_BufferID, 0, 0, 0, m_Width, m_Height, dataFormat, dataType, data));
    }

    void Texture::SetData(const void* data, uint32_t width, uint32_t height, int components)
    {
        TextureFormat format = ComponentCountToTextureFormat(components);
        SetData(data, width, height, format);
    }

    void Texture::SetData(const void* data, uint32_t width, uint32_t height, TextureFormat format)
    {
        if (!data)
        {
            LUMINA_LOG_ERROR("Cannot set null data to texture");
            return;
        }

        m_Width = width;
        m_Height = height;
        m_Format = format;

        uint32_t internalFormat = TextureFormatToOpenGLInternalFormat(format);
        uint32_t dataFormat = TextureFormatToOpenGLDataFormat(format);
        uint32_t dataType = TextureFormatToOpenGLDataType(format);

        // Reallocate if needed
        GLCALL(glDeleteTextures(1, &m_BufferID));
        GLCALL(glCreateTextures(GL_TEXTURE_2D, 1, &m_BufferID));
        GLCALL(glTextureStorage2D(m_BufferID, 1, internalFormat, width, height));
        GLCALL(glTextureSubImage2D(m_BufferID, 0, 0, 0, width, height, dataFormat, dataType, data));

        // Set texture parameters
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_WRAP_T, GL_REPEAT));
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        GLCALL(glGenerateTextureMipmap(m_BufferID));
    }

    int Texture::GetComponentCount() const
    {
        switch (m_Format)
        {
        case TextureFormat::R8:
        case TextureFormat::R16F:
        case TextureFormat::R32F:
            return 1;
        case TextureFormat::RG8:
        case TextureFormat::RG16F:
        case TextureFormat::RG32F:
            return 2;
        case TextureFormat::RGB8:
        case TextureFormat::RGB16F:
        case TextureFormat::RGB32F:
            return 3;
        case TextureFormat::RGBA8:
        case TextureFormat::RGBA16F:
        case TextureFormat::RGBA32F:
            return 4;
        default:
            return 4; // Default to RGBA
        }
    }

    // Private helper methods
    void Texture::LoadFromFile(const std::string& path)
    {
        // Create texture using DSA
        GLCALL(glCreateTextures(GL_TEXTURE_2D, 1, &m_BufferID));

        int channels;
        int width, height;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

        LUMINA_ASSERT(data, "Failed to load texture: {0}", path);

        m_Width = width;
        m_Height = height;
        m_Path = path;
        m_Format = ComponentCountToTextureFormat(channels);

        uint32_t internalFormat = TextureFormatToOpenGLInternalFormat(m_Format);
        uint32_t dataFormat = TextureFormatToOpenGLDataFormat(m_Format);

        // Set texture parameters using DSA
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_WRAP_T, GL_REPEAT));
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        // Allocate and upload texture data using DSA
        GLCALL(glTextureStorage2D(m_BufferID, 1, internalFormat, m_Width, m_Height));
        GLCALL(glTextureSubImage2D(m_BufferID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data));

        // Generate mipmaps using DSA
        GLCALL(glGenerateTextureMipmap(m_BufferID));

        stbi_image_free(data);
    }

    void Texture::CreateTexture(uint32_t width, uint32_t height, TextureFormat format, const void* data)
    {
        LUMINA_ASSERT(width > 0 && height > 0, "Texture dimensions must be greater than zero");

        GLCALL(glCreateTextures(GL_TEXTURE_2D, 1, &m_BufferID));

        uint32_t internalFormat = TextureFormatToOpenGLInternalFormat(format);
        uint32_t dataFormat = TextureFormatToOpenGLDataFormat(format);
        uint32_t dataType = TextureFormatToOpenGLDataType(format);

        // Allocate texture storage using DSA
        GLCALL(glTextureStorage2D(m_BufferID, 1, internalFormat, width, height));

        // Upload data if provided
        if (data)
        {
            GLCALL(glTextureSubImage2D(m_BufferID, 0, 0, 0, width, height, dataFormat, dataType, data));
        }

        // Set texture parameters using DSA
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_WRAP_T, GL_REPEAT));
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    }

    void Texture::CreateCubemapTexture(uint32_t width, uint32_t height, const void* data)
    {
        m_IsCubemap = true;

        GLCALL(glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_BufferID));

        uint32_t internalFormat = TextureFormatToOpenGLInternalFormat(m_Format);
        uint32_t dataFormat = TextureFormatToOpenGLDataFormat(m_Format);
        uint32_t dataType = TextureFormatToOpenGLDataType(m_Format);

        // Allocate storage for cubemap
        GLCALL(glTextureStorage2D(m_BufferID, 1, internalFormat, width, height));

        // If data is provided, it should contain 6 faces worth of data
        if (data)
        {
            const uint8_t* faceData = static_cast<const uint8_t*>(data);
            uint32_t faceSize = width * height * GetComponentCount();

            for (uint32_t i = 0; i < 6; ++i)
            {
                const void* currentFaceData = faceData + i * faceSize;
                GLCALL(glTextureSubImage3D(m_BufferID, 0, 0, 0, i, width, height, 1, dataFormat, dataType, currentFaceData));
            }
        }

        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
    }

    // Static helper functions
    uint32_t Texture::TextureFormatToOpenGLInternalFormat(TextureFormat format)
    {
        switch (format)
        {
        case TextureFormat::R8:     return GL_R8;
        case TextureFormat::RG8:    return GL_RG8;
        case TextureFormat::RGB8:   return GL_RGB8;
        case TextureFormat::RGBA8:  return GL_RGBA8;
        case TextureFormat::R16F:   return GL_R16F;
        case TextureFormat::RG16F:  return GL_RG16F;
        case TextureFormat::RGB16F: return GL_RGB16F;
        case TextureFormat::RGBA16F: return GL_RGBA16F;
        case TextureFormat::R32F:   return GL_R32F;
        case TextureFormat::RG32F:  return GL_RG32F;
        case TextureFormat::RGB32F: return GL_RGB32F;
        case TextureFormat::RGBA32F: return GL_RGBA32F;
        default: return GL_RGBA8;
        }
    }

    uint32_t Texture::TextureFormatToOpenGLDataFormat(TextureFormat format)
    {
        switch (format)
        {
        case TextureFormat::R8:
        case TextureFormat::R16F:
        case TextureFormat::R32F:
            return GL_RED;
        case TextureFormat::RG8:
        case TextureFormat::RG16F:
        case TextureFormat::RG32F:
            return GL_RG;
        case TextureFormat::RGB8:
        case TextureFormat::RGB16F:
        case TextureFormat::RGB32F:
            return GL_RGB;
        case TextureFormat::RGBA8:
        case TextureFormat::RGBA16F:
        case TextureFormat::RGBA32F:
            return GL_RGBA;
        default:
            return GL_RGBA;
        }
    }

    uint32_t Texture::TextureFormatToOpenGLDataType(TextureFormat format)
    {
        switch (format)
        {
        case TextureFormat::R8:
        case TextureFormat::RG8:
        case TextureFormat::RGB8:
        case TextureFormat::RGBA8:
            return GL_UNSIGNED_BYTE;
        case TextureFormat::R16F:
        case TextureFormat::RG16F:
        case TextureFormat::RGB16F:
        case TextureFormat::RGBA16F:
        case TextureFormat::R32F:
        case TextureFormat::RG32F:
        case TextureFormat::RGB32F:
        case TextureFormat::RGBA32F:
            return GL_FLOAT;
        default:
            return GL_UNSIGNED_BYTE;
        }
    }

    TextureFormat Texture::ComponentCountToTextureFormat(int components)
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
}