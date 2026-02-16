#pragma once

#include <lumina/core/base.h>

#include <glm/glm.hpp>

#include <algorithm>
#include <limits>

namespace lumina::graphics
{
    /// Camera movement constraints.
    struct camera2d_bounds
    {
        float min_x = -std::numeric_limits<float>::max();
        float max_x = std::numeric_limits<float>::max();
        float min_y = -std::numeric_limits<float>::max();
        float max_y = std::numeric_limits<float>::max();

        [[nodiscard]] bool is_unbounded() const noexcept
        {
            return min_x == -std::numeric_limits<float>::max() &&
                   max_x == std::numeric_limits<float>::max() &&
                   min_y == -std::numeric_limits<float>::max() &&
                   max_y == std::numeric_limits<float>::max();
        }

        [[nodiscard]] glm::vec2 clamp(const glm::vec2& pos) const noexcept
        {
            return glm::vec2(
                std::clamp(pos.x, min_x, max_x),
                std::clamp(pos.y, min_y, max_y)
            );
        }
    };

    /// Camera shake configuration.
    struct camera2d_shake_params
    {
        float intensity = 1.0f;      // Maximum shake offset in world units
        float frequency = 20.0f;     // Oscillation frequency
        float decay = 5.0f;          // How fast shake diminishes
        float duration = 0.0f;       // Total shake duration (0 = infinite until stopped)
    };

    /// 2D orthographic camera with follow, shake, and bounds support.
    /// Call update() once per frame to apply follow/shake behavior.
    class camera2d
    {
    public:
        camera2d();
        explicit camera2d(float view_height, float aspect_ratio = 16.0f / 9.0f);
        ~camera2d() = default;

        /// Core transforms
        void set_position(const glm::vec2& pos);
        void set_position(float x, float y) { set_position({x, y}); }
        void move(const glm::vec2& delta);
        void move(float dx, float dy) { move({dx, dy}); }

        void set_zoom(float zoom);
        [[nodiscard]] float get_zoom() const noexcept { return m_zoom; }

        void set_rotation(float radians);
        [[nodiscard]] float get_rotation() const noexcept { return m_rotation; }

        void set_view_height(float height);
        [[nodiscard]] float get_view_height() const noexcept { return m_view_height; }

        void set_aspect_ratio(float ratio);
        [[nodiscard]] float get_aspect_ratio() const noexcept { return m_aspect_ratio; }

        void set_near_far(float near_plane, float far_plane);

        [[nodiscard]] glm::vec2 get_position() const noexcept { return m_position; }

        /// Follow/Lerp - camera smoothly follows a target position
        void set_target(const glm::vec2& target);
        void set_target(float x, float y) { set_target({x, y}); }
        void set_follow_speed(float speed);
        void set_follow_deadzone(float radius);
        void clear_target();
        [[nodiscard]] bool has_target() const noexcept { return m_has_target; }
        [[nodiscard]] glm::vec2 get_target() const noexcept { return m_target; }

        /// Shake - camera vibration effect
        void start_shake(const camera2d_shake_params& params);
        void start_shake(float intensity, float duration = 0.5f);
        void stop_shake();
        [[nodiscard]] bool is_shaking() const noexcept { return m_shake_remaining > 0.0f; }

        /// Bounds - constrain camera position
        void set_bounds(const camera2d_bounds& bounds);
        void set_bounds(float min_x, float min_y, float max_x, float max_y);
        void clear_bounds();
        [[nodiscard]] const camera2d_bounds& get_bounds() const noexcept { return m_bounds; }

        /// Update (call once per frame)
        void update(float delta_time);

        /// Matrix access
        [[nodiscard]] const glm::mat4& get_view_matrix() const noexcept { return m_view_matrix; }
        [[nodiscard]] const glm::mat4& get_projection_matrix() const noexcept { return m_projection_matrix; }
        [[nodiscard]] glm::mat4 get_view_projection_matrix() const;

        /// Coordinate conversion
        [[nodiscard]] glm::vec2 screen_to_world(const glm::vec2& screen_pos, const glm::vec2& screen_size) const;
        [[nodiscard]] glm::vec2 world_to_screen(const glm::vec2& world_pos, const glm::vec2& screen_size) const;

        /// Viewport info
        [[nodiscard]] float get_visible_width() const;
        [[nodiscard]] float get_visible_height() const;
        [[nodiscard]] glm::vec4 get_visible_bounds() const;

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
