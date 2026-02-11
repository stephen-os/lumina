#include "camera2d.h"

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <algorithm>

namespace lumina::graphics
{
    camera2d::camera2d()
    {
        update_projection_matrix();
        update_view_matrix();
    }

    camera2d::camera2d(float view_height, float aspect_ratio)
        : m_view_height(view_height)
        , m_aspect_ratio(aspect_ratio)
    {
        update_projection_matrix();
        update_view_matrix();
    }

    void camera2d::set_position(const glm::vec2& pos)
    {
        m_position = pos;
        apply_bounds();
        m_view_dirty = true;
    }

    void camera2d::move(const glm::vec2& delta)
    {
        m_position += delta;
        apply_bounds();
        m_view_dirty = true;
    }

    void camera2d::set_zoom(float zoom)
    {
        m_zoom = std::max(0.001f, zoom);
        m_projection_dirty = true;
    }

    void camera2d::set_rotation(float radians)
    {
        m_rotation = radians;
        m_view_dirty = true;
    }

    void camera2d::set_view_height(float height)
    {
        m_view_height = std::max(0.001f, height);
        m_projection_dirty = true;
    }

    void camera2d::set_aspect_ratio(float ratio)
    {
        m_aspect_ratio = std::max(0.001f, ratio);
        m_projection_dirty = true;
    }

    void camera2d::set_near_far(float near_plane, float far_plane)
    {
        m_near_plane = near_plane;
        m_far_plane = far_plane;
        m_projection_dirty = true;
    }

    void camera2d::set_target(const glm::vec2& target)
    {
        m_target = target;
        m_has_target = true;
    }

    void camera2d::set_follow_speed(float speed)
    {
        m_follow_speed = std::max(0.0f, speed);
    }

    void camera2d::set_follow_deadzone(float radius)
    {
        m_follow_deadzone = std::max(0.0f, radius);
    }

    void camera2d::clear_target()
    {
        m_has_target = false;
    }

    void camera2d::start_shake(const camera2d_shake_params& params)
    {
        m_shake_params = params;
        m_shake_remaining = params.duration > 0.0f ? params.duration : std::numeric_limits<float>::max();
        m_shake_time = 0.0f;
    }

    void camera2d::start_shake(float intensity, float duration)
    {
        camera2d_shake_params params;
        params.intensity = intensity;
        params.duration = duration;
        params.frequency = 20.0f;
        params.decay = 5.0f;
        start_shake(params);
    }

    void camera2d::stop_shake()
    {
        m_shake_remaining = 0.0f;
        m_shake_time = 0.0f;
        m_view_dirty = true;
    }

    void camera2d::set_bounds(const camera2d_bounds& bounds)
    {
        m_bounds = bounds;
        apply_bounds();
        m_view_dirty = true;
    }

    void camera2d::set_bounds(float min_x, float min_y, float max_x, float max_y)
    {
        m_bounds.min_x = min_x;
        m_bounds.min_y = min_y;
        m_bounds.max_x = max_x;
        m_bounds.max_y = max_y;
        apply_bounds();
        m_view_dirty = true;
    }

    void camera2d::clear_bounds()
    {
        m_bounds = camera2d_bounds{};
    }

    void camera2d::update(float delta_time)
    {
        apply_follow(delta_time);
        apply_shake(delta_time);
        apply_bounds();

        if (m_view_dirty)
        {
            update_view_matrix();
            m_view_dirty = false;
        }
        if (m_projection_dirty)
        {
            update_projection_matrix();
            m_projection_dirty = false;
        }
    }

    glm::mat4 camera2d::get_view_projection_matrix() const
    {
        return m_projection_matrix * m_view_matrix;
    }

    glm::vec2 camera2d::screen_to_world(const glm::vec2& screen_pos, const glm::vec2& screen_size) const
    {
        // Convert screen position to NDC (-1 to 1)
        glm::vec2 ndc;
        ndc.x = (screen_pos.x / screen_size.x) * 2.0f - 1.0f;
        ndc.y = 1.0f - (screen_pos.y / screen_size.y) * 2.0f;  // Flip Y

        // Transform by inverse view-projection
        glm::mat4 inv_vp = glm::inverse(get_view_projection_matrix());
        glm::vec4 world_pos = inv_vp * glm::vec4(ndc.x, ndc.y, 0.0f, 1.0f);

        return glm::vec2(world_pos.x, world_pos.y);
    }

