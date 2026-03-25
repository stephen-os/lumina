#include "font_atlas.h"

#include <lumina/core/device.h>
#include <lumina/core/log.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#include <fstream>
#include <cmath>
#include <algorithm>

namespace Lumina
{
    FontAtlas::~FontAtlas()
    {
        m_Texture.reset();
        m_Glyphs.clear();
        m_TtfData.clear();
    }

    Ref<FontAtlas> FontAtlas::Create(Core::Device& dev, const uint8_t* ttfData, size_t ttfSize, float pixelHeight, const char* charset)
    {
        if (!ttfData || ttfSize == 0)
        {
            LUMINA_LOG_ERROR("FontAtlas::Create - Invalid TTF data");
            return nullptr;
        }

        std::string defaultCharset;
        if (!charset)
        {
            for (int c = 32; c <= 126; c++)
                defaultCharset += static_cast<char>(c);
            charset = defaultCharset.c_str();
        }

        size_t numChars = strlen(charset);
        if (numChars == 0)
        {
            LUMINA_LOG_ERROR("FontAtlas::Create - Empty charset");
            return nullptr;
        }

        stbtt_fontinfo fontInfo;
        if (!stbtt_InitFont(&fontInfo, ttfData, stbtt_GetFontOffsetForIndex(ttfData, 0)))
        {
            LUMINA_LOG_ERROR("FontAtlas::Create - Failed to initialize font");
            return nullptr;
        }

        float scale = stbtt_ScaleForPixelHeight(&fontInfo, pixelHeight);

        int ascentI, descentI, lineGapI;
        stbtt_GetFontVMetrics(&fontInfo, &ascentI, &descentI, &lineGapI);

        float ascent = ascentI * scale;
        float descent = descentI * scale;
        float lineGap = lineGapI * scale;
        float lineHeight = ascent - descent + lineGap;

        int atlasWidth = 256;
        int atlasHeight = 256;

        int estimatedArea = static_cast<int>(numChars * pixelHeight * pixelHeight * 1.5f);
        while (atlasWidth * atlasHeight < estimatedArea && atlasWidth < 4096)
        {
            if (atlasWidth <= atlasHeight)
                atlasWidth *= 2;
            else
                atlasHeight *= 2;
        }

        std::vector<uint8_t> atlasBitmap(atlasWidth * atlasHeight, 0);

        stbtt_pack_context packCtx;
        if (!stbtt_PackBegin(&packCtx, atlasBitmap.data(), atlasWidth, atlasHeight, 0, 1, nullptr))
        {
            LUMINA_LOG_ERROR("FontAtlas::Create - Failed to begin packing");
            return nullptr;
        }

        stbtt_PackSetOversampling(&packCtx, 2, 2);

        std::vector<stbtt_packedchar> packedChars(numChars);

        stbtt_pack_range range;
        range.font_size = pixelHeight;
        range.first_unicode_codepoint_in_range = 0;
        range.array_of_unicode_codepoints = nullptr;
        range.num_chars = static_cast<int>(numChars);
        range.chardata_for_range = packedChars.data();

        std::vector<int> codepoints(numChars);
        for (size_t i = 0; i < numChars; i++)
            codepoints[i] = static_cast<unsigned char>(charset[i]);

        range.array_of_unicode_codepoints = codepoints.data();

        if (!stbtt_PackFontRanges(&packCtx, ttfData, 0, &range, 1))
        {
            stbtt_PackEnd(&packCtx);

            atlasWidth *= 2;
            atlasHeight *= 2;

            if (atlasWidth > 4096 || atlasHeight > 4096)
            {
                LUMINA_LOG_ERROR("FontAtlas::Create - Font requires atlas larger than 4096x4096");
                return nullptr;
            }

            atlasBitmap.resize(atlasWidth * atlasHeight, 0);

            if (!stbtt_PackBegin(&packCtx, atlasBitmap.data(), atlasWidth, atlasHeight, 0, 1, nullptr))
            {
                LUMINA_LOG_ERROR("FontAtlas::Create - Failed to begin packing (retry)");
                return nullptr;
            }

            stbtt_PackSetOversampling(&packCtx, 2, 2);

            if (!stbtt_PackFontRanges(&packCtx, ttfData, 0, &range, 1))
            {
                LUMINA_LOG_ERROR("FontAtlas::Create - Failed to pack font ranges");
                stbtt_PackEnd(&packCtx);
                return nullptr;
            }
        }

        stbtt_PackEnd(&packCtx);

        std::vector<uint8_t> rgbaBitmap(atlasWidth * atlasHeight * 4);
        for (int i = 0; i < atlasWidth * atlasHeight; i++)
        {
            uint8_t alpha = atlasBitmap[i];
            rgbaBitmap[i * 4 + 0] = 255;
            rgbaBitmap[i * 4 + 1] = 255;
            rgbaBitmap[i * 4 + 2] = 255;
            rgbaBitmap[i * 4 + 3] = alpha;
        }

        auto texture = Texture::Create(dev, atlasWidth, atlasHeight, Format::RGBA8Unorm, rgbaBitmap.data());
        if (!texture)
        {
            LUMINA_LOG_ERROR("FontAtlas::Create - Failed to create atlas texture");
            return nullptr;
        }

        std::unordered_map<uint32_t, GlyphInfo> glyphs;
        float invW = 1.0f / atlasWidth;
        float invH = 1.0f / atlasHeight;

        for (size_t i = 0; i < numChars; i++)
        {
            const stbtt_packedchar& pc = packedChars[i];
            uint32_t codepoint = static_cast<uint32_t>(codepoints[i]);

            GlyphInfo glyph;
            glyph.u0 = pc.x0 * invW;
            glyph.v0 = pc.y0 * invH;
            glyph.u1 = pc.x1 * invW;
            glyph.v1 = pc.y1 * invH;
            glyph.x0 = pc.xoff;
            glyph.y0 = pc.yoff + ascent;
            glyph.x1 = pc.xoff2;
            glyph.y1 = pc.yoff2 + ascent;
            glyph.advanceX = pc.xadvance;

            glyphs[codepoint] = glyph;
        }

        bool hasKerning = stbtt_GetKerningTableLength(&fontInfo) > 0;

        auto atlas = Ref<FontAtlas>(new FontAtlas());
        atlas->m_Texture = texture;
        atlas->m_Glyphs = std::move(glyphs);
        atlas->m_PixelHeight = pixelHeight;
        atlas->m_LineHeight = lineHeight;
        atlas->m_Ascent = ascent;
        atlas->m_Descent = descent;
        atlas->m_HasKerning = hasKerning;

        if (hasKerning)
        {
            atlas->m_TtfData.assign(ttfData, ttfData + ttfSize);
        }

        LUMINA_LOG_INFO("FontAtlas created: {}x{} atlas, {} glyphs, {:.1f}px height",
                        atlasWidth, atlasHeight, numChars, pixelHeight);

        return atlas;
    }

