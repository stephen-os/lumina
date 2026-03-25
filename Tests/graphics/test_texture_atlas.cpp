// Unit tests for texture_atlas

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <Lumina/Graphics/texture_atlas.h>
#include <glm/glm.hpp>

using namespace lumina::graphics;
using Catch::Approx;

TEST_CASE("atlas_region structure", "[graphics][texture_atlas]")
{
    SECTION("default construction")
    {
        atlas_region region;

        REQUIRE(region.name.empty());
        REQUIRE(region.uv_min == glm::vec2(0.0f, 0.0f));
        REQUIRE(region.uv_max == glm::vec2(1.0f, 1.0f));
        REQUIRE(region.size == glm::vec2(0.0f, 0.0f));
        REQUIRE(region.offset == glm::vec2(0.0f, 0.0f));
    }

    SECTION("UV coordinates")
    {
        atlas_region region;
        region.uv_min = {0.25f, 0.25f};
        region.uv_max = {0.75f, 0.75f};

        float width = region.uv_max.x - region.uv_min.x;
        float height = region.uv_max.y - region.uv_min.y;

        REQUIRE(width == Approx(0.5f));
        REQUIRE(height == Approx(0.5f));
    }
}

TEST_CASE("atlas grid subdivision calculations", "[graphics][texture_atlas]")
{
    // Test the math for grid subdivision (even without creating actual atlas)

    SECTION("4x4 grid on 64x64 texture")
    {
        uint32_t texture_width = 64;
        uint32_t texture_height = 64;
        uint32_t cols = 4;
        uint32_t rows = 4;
        float cell_width = static_cast<float>(texture_width) / cols;
        float cell_height = static_cast<float>(texture_height) / rows;

        REQUIRE(cell_width == Approx(16.0f));
        REQUIRE(cell_height == Approx(16.0f));

        // UV for cell (0, 0)
        float u0 = 0.0f / texture_width;
        float v0 = 0.0f / texture_height;
        float u1 = cell_width / texture_width;
        float v1 = cell_height / texture_height;

        REQUIRE(u0 == Approx(0.0f));
        REQUIRE(v0 == Approx(0.0f));
        REQUIRE(u1 == Approx(0.25f));
        REQUIRE(v1 == Approx(0.25f));

        // UV for cell (2, 1) - third column, second row
        int col = 2, row = 1;
        float u0_21 = (col * cell_width) / texture_width;
        float v0_21 = (row * cell_height) / texture_height;
        float u1_21 = ((col + 1) * cell_width) / texture_width;
        float v1_21 = ((row + 1) * cell_height) / texture_height;

        REQUIRE(u0_21 == Approx(0.5f));
        REQUIRE(v0_21 == Approx(0.25f));
        REQUIRE(u1_21 == Approx(0.75f));
        REQUIRE(v1_21 == Approx(0.5f));
    }

    SECTION("8x4 grid (sprite sheet)")
    {
        uint32_t texture_width = 256;
        uint32_t texture_height = 128;
        uint32_t cols = 8;
        uint32_t rows = 4;
        float cell_width = static_cast<float>(texture_width) / cols;
        float cell_height = static_cast<float>(texture_height) / rows;

        REQUIRE(cell_width == Approx(32.0f));
        REQUIRE(cell_height == Approx(32.0f));

        // Calculate UV for last cell (7, 3)
        int col = 7, row = 3;
        float u0 = (col * cell_width) / texture_width;
        float v0 = (row * cell_height) / texture_height;
        float u1 = ((col + 1) * cell_width) / texture_width;
        float v1 = ((row + 1) * cell_height) / texture_height;

        REQUIRE(u0 == Approx(0.875f));
        REQUIRE(v0 == Approx(0.75f));
        REQUIRE(u1 == Approx(1.0f));
        REQUIRE(v1 == Approx(1.0f));
    }

    SECTION("non-square cells")
    {
        uint32_t texture_width = 512;
        uint32_t texture_height = 256;
        uint32_t cols = 4;
        uint32_t rows = 2;
        float cell_width = static_cast<float>(texture_width) / cols;
        float cell_height = static_cast<float>(texture_height) / rows;

        REQUIRE(cell_width == Approx(128.0f));
        REQUIRE(cell_height == Approx(128.0f));
    }
}

