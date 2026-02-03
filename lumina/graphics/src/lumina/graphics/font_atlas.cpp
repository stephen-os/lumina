#include "font_atlas.h"

#include <lumina/core/device.h>
#include <lumina/core/log.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#include <fstream>
#include <cmath>
#include <algorithm>

namespace lumina::graphics
{
    font_atlas::~font_atlas()
    {
        m_texture.reset();
        m_glyphs.clear();
        m_ttf_data.clear();
    }

    ref<font_atlas> font_atlas::create(core::device& dev, const uint8_t* ttf_data, size_t ttf_size, float pixel_height, const char* charset)
    {
        if (!ttf_data || ttf_size == 0)
        {
            LUMINA_LOG_ERROR("font_atlas::create - Invalid TTF data");
            return nullptr;
        }

        std::string default_charset;
        if (!charset)
        {
            for (int c = 32; c <= 126; c++)
                default_charset += static_cast<char>(c);
            charset = default_charset.c_str();
        }

        size_t num_chars = strlen(charset);
        if (num_chars == 0)
        {
            LUMINA_LOG_ERROR("font_atlas::create - Empty charset");
            return nullptr;
        }

        stbtt_fontinfo font_info;
        if (!stbtt_InitFont(&font_info, ttf_data, stbtt_GetFontOffsetForIndex(ttf_data, 0)))
        {
            LUMINA_LOG_ERROR("font_atlas::create - Failed to initialize font");
            return nullptr;
        }

        float scale = stbtt_ScaleForPixelHeight(&font_info, pixel_height);

        int ascent_i, descent_i, line_gap_i;
        stbtt_GetFontVMetrics(&font_info, &ascent_i, &descent_i, &line_gap_i);

        float ascent = ascent_i * scale;
        float descent = descent_i * scale;
        float line_gap = line_gap_i * scale;
        float line_height = ascent - descent + line_gap;

        int atlas_width = 256;
        int atlas_height = 256;

        int estimated_area = static_cast<int>(num_chars * pixel_height * pixel_height * 1.5f);
        while (atlas_width * atlas_height < estimated_area && atlas_width < 4096)
        {
            if (atlas_width <= atlas_height)
                atlas_width *= 2;
            else
                atlas_height *= 2;
        }

        std::vector<uint8_t> atlas_bitmap(atlas_width * atlas_height, 0);

        stbtt_pack_context pack_ctx;
        if (!stbtt_PackBegin(&pack_ctx, atlas_bitmap.data(), atlas_width, atlas_height, 0, 1, nullptr))
        {
            LUMINA_LOG_ERROR("font_atlas::create - Failed to begin packing");
            return nullptr;
        }

        stbtt_PackSetOversampling(&pack_ctx, 2, 2);

        std::vector<stbtt_packedchar> packed_chars(num_chars);

        stbtt_pack_range range;
        range.font_size = pixel_height;
        range.first_unicode_codepoint_in_range = 0;
        range.array_of_unicode_codepoints = nullptr;
        range.num_chars = static_cast<int>(num_chars);
        range.chardata_for_range = packed_chars.data();

        std::vector<int> codepoints(num_chars);
        for (size_t i = 0; i < num_chars; i++)
            codepoints[i] = static_cast<unsigned char>(charset[i]);

        range.array_of_unicode_codepoints = codepoints.data();

        if (!stbtt_PackFontRanges(&pack_ctx, ttf_data, 0, &range, 1))
        {
            stbtt_PackEnd(&pack_ctx);

            atlas_width *= 2;
            atlas_height *= 2;

            if (atlas_width > 4096 || atlas_height > 4096)
            {
                LUMINA_LOG_ERROR("font_atlas::create - Font requires atlas larger than 4096x4096");
                return nullptr;
            }

            atlas_bitmap.resize(atlas_width * atlas_height, 0);

            if (!stbtt_PackBegin(&pack_ctx, atlas_bitmap.data(), atlas_width, atlas_height, 0, 1, nullptr))
            {
                LUMINA_LOG_ERROR("font_atlas::create - Failed to begin packing (retry)");
                return nullptr;
            }

            stbtt_PackSetOversampling(&pack_ctx, 2, 2);

            if (!stbtt_PackFontRanges(&pack_ctx, ttf_data, 0, &range, 1))
            {
                LUMINA_LOG_ERROR("font_atlas::create - Failed to pack font ranges");
                stbtt_PackEnd(&pack_ctx);
                return nullptr;
            }
        }

        stbtt_PackEnd(&pack_ctx);

        std::vector<uint8_t> rgba_bitmap(atlas_width * atlas_height * 4);
        for (int i = 0; i < atlas_width * atlas_height; i++)
        {
            uint8_t alpha = atlas_bitmap[i];
            rgba_bitmap[i * 4 + 0] = 255;
            rgba_bitmap[i * 4 + 1] = 255;
            rgba_bitmap[i * 4 + 2] = 255;
            rgba_bitmap[i * 4 + 3] = alpha;
        }

        auto texture = texture::create(dev, atlas_width, atlas_height, format::rgba8_unorm, rgba_bitmap.data());
        if (!texture)
        {
            LUMINA_LOG_ERROR("font_atlas::create - Failed to create atlas texture");
            return nullptr;
        }

        std::unordered_map<uint32_t, glyph_info> glyphs;
        float inv_w = 1.0f / atlas_width;
        float inv_h = 1.0f / atlas_height;

        for (size_t i = 0; i < num_chars; i++)
        {
            const stbtt_packedchar& pc = packed_chars[i];
            uint32_t codepoint = static_cast<uint32_t>(codepoints[i]);

            glyph_info glyph;
            glyph.u0 = pc.x0 * inv_w;
            glyph.v0 = pc.y0 * inv_h;
            glyph.u1 = pc.x1 * inv_w;
            glyph.v1 = pc.y1 * inv_h;
            glyph.x0 = pc.xoff;
            glyph.y0 = pc.yoff + ascent;
            glyph.x1 = pc.xoff2;
            glyph.y1 = pc.yoff2 + ascent;
            glyph.advance_x = pc.xadvance;

            glyphs[codepoint] = glyph;
        }

        bool has_kerning = stbtt_GetKerningTableLength(&font_info) > 0;

        auto atlas = ref<font_atlas>(new font_atlas());
        atlas->m_texture = texture;
        atlas->m_glyphs = std::move(glyphs);
        atlas->m_pixel_height = pixel_height;
        atlas->m_line_height = line_height;
        atlas->m_ascent = ascent;
        atlas->m_descent = descent;
        atlas->m_has_kerning = has_kerning;

        if (has_kerning)
        {
            atlas->m_ttf_data.assign(ttf_data, ttf_data + ttf_size);
        }

        LUMINA_LOG_INFO("font_atlas created: {}x{} atlas, {} glyphs, {:.1f}px height",
                        atlas_width, atlas_height, num_chars, pixel_height);

        return atlas;
    }

