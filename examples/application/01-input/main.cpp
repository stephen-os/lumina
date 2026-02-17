// application/01-input: Input System Demo
// Demonstrates: Keyboard polling, mouse position, mouse buttons, visual feedback

#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/core/input.h>
#include <lumina/graphics/graphics.h>
#include <lumina/ui/ui.h>

#include <glm/glm.hpp>
#include <algorithm>

namespace ui = lumina::ui;
namespace gfx = lumina::graphics;
namespace input = lumina::core::input;

class input_layer : public lumina::core::layer
{
public:
    input_layer() : layer("input") {}

    void on_attach() override
    {
        gfx::renderer::init(512, 512);
    }

    void on_detach() override
    {
        gfx::renderer::shutdown();
    }

    void on_update(float dt) override
    {
        // Move player with WASD or arrow keys
        float speed = 200.0f * dt;
        glm::vec2 move_dir{0.0f};

        if (input::is_key_pressed(input::key_code::w) || input::is_key_pressed(input::key_code::up))
            move_dir.y -= 1.0f;
        if (input::is_key_pressed(input::key_code::s) || input::is_key_pressed(input::key_code::down))
            move_dir.y += 1.0f;
        if (input::is_key_pressed(input::key_code::a) || input::is_key_pressed(input::key_code::left))
            move_dir.x -= 1.0f;
        if (input::is_key_pressed(input::key_code::d) || input::is_key_pressed(input::key_code::right))
            move_dir.x += 1.0f;

        // Normalize diagonal movement
        if (glm::length(move_dir) > 0.0f)
        {
            move_dir = glm::normalize(move_dir);
            m_player_pos += move_dir * speed;
        }

        // Clamp to bounds
        m_player_pos.x = std::clamp(m_player_pos.x, 25.0f, 487.0f);
        m_player_pos.y = std::clamp(m_player_pos.y, 25.0f, 487.0f);

        // Track modifier keys
        m_shift_held = input::is_key_pressed(input::key_code::left_shift) ||
                       input::is_key_pressed(input::key_code::right_shift);
        m_ctrl_held = input::is_key_pressed(input::key_code::left_control) ||
                      input::is_key_pressed(input::key_code::right_control);
        m_alt_held = input::is_key_pressed(input::key_code::left_alt) ||
                     input::is_key_pressed(input::key_code::right_alt);

        // Track mouse buttons
        m_mouse_left = input::is_mouse_button_pressed(input::mouse_code::left);
        m_mouse_right = input::is_mouse_button_pressed(input::mouse_code::right);
        m_mouse_middle = input::is_mouse_button_pressed(input::mouse_code::middle);

        // Get mouse position
        auto mouse = input::get_mouse_position();
        m_mouse_pos = {mouse.x, mouse.y};
    }

