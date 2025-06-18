#include "TextureAtlas.h"

#include <spdlog/spdlog.h>

namespace Lumina
{
#if 0
	std::shared_ptr<TextureAtlas> TextureAtlas::Create(std::string& source, int width, int height) 
    { 
        return std::make_shared<TextureAtlas>(TextureAtlas(source, width, height));
    }

    std::shared_ptr<TextureAtlas> TextureAtlas::Create(int width, int height) 
    { 
        return std::make_shared<TextureAtlas>(TextureAtlas(width, height));
    }

#endif 
    TextureAtlas::TextureAtlas(int width, int height)
    {
        Resize(width, height); 
    }

	TextureAtlas::TextureAtlas(std::string& source, int width, int height) 
	{
        SetTexture(source);
		Resize(width, height); 
	}

	void TextureAtlas::Resize(int width, int height)
	{
		m_GridWidth = width;
		m_GridHeight = height;

        m_TexWidth = 1.0f / static_cast<float>(m_GridWidth);
        m_TexHeight = 1.0f / static_cast<float>(m_GridHeight);

        m_TexCoords.clear();
        m_TexCoords.reserve(m_GridWidth * m_GridHeight);

        for (int y = 0; y < m_GridHeight; y++)
        {
            for (int x = 0; x < m_GridWidth; x++)
            {
                float uMin = x * m_TexWidth;
                float vMin = y * m_TexHeight;
                float uMax = uMin + m_TexWidth;
                float vMax = vMin + m_TexHeight;

                m_TexCoords.emplace_back(uMin, vMin, uMax, vMax);
            }
        }
	}

    void TextureAtlas::SetTexture(std::string& source)
    {
#if 0
        m_Texture = Texture::Create(source);
#endif
        m_HasTexture = true; 
    }

    void TextureAtlas::RemoveTexture()
    {
        m_Texture.reset();
        m_HasTexture = false; 
    }

    glm::vec4 TextureAtlas::GetTextureCoords(int index) const
    {
        if (index < 0 || index >= static_cast<int>(m_TexCoords.size()))
        {
            spdlog::error("[Texture Atlas] Invalid texture index: {}", index);
            return glm::vec4(0.0f);
        }

        return m_TexCoords[index];
    }

    glm::vec2 TextureAtlas::GetOffset(int index) const
    {
        int row = index / m_GridWidth;
        int col = index % m_GridWidth;

        float xOffset = col * m_TexWidth;
        float yOffset = row * m_TexHeight;

        return glm::vec2(xOffset, yOffset);
    }

    glm::vec2 TextureAtlas::GetPosition(int index) const
    {
        if (index < 0 || index >= m_GridWidth * m_GridHeight)
            return glm::vec2(0.0f);

        int x = index % m_GridWidth;
        int y = index / m_GridWidth;

        return glm::vec2(x, y);
    }
}