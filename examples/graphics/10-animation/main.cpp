// graphics/10-animation: Sprite Animation Demo
// Demonstrates: Frame-based animation, animation states, procedural animation

#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/core/input.h>
#include <lumina/graphics/graphics.h>
#include <lumina/ui/ui.h>

#include <glm/glm.hpp>
#include <filesystem>
#include <vector>
#include <cmath>

namespace ui = lumina::ui;
namespace gfx = lumina::graphics;
namespace input = lumina::core::input;

// Simple animation class
class sprite_animation
{
public:
    sprite_animation() = default;

    void add_frame(uint32_t region_index)
    {
        m_frames.push_back(region_index);
    }

    void set_frame_duration(float duration)
    {
        m_frame_duration = duration;
    }

    void update(float dt)
    {
        if (m_frames.empty() || m_paused) return;

        m_time += dt * m_speed;
        while (m_time >= m_frame_duration)
        {
            m_time -= m_frame_duration;
            m_current_frame = (m_current_frame + 1) % m_frames.size();
        }
    }

    [[nodiscard]] uint32_t get_current_region() const
    {
        if (m_frames.empty()) return 0;
        return m_frames[m_current_frame];
    }

    [[nodiscard]] size_t get_frame_count() const { return m_frames.size(); }
    [[nodiscard]] size_t get_current_frame_index() const { return m_current_frame; }

    void set_speed(float speed) { m_speed = speed; }
    [[nodiscard]] float get_speed() const { return m_speed; }

    void set_paused(bool paused) { m_paused = paused; }
    [[nodiscard]] bool is_paused() const { return m_paused; }

    void reset()
    {
        m_current_frame = 0;
        m_time = 0.0f;
    }

private:
    std::vector<uint32_t> m_frames;
    float m_frame_duration = 0.1f;
    float m_time = 0.0f;
    float m_speed = 1.0f;
    size_t m_current_frame = 0;
    bool m_paused = false;
};

// Get asset path relative to source file
static std::string get_asset_path(const std::string& filename)
{
    std::filesystem::path source_dir = std::filesystem::path(__FILE__).parent_path();
    // Use the atlas from 07-atlas example
    return (source_dir / ".." / "07-atlas" / filename).string();
}

class animation_layer : public lumina::core::layer
{
public:
    animation_layer() : layer("animation") {}

    void on_attach() override
    {
        gfx::renderer::init(600, 400);

        auto& device = lumina::core::application::get().get_device();

        // Load atlas texture
        m_atlas_texture = gfx::texture::load_from_file(device, get_asset_path("factory_atlas.png"));
        if (!m_atlas_texture)
        {
            LUMINA_LOG_ERROR("Failed to load atlas texture");
            return;
        }

        // Create atlas with grid
        m_atlas = gfx::texture_atlas::create(m_atlas_texture);
        uint32_t tex_width = m_atlas_texture->get_width();
        uint32_t tex_height = m_atlas_texture->get_height();
        m_tile_size = glm::vec2(tex_width / 16.0f, tex_height / 16.0f);
        m_atlas->add_grid("tile_", 16, 16, m_tile_size);

        // Create animations using atlas tiles as frames
        // Animation 1: Cycle through first row (frames 0-7)
        m_anim_row.set_frame_duration(0.15f);
        for (int i = 0; i < 8; i++)
            m_anim_row.add_frame(i);

        // Animation 2: Cycle through first column (frames 0, 16, 32, 48)
        m_anim_col.set_frame_duration(0.2f);
        for (int i = 0; i < 4; i++)
            m_anim_col.add_frame(i * 16);

        // Animation 3: Diagonal pattern
        m_anim_diag.set_frame_duration(0.12f);
        for (int i = 0; i < 8; i++)
            m_anim_diag.add_frame(i * 17);  // 0, 17, 34, 51...

        LUMINA_LOG_INFO("Animation demo loaded");
    }

    void on_detach() override
    {
        m_atlas.reset();
        m_atlas_texture.reset();
        gfx::renderer::shutdown();
    }