    void on_render() override
    {
        gfx::renderer::begin();
        gfx::renderer::clear({0.12f, 0.12f, 0.15f, 1.0f});

        // Draw grid for reference
        for (int i = 0; i <= 8; i++)
        {
            float pos = i * 64.0f;
            gfx::renderer::draw_line({
                .start = {pos, 0, 0},
                .end = {pos, 512, 0},
                .color = {0.2f, 0.2f, 0.25f, 1.0f}
            });
            gfx::renderer::draw_line({
                .start = {0, pos, 0},
                .end = {512, pos, 0},
                .color = {0.2f, 0.2f, 0.25f, 1.0f}
            });
        }

        // Draw player square (changes color based on modifiers)
        glm::vec4 player_color = {0.2f, 0.6f, 1.0f, 1.0f};  // Blue
        if (m_shift_held) player_color = {1.0f, 0.6f, 0.2f, 1.0f};  // Orange
        if (m_ctrl_held) player_color = {0.2f, 1.0f, 0.6f, 1.0f};   // Green
        if (m_alt_held) player_color = {1.0f, 0.2f, 0.6f, 1.0f};    // Pink

        float player_size = 40.0f;
        if (m_mouse_left) player_size = 50.0f;  // Grow when clicking

        gfx::renderer::draw_quad({
            .position = {m_player_pos.x, m_player_pos.y, 0.1f},
            .size = {player_size, player_size},
            .color = player_color
        });

        // Draw mouse cursor indicator (in render target space)
        // This won't match exactly since mouse is in window coords, but shows the concept
        if (m_mouse_right)
        {
            gfx::renderer::draw_circle({
                .position = {m_player_pos.x, m_player_pos.y, 0.2f},
                .radius = {30, 30},
                .color = {1.0f, 1.0f, 0.0f, 0.5f},
                .thickness = 0.1f
            });
        }

        // Draw direction indicator based on arrow keys
        glm::vec2 indicator_dir{0.0f};
        if (input::is_key_pressed(input::key_code::up)) indicator_dir.y -= 1.0f;
        if (input::is_key_pressed(input::key_code::down)) indicator_dir.y += 1.0f;
        if (input::is_key_pressed(input::key_code::left)) indicator_dir.x -= 1.0f;
        if (input::is_key_pressed(input::key_code::right)) indicator_dir.x += 1.0f;

        if (glm::length(indicator_dir) > 0.0f)
        {
            indicator_dir = glm::normalize(indicator_dir) * 40.0f;
            gfx::renderer::draw_line({
                .start = {m_player_pos.x, m_player_pos.y, 0.3f},
                .end = {m_player_pos.x + indicator_dir.x, m_player_pos.y + indicator_dir.y, 0.3f},
                .color = {1.0f, 1.0f, 1.0f, 0.8f},
                .thickness = 3.0f
            });
        }

        gfx::renderer::end();

        // UI
        render_ui();
    }

    void render_ui()
    {
        ui::begin_window("Input Demo");
        ui::text("Input System Demo");
        ui::separator();

        ui::text("Keyboard:");
        ui::text("  WASD/Arrows - Move square");
        ui::text("  Shift - Orange color");
        ui::text("  Ctrl - Green color");
        ui::text("  Alt - Pink color");
        ui::separator();

        ui::text("Mouse:");
        ui::text("  Left click - Enlarge square");
        ui::text("  Right click - Show ring");
        ui::separator();

        ui::text("State:");
        ui::text_fmt("  Position: ({:.0f}, {:.0f})", m_player_pos.x, m_player_pos.y);
        ui::text_fmt("  Mouse: ({:.0f}, {:.0f})", m_mouse_pos.x, m_mouse_pos.y);
        ui::separator();

        ui::text("Modifiers:");
        ui::text_fmt("  Shift: {}", m_shift_held ? "HELD" : "-");
        ui::text_fmt("  Ctrl: {}", m_ctrl_held ? "HELD" : "-");
        ui::text_fmt("  Alt: {}", m_alt_held ? "HELD" : "-");
        ui::separator();

        ui::text("Mouse Buttons:");
        ui::text_fmt("  Left: {}", m_mouse_left ? "PRESSED" : "-");
        ui::text_fmt("  Right: {}", m_mouse_right ? "PRESSED" : "-");
        ui::text_fmt("  Middle: {}", m_mouse_middle ? "PRESSED" : "-");
        ui::separator();

        const auto& stats = gfx::renderer::get_stats();
        ui::text_fmt("Draw Calls: {}", stats.draw_calls);
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
    glm::vec2 m_player_pos{256.0f, 256.0f};
    glm::vec2 m_mouse_pos{0.0f, 0.0f};

    bool m_shift_held = false;
    bool m_ctrl_held = false;
    bool m_alt_held = false;

    bool m_mouse_left = false;
    bool m_mouse_right = false;
    bool m_mouse_middle = false;
};

lumina::core::application* lumina::core::create_application(int argc, char** argv)
{
    auto* app = new lumina::core::application();
    app->set_title("application/01-input");
    app->push_layer<input_layer>();
    return app;
}
