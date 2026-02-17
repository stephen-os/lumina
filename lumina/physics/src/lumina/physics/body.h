#pragma once

#include <lumina/core/base.h>
#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include <vector>
#include <any>

namespace lumina::physics
{
    class world;
    class shape;
    struct shape_def;

    enum class body_type
    {
        static_body,
        kinematic,
        dynamic
    };

    struct body_def
    {
        body_type type = body_type::dynamic;
        glm::vec2 position = { 0.0f, 0.0f };
        float rotation = 0.0f;
        glm::vec2 linear_velocity = { 0.0f, 0.0f };
        float angular_velocity = 0.0f;
        float linear_damping = 0.0f;
        float angular_damping = 0.0f;
        float gravity_scale = 1.0f;
        bool is_bullet = false;
        bool is_awake = true;
        bool enable_sleep = true;
        bool is_enabled = true;
        std::any user_data;
    };

    class body : public std::enable_shared_from_this<body>
    {
    public:
        // Shape attachment
        ref<shape> add_circle(float radius, const glm::vec2& offset = {});
        ref<shape> add_circle(float radius, const glm::vec2& offset, const shape_def& def);
        ref<shape> add_box(float half_width, float half_height, const glm::vec2& offset = {}, float rotation = 0.0f);
        ref<shape> add_box(float half_width, float half_height, const glm::vec2& offset, float rotation, const shape_def& def);
        ref<shape> add_polygon(const std::vector<glm::vec2>& vertices);
        ref<shape> add_polygon(const std::vector<glm::vec2>& vertices, const shape_def& def);
        ref<shape> add_capsule(const glm::vec2& center1, const glm::vec2& center2, float radius);
        ref<shape> add_capsule(const glm::vec2& center1, const glm::vec2& center2, float radius, const shape_def& def);
        ref<shape> add_segment(const glm::vec2& point1, const glm::vec2& point2);
        ref<shape> add_segment(const glm::vec2& point1, const glm::vec2& point2, const shape_def& def);

        void remove_shape(ref<shape>& s);
        [[nodiscard]] const std::vector<ref<shape>>& get_shapes() const { return m_shapes; }

        // Transform
        [[nodiscard]] glm::vec2 get_position() const;
        [[nodiscard]] float get_rotation() const;
        void set_transform(const glm::vec2& position, float rotation);
        void set_position(const glm::vec2& position);
        void set_rotation(float rotation);

        // Velocity
        [[nodiscard]] glm::vec2 get_linear_velocity() const;
        [[nodiscard]] float get_angular_velocity() const;
        void set_linear_velocity(const glm::vec2& velocity);
        void set_angular_velocity(float velocity);

        // Forces and impulses
        void apply_force(const glm::vec2& force, const glm::vec2& point, bool wake = true);
        void apply_force_to_center(const glm::vec2& force, bool wake = true);
        void apply_torque(float torque, bool wake = true);
        void apply_linear_impulse(const glm::vec2& impulse, const glm::vec2& point, bool wake = true);
        void apply_linear_impulse_to_center(const glm::vec2& impulse, bool wake = true);
        void apply_angular_impulse(float impulse, bool wake = true);

        // Mass properties
        [[nodiscard]] float get_mass() const;
        [[nodiscard]] float get_inertia() const;
        [[nodiscard]] glm::vec2 get_center_of_mass() const;
        void set_mass_data(float mass, const glm::vec2& center, float inertia);
        void reset_mass_data();

        // Body properties
        [[nodiscard]] body_type get_type() const;
        void set_type(body_type type);
        [[nodiscard]] bool is_awake() const;
        void set_awake(bool awake);
        [[nodiscard]] bool is_enabled() const;
        void set_enabled(bool enabled);
        [[nodiscard]] bool is_bullet() const;
        void set_bullet(bool bullet);
        [[nodiscard]] float get_gravity_scale() const;
        void set_gravity_scale(float scale);
        [[nodiscard]] float get_linear_damping() const;
        void set_linear_damping(float damping);
        [[nodiscard]] float get_angular_damping() const;
        void set_angular_damping(float damping);

        // User data
        template<typename T>
        void set_user_data(T&& data) { m_user_data = std::forward<T>(data); }

        template<typename T>
        [[nodiscard]] T* get_user_data() { return std::any_cast<T>(&m_user_data); }

        template<typename T>
        [[nodiscard]] const T* get_user_data() const { return std::any_cast<T>(&m_user_data); }

        [[nodiscard]] bool has_user_data() const { return m_user_data.has_value(); }

        // World coordinate conversion
        [[nodiscard]] glm::vec2 get_world_point(const glm::vec2& local_point) const;
        [[nodiscard]] glm::vec2 get_local_point(const glm::vec2& world_point) const;
        [[nodiscard]] glm::vec2 get_world_vector(const glm::vec2& local_vector) const;
        [[nodiscard]] glm::vec2 get_local_vector(const glm::vec2& world_vector) const;

        // Internal
        [[nodiscard]] b2BodyId get_native_id() const { return m_body_id; }
        [[nodiscard]] bool is_valid() const { return b2Body_IsValid(m_body_id); }

    private:
        friend class world;
        body(b2BodyId id, const body_def& def);

        b2ShapeDef make_shape_def(const shape_def& def) const;

        b2BodyId m_body_id = {};
        std::vector<ref<shape>> m_shapes;
        std::any m_user_data;
    };
}