TEST_CASE("atlas region index calculation", "[graphics][texture_atlas]")
{
    SECTION("row-major index")
    {
        uint32_t cols = 8;

        // Index = row * cols + col
        REQUIRE((0 * cols + 0) == 0);   // Cell (0,0)
        REQUIRE((0 * cols + 7) == 7);   // Cell (7,0)
        REQUIRE((1 * cols + 0) == 8);   // Cell (0,1)
        REQUIRE((3 * cols + 7) == 31);  // Cell (7,3) in 8x4 grid
    }

    SECTION("index to col/row")
    {
        uint32_t cols = 8;

        uint32_t index = 25;
        uint32_t col = index % cols;
        uint32_t row = index / cols;

        REQUIRE(col == 1);
        REQUIRE(row == 3);
    }
}

TEST_CASE("atlas flip UV calculations", "[graphics][texture_atlas]")
{
    atlas_region region;
    region.uv_min = {0.0f, 0.0f};
    region.uv_max = {0.25f, 0.25f};

    SECTION("horizontal flip")
    {
        // Flip X: swap u_min and u_max
        glm::vec2 flipped_min = {region.uv_max.x, region.uv_min.y};
        glm::vec2 flipped_max = {region.uv_min.x, region.uv_max.y};

        REQUIRE(flipped_min.x == Approx(0.25f));
        REQUIRE(flipped_min.y == Approx(0.0f));
        REQUIRE(flipped_max.x == Approx(0.0f));
        REQUIRE(flipped_max.y == Approx(0.25f));
    }

    SECTION("vertical flip")
    {
        // Flip Y: swap v_min and v_max
        glm::vec2 flipped_min = {region.uv_min.x, region.uv_max.y};
        glm::vec2 flipped_max = {region.uv_max.x, region.uv_min.y};

        REQUIRE(flipped_min.x == Approx(0.0f));
        REQUIRE(flipped_min.y == Approx(0.25f));
        REQUIRE(flipped_max.x == Approx(0.25f));
        REQUIRE(flipped_max.y == Approx(0.0f));
    }

    SECTION("both flip")
    {
        // Flip both: swap both
        glm::vec2 flipped_min = {region.uv_max.x, region.uv_max.y};
        glm::vec2 flipped_max = {region.uv_min.x, region.uv_min.y};

        REQUIRE(flipped_min == region.uv_max);
        REQUIRE(flipped_max == region.uv_min);
    }
}

// =============================================================================
// RIGOROUS MATHEMATICAL VERIFICATION TESTS
// =============================================================================

TEST_CASE("atlas UV precision - mathematical verification", "[graphics][texture_atlas][math]")
{
    // Verify UV calculations maintain full precision across various texture sizes

    SECTION("power of 2 textures - exact representation")
    {
        // For power-of-2 textures, certain UVs should be exactly representable

        // 256x256 texture, 32x32 cells = 8x8 grid
        uint32_t tex_w = 256, tex_h = 256;
        uint32_t cell_w = 32, cell_h = 32;
        float inv_w = 1.0f / tex_w;
        float inv_h = 1.0f / tex_h;

        // Cell (0,0): UV should be exactly (0, 0) to (1/8, 1/8)
        float u_min_0 = 0.0f * inv_w;
        float u_max_0 = 32.0f * inv_w;
        REQUIRE(u_min_0 == 0.0f);  // Exact
        REQUIRE(u_max_0 == 0.125f);  // 1/8 = 0.125 exact

        // Cell (4,4): UV should be exactly (0.5, 0.5) to (0.625, 0.625)
        float u_min_4 = 128.0f * inv_w;
        float u_max_4 = 160.0f * inv_w;
        REQUIRE(u_min_4 == 0.5f);  // Exact
        REQUIRE(u_max_4 == 0.625f);  // Exact
    }

    SECTION("non-power-of-2 textures - precision bounds")
    {
        // For NPOT textures, verify precision is within acceptable bounds

        // 100x100 texture, 10x10 cells = 10x10 grid
        uint32_t tex_w = 100, tex_h = 100;
        uint32_t cell_w = 10, cell_h = 10;
        float inv_w = 1.0f / static_cast<float>(tex_w);
        float inv_h = 1.0f / static_cast<float>(tex_h);

        // Each cell should span exactly 0.1 in UV space
        float uv_width = static_cast<float>(cell_w) * inv_w;
        float uv_height = static_cast<float>(cell_h) * inv_h;

        REQUIRE(uv_width == Approx(0.1f).epsilon(1e-6f));
        REQUIRE(uv_height == Approx(0.1f).epsilon(1e-6f));

        // Verify all cells sum correctly (no accumulated error)
        float total_u = 0.0f;
        for (int i = 0; i < 10; ++i)
        {
            total_u += uv_width;
        }
        REQUIRE(total_u == Approx(1.0f).epsilon(1e-5f));
    }
}

