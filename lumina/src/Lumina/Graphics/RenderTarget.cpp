#include "RenderTarget.h"
#include "RendererDebug.h"

#include "Lumina/Core/Assert.h"
#include "Lumina/Core/Log.h"

#include <glad/glad.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <algorithm>

namespace Lumina
{
    Ref<RenderTarget> RenderTarget::Create(uint32_t width, uint32_t height, uint32_t numColorAttachments)
    {
        return CreateRef<RenderTarget>(width, height, numColorAttachments);
    }

    RenderTarget::RenderTarget(uint32_t width, uint32_t height, uint32_t numColorAttachments)
        : m_Width(std::max(width, 1u)), m_Height(std::max(height, 1u)), m_NumColorAttachments(numColorAttachments)
    {
        CreateFramebuffer();
    }

    RenderTarget::~RenderTarget()
    {
        DeleteFramebuffer();
    }

    void RenderTarget::CreateFramebuffer()
    {
        // Generate framebuffer
        GLCALL(glGenFramebuffers(1, &m_FramebufferID));
        LUMINA_ASSERT(m_FramebufferID != 0, "Failed to generate framebuffer");

        Bind();

        // Create color attachments
        m_ColorAttachments.resize(m_NumColorAttachments);
        std::vector<GLenum> drawBuffers(m_NumColorAttachments);

        for (uint32_t i = 0; i < m_NumColorAttachments; i++)
        {
            // Generate texture for this color attachment
            GLCALL(glGenTextures(1, &m_ColorAttachments[i]));
            LUMINA_ASSERT(m_ColorAttachments[i] != 0, "Failed to generate texture for color attachment");

            GLCALL(glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[i]));
            GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, nullptr));
            GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
            GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

            // Attach to framebuffer
            GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_ColorAttachments[i], 0));

            drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
        }

        // Tell OpenGL which color attachments we'll draw to
        GLCALL(glDrawBuffers(m_NumColorAttachments, drawBuffers.data()));

        // Create depth attachment (renderbuffer)
        GLCALL(glGenRenderbuffers(1, &m_DepthAttachment));
        LUMINA_ASSERT(m_DepthAttachment != 0, "Failed to generate renderbuffer for depth attachment");

        GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachment));
        GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height));
        GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthAttachment));

        // Check framebuffer completeness
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        LUMINA_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "RenderTarget framebuffer is incomplete!");

        Unbind();
    }

    void RenderTarget::DeleteFramebuffer()
    {
        if (m_FramebufferID != 0)
        {
            GLCALL(glDeleteFramebuffers(1, &m_FramebufferID));
            m_FramebufferID = 0;
        }

        for (uint32_t colorAttachment : m_ColorAttachments)
        {
            if (colorAttachment != 0)
                GLCALL(glDeleteTextures(1, &colorAttachment));
        }
        m_ColorAttachments.clear();

        if (m_DepthAttachment != 0)
        {
            GLCALL(glDeleteRenderbuffers(1, &m_DepthAttachment));
            m_DepthAttachment = 0;
        }
    }

    void RenderTarget::Bind() const
    {
        LUMINA_ASSERT(m_FramebufferID != 0, "Cannot bind uninitialized framebuffer");
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID));
    }

    void RenderTarget::Unbind() const
    {
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    void RenderTarget::Resize(uint32_t width, uint32_t height)
    {
        if (m_Width == width && m_Height == height)
            return;

        m_Width = std::max(width, 1u);
        m_Height = std::max(height, 1u);

        for (uint32_t i = 0; i < m_ColorAttachments.size(); i++)
        {
            GLCALL(glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[i]));
            GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, nullptr));
            GLCALL(glGenerateMipmap(GL_TEXTURE_2D));
        }

        GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachment));
        GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height));

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        LUMINA_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete after resize");
    }

    void RenderTarget::Resize(float width, float height)
    {
        Resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    }

    void RenderTarget::Resize(const glm::vec2& size)
    {
        Resize(static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y));
    }

    uint32_t RenderTarget::GetTexture(uint32_t attachmentIndex) const
    {
        LUMINA_ASSERT(attachmentIndex < m_ColorAttachments.size(), "Color attachment index out of range");
        return m_ColorAttachments[attachmentIndex];
    }

    void RenderTarget::BindColorTexture(uint32_t attachmentIndex, uint32_t textureUnit) const
    {
        LUMINA_ASSERT(attachmentIndex < m_ColorAttachments.size(), "Color attachment index out of range");
        GLCALL(glActiveTexture(GL_TEXTURE0 + textureUnit));
        GLCALL(glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[attachmentIndex]));
    }

    void RenderTarget::BlitTo(Ref<RenderTarget> target) const
    {
        LUMINA_ASSERT(target != nullptr, "Cannot blit to null target");

        GLCALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FramebufferID));
        GLCALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target->m_FramebufferID));

        GLCALL(glBlitFramebuffer(
            0, 0, m_Width, m_Height,
            0, 0, target->m_Width, target->m_Height,
            GL_COLOR_BUFFER_BIT, GL_LINEAR
        ));

        GLCALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
        GLCALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
    }

    void RenderTarget::ReadPixels(int x, int y, uint32_t width, uint32_t height, void* data) const
    {
        LUMINA_ASSERT(data != nullptr, "Cannot read pixels into null data pointer");
        LUMINA_ASSERT(width > 0 && height > 0, "ReadPixels dimensions must be positive");

        Bind();
        GLCALL(glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data));
        Unbind();
    }

    bool RenderTarget::SaveToFile(const std::string& path)
    {
        std::vector<uint8_t> pixels(m_Width * m_Height * 4);

        Bind();
        ReadPixels(0, 0, m_Width, m_Height, pixels.data());
        Unbind();

        std::string extension = path.substr(path.find_last_of("."));
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

        int result = 0;
        if (extension == ".png")
        {
            result = stbi_write_png(path.c_str(), m_Width, m_Height, 4, pixels.data(), m_Width * 4);
        }
        else if (extension == ".jpg" || extension == ".jpeg")
        {
            result = stbi_write_jpg(path.c_str(), m_Width, m_Height, 4, pixels.data(), 90);
        }
        else if (extension == ".bmp")
        {
            result = stbi_write_bmp(path.c_str(), m_Width, m_Height, 4, pixels.data());
        }
        else if (extension == ".tga")
        {
            result = stbi_write_tga(path.c_str(), m_Width, m_Height, 4, pixels.data());
        }
        else
        {
            LUMINA_LOG_ERROR("Unsupported file format: {}", extension);
            return false;
        }

        if (result == 0)
        {
            LUMINA_LOG_ERROR("Failed to save render target to file: {}", path);
            return false;
        }

        LUMINA_LOG_INFO("Successfully saved render target to: {}", path);
        return true;
    }
}