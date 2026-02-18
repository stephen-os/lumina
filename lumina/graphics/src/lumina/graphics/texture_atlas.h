#pragma once

#include "texture.h"

#include <lumina/core/base.h>

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace lumina::graphics
{
    /// Describes a rectangular region within a texture atlas.
    struct atlas_region
    {
        std::string name;
        glm::vec2 uv_min{0.0f};      // Top-left UV coordinates
        glm::vec2 uv_max{1.0f};      // Bottom-right UV coordinates
        glm::vec2 size{0.0f};        // Original size in pixels
        glm::vec2 offset{0.0f};      // Offset if trimmed (for packed atlases)

        [[nodiscard]] float width() const noexcept { return size.x; }
        [[nodiscard]] float height() const noexcept { return size.y; }
    };

    /// Texture atlas for efficient sprite sheet rendering.
    /// Stores multiple sprite regions within a single texture.
    class texture_atlas
    {
    public:
        ~texture_atlas() = default;

        texture_atlas(const texture_atlas&) = delete;
        texture_atlas& operator=(const texture_atlas&) = delete;

        /// Creates an empty atlas from an existing texture.
        [[nodiscard]] static ref<texture_atlas> create(ref<texture> texture);

        /// Adds a region using normalized UV coordinates (0-1).
        void add_region(const std::string& name, const glm::vec2& uv_min, const glm::vec2& uv_max);

        /// Adds a region using pixel coordinates.
        void add_region_pixels(const std::string& name, float x, float y, float width, float height);

        /// Adds a grid of uniformly-sized regions.
        /// Creates regions named: prefix_0, prefix_1, ... prefix_N
        void add_grid(const std::string& prefix, uint32_t cols, uint32_t rows,
                     const glm::vec2& cell_size, const glm::vec2& offset = {0.0f, 0.0f},
                     const glm::vec2& padding = {0.0f, 0.0f});

        /// Adds a grid of regions with individual names.
        void add_grid(const std::vector<std::string>& names, uint32_t cols,
                     const glm::vec2& cell_size, const glm::vec2& offset = {0.0f, 0.0f},
                     const glm::vec2& padding = {0.0f, 0.0f});

        /// Gets a region by name. Returns nullptr if not found.
        [[nodiscard]] const atlas_region* get_region(const std::string& name) const;

        /// Gets a region by index. Returns nullptr if out of bounds.
        [[nodiscard]] const atlas_region* get_region(uint32_t index) const;

        [[nodiscard]] bool has_region(const std::string& name) const;
        [[nodiscard]] uint32_t get_region_count() const noexcept { return static_cast<uint32_t>(m_regions.size()); }
        [[nodiscard]] std::vector<std::string> get_region_names() const;

        [[nodiscard]] ref<texture> get_texture() const noexcept { return m_texture; }
        [[nodiscard]] uint32_t get_width() const noexcept { return m_width; }
        [[nodiscard]] uint32_t get_height() const noexcept { return m_height; }

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
        std::unordered_map<std::string, uint32_t> m_region_lookup;
    };
}
