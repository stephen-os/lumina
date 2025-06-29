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
        TextureFormat format = TextureFormats::FromComponentCount(components);
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

                // Determine format from first face
                texture->m_Format = TextureFormats::FromComponentCount(channels);
                auto formatInfo = TextureFormats::GetInfo(texture->m_Format);

                // Allocate storage for the entire cubemap
                GLCALL(glTextureStorage2D(texture->m_BufferID, 1, formatInfo.internalFormat, width, height));
            }
            else if (faceWidth != width || faceHeight != height)
            {
                LUMINA_LOG_ERROR("All cubemap faces must have the same dimensions");
                stbi_image_free(data);
                return nullptr;
            }

            // Get format info for uploading
            TextureFormat faceFormat = TextureFormats::FromComponentCount(channels);
            auto formatInfo = TextureFormats::GetInfo(faceFormat);

            // Upload face data
            GLCALL(glTextureSubImage3D(texture->m_BufferID, 0, 0, 0, i, width, height, 1,
                formatInfo.dataFormat, formatInfo.dataType, data));

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

        uint32_t expectedSize = m_Width * m_Height * TextureFormats::GetBytesPerPixel(m_Format);
        LUMINA_ASSERT(size == expectedSize, "Texture::SetData - Data size mismatch. Expected: {0}, got: {1}", expectedSize, size);

        auto formatInfo = TextureFormats::GetInfo(m_Format);
        GLCALL(glTextureSubImage2D(m_BufferID, 0, 0, 0, m_Width, m_Height,
            formatInfo.dataFormat, formatInfo.dataType, data));
    }

    void Texture::SetData(const void* data, uint32_t width, uint32_t height, int components)
    {
        TextureFormat format = TextureFormats::FromComponentCount(components);
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

        auto formatInfo = TextureFormats::GetInfo(format);

        // Reallocate if needed
        GLCALL(glDeleteTextures(1, &m_BufferID));
        GLCALL(glCreateTextures(GL_TEXTURE_2D, 1, &m_BufferID));
        GLCALL(glTextureStorage2D(m_BufferID, 1, formatInfo.internalFormat, width, height));
        GLCALL(glTextureSubImage2D(m_BufferID, 0, 0, 0, width, height,
            formatInfo.dataFormat, formatInfo.dataType, data));

        // Set texture parameters
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_WRAP_T, GL_REPEAT));
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        GLCALL(glGenerateTextureMipmap(m_BufferID));
    }

    int Texture::GetComponentCount() const
    {
        return TextureFormats::GetComponentCount(m_Format);
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
        m_Format = TextureFormats::FromComponentCount(channels);

        auto formatInfo = TextureFormats::GetInfo(m_Format);

        // Set texture parameters using DSA
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_WRAP_T, GL_REPEAT));
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        // Allocate and upload texture data using DSA
        GLCALL(glTextureStorage2D(m_BufferID, 1, formatInfo.internalFormat, m_Width, m_Height));
        GLCALL(glTextureSubImage2D(m_BufferID, 0, 0, 0, m_Width, m_Height,
            formatInfo.dataFormat, GL_UNSIGNED_BYTE, data));

        // Generate mipmaps using DSA
        GLCALL(glGenerateTextureMipmap(m_BufferID));

        stbi_image_free(data);
    }

    void Texture::CreateTexture(uint32_t width, uint32_t height, TextureFormat format, const void* data)
    {
        LUMINA_ASSERT(width > 0 && height > 0, "Texture dimensions must be greater than zero");

        GLCALL(glCreateTextures(GL_TEXTURE_2D, 1, &m_BufferID));

        auto formatInfo = TextureFormats::GetInfo(format);

        // Allocate texture storage using DSA
        GLCALL(glTextureStorage2D(m_BufferID, 1, formatInfo.internalFormat, width, height));

        // Upload data if provided
        if (data)
        {
            GLCALL(glTextureSubImage2D(m_BufferID, 0, 0, 0, width, height,
                formatInfo.dataFormat, formatInfo.dataType, data));
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

        auto formatInfo = TextureFormats::GetInfo(m_Format);

        // Allocate storage for cubemap
        GLCALL(glTextureStorage2D(m_BufferID, 1, formatInfo.internalFormat, width, height));

        // If data is provided, it should contain 6 faces worth of data
        if (data)
        {
            const uint8_t* faceData = static_cast<const uint8_t*>(data);
            uint32_t faceSize = width * height * TextureFormats::GetBytesPerPixel(m_Format);

            for (uint32_t i = 0; i < 6; ++i)
            {
                const void* currentFaceData = faceData + i * faceSize;
                GLCALL(glTextureSubImage3D(m_BufferID, 0, 0, 0, i, width, height, 1,
                    formatInfo.dataFormat, formatInfo.dataType, currentFaceData));
            }
        }

        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCALL(glTextureParameteri(m_BufferID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
    }
}