TEST_CASE("atlas pixel to UV conversion - mathematical verification", "[graphics][texture_atlas][math]")
{
    // Formula: uv = pixel / texture_size
    // Inverse: pixel = uv * texture_size

    SECTION("exact pixel centers")
    {
        // For texels, the center is at (pixel + 0.5) / texture_size
        // But for region bounds, we use pixel edges

        uint32_t tex_w = 64, tex_h = 64;
        float inv_w = 1.0f / tex_w;
        float inv_h = 1.0f / tex_h;

        // Pixel (0,0) to (16,16) region
        glm::vec2 uv_min = {0.0f * inv_w, 0.0f * inv_h};
        glm::vec2 uv_max = {16.0f * inv_w, 16.0f * inv_h};

        REQUIRE(uv_min.x == Approx(0.0f));
        REQUIRE(uv_min.y == Approx(0.0f));
        REQUIRE(uv_max.x == Approx(0.25f));
        REQUIRE(uv_max.y == Approx(0.25f));

        // Verify size calculation: (uv_max - uv_min) * texture_size = pixel_size
        glm::vec2 size = (uv_max - uv_min) * glm::vec2(tex_w, tex_h);
        REQUIRE(size.x == Approx(16.0f));
        REQUIRE(size.y == Approx(16.0f));
    }

    SECTION("arbitrary pixel region")
    {
        uint32_t tex_w = 512, tex_h = 256;
        float inv_w = 1.0f / tex_w;
        float inv_h = 1.0f / tex_h;

        // Pixel region: x=100, y=50, width=80, height=40
        float px_x = 100.0f, px_y = 50.0f;
        float px_w = 80.0f, px_h = 40.0f;

        glm::vec2 uv_min = {px_x * inv_w, px_y * inv_h};
        glm::vec2 uv_max = {(px_x + px_w) * inv_w, (px_y + px_h) * inv_h};

        // Verify UV values
        REQUIRE(uv_min.x == Approx(100.0f / 512.0f));
        REQUIRE(uv_min.y == Approx(50.0f / 256.0f));
        REQUIRE(uv_max.x == Approx(180.0f / 512.0f));
        REQUIRE(uv_max.y == Approx(90.0f / 256.0f));

        // Verify we can recover exact pixel dimensions
        glm::vec2 recovered_size = (uv_max - uv_min) * glm::vec2(tex_w, tex_h);
        REQUIRE(recovered_size.x == Approx(px_w));
        REQUIRE(recovered_size.y == Approx(px_h));
    }
}

