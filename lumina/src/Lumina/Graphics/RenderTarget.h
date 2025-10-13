#pragma once

#include "Lumina/Core/Base.h"

#include <glm/glm.hpp>

#include <vector>
#include <cstdint>

namespace Lumina
{
    class RenderTarget
    {
    public:
        static Ref<RenderTarget> Create(uint32_t width, uint32_t height, uint32_t numColorAttachments = 1);

        RenderTarget(uint32_t width, uint32_t height, uint32_t numColorAttachments = 1);
        ~RenderTarget();

        void Bind() const;
        void Unbind() const;
        void Resize(uint32_t width, uint32_t height);
        void Resize(float width, float height);
        void Resize(const glm::vec2& size);

        bool SaveToFile(const std::string& path);
        void ReadPixels(int x, int y, uint32_t width, uint32_t height, void* data) const;

        // Texture access
        uint32_t GetTexture(uint32_t attachmentIndex = 0) const;
        void BindColorTexture(uint32_t attachmentIndex, uint32_t textureUnit) const;

        // Framebuffer operations
        void BlitTo(Ref<RenderTarget> target) const;

        // Getters
        uint32_t GetID() const { return m_FramebufferID; }
        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        uint32_t GetNumColorAttachments() const { return m_NumColorAttachments; }

        // Backward compatibility
        uint32_t GetColorAttachment() const { return GetTexture(0); }

    private:
        void CreateFramebuffer();
        void DeleteFramebuffer();

        uint32_t m_FramebufferID = 0;
        std::vector<uint32_t> m_ColorAttachments;
        uint32_t m_DepthAttachment = 0;

        uint32_t m_Width = 900;
        uint32_t m_Height = 900;
        uint32_t m_NumColorAttachments = 1;
    };
}