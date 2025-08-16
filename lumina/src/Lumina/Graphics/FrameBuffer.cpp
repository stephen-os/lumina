#include "FrameBuffer.h"
#include "RendererDebug.h"

#include <glad/glad.h>
#include <iostream>

#include "../Core/Assert.h"
#include "../Core/Log.h"

namespace Lumina
{
    Ref<FrameBuffer> FrameBuffer::Create()
	{
		return CreateRef<FrameBuffer>();
	}

    FrameBuffer::FrameBuffer()
    {
        GLCALL(glGenFramebuffers(1, &m_BufferID));
        LUMINA_ASSERT(m_BufferID != 0, "Failed to generate framebuffer");

        Bind();

        // Create color attachment (Texture)
        GLCALL(glGenTextures(1, &m_ColorAttachment));
        LUMINA_ASSERT(m_ColorAttachment != 0, "Failed to generate texture for color attachment");

        GLCALL(glBindTexture(GL_TEXTURE_2D, m_ColorAttachment));
        GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCALL(glGenerateMipmap(GL_TEXTURE_2D));

        GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0));

        // Create depth attachment (optional)
        GLCALL(glGenRenderbuffers(1, &m_DepthAttachment));
        LUMINA_ASSERT(m_DepthAttachment != 0, "Failed to generate renderbuffer for depth attachment");

        GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachment));
        GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_Width, m_Height));
        GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthAttachment));

        // Check if FrameBuffer is complete
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        LUMINA_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

        Unbind(); // Unbind the FrameBuffer
    }

    FrameBuffer::~FrameBuffer()
    {
        LUMINA_ASSERT(m_BufferID != 0, "Framebuffer already deleted or uninitialized");

        GLCALL(glDeleteFramebuffers(1, &m_BufferID));

        if (m_ColorAttachment)
            GLCALL(glDeleteTextures(1, &m_ColorAttachment));

        if (m_DepthAttachment)
            GLCALL(glDeleteRenderbuffers(1, &m_DepthAttachment));
    }

    void FrameBuffer::Bind() const
    {
        LUMINA_ASSERT(m_BufferID != 0, "Cannot bind uninitialized framebuffer");
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID));
    }

    void FrameBuffer::Unbind() const
    {
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0)); // Bind default FrameBuffer (screen)
    }

    void FrameBuffer::Resize(uint32_t width, uint32_t height)
    {
        m_Width = std::max(width, 1u);
        m_Height = std::max(height, 1u);

        // Resize the color attachment (texture)
        GLCALL(glBindTexture(GL_TEXTURE_2D, m_ColorAttachment));
        GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
        GLCALL(glGenerateMipmap(GL_TEXTURE_2D));

        // Resize the depth attachment (renderbuffer)
        GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachment));
        GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height));

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        LUMINA_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete after resize");
    }

    void FrameBuffer::ReadPixels(int x, int y, uint32_t width, uint32_t height, void* data) const
    {
        LUMINA_ASSERT(data != nullptr, "Cannot read pixels into null data pointer");
        LUMINA_ASSERT(width > 0 && height > 0, "ReadPixels dimensions must be positive");

        Bind();
        GLCALL(glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data));
        Unbind();
    }
}