    glm::vec2 camera2d::world_to_screen(const glm::vec2& world_pos, const glm::vec2& screen_size) const
    {
        // Transform to clip space
        glm::vec4 clip_pos = get_view_projection_matrix() * glm::vec4(world_pos.x, world_pos.y, 0.0f, 1.0f);

        // NDC
        glm::vec2 ndc = glm::vec2(clip_pos.x, clip_pos.y) / clip_pos.w;

        // Screen coordinates
        glm::vec2 screen;
        screen.x = (ndc.x + 1.0f) * 0.5f * screen_size.x;
        screen.y = (1.0f - ndc.y) * 0.5f * screen_size.y;  // Flip Y

        return screen;
    }

    float camera2d::get_visible_width() const
    {
        return (m_view_height / m_zoom) * m_aspect_ratio;
    }

    float camera2d::get_visible_height() const
    {
        return m_view_height / m_zoom;
    }

    glm::vec4 camera2d::get_visible_bounds() const
    {
        float width = get_visible_width();
        float height = get_visible_height();
        return glm::vec4(m_position.x, m_position.y, width, height);
    }

    void camera2d::update_view_matrix()
    {
        glm::vec2 final_pos = m_position + calculate_shake_offset();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f),
            glm::vec3(final_pos.x, final_pos.y, 0.0f));

        if (std::abs(m_rotation) > 0.0001f)
        {
            transform = glm::rotate(transform, m_rotation, glm::vec3(0.0f, 0.0f, 1.0f));
        }

        m_view_matrix = glm::inverse(transform);
    }

    void camera2d::update_projection_matrix()
    {
        float height = m_view_height / m_zoom;
        float width = height * m_aspect_ratio;

        m_projection_matrix = glm::ortho(
            -width * 0.5f, width * 0.5f,
            -height * 0.5f, height * 0.5f,
            m_near_plane, m_far_plane
        );
    }

    void camera2d::apply_shake(float delta_time)
    {
        if (m_shake_remaining <= 0.0f)
            return;

        m_shake_time += delta_time;
        m_shake_remaining -= delta_time;

        if (m_shake_remaining <= 0.0f)
        {
            m_shake_remaining = 0.0f;
            m_shake_time = 0.0f;
        }

        m_view_dirty = true;
    }

    void camera2d::apply_follow(float delta_time)
    {
        if (!m_has_target)
            return;

        glm::vec2 diff = m_target - m_position;
        float distance = glm::length(diff);

        // Check deadzone
        if (distance <= m_follow_deadzone)
            return;

        // Smooth interpolation toward target
        float t = 1.0f - std::exp(-m_follow_speed * delta_time);
        m_position = glm::mix(m_position, m_target, t);

        m_view_dirty = true;
    }

    void camera2d::apply_bounds()
    {
        if (m_bounds.is_unbounded())
            return;

        m_position = m_bounds.clamp(m_position);
    }

    glm::vec2 camera2d::calculate_shake_offset() const
    {
        if (m_shake_remaining <= 0.0f)
            return glm::vec2(0.0f);

        // Calculate decay factor
        float decay_factor = 1.0f;
        if (m_shake_params.duration > 0.0f)
        {
            float elapsed = m_shake_params.duration - m_shake_remaining;
            decay_factor = std::exp(-m_shake_params.decay * elapsed);
        }

        // Use sin/cos with different frequencies for x/y to avoid circular patterns
        float time = m_shake_time * m_shake_params.frequency;
        float offset_x = std::sin(time * 1.0f) * std::cos(time * 0.7f);
        float offset_y = std::sin(time * 1.3f) * std::cos(time * 0.9f);

        return glm::vec2(offset_x, offset_y) * m_shake_params.intensity * decay_factor;
    }
}
