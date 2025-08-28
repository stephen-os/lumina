#include "RenderTarget.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

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

    bool RenderTarget::SaveToFile(const std::string& path)
    {
        std::vector<uint8_t> pixels(m_Width * m_Height * 4);

        m_FrameBuffer->Bind();
        m_FrameBuffer->ReadPixels(0, 0, m_Width, m_Height, pixels.data());
        m_FrameBuffer->Unbind();

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