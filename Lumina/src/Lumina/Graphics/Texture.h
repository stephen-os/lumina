#pragma once

#include <string>
#include <vector>

#include "../Core/Ref.h"

namespace Lumina
{
    enum class TextureFormat
    {
        None = 0,
        R8,          // 1 component - grayscale
        RG8,         // 2 components - grayscale + alpha  
        RGB8,        // 3 components - RGB
        RGBA8,       // 4 components - RGBA

        // Additional formats you might want later
        R16F,        // 16-bit float red
        RG16F,       // 16-bit float RG
        RGB16F,      // 16-bit float RGB
        RGBA16F,     // 16-bit float RGBA
        R32F,        // 32-bit float red
        RG32F,       // 32-bit float RG
        RGB32F,      // 32-bit float RGB
        RGBA32F      // 32-bit float RGBA
    };

    class Texture : public Referencable
    {
    public:
        // Create from file
        static Ref<Texture> Create(const std::string& source);

        // Create empty texture
        static Ref<Texture> Create(uint32_t width, uint32_t height, TextureFormat format = TextureFormat::RGBA8);

        // Create from raw data
        static Ref<Texture> CreateFromData(const void* data, uint32_t width, uint32_t height, int components);
        static Ref<Texture> CreateFromData(const void* data, uint32_t width, uint32_t height, TextureFormat format);

        // Create cubemap (for your model loading system)
        static Ref<Texture> CreateCubemap(const std::vector<std::string>& faces);
        static Ref<Texture> CreateCubemap(uint32_t width, uint32_t height, const void* data);

        Texture(const std::string& source);
        Texture(uint32_t width, uint32_t height, TextureFormat format = TextureFormat::RGBA8);
        ~Texture();

        void Bind(uint32_t slot = 0) const;
        void Unbind() const;

        bool SetResolution(uint32_t width, uint32_t height);
        void SetData(const void* data, uint32_t size);
        void SetData(const void* data, uint32_t width, uint32_t height, int components);
        void SetData(const void* data, uint32_t width, uint32_t height, TextureFormat format);

        uint32_t GetID() const { return m_BufferID; }
        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        TextureFormat GetFormat() const { return m_Format; }
        int GetComponentCount() const;
        const std::string& GetPath() const { return m_Path; }

        bool IsCubemap() const { return m_IsCubemap; }

    private:
        void LoadFromFile(const std::string& path);
        void CreateTexture(uint32_t width, uint32_t height, TextureFormat format, const void* data = nullptr);
        void CreateCubemapTexture(uint32_t width, uint32_t height, const void* data = nullptr);

        // Helper functions
        static uint32_t TextureFormatToOpenGLInternalFormat(TextureFormat format);
        static uint32_t TextureFormatToOpenGLDataFormat(TextureFormat format);
        static uint32_t TextureFormatToOpenGLDataType(TextureFormat format);
        static TextureFormat ComponentCountToTextureFormat(int components);

    private:
        std::string m_Path;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        uint32_t m_BufferID = 0;
        TextureFormat m_Format = TextureFormat::RGBA8;
        bool m_IsCubemap = false;
    };
}