#pragma once

#include <string>
#include <vector>

#include "Lumina/Core/Base.h"

#include "Formats/TextureFormat.h"

namespace Lumina
{
    class Texture
    {
    public:
        static Ref<Texture> Create(const std::string& source);
        static Ref<Texture> Create(uint32_t width, uint32_t height, TextureFormat format = TextureFormat::RGBA8);

        static Ref<Texture> CreateFromData(const void* data, uint32_t width, uint32_t height, int components);
        static Ref<Texture> CreateFromData(const void* data, uint32_t width, uint32_t height, TextureFormat format);

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

    private:
        std::string m_Path = "";
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        uint32_t m_BufferID = 0;
        TextureFormat m_Format = TextureFormat::RGBA8;
        bool m_IsCubemap = false;
    };
}