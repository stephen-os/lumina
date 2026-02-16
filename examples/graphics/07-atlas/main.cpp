// 07-atlas: Texture Atlas and Camera2D Demo
// Demonstrates: texture_atlas, camera2d, sprite rendering with the new optimizations

#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/core/input.h>
#include <lumina/graphics/graphics.h>
#include <lumina/ui/ui.h>

#include <glm/glm.hpp>
#include <filesystem>

namespace ui = lumina::ui;
namespace gfx = lumina::graphics;
namespace input = lumina::core::input;

// Get asset path relative to this source file
static std::string get_asset_path(const std::string& filename)
{
    std::filesystem::path source_dir = std::filesystem::path(__FILE__).parent_path();
    return (source_dir / filename).string();
}

class atlas_layer : public lumina::core::layer
{
public:
    atlas_layer() : layer("atlas") {}

    void on_attach() override
    {
        auto& device = lumina::core::application::get().get_device();
        m_renderer = std::make_unique<gfx::renderer2d>(device);
        m_renderer->init();

        // Create render target for the viewport (square)
        m_render_target = gfx::render_target::create(
            device, 600, 600, gfx::format::rgba8_unorm
        );

        // Load the factory atlas texture (relative to source file)
        m_atlas_texture = gfx::texture::load_from_file(device, get_asset_path("factory_atlas.png"));
        if (!m_atlas_texture)
        {
            LUMINA_LOG_ERROR("Failed to load factory_atlas.png");
            return;
        }

        // Create texture atlas from the texture
        m_atlas = gfx::texture_atlas::create(m_atlas_texture);
        if (!m_atlas)
        {
            LUMINA_LOG_ERROR("Failed to create texture atlas");
            return;
        }

        // Get tile size from texture dimensions (16x16 grid)
        uint32_t tex_width = m_atlas_texture->get_width();
        uint32_t tex_height = m_atlas_texture->get_height();
        m_tile_size = glm::vec2(tex_width / 16.0f, tex_height / 16.0f);

        // Add a 16x16 grid of tiles to the atlas
        m_atlas->add_grid("tile_", 16, 16, m_tile_size);

        LUMINA_LOG_INFO("Atlas created: {}x{} texture, {:.0f}x{:.0f} tile size, {} regions",
            tex_width, tex_height, m_tile_size.x, m_tile_size.y, m_atlas->get_region_count());

        // Initialize camera (square aspect ratio)
        m_camera = gfx::camera2d(600.0f, 1.0f);
        m_camera.set_position({300.0f, 300.0f});
        m_camera.update(0.0f);  // Process dirty flags
    }

    void on_detach() override
    {
        m_atlas.reset();
        m_atlas_texture.reset();
        m_render_target.reset();
        m_renderer.reset();
    }

    void on_update(float dt) override
    {
        m_time += dt;

        // Camera controls
        float move_speed = 300.0f * dt;

        glm::vec2 move_dir{0.0f};
        if (input::is_key_pressed(input::key_code::w) || input::is_key_pressed(input::key_code::up))
            move_dir.y += 1.0f;
        if (input::is_key_pressed(input::key_code::s) || input::is_key_pressed(input::key_code::down))
            move_dir.y -= 1.0f;
        if (input::is_key_pressed(input::key_code::a) || input::is_key_pressed(input::key_code::left))
            move_dir.x -= 1.0f;
        if (input::is_key_pressed(input::key_code::d) || input::is_key_pressed(input::key_code::right))
            move_dir.x += 1.0f;

        if (glm::length(move_dir) > 0.0f)
        {
            move_dir = glm::normalize(move_dir);
            m_camera.move(move_dir * move_speed / m_camera.get_zoom());
        }

        // Zoom controls
        if (input::is_key_pressed(input::key_code::q))
            m_camera.set_zoom(m_camera.get_zoom() * (1.0f + dt));
        if (input::is_key_pressed(input::key_code::e))
            m_camera.set_zoom(m_camera.get_zoom() * (1.0f - dt));

        // Rotation controls
        if (input::is_key_pressed(input::key_code::z))
            m_camera.set_rotation(m_camera.get_rotation() + dt);
        if (input::is_key_pressed(input::key_code::x))
            m_camera.set_rotation(m_camera.get_rotation() - dt);

        // Camera shake (space key)
        if (input::is_key_pressed(input::key_code::space) && !m_camera.is_shaking())
        {
            m_camera.start_shake(10.0f, 0.3f);
        }

        // Update camera
        m_camera.update(dt);
    }

