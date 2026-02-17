#pragma once

#include <lumina/core/base.h>
#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include <vector>

#include "events.h"

namespace lumina::physics
{
    class body;
    struct body_def;

    // World configuration
    struct world_def
    {
        glm::vec2 gravity = { 0.0f, -9.81f };
        float time_step = 1.0f / 60.0f;
        int sub_step_count = 4;
        bool enable_sleep = true;
        bool enable_continuous = true;
    };

    // Raycast result
    struct ray_cast_result
    {
        b2ShapeId shape_id;
        glm::vec2 point;
        glm::vec2 normal;
        float fraction;
        bool hit = false;

        explicit operator bool() const { return hit; }
    };

    // Physics world - owns the simulation
    class world
    {
    public:
        explicit world(const world_def& def = {});
        ~world();

        // Non-copyable, movable
        world(const world&) = delete;
        world& operator=(const world&) = delete;
        world(world&& other) noexcept;
        world& operator=(world&& other) noexcept;

        // Simulation control
        void step();
        void step(float time_step);
        void step(float time_step, int sub_steps);

        // World properties
        void set_gravity(const glm::vec2& gravity);
        [[nodiscard]] glm::vec2 get_gravity() const;

        // Body creation/destruction
        ref<body> create_body(const body_def& def);
        void destroy_body(ref<body>& b);

        // Event access (call after step())
        [[nodiscard]] const std::vector<contact_event>& get_contact_events() const { return m_contact_events; }
        [[nodiscard]] const std::vector<sensor_event>& get_sensor_events() const { return m_sensor_events; }

        // Queries
        [[nodiscard]] ray_cast_result ray_cast_closest(glm::vec2 origin, glm::vec2 direction, float max_distance) const;

        // Internal access
        [[nodiscard]] b2WorldId get_native_id() const { return m_world_id; }
        [[nodiscard]] bool is_valid() const { return b2World_IsValid(m_world_id); }

        // Get all bodies
        [[nodiscard]] const std::vector<ref<body>>& get_bodies() const { return m_bodies; }

    private:
        void process_events();

        b2WorldId m_world_id = {};
        world_def m_config;

        std::vector<ref<body>> m_bodies;
        std::vector<contact_event> m_contact_events;
        std::vector<sensor_event> m_sensor_events;
    };
}
