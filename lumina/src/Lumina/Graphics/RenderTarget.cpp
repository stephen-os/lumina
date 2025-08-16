#include "RenderTarget.h"

namespace Lumina
{
    Ref<RenderTarget> RenderTarget::Create(uint32_t width, uint32_t height)
    {
        return CreateRef<RenderTarget>(width, height);
    }

    RenderTarget::RenderTarget(uint32_t width, uint32_t height)
        : m_Width(width), m_Height(height)
    {
        m_FrameBuffer = FrameBuffer::Create();
        m_FrameBuffer->Resize(width, height);
    }

    void RenderTarget::Bind()
    {
        m_FrameBuffer->Bind();
    }

    void RenderTarget::Unbind()
    {
        m_FrameBuffer->Unbind();
    }

    void RenderTarget::Resize(uint32_t width, uint32_t height)
    {
        if (m_Width != width || m_Height != height)
        {
            m_Width = width;
            m_Height = height;
            m_FrameBuffer->Resize(width, height);
        }
    }

	void RenderTarget::Resize(float width, float height)
    {
        Resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
	}

    void RenderTarget::Resize(const glm::vec2& size)
    {
        Resize(static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y));
	}
}