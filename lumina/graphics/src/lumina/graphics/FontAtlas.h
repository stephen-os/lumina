#pragma once

#include "Texture.h"

#include <lumina/core/Base.h>

#include <glm/glm.hpp>

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace Lumina { class Device; }

namespace Lumina
{
    /// Glyph metrics for text rendering.
    struct GlyphInfo
    {
        float U0, V0, U1, V1;   // UV coordinates in atlas texture
        float X0, Y0, X1, Y1;   // Quad offsets from cursor position (pixels)
        float AdvanceX;        // How far to move cursor after this glyph
    };

    /// Font atlas for GPU text rendering.
    /// Rasterizes TrueType fonts into a texture atlas with glyph metrics.
    class FontAtlas
    {
    public:
        ~FontAtlas();

        FontAtlas(const FontAtlas&) = delete;
        FontAtlas& operator=(const FontAtlas&) = delete;

        /// Creates a font atlas from raw TTF data. Returns nullptr on failure.
        [[nodiscard]] static ref<FontAtlas> Create(
            Device& dev,
            const uint8_t* ttfData,
            size_t ttfSize,
            float pixelHeight,
            const char* charset = nullptr);

        /// Loads a font atlas from a TTF file. Returns nullptr on failure.
        [[nodiscard]] static ref<FontAtlas> Load(
            Device& dev,
            const std::string& path,
            float pixelHeight,
            const char* charset = nullptr);

        /// Gets glyph info for a Unicode codepoint. Returns nullptr if not in atlas.
        [[nodiscard]] const GlyphInfo* GetGlyph(uint32_t codepoint) const;

        [[nodiscard]] ref<Texture> GetTexture() const noexcept { return m_Texture; }
        [[nodiscard]] float GetPixelHeight() const noexcept { return m_PixelHeight; }
        [[nodiscard]] float GetLineHeight() const noexcept { return m_LineHeight; }
        [[nodiscard]] float GetAscent() const noexcept { return m_Ascent; }
        [[nodiscard]] float GetDescent() const noexcept { return m_Descent; }

        /// Measures text dimensions in pixels.
        [[nodiscard]] glm::vec2 MeasureText(std::string_view text) const;

        /// Gets kerning adjustment between two glyphs.
        [[nodiscard]] float GetKerning(uint32_t first, uint32_t second) const;

    private:
        FontAtlas() = default;

        ref<Texture> m_Texture;
        std::unordered_map<uint32_t, GlyphInfo> m_Glyphs;

        float m_PixelHeight = 0.0f;
        float m_LineHeight = 0.0f;
        float m_Ascent = 0.0f;
        float m_Descent = 0.0f;

        std::vector<uint8_t> m_TtfData;
        bool m_HasKerning = false;
    };
}
