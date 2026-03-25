#pragma once

#include <lumina/core/base.h>
#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include <vector>

namespace Lumina
{
    class Body;
    class Shape;

    // Contact point information
    struct ContactPoint
    {
        glm::vec2 Position;
        glm::vec2 Normal;
        float Separation;
        float NormalImpulse;
        float TangentImpulse;
    };

    // Full contact information between two shapes
    struct ContactInfo
    {
        ref<Shape> ShapeA;
        ref<Shape> ShapeB;
        ref<Body> BodyA;
        ref<Body> BodyB;
        std::vector<ContactPoint> Points;
        bool IsTouching;
    };

    // Contact event types
    enum class ContactEventType
    {
        Begin,  // Contact started
        End,    // Contact ended
        Hit     // High-speed collision
    };

    // Contact event (generated each physics step)
    struct ContactEvent
    {
        ContactEventType Type;
        b2ShapeId ShapeIdA;
        b2ShapeId ShapeIdB;
        glm::vec2 Point;
        glm::vec2 Normal;
        float ApproachSpeed;
    };

    // Sensor event types
    enum class SensorEventType
    {
        Begin,  // Shape entered sensor
        End     // Shape exited sensor
    };

    // Sensor event (generated each physics step)
    struct SensorEvent
    {
        SensorEventType Type;
        b2ShapeId SensorShapeId;
        b2ShapeId VisitorShapeId;
    };
}
