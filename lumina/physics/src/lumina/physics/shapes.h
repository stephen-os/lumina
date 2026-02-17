#pragma once

#include <lumina/core/base.h>
#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include <any>
#include <vector>

namespace lumina::physics
{
    class body;

    // Collision filtering
    struct collision_filter
    {
        uint64_t category_bits = 1;
        uint64_t mask_bits = UINT64_MAX;
        int group_index = 0;
    };

    // Shape material properties
    struct shape_material
    {
        float friction = 0.6f;
        float restitution = 0.0f;
        float density = 1.0f;
    };

    // Base shape definition
    struct shape_def
    {
        shape_material material;
        collision_filter filter;
        bool is_sensor = false;
        bool enable_contact_events = false;
        bool enable_sensor_events = false;
        bool enable_hit_events = false;
        std::any user_data;
    };

    enum class shape_type
    {
        circle,
        polygon,
        capsule,
        segment
    };

    class shape : public std::enable_shared_from_this<shape>
    {
    public:
        // Shape properties
        [[nodiscard]] shape_type get_type() const;
        [[nodiscard]] ref<body> get_body() const;

        // Material
        [[nodiscard]] float get_friction() const;
        void set_friction(float friction);
        [[nodiscard]] float get_restitution() const;
        void set_restitution(float restitution);
        [[nodiscard]] float get_density() const;
        void set_density(float density);

        // Filtering
        [[nodiscard]] collision_filter get_filter() const;
        void set_filter(const collision_filter& filter);

        // Sensor (can only be set at creation time via shape_def::is_sensor)
        [[nodiscard]] bool is_sensor() const;

        // Events
        void enable_contact_events(bool enable);
        void enable_sensor_events(bool enable);
        void enable_hit_events(bool enable);

        // Geometry queries
        [[nodiscard]] bool test_point(const glm::vec2& point) const;
        [[nodiscard]] glm::vec2 get_centroid() const;

        // User data
        template<typename T>
        void set_user_data(T&& data) { m_user_data = std::forward<T>(data); }

        template<typename T>
        [[nodiscard]] T* get_user_data() { return std::any_cast<T>(&m_user_data); }

        template<typename T>
        [[nodiscard]] const T* get_user_data() const { return std::any_cast<T>(&m_user_data); }

        [[nodiscard]] bool has_user_data() const { return m_user_data.has_value(); }

        // Internal
        [[nodiscard]] b2ShapeId get_native_id() const { return m_shape_id; }
        [[nodiscard]] bool is_valid() const { return b2Shape_IsValid(m_shape_id); }

    private:
        friend class body;
        shape(b2ShapeId id, ref<body> parent, const shape_def& def);

        b2ShapeId m_shape_id = {};
        std::weak_ptr<body> m_body;
        std::any m_user_data;
    };

    // Shape definition helpers
    struct circle_def : shape_def
    {
        glm::vec2 center = { 0.0f, 0.0f };
        float radius = 0.5f;
    };

    struct box_def : shape_def
    {
        float half_width = 0.5f;
        float half_height = 0.5f;
        glm::vec2 center = { 0.0f, 0.0f };
        float rotation = 0.0f;
    };

    struct polygon_def : shape_def
    {
        std::vector<glm::vec2> vertices;
    };

    struct capsule_def : shape_def
    {
        glm::vec2 center1 = { 0.0f, -0.5f };
        glm::vec2 center2 = { 0.0f, 0.5f };
        float radius = 0.25f;
    };

    struct segment_def : shape_def
    {
        glm::vec2 point1 = { -0.5f, 0.0f };
        glm::vec2 point2 = { 0.5f, 0.0f };
    };
}