    void on_update(float dt) override
    {
        m_time += dt;

        // Update animations
        m_anim_row.update(dt);
        m_anim_col.update(dt);
        m_anim_diag.update(dt);

        // Speed controls
        if (input::is_key_pressed(input::key_code::up))
            m_animation_speed = std::min(m_animation_speed + dt * 2.0f, 5.0f);
        if (input::is_key_pressed(input::key_code::down))
            m_animation_speed = std::max(m_animation_speed - dt * 2.0f, 0.1f);

        m_anim_row.set_speed(m_animation_speed);
        m_anim_col.set_speed(m_animation_speed);
        m_anim_diag.set_speed(m_animation_speed);

        // Pause toggle
        if (input::is_key_pressed(input::key_code::space))
        {
            if (!m_space_held)
            {
                m_paused = !m_paused;
                m_anim_row.set_paused(m_paused);
                m_anim_col.set_paused(m_paused);
                m_anim_diag.set_paused(m_paused);
            }
            m_space_held = true;
        }
        else
        {
            m_space_held = false;
        }
    }

    void on_render() override
    {
        if (!m_atlas) return;

        gfx::renderer::begin();
        gfx::renderer::clear({0.12f, 0.12f, 0.15f, 1.0f});
        gfx::renderer::set_filter_mode(gfx::filter_mode::point);

        float y_offset = 80.0f;
        float sprite_scale = 2.0f;
        glm::vec2 display_size = m_tile_size * sprite_scale;

        // Row animation
        draw_animation_demo("Row Animation", {100.0f, y_offset}, m_anim_row, display_size);

        // Column animation
        draw_animation_demo("Column Animation", {300.0f, y_offset}, m_anim_col, display_size);

        // Diagonal animation
        draw_animation_demo("Diagonal Animation", {500.0f, y_offset}, m_anim_diag, display_size);

        // Procedural animation section
        float proc_y = 220.0f;

        // Bouncing circle
        float bounce = std::abs(std::sin(m_time * 3.0f)) * 30.0f;
        gfx::renderer::draw_circle({
            .position = {100.0f, proc_y + 50.0f - bounce, 0},
            .radius = {20, 20},
            .color = {0.2f, 0.8f, 0.4f, 1.0f}
        });
        gfx::renderer::draw_text({
            .text = "Bounce",
            .position = {100.0f, proc_y + 90.0f, 0},
            .scale = 0.8f,
            .color = {0.7f, 0.7f, 0.7f, 1.0f},
            .alignment = gfx::text_alignment::center
        });

        // Pulsing square
        float pulse = 1.0f + std::sin(m_time * 4.0f) * 0.3f;
        gfx::renderer::draw_quad({
            .position = {250.0f, proc_y + 50.0f, 0},
            .size = {30.0f * pulse, 30.0f * pulse},
            .color = {0.8f, 0.4f, 0.2f, 1.0f}
        });
        gfx::renderer::draw_text({
            .text = "Pulse",
            .position = {250.0f, proc_y + 90.0f, 0},
            .scale = 0.8f,
            .color = {0.7f, 0.7f, 0.7f, 1.0f},
            .alignment = gfx::text_alignment::center
        });

        // Rotating square
        gfx::renderer::draw_quad({
            .position = {400.0f, proc_y + 50.0f, 0},
            .size = {35.0f, 35.0f},
            .color = {0.4f, 0.4f, 0.9f, 1.0f},
            .rotation = m_time * 2.0f
        });
        gfx::renderer::draw_text({
            .text = "Rotate",
            .position = {400.0f, proc_y + 90.0f, 0},
            .scale = 0.8f,
            .color = {0.7f, 0.7f, 0.7f, 1.0f},
            .alignment = gfx::text_alignment::center
        });

        // Color cycling
        float hue = std::fmod(m_time * 0.5f, 1.0f);
        glm::vec4 rainbow = hsv_to_rgb(hue, 0.8f, 1.0f);
        gfx::renderer::draw_quad({
            .position = {550.0f, proc_y + 50.0f, 0},
            .size = {35.0f, 35.0f},
            .color = rainbow
        });
        gfx::renderer::draw_text({
            .text = "Color",
            .position = {550.0f, proc_y + 90.0f, 0},
            .scale = 0.8f,
            .color = {0.7f, 0.7f, 0.7f, 1.0f},
            .alignment = gfx::text_alignment::center
        });

        // Section labels
        gfx::renderer::draw_text({
            .text = "Frame-based Animation (Atlas Tiles)",
            .position = {300.0f, 20.0f, 0},
            .scale = 1.2f,
            .color = {1.0f, 0.9f, 0.5f, 1.0f},
            .alignment = gfx::text_alignment::center
        });

        gfx::renderer::draw_text({
            .text = "Procedural Animation",
            .position = {300.0f, proc_y - 20.0f, 0},
            .scale = 1.2f,
            .color = {1.0f, 0.9f, 0.5f, 1.0f},
            .alignment = gfx::text_alignment::center
        });

        // Controls hint
        gfx::renderer::draw_text({
            .text = "Up/Down: Speed | Space: Pause",
            .position = {300.0f, 380.0f, 0},
            .scale = 0.9f,
            .color = {0.5f, 0.5f, 0.5f, 1.0f},
            .alignment = gfx::text_alignment::center
        });

        gfx::renderer::end();

        // UI
        render_ui();
    }

