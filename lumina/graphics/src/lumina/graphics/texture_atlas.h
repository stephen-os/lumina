#pragma once

#include "texture.h"

#include <lumina/core/base.h>

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace lumina::graphics
{
    struct atlas_region
    {
        std::string name;
        glm::vec2 uv_min{0.0f};      // Top-left UV coordinates
        glm::vec2 uv_max{1.0f};      // Bottom-right UV coordinates
        glm::vec2 size{0.0f};        // Original size in pixels
        glm::vec2 offset{0.0f};      // Offset if trimmed (for packed atlases)

        // Helper to get the width/height of the region in pixels
        float width() const { return size.x; }
        float height() const { return size.y; }
    };

    class texture_atlas
    {
    public:
        ~texture_atlas() = default;

        texture_atlas(const texture_atlas&) = delete;
        texture_atlas& operator=(const texture_atlas&) = delete;

        // === Factory Methods ===

        // Create an empty atlas from an existing texture
        static ref<texture_atlas> create(ref<texture> texture);

        // === Region Management ===

        // Add a region manually using UV coordinates (0-1 normalized)
        void add_region(const std::string& name, const glm::vec2& uv_min, const glm::vec2& uv_max);

        // Add a region using pixel coordinates
        void add_region_pixels(const std::string& name, float x, float y, float width, float height);

        // Add a grid of regions (useful for uniform sprite sheets)
        // Creates regions named: prefix_0, prefix_1, ... prefix_N
        // Iterates left-to-right, top-to-bottom
        void add_grid(const std::string& prefix, uint32_t cols, uint32_t rows,
                     const glm::vec2& cell_size, const glm::vec2& offset = {0.0f, 0.0f},
                     const glm::vec2& padding = {0.0f, 0.0f});

        // Add a grid of regions with individual names
        void add_grid(const std::vector<std::string>& names, uint32_t cols,
                     const glm::vec2& cell_size, const glm::vec2& offset = {0.0f, 0.0f},
                     const glm::vec2& padding = {0.0f, 0.0f});

        // === Region Access ===

        // Get a region by name (returns nullptr if not found)
        const atlas_region* get_region(const std::string& name) const;

        // Get a region by index
        const atlas_region* get_region(uint32_t index) const;

        // Check if a region exists
        bool has_region(const std::string& name) const;

        // Get the number of regions
        uint32_t get_region_count() const { return static_cast<uint32_t>(m_regions.size()); }

        // Get all region names
        std::vector<std::string> get_region_names() const;

        // === Texture Access ===

        ref<texture> get_texture() const { return m_texture; }

        uint32_t get_width() const { return m_width; }
        uint32_t get_height() const { return m_height; }

    private:
        texture_atlas(ref<texture> texture, uint32_t width, uint32_t height)
            : m_texture(texture)
            , m_width(width)
            , m_height(height)
        {}

        ref<texture> m_texture;
        uint32_t m_width = 0;
        uint32_t m_height = 0;

        std::vector<atlas_region> m_regions;
        std::unordered_map<std::string, uint32_t> m_region_lookup;  // name -> index
    };
}
