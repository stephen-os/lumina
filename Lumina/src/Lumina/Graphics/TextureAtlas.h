#pragma once

#include "Texture.h"

#include <Lumina/Core/Base.h>

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace Lumina
{
    /// Describes a rectangular region within a texture atlas.
    struct AtlasRegion
    {
        std::string Name;
        glm::vec2 UVMin{0.0f};      // Top-left UV coordinates
        glm::vec2 UVMax{1.0f};      // Bottom-right UV coordinates
        glm::vec2 Size{0.0f};        // Original size in pixels
        glm::vec2 Offset{0.0f};      // Offset if trimmed (for packed atlases)

        [[nodiscard]] float Width() const noexcept { return Size.x; }
        [[nodiscard]] float Height() const noexcept { return Size.y; }
    };

    /// Texture atlas for efficient sprite sheet rendering.
    /// Stores multiple sprite regions within a single texture.
    class TextureAtlas
    {
    public:
        ~TextureAtlas() = default;

        TextureAtlas(const TextureAtlas&) = delete;
        TextureAtlas& operator=(const TextureAtlas&) = delete;

        /// Creates an empty atlas from an existing texture.
        [[nodiscard]] static ref<TextureAtlas> Create(ref<Texture> texture);

        /// Adds a region using normalized UV coordinates (0-1).
        void AddRegion(const std::string& name, const glm::vec2& uvMin, const glm::vec2& uvMax);

        /// Adds a region using pixel coordinates.
        void AddRegionPixels(const std::string& name, float x, float y, float width, float height);

        /// Adds a grid of uniformly-sized regions.
        /// Creates regions named: prefix_0, prefix_1, ... prefix_N
        void AddGrid(const std::string& prefix, uint32_t cols, uint32_t rows,
                     const glm::vec2& cellSize, const glm::vec2& offset = {0.0f, 0.0f},
                     const glm::vec2& padding = {0.0f, 0.0f});

        /// Adds a grid of regions with individual names.
        void AddGrid(const std::vector<std::string>& names, uint32_t cols,
                     const glm::vec2& cellSize, const glm::vec2& offset = {0.0f, 0.0f},
                     const glm::vec2& padding = {0.0f, 0.0f});

        /// Gets a region by name. Returns nullptr if not found.
        [[nodiscard]] const AtlasRegion* GetRegion(const std::string& name) const;

        /// Gets a region by index. Returns nullptr if out of bounds.
        [[nodiscard]] const AtlasRegion* GetRegion(uint32_t index) const;

        [[nodiscard]] bool HasRegion(const std::string& name) const;
        [[nodiscard]] uint32_t GetRegionCount() const noexcept { return static_cast<uint32_t>(m_Regions.size()); }
        [[nodiscard]] std::vector<std::string> GetRegionNames() const;

        [[nodiscard]] ref<Texture> GetTexture() const noexcept { return m_Texture; }
        [[nodiscard]] uint32_t GetWidth() const noexcept { return m_Width; }
        [[nodiscard]] uint32_t GetHeight() const noexcept { return m_Height; }

    private:
        TextureAtlas(ref<Texture> texture, uint32_t width, uint32_t height)
            : m_Texture(texture)
            , m_Width(width)
            , m_Height(height)
        {}

        ref<Texture> m_Texture;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;

        std::vector<AtlasRegion> m_Regions;
        std::unordered_map<std::string, uint32_t> m_RegionLookup;
    };
}
