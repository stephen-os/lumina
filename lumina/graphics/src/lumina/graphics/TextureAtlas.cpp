#include "TextureAtlas.h"

#include <lumina/core/Log.h>

namespace Lumina
{
    Ref<TextureAtlas> TextureAtlas::Create(Ref<Texture> tex)
    {
        if (!tex)
        {
            LUMINA_LOG_ERROR("Cannot create TextureAtlas from null texture");
            return nullptr;
        }

        uint32_t width = tex->GetWidth();
        uint32_t height = tex->GetHeight();

        return Ref<TextureAtlas>(new TextureAtlas(tex, width, height));
    }

    void TextureAtlas::AddRegion(const std::string& name, const glm::vec2& uvMin, const glm::vec2& uvMax)
    {
        // Check if region already exists
        auto it = m_RegionLookup.find(name);
        if (it != m_RegionLookup.end())
        {
            LUMINA_LOG_WARN("Region '{}' already exists in atlas, overwriting", name);
            // Update existing region
            m_Regions[it->second].uvMin = uvMin;
            m_Regions[it->second].uvMax = uvMax;
            m_Regions[it->second].size = glm::vec2(
                (uvMax.x - uvMin.x) * static_cast<float>(m_Width),
                (uvMax.y - uvMin.y) * static_cast<float>(m_Height)
            );
            return;
        }

        AtlasRegion region;
        region.name = name;
        region.uvMin = uvMin;
        region.uvMax = uvMax;
        region.size = glm::vec2(
            (uvMax.x - uvMin.x) * static_cast<float>(m_Width),
            (uvMax.y - uvMin.y) * static_cast<float>(m_Height)
        );
        region.offset = {0.0f, 0.0f};

        uint32_t index = static_cast<uint32_t>(m_Regions.size());
        m_Regions.push_back(region);
        m_RegionLookup[name] = index;
    }

    void TextureAtlas::AddRegionPixels(const std::string& name, float x, float y, float width, float height)
    {
        float invW = 1.0f / static_cast<float>(m_Width);
        float invH = 1.0f / static_cast<float>(m_Height);

        glm::vec2 uvMin = glm::vec2(x * invW, y * invH);
        glm::vec2 uvMax = glm::vec2((x + width) * invW, (y + height) * invH);

        // Check if region already exists
        auto it = m_RegionLookup.find(name);
        if (it != m_RegionLookup.end())
        {
            LUMINA_LOG_WARN("Region '{}' already exists in atlas, overwriting", name);
            m_Regions[it->second].uvMin = uvMin;
            m_Regions[it->second].uvMax = uvMax;
            m_Regions[it->second].size = glm::vec2(width, height);
            return;
        }

        AtlasRegion region;
        region.name = name;
        region.uvMin = uvMin;
        region.uvMax = uvMax;
        region.size = glm::vec2(width, height);
        region.offset = {0.0f, 0.0f};

        uint32_t index = static_cast<uint32_t>(m_Regions.size());
        m_Regions.push_back(region);
        m_RegionLookup[name] = index;
    }

    void TextureAtlas::AddGrid(const std::string& prefix, uint32_t cols, uint32_t rows,
                                 const glm::vec2& cellSize, const glm::vec2& offset,
                                 const glm::vec2& padding)
    {
        float invW = 1.0f / static_cast<float>(m_Width);
        float invH = 1.0f / static_cast<float>(m_Height);

        uint32_t index = 0;
        for (uint32_t row = 0; row < rows; row++)
        {
            for (uint32_t col = 0; col < cols; col++)
            {
                float x = offset.x + col * (cellSize.x + padding.x);
                float y = offset.y + row * (cellSize.y + padding.y);

                std::string name = prefix + std::to_string(index);

                AtlasRegion region;
                region.name = name;
                region.uvMin = glm::vec2(x * invW, y * invH);
                region.uvMax = glm::vec2((x + cellSize.x) * invW, (y + cellSize.y) * invH);
                region.size = cellSize;
                region.offset = {0.0f, 0.0f};

                uint32_t regionIndex = static_cast<uint32_t>(m_Regions.size());
                m_Regions.push_back(region);
                m_RegionLookup[name] = regionIndex;

                index++;
            }
        }
    }

    void TextureAtlas::AddGrid(const std::vector<std::string>& names, uint32_t cols,
                                 const glm::vec2& cellSize, const glm::vec2& offset,
                                 const glm::vec2& padding)
    {
        float invW = 1.0f / static_cast<float>(m_Width);
        float invH = 1.0f / static_cast<float>(m_Height);

        uint32_t rows = static_cast<uint32_t>((names.size() + cols - 1) / cols);

        uint32_t index = 0;
        for (uint32_t row = 0; row < rows && index < names.size(); row++)
        {
            for (uint32_t col = 0; col < cols && index < names.size(); col++)
            {
                float x = offset.x + col * (cellSize.x + padding.x);
                float y = offset.y + row * (cellSize.y + padding.y);

                const std::string& name = names[index];

                AtlasRegion region;
                region.name = name;
                region.uvMin = glm::vec2(x * invW, y * invH);
                region.uvMax = glm::vec2((x + cellSize.x) * invW, (y + cellSize.y) * invH);
                region.size = cellSize;
                region.offset = {0.0f, 0.0f};

                uint32_t regionIndex = static_cast<uint32_t>(m_Regions.size());
                m_Regions.push_back(region);
                m_RegionLookup[name] = regionIndex;

                index++;
            }
        }
    }

    const AtlasRegion* TextureAtlas::GetRegion(const std::string& name) const
    {
        auto it = m_RegionLookup.find(name);
        if (it == m_RegionLookup.end())
            return nullptr;

        return &m_Regions[it->second];
    }

    const AtlasRegion* TextureAtlas::GetRegion(uint32_t index) const
    {
        if (index >= m_Regions.size())
            return nullptr;

        return &m_Regions[index];
    }

    bool TextureAtlas::HasRegion(const std::string& name) const
    {
        return m_RegionLookup.find(name) != m_RegionLookup.end();
    }

    std::vector<std::string> TextureAtlas::GetRegionNames() const
    {
        std::vector<std::string> names;
        names.reserve(m_Regions.size());
        for (const auto& region : m_Regions)
        {
            names.push_back(region.name);
        }
        return names;
    }
}