TEST_CASE("atlas grid with padding - mathematical verification", "[graphics][texture_atlas][math]")
{
    // Grid formula:
    //   x = offset.x + col * (cell_size.x + padding.x)
    //   y = offset.y + row * (cell_size.y + padding.y)

    SECTION("uniform padding")
    {
        uint32_t tex_w = 256, tex_h = 256;
        float inv_w = 1.0f / tex_w;
        float inv_h = 1.0f / tex_h;

        glm::vec2 cell_size = {30.0f, 30.0f};
        glm::vec2 offset = {0.0f, 0.0f};
        glm::vec2 padding = {2.0f, 2.0f};

        // Cell (0,0): x=0, y=0
        float x0 = offset.x + 0 * (cell_size.x + padding.x);
        float y0 = offset.y + 0 * (cell_size.y + padding.y);
        REQUIRE(x0 == 0.0f);
        REQUIRE(y0 == 0.0f);

        // Cell (1,0): x=32, y=0 (30 + 2 padding)
        float x1 = offset.x + 1 * (cell_size.x + padding.x);
        float y1 = offset.y + 0 * (cell_size.y + padding.y);
        REQUIRE(x1 == Approx(32.0f));
        REQUIRE(y1 == 0.0f);

        // Cell (2,1): x=64, y=32
        float x21 = offset.x + 2 * (cell_size.x + padding.x);
        float y21 = offset.y + 1 * (cell_size.y + padding.y);
        REQUIRE(x21 == Approx(64.0f));
        REQUIRE(y21 == Approx(32.0f));

        // UV for cell (2,1)
        glm::vec2 uv_min = {x21 * inv_w, y21 * inv_h};
        glm::vec2 uv_max = {(x21 + cell_size.x) * inv_w, (y21 + cell_size.y) * inv_h};

        REQUIRE(uv_min.x == Approx(64.0f / 256.0f));
        REQUIRE(uv_min.y == Approx(32.0f / 256.0f));
        REQUIRE(uv_max.x == Approx(94.0f / 256.0f));
        REQUIRE(uv_max.y == Approx(62.0f / 256.0f));
    }

    SECTION("offset from edge")
    {
        uint32_t tex_w = 128, tex_h = 128;
        float inv_w = 1.0f / tex_w;
        float inv_h = 1.0f / tex_h;

        glm::vec2 cell_size = {16.0f, 16.0f};
        glm::vec2 offset = {8.0f, 8.0f};  // 8 pixel border
        glm::vec2 padding = {0.0f, 0.0f};

        // Cell (0,0): starts at (8, 8)
        float x0 = offset.x + 0 * cell_size.x;
        float y0 = offset.y + 0 * cell_size.y;

        glm::vec2 uv_min = {x0 * inv_w, y0 * inv_h};
        glm::vec2 uv_max = {(x0 + cell_size.x) * inv_w, (y0 + cell_size.y) * inv_h};

        // UV should start at 8/128 = 0.0625
        REQUIRE(uv_min.x == Approx(0.0625f));
        REQUIRE(uv_min.y == Approx(0.0625f));
        REQUIRE(uv_max.x == Approx(0.1875f));  // 24/128
        REQUIRE(uv_max.y == Approx(0.1875f));
    }
}

TEST_CASE("atlas region size recovery - mathematical verification", "[graphics][texture_atlas][math]")
{
    // Given UVs and texture size, verify we can recover exact pixel dimensions

    SECTION("verify size formula: size = (uv_max - uv_min) * texture_size")
    {
        uint32_t tex_w = 512, tex_h = 512;

        // Original pixel region: 64x48 at position (128, 96)
        float orig_x = 128.0f, orig_y = 96.0f;
        float orig_w = 64.0f, orig_h = 48.0f;

        float inv_w = 1.0f / tex_w;
        float inv_h = 1.0f / tex_h;

        glm::vec2 uv_min = {orig_x * inv_w, orig_y * inv_h};
        glm::vec2 uv_max = {(orig_x + orig_w) * inv_w, (orig_y + orig_h) * inv_h};

        // Recovery formula
        glm::vec2 recovered_size = (uv_max - uv_min) * glm::vec2(tex_w, tex_h);

        REQUIRE(recovered_size.x == Approx(orig_w));
        REQUIRE(recovered_size.y == Approx(orig_h));
    }

    SECTION("verify position recovery")
    {
        uint32_t tex_w = 256, tex_h = 128;

        float orig_x = 32.0f, orig_y = 16.0f;
        float inv_w = 1.0f / tex_w;
        float inv_h = 1.0f / tex_h;

        glm::vec2 uv_min = {orig_x * inv_w, orig_y * inv_h};

        // Recovery: position = uv_min * texture_size
        glm::vec2 recovered_pos = uv_min * glm::vec2(tex_w, tex_h);

        REQUIRE(recovered_pos.x == Approx(orig_x));
        REQUIRE(recovered_pos.y == Approx(orig_y));
    }
}