    Ref<FontAtlas> FontAtlas::Load(Core::Device& dev, const std::string& path, float pixelHeight, const char* charset)
    {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            LUMINA_LOG_ERROR("FontAtlas::Load - Failed to open file: {}", path);
            return nullptr;
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> ttfData(fileSize);
        if (!file.read(reinterpret_cast<char*>(ttfData.data()), fileSize))
        {
            LUMINA_LOG_ERROR("FontAtlas::Load - Failed to read file: {}", path);
            return nullptr;
        }

        return Create(dev, ttfData.data(), ttfData.size(), pixelHeight, charset);
    }

    const GlyphInfo* FontAtlas::GetGlyph(uint32_t codepoint) const
    {
        auto it = m_Glyphs.find(codepoint);
        if (it != m_Glyphs.end())
            return &it->second;

        it = m_Glyphs.find(' ');
        if (it != m_Glyphs.end())
            return &it->second;

        return nullptr;
    }

    glm::vec2 FontAtlas::MeasureText(std::string_view text) const
    {
        if (text.empty())
            return glm::vec2(0.0f);

        float width = 0.0f;
        float maxHeight = m_LineHeight;
        int lineCount = 1;

        uint32_t prevCodepoint = 0;

        for (size_t i = 0; i < text.size(); i++)
        {
            uint32_t codepoint = static_cast<uint8_t>(text[i]);

            if (codepoint == '\n')
            {
                lineCount++;
                prevCodepoint = 0;
                continue;
            }

            const GlyphInfo* glyph = GetGlyph(codepoint);
            if (!glyph)
                continue;

            if (prevCodepoint != 0 && m_HasKerning)
            {
                width += GetKerning(prevCodepoint, codepoint);
            }

            width += glyph->advanceX;
            prevCodepoint = codepoint;
        }

        return glm::vec2(width, m_LineHeight * lineCount);
    }

    float FontAtlas::GetKerning(uint32_t first, uint32_t second) const
    {
        if (!m_HasKerning || m_TtfData.empty())
            return 0.0f;

        stbtt_fontinfo fontInfo;
        if (!stbtt_InitFont(&fontInfo, m_TtfData.data(), 0))
            return 0.0f;

        float scale = stbtt_ScaleForPixelHeight(&fontInfo, m_PixelHeight);
        int kern = stbtt_GetCodepointKernAdvance(&fontInfo, first, second);

        return kern * scale;
    }
}
