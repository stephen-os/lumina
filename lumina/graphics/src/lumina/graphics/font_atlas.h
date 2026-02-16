#pragma once

#include "texture.h"

#include <lumina/core/base.h>

#include <glm/glm.hpp>

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace lumina::core { class device; }

namespace lumina::graphics
{
    /// Glyph metrics for text rendering.
    struct glyph_info
    {
        float u0, v0, u1, v1;   // UV coordinates in atlas texture
        float x0, y0, x1, y1;   // Quad offsets from cursor position (pixels)
        float advance_x;        // How far to move cursor after this glyph
    };

    /// Font atlas for GPU text rendering.
    /// Rasterizes TrueType fonts into a texture atlas with glyph metrics.
    class font_atlas
    {
    public:
        ~font_atlas();

        font_atlas(const font_atlas&) = delete;
        font_atlas& operator=(const font_atlas&) = delete;

        /// Creates a font atlas from raw TTF data. Returns nullptr on failure.
        [[nodiscard]] static ref<font_atlas> create(
            core::device& dev,
            const uint8_t* ttf_data,
            size_t ttf_size,
            float pixel_height,
            const char* charset = nullptr);

        /// Loads a font atlas from a TTF file. Returns nullptr on failure.
        [[nodiscard]] static ref<font_atlas> load(
            core::device& dev,
            const std::string& path,
            float pixel_height,
            const char* charset = nullptr);

        /// Gets glyph info for a Unicode codepoint. Returns nullptr if not in atlas.
        [[nodiscard]] const glyph_info* get_glyph(uint32_t codepoint) const;

        [[nodiscard]] ref<texture> get_texture() const noexcept { return m_texture; }
        [[nodiscard]] float get_pixel_height() const noexcept { return m_pixel_height; }
        [[nodiscard]] float get_line_height() const noexcept { return m_line_height; }
        [[nodiscard]] float get_ascent() const noexcept { return m_ascent; }
        [[nodiscard]] float get_descent() const noexcept { return m_descent; }

        /// Measures text dimensions in pixels.
        [[nodiscard]] glm::vec2 measure_text(std::string_view text) const;

        /// Gets kerning adjustment between two glyphs.
        [[nodiscard]] float get_kerning(uint32_t first, uint32_t second) const;

    private:
        font_atlas() = default;

        ref<texture> m_texture;
        std::unordered_map<uint32_t, glyph_info> m_glyphs;

        float m_pixel_height = 0.0f;
        float m_line_height = 0.0f;
        float m_ascent = 0.0f;
        float m_descent = 0.0f;

        std::vector<uint8_t> m_ttf_data;
        bool m_has_kerning = false;
    };
}
