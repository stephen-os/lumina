#include "texture_atlas.h"

#include <lumina/core/log.h>

namespace lumina::graphics
{
    ref<texture_atlas> texture_atlas::create(ref<texture> tex)
    {
        if (!tex)
        {
            LUMINA_LOG_ERROR("Cannot create texture_atlas from null texture");
            return nullptr;
        }

        uint32_t width = tex->get_width();
        uint32_t height = tex->get_height();

        return ref<texture_atlas>(new texture_atlas(tex, width, height));
    }

    void texture_atlas::add_region(const std::string& name, const glm::vec2& uv_min, const glm::vec2& uv_max)
    {
        // Check if region already exists
        auto it = m_region_lookup.find(name);
        if (it != m_region_lookup.end())
        {
            LUMINA_LOG_WARN("Region '{}' already exists in atlas, overwriting", name);
            // Update existing region
            m_regions[it->second].uv_min = uv_min;
            m_regions[it->second].uv_max = uv_max;
            m_regions[it->second].size = glm::vec2(
                (uv_max.x - uv_min.x) * static_cast<float>(m_width),
                (uv_max.y - uv_min.y) * static_cast<float>(m_height)
            );
            return;
        }

        atlas_region region;
        region.name = name;
        region.uv_min = uv_min;
        region.uv_max = uv_max;
        region.size = glm::vec2(
            (uv_max.x - uv_min.x) * static_cast<float>(m_width),
            (uv_max.y - uv_min.y) * static_cast<float>(m_height)
        );
        region.offset = {0.0f, 0.0f};

        uint32_t index = static_cast<uint32_t>(m_regions.size());
        m_regions.push_back(region);
        m_region_lookup[name] = index;
    }

    void texture_atlas::add_region_pixels(const std::string& name, float x, float y, float width, float height)
    {
        float inv_w = 1.0f / static_cast<float>(m_width);
        float inv_h = 1.0f / static_cast<float>(m_height);

        glm::vec2 uv_min = glm::vec2(x * inv_w, y * inv_h);
        glm::vec2 uv_max = glm::vec2((x + width) * inv_w, (y + height) * inv_h);

        // Check if region already exists
        auto it = m_region_lookup.find(name);
        if (it != m_region_lookup.end())
        {
            LUMINA_LOG_WARN("Region '{}' already exists in atlas, overwriting", name);
            m_regions[it->second].uv_min = uv_min;
            m_regions[it->second].uv_max = uv_max;
            m_regions[it->second].size = glm::vec2(width, height);
            return;
        }

        atlas_region region;
        region.name = name;
        region.uv_min = uv_min;
        region.uv_max = uv_max;
        region.size = glm::vec2(width, height);
        region.offset = {0.0f, 0.0f};

        uint32_t index = static_cast<uint32_t>(m_regions.size());
        m_regions.push_back(region);
        m_region_lookup[name] = index;
    }

    void texture_atlas::add_grid(const std::string& prefix, uint32_t cols, uint32_t rows,
                                 const glm::vec2& cell_size, const glm::vec2& offset,
                                 const glm::vec2& padding)
    {
        float inv_w = 1.0f / static_cast<float>(m_width);
        float inv_h = 1.0f / static_cast<float>(m_height);

        uint32_t index = 0;
        for (uint32_t row = 0; row < rows; row++)
        {
            for (uint32_t col = 0; col < cols; col++)
            {
                float x = offset.x + col * (cell_size.x + padding.x);
                float y = offset.y + row * (cell_size.y + padding.y);

                std::string name = prefix + std::to_string(index);

                atlas_region region;
                region.name = name;
                region.uv_min = glm::vec2(x * inv_w, y * inv_h);
                region.uv_max = glm::vec2((x + cell_size.x) * inv_w, (y + cell_size.y) * inv_h);
                region.size = cell_size;
                region.offset = {0.0f, 0.0f};

                uint32_t region_index = static_cast<uint32_t>(m_regions.size());
                m_regions.push_back(region);
                m_region_lookup[name] = region_index;

                index++;
            }
        }
    }

    void texture_atlas::add_grid(const std::vector<std::string>& names, uint32_t cols,
                                 const glm::vec2& cell_size, const glm::vec2& offset,
                                 const glm::vec2& padding)
    {
        float inv_w = 1.0f / static_cast<float>(m_width);
        float inv_h = 1.0f / static_cast<float>(m_height);

        uint32_t rows = static_cast<uint32_t>((names.size() + cols - 1) / cols);

        uint32_t index = 0;
        for (uint32_t row = 0; row < rows && index < names.size(); row++)
        {
            for (uint32_t col = 0; col < cols && index < names.size(); col++)
            {
                float x = offset.x + col * (cell_size.x + padding.x);
                float y = offset.y + row * (cell_size.y + padding.y);

                const std::string& name = names[index];

                atlas_region region;
                region.name = name;
                region.uv_min = glm::vec2(x * inv_w, y * inv_h);
                region.uv_max = glm::vec2((x + cell_size.x) * inv_w, (y + cell_size.y) * inv_h);
                region.size = cell_size;
                region.offset = {0.0f, 0.0f};

                uint32_t region_index = static_cast<uint32_t>(m_regions.size());
                m_regions.push_back(region);
                m_region_lookup[name] = region_index;

                index++;
            }
        }
    }

    const atlas_region* texture_atlas::get_region(const std::string& name) const
    {
        auto it = m_region_lookup.find(name);
        if (it == m_region_lookup.end())
            return nullptr;

        return &m_regions[it->second];
    }

    const atlas_region* texture_atlas::get_region(uint32_t index) const
    {
        if (index >= m_regions.size())
            return nullptr;

        return &m_regions[index];
    }

    bool texture_atlas::has_region(const std::string& name) const
    {
        return m_region_lookup.find(name) != m_region_lookup.end();
    }

    std::vector<std::string> texture_atlas::get_region_names() const
    {
        std::vector<std::string> names;
        names.reserve(m_regions.size());
        for (const auto& region : m_regions)
        {
            names.push_back(region.name);
        }
        return names;
    }
}