    ref<font_atlas> font_atlas::load(core::device& dev, const std::string& path, float pixel_height, const char* charset)
    {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            LUMINA_LOG_ERROR("font_atlas::load - Failed to open file: {}", path);
            return nullptr;
        }

        size_t file_size = static_cast<size_t>(file.tellg());
        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> ttf_data(file_size);
        if (!file.read(reinterpret_cast<char*>(ttf_data.data()), file_size))
        {
            LUMINA_LOG_ERROR("font_atlas::load - Failed to read file: {}", path);
            return nullptr;
        }

        return create(dev, ttf_data.data(), ttf_data.size(), pixel_height, charset);
    }

    const glyph_info* font_atlas::get_glyph(uint32_t codepoint) const
    {
        auto it = m_glyphs.find(codepoint);
        if (it != m_glyphs.end())
            return &it->second;

        it = m_glyphs.find(' ');
        if (it != m_glyphs.end())
            return &it->second;

        return nullptr;
    }

    glm::vec2 font_atlas::measure_text(std::string_view text) const
    {
        if (text.empty())
            return glm::vec2(0.0f);

        float width = 0.0f;
        float max_height = m_line_height;
        int line_count = 1;

        uint32_t prev_codepoint = 0;

        for (size_t i = 0; i < text.size(); i++)
        {
            uint32_t codepoint = static_cast<uint8_t>(text[i]);

            if (codepoint == '\n')
            {
                line_count++;
                prev_codepoint = 0;
                continue;
            }

            const glyph_info* glyph = get_glyph(codepoint);
            if (!glyph)
                continue;

            if (prev_codepoint != 0 && m_has_kerning)
            {
                width += get_kerning(prev_codepoint, codepoint);
            }

            width += glyph->advance_x;
            prev_codepoint = codepoint;
        }

        return glm::vec2(width, m_line_height * line_count);
    }

    float font_atlas::get_kerning(uint32_t first, uint32_t second) const
    {
        if (!m_has_kerning || m_ttf_data.empty())
            return 0.0f;

        stbtt_fontinfo font_info;
        if (!stbtt_InitFont(&font_info, m_ttf_data.data(), 0))
            return 0.0f;

        float scale = stbtt_ScaleForPixelHeight(&font_info, m_pixel_height);
        int kern = stbtt_GetCodepointKernAdvance(&font_info, first, second);

        return kern * scale;
    }
}
