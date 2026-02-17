#pragma once

#include <lumina/core/base.h>
#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include <vector>

namespace lumina::physics
{
    class body;
    class shape;

    // Contact point information
    struct contact_point
    {
        glm::vec2 position;
        glm::vec2 normal;
        float separation;
        float normal_impulse;
        float tangent_impulse;
    };

    // Full contact information between two shapes
    struct contact_info
    {
        ref<shape> shape_a;
        ref<shape> shape_b;
        ref<body> body_a;
        ref<body> body_b;
        std::vector<contact_point> points;
        bool is_touching;
    };

    // Contact event types
    enum class contact_event_type
    {
        begin,  // Contact started
        end,    // Contact ended
        hit     // High-speed collision
    };

    // Contact event (generated each physics step)
    struct contact_event
    {
        contact_event_type type;
        b2ShapeId shape_id_a;
        b2ShapeId shape_id_b;
        glm::vec2 point;
        glm::vec2 normal;
        float approach_speed;
    };

    // Sensor event types
    enum class sensor_event_type
    {
        begin,  // Shape entered sensor
        end     // Shape exited sensor
    };

    // Sensor event (generated each physics step)
    struct sensor_event
    {
        sensor_event_type type;
        b2ShapeId sensor_shape_id;
        b2ShapeId visitor_shape_id;
    };
}