    void on_render() override
    {
        if (!m_renderer || !m_render_target || !m_atlas) return;

        // Begin rendering with the camera
        m_renderer->begin(m_camera);
        m_renderer->set_render_target(m_render_target);

        // Use point filtering for pixel art
        m_renderer->set_filter_mode(gfx::filter_mode::point);

        // Draw background
        m_renderer->draw_quad({
            .position = {400, 300, -0.1f},
            .size = {2000, 2000},
            .color = {0.15f, 0.15f, 0.2f, 1.0f},
            .layer = gfx::render_layer::background
        });

        // Draw a grid of tiles from the atlas
        float spacing = m_tile_size.x * 1.2f;
        int grid_cols = 8;
        int grid_rows = 4;
        float start_x = 400.0f - (grid_cols * spacing) / 2.0f + spacing / 2.0f;
        float start_y = 300.0f - (grid_rows * spacing) / 2.0f + spacing / 2.0f;

        int tile_index = 0;
        for (int row = 0; row < grid_rows; row++)
        {
            for (int col = 0; col < grid_cols; col++)
            {
                float x = start_x + col * spacing;
                float y = start_y + row * spacing;

                // Use the tile from atlas
                m_renderer->draw_sprite(*m_atlas, static_cast<uint32_t>(tile_index), {
                    .position = {x, y, 0.0f},
                    .size = m_tile_size,
                    .color = {1.0f, 1.0f, 1.0f, 1.0f},
                    .layer = gfx::render_layer::sprites
                });

                tile_index++;
            }
        }

        // Draw some animated sprites
        float anim_offset = std::sin(m_time * 2.0f) * 50.0f;

        // Sprite 1 - bouncing
        m_renderer->draw_sprite(*m_atlas, 0, {
            .position = {150.0f, 150.0f + anim_offset, 0.1f},
            .size = m_tile_size * 2.0f,
            .color = {1.0f, 0.8f, 0.8f, 1.0f},
            .rotation = m_time,
            .layer = gfx::render_layer::sprites
        });

        // Sprite 2 - flipped
        m_renderer->draw_sprite(*m_atlas, 1, {
            .position = {650.0f, 150.0f - anim_offset, 0.1f},
            .size = m_tile_size * 2.0f,
            .color = {0.8f, 1.0f, 0.8f, 1.0f},
            .flip_x = true,
            .layer = gfx::render_layer::sprites
        });

        // Sprite 3 - pulsing
        float pulse = 1.0f + std::sin(m_time * 4.0f) * 0.2f;
        m_renderer->draw_sprite(*m_atlas, 16, {
            .position = {300.0f, 500.0f, 0.1f},
            .size = m_tile_size * pulse * 1.5f,
            .color = {0.8f, 0.8f, 1.0f, 1.0f},
            .layer = gfx::render_layer::sprites
        });

        // Blend mode demonstrations
        float blend_y = 500.0f;

        // Additive blend (glow effect)
        m_renderer->draw_sprite(*m_atlas, 17, {
            .position = {400.0f, blend_y, 0.2f},
            .size = m_tile_size * 1.5f,
            .color = {1.0f, 0.8f, 0.2f, 0.8f},
            .layer = gfx::render_layer::effects,
            .blend = gfx::blend_mode::additive
        });

        // Multiply blend (shadow/darken)
        m_renderer->draw_sprite(*m_atlas, 18, {
            .position = {500.0f, blend_y, 0.2f},
            .size = m_tile_size * 1.5f,
            .color = {0.5f, 0.5f, 0.8f, 1.0f},
            .layer = gfx::render_layer::effects,
            .blend = gfx::blend_mode::multiply
        });

        m_renderer->end();

        // UI
        ui::begin_window("Atlas Demo");
        ui::text("Texture Atlas & Camera2D Demo");
        ui::separator();

        ui::text("Controls:");
        ui::text("  WASD/Arrows - Pan camera");
        ui::text("  Q/E - Zoom in/out");
        ui::text("  Z/X - Rotate camera");
        ui::text("  Space - Camera shake");
        ui::separator();

        auto pos = m_camera.get_position();
        ui::text_fmt("Camera Pos: ({:.0f}, {:.0f})", pos.x, pos.y);
        ui::text_fmt("Zoom: {:.2f}x", m_camera.get_zoom());
        ui::text_fmt("Rotation: {:.1f} deg", glm::degrees(m_camera.get_rotation()));
        ui::text_fmt("Shaking: {}", m_camera.is_shaking() ? "Yes" : "No");
        ui::separator();

        ui::text_fmt("Atlas Regions: {}", m_atlas->get_region_count());
        ui::text_fmt("Tile Size: {:.0f}x{:.0f}", m_tile_size.x, m_tile_size.y);
        ui::separator();

        ui::text("Blend Modes:");
        ui::text("  Yellow sprite - Additive (glow)");
        ui::text("  Blue sprite - Multiply (darken)");
        ui::separator();

        const auto& stats = m_renderer->get_stats();
        ui::text_fmt("Draw Calls: {}", stats.draw_calls);
        ui::text_fmt("Quads: {}", stats.quad_count);
        m_renderer->reset_stats();
        ui::end_window();

        // Viewport
        ui::begin_window("Viewport");
        auto tex = m_render_target->get_color_texture();
        if (tex)
        {
            ui::image(tex->get_texture(), ui::get_content_size());
        }
        ui::end_window();
    }

private:
    std::unique_ptr<gfx::renderer2d> m_renderer;
    lumina::ref<gfx::render_target> m_render_target;
    lumina::ref<gfx::texture> m_atlas_texture;
    lumina::ref<gfx::texture_atlas> m_atlas;
    gfx::camera2d m_camera;

    glm::vec2 m_tile_size{32.0f};
    float m_time = 0.0f;
};

lumina::core::application* lumina::core::create_application(int argc, char** argv)
{
    auto* app = new lumina::core::application();
    app->set_title("graphics/07-atlas");
    app->push_layer<atlas_layer>();
    return app;
}