    void draw_animation_demo(const char* label, glm::vec2 pos, const sprite_animation& anim, glm::vec2 size)
    {
        // Draw current frame
        gfx::renderer::draw_sprite(*m_atlas, anim.get_current_region(), {
            .position = {pos.x, pos.y, 0},
            .size = size,
            .color = {1.0f, 1.0f, 1.0f, 1.0f}
        });

        // Draw label
        gfx::renderer::draw_text({
            .text = label,
            .position = {pos.x, pos.y + size.y / 2.0f + 20.0f, 0},
            .scale = 0.8f,
            .color = {0.7f, 0.7f, 0.7f, 1.0f},
            .alignment = gfx::text_alignment::center
        });

        // Draw frame indicator
        std::string frame_text = std::to_string(anim.get_current_frame_index() + 1) + "/" +
                                  std::to_string(anim.get_frame_count());
        gfx::renderer::draw_text({
            .text = frame_text.c_str(),
            .position = {pos.x, pos.y + size.y / 2.0f + 40.0f, 0},
            .scale = 0.7f,
            .color = {0.5f, 0.5f, 0.5f, 1.0f},
            .alignment = gfx::text_alignment::center
        });
    }

    void render_ui()
    {
        ui::begin_window("Animation Demo");
        ui::text("Sprite Animation Demo");
        ui::separator();

        ui::text_fmt("Speed: {:.1f}x", m_animation_speed);
        ui::text_fmt("Paused: {}", m_paused ? "Yes" : "No");
        ui::separator();

        ui::text("Controls:");
        ui::text("  Up/Down - Adjust speed");
        ui::text("  Space - Pause/Resume");
        ui::separator();

        const auto& stats = gfx::renderer::get_stats();
        ui::text_fmt("Draw Calls: {}", stats.draw_calls);
        ui::text_fmt("Quads: {}", stats.quad_count);
        gfx::renderer::reset_stats();

        ui::end_window();

        // Viewport
        ui::begin_window("Viewport");
        auto tex = gfx::renderer::get_texture();
        if (tex)
        {
            ui::image(tex->get_texture(), ui::get_content_size());
        }
        ui::end_window();
    }

private:
    glm::vec4 hsv_to_rgb(float h, float s, float v)
    {
        float c = v * s;
        float x = c * (1 - std::abs(std::fmod(h * 6.0f, 2.0f) - 1));
        float m = v - c;
        float r, g, b;
        if (h < 1.0f/6.0f) { r = c; g = x; b = 0; }
        else if (h < 2.0f/6.0f) { r = x; g = c; b = 0; }
        else if (h < 3.0f/6.0f) { r = 0; g = c; b = x; }
        else if (h < 4.0f/6.0f) { r = 0; g = x; b = c; }
        else if (h < 5.0f/6.0f) { r = x; g = 0; b = c; }
        else { r = c; g = 0; b = x; }
        return {r + m, g + m, b + m, 1.0f};
    }

    lumina::ref<gfx::texture> m_atlas_texture;
    lumina::ref<gfx::texture_atlas> m_atlas;
    glm::vec2 m_tile_size{32.0f};

    sprite_animation m_anim_row;
    sprite_animation m_anim_col;
    sprite_animation m_anim_diag;

    float m_time = 0.0f;
    float m_animation_speed = 1.0f;
    bool m_paused = false;
    bool m_space_held = false;
};

lumina::core::application* lumina::core::create_application(int argc, char** argv)
{
    auto* app = new lumina::core::application();
    app->set_title("graphics/10-animation");
    app->push_layer<animation_layer>();
    return app;
}
