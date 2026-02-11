#pragma once

#include <lumina/core/base.h>

#include <glm/glm.hpp>

#include <algorithm>
#include <limits>

namespace lumina::graphics
{
    struct camera2d_bounds
    {
        float min_x = -std::numeric_limits<float>::max();
        float max_x = std::numeric_limits<float>::max();
        float min_y = -std::numeric_limits<float>::max();
        float max_y = std::numeric_limits<float>::max();

        bool is_unbounded() const
        {
            return min_x == -std::numeric_limits<float>::max() &&
                   max_x == std::numeric_limits<float>::max() &&
                   min_y == -std::numeric_limits<float>::max() &&
                   max_y == std::numeric_limits<float>::max();
        }

        glm::vec2 clamp(const glm::vec2& pos) const
        {
            return glm::vec2(
                std::clamp(pos.x, min_x, max_x),
                std::clamp(pos.y, min_y, max_y)
            );
        }
    };

    struct camera2d_shake_params
    {
        float intensity = 1.0f;      // Maximum shake offset in world units
        float frequency = 20.0f;     // Oscillation frequency
        float decay = 5.0f;          // How fast shake diminishes
        float duration = 0.0f;       // Total shake duration (0 = infinite until stopped)
    };

    class camera2d
    {
    public:
        camera2d();
        explicit camera2d(float view_height, float aspect_ratio = 16.0f / 9.0f);
        ~camera2d() = default;

        // === Core transforms ===
        void set_position(const glm::vec2& pos);
        void set_position(float x, float y) { set_position({x, y}); }
        void move(const glm::vec2& delta);
        void move(float dx, float dy) { move({dx, dy}); }

        void set_zoom(float zoom);
        float get_zoom() const { return m_zoom; }

        void set_rotation(float radians);
        float get_rotation() const { return m_rotation; }

        void set_view_height(float height);
        float get_view_height() const { return m_view_height; }

        void set_aspect_ratio(float ratio);
        float get_aspect_ratio() const { return m_aspect_ratio; }

        void set_near_far(float near_plane, float far_plane);

        glm::vec2 get_position() const { return m_position; }

        // === Follow/Lerp ===
        void set_target(const glm::vec2& target);
        void set_target(float x, float y) { set_target({x, y}); }
        void set_follow_speed(float speed);  // Higher = faster follow
        void set_follow_deadzone(float radius);  // Don't move if target within deadzone
        void clear_target();
        bool has_target() const { return m_has_target; }
        glm::vec2 get_target() const { return m_target; }

        // === Shake ===
        void start_shake(const camera2d_shake_params& params);
        void start_shake(float intensity, float duration = 0.5f);
        void stop_shake();
        bool is_shaking() const { return m_shake_remaining > 0.0f; }

        // === Bounds ===
        void set_bounds(const camera2d_bounds& bounds);
        void set_bounds(float min_x, float min_y, float max_x, float max_y);
        void clear_bounds();
        const camera2d_bounds& get_bounds() const { return m_bounds; }

        // === Update (call once per frame) ===
        void update(float delta_time);

        // === Matrix access ===
        const glm::mat4& get_view_matrix() const { return m_view_matrix; }
        const glm::mat4& get_projection_matrix() const { return m_projection_matrix; }
        glm::mat4 get_view_projection_matrix() const;

        // === Coordinate conversion ===
        glm::vec2 screen_to_world(const glm::vec2& screen_pos, const glm::vec2& screen_size) const;
        glm::vec2 world_to_screen(const glm::vec2& world_pos, const glm::vec2& screen_size) const;

        // === Viewport info ===
        float get_visible_width() const;
        float get_visible_height() const;
        glm::vec4 get_visible_bounds() const;  // x, y (center), width, height

    private:
        void update_view_matrix();
        void update_projection_matrix();
        void apply_shake(float delta_time);
        void apply_follow(float delta_time);
        void apply_bounds();
        glm::vec2 calculate_shake_offset() const;

        // Core state
        glm::vec2 m_position{0.0f};
        float m_zoom = 1.0f;
        float m_rotation = 0.0f;
        float m_view_height = 10.0f;
        float m_aspect_ratio = 16.0f / 9.0f;
        float m_near_plane = -1000.0f;
        float m_far_plane = 1000.0f;

        // Follow state
        bool m_has_target = false;
        glm::vec2 m_target{0.0f};
        float m_follow_speed = 5.0f;
        float m_follow_deadzone = 0.0f;

        // Shake state
        camera2d_shake_params m_shake_params{};
        float m_shake_remaining = 0.0f;
        float m_shake_time = 0.0f;

        // Bounds
        camera2d_bounds m_bounds{};

        // Cached matrices
        glm::mat4 m_view_matrix{1.0f};
        glm::mat4 m_projection_matrix{1.0f};
        bool m_view_dirty = true;
        bool m_projection_dirty = true;
    };
}