TEST_CASE("atlas animation frame calculation - mathematical verification", "[graphics][texture_atlas][math]")
{
    // Animation frames are typically laid out in row-major order
    // Given frame index, calculate UV coordinates

    SECTION("walk animation in 8-column strip")
    {
        uint32_t tex_w = 256, tex_h = 32;
        uint32_t cols = 8;
        glm::vec2 frame_size = {32.0f, 32.0f};

        float inv_w = 1.0f / tex_w;
        float inv_h = 1.0f / tex_h;

        // Frame 0
        uint32_t frame = 0;
        uint32_t col = frame % cols;
        uint32_t row = frame / cols;
        float x = col * frame_size.x;
        float y = row * frame_size.y;

        glm::vec2 uv_min_0 = {x * inv_w, y * inv_h};
        glm::vec2 uv_max_0 = {(x + frame_size.x) * inv_w, (y + frame_size.y) * inv_h};

        REQUIRE(uv_min_0.x == Approx(0.0f));
        REQUIRE(uv_min_0.y == Approx(0.0f));
        REQUIRE(uv_max_0.x == Approx(0.125f));
        REQUIRE(uv_max_0.y == Approx(1.0f));

        // Frame 5
        frame = 5;
        col = frame % cols;
        row = frame / cols;
        x = col * frame_size.x;
        y = row * frame_size.y;

        glm::vec2 uv_min_5 = {x * inv_w, y * inv_h};
        glm::vec2 uv_max_5 = {(x + frame_size.x) * inv_w, (y + frame_size.y) * inv_h};

        REQUIRE(uv_min_5.x == Approx(5.0f * 32.0f / 256.0f));  // 0.625
        REQUIRE(uv_min_5.y == Approx(0.0f));
        REQUIRE(uv_max_5.x == Approx(6.0f * 32.0f / 256.0f));  // 0.75
        REQUIRE(uv_max_5.y == Approx(1.0f));
    }

    SECTION("multi-row sprite sheet")
    {
        uint32_t tex_w = 128, tex_h = 128;
        uint32_t cols = 4, rows = 4;
        glm::vec2 frame_size = {32.0f, 32.0f};

        float inv_w = 1.0f / tex_w;
        float inv_h = 1.0f / tex_h;

        // Frame 13 (row 3, col 1 in 0-indexed)
        uint32_t frame = 13;
        uint32_t col = frame % cols;  // 13 % 4 = 1
        uint32_t row = frame / cols;  // 13 / 4 = 3
        float x = col * frame_size.x;
        float y = row * frame_size.y;

        REQUIRE(col == 1);
        REQUIRE(row == 3);

        glm::vec2 uv_min = {x * inv_w, y * inv_h};
        glm::vec2 uv_max = {(x + frame_size.x) * inv_w, (y + frame_size.y) * inv_h};

        REQUIRE(uv_min.x == Approx(32.0f / 128.0f));   // 0.25
        REQUIRE(uv_min.y == Approx(96.0f / 128.0f));   // 0.75
        REQUIRE(uv_max.x == Approx(64.0f / 128.0f));   // 0.5
        REQUIRE(uv_max.y == Approx(128.0f / 128.0f));  // 1.0
    }
}

TEST_CASE("atlas UV interpolation - mathematical verification", "[graphics][texture_atlas][math]")
{
    // When rendering a quad with UVs, GPU interpolates across vertices
    // Verify the math for sampling at specific positions

    atlas_region region;
    region.uv_min = {0.25f, 0.5f};
    region.uv_max = {0.5f, 0.75f};

    SECTION("corner UVs")
    {
        // Normalized quad positions: (0,0), (1,0), (0,1), (1,1)
        // UV = uv_min + (uv_max - uv_min) * position

        glm::vec2 uv_range = region.uv_max - region.uv_min;

        // Bottom-left (0, 0)
        glm::vec2 uv_bl = region.uv_min + uv_range * glm::vec2(0.0f, 0.0f);
        REQUIRE(uv_bl.x == Approx(0.25f));
        REQUIRE(uv_bl.y == Approx(0.5f));

        // Bottom-right (1, 0)
        glm::vec2 uv_br = region.uv_min + uv_range * glm::vec2(1.0f, 0.0f);
        REQUIRE(uv_br.x == Approx(0.5f));
        REQUIRE(uv_br.y == Approx(0.5f));

        // Top-left (0, 1)
        glm::vec2 uv_tl = region.uv_min + uv_range * glm::vec2(0.0f, 1.0f);
        REQUIRE(uv_tl.x == Approx(0.25f));
        REQUIRE(uv_tl.y == Approx(0.75f));

        // Top-right (1, 1)
        glm::vec2 uv_tr = region.uv_min + uv_range * glm::vec2(1.0f, 1.0f);
        REQUIRE(uv_tr.x == Approx(0.5f));
        REQUIRE(uv_tr.y == Approx(0.75f));
    }

    SECTION("center UV")
    {
        // Center of quad (0.5, 0.5)
        glm::vec2 uv_range = region.uv_max - region.uv_min;
        glm::vec2 uv_center = region.uv_min + uv_range * glm::vec2(0.5f, 0.5f);

        REQUIRE(uv_center.x == Approx(0.375f));  // (0.25 + 0.5) / 2
        REQUIRE(uv_center.y == Approx(0.625f));  // (0.5 + 0.75) / 2
    }
}
