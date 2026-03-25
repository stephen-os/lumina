#pragma once

#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include <box2d/math_functions.h>

namespace Lumina
{
    // GLM vec2 <-> Box2D vec2

    [[nodiscard]] inline b2Vec2 ToB2(const glm::vec2& v)
    {
        return { v.x, v.y };
    }

    [[nodiscard]] inline glm::vec2 ToGlm(b2Vec2 v)
    {
        return { v.x, v.y };
    }

    // Rotation conversions

    [[nodiscard]] inline b2Rot ToB2Rot(float radians)
    {
        return b2MakeRot(radians);
    }

    [[nodiscard]] inline float ToRadians(b2Rot rot)
    {
        return b2Rot_GetAngle(rot);
    }

    // Transform conversions

    struct Transform2D
    {
        glm::vec2 Position;
        float Rotation; // radians
    };

    [[nodiscard]] inline Transform2D ToTransform(b2Transform t)
    {
        return { ToGlm(t.p), ToRadians(t.q) };
    }

    [[nodiscard]] inline b2Transform ToB2Transform(const glm::vec2& position, float rotation)
    {
        return { ToB2(position), ToB2Rot(rotation) };
    }

    // Color conversions (for debug drawing)

    [[nodiscard]] inline glm::vec4 ToGlm(b2HexColor hex, float alpha = 1.0f)
    {
        float r = ((hex >> 16) & 0xFF) / 255.0f;
        float g = ((hex >> 8) & 0xFF) / 255.0f;
        float b = (hex & 0xFF) / 255.0f;
        return { r, g, b, alpha };
    }
}
