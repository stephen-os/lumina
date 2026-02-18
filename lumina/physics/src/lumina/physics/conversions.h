#pragma once

#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include <box2d/math_functions.h>

namespace lumina::physics
{
    // GLM vec2 <-> Box2D vec2

    [[nodiscard]] inline b2Vec2 to_b2(const glm::vec2& v)
    {
        return { v.x, v.y };
    }

    [[nodiscard]] inline glm::vec2 to_glm(b2Vec2 v)
    {
        return { v.x, v.y };
    }

    // Rotation conversions

    [[nodiscard]] inline b2Rot to_b2_rot(float radians)
    {
        return b2MakeRot(radians);
    }

    [[nodiscard]] inline float to_radians(b2Rot rot)
    {
        return b2Rot_GetAngle(rot);
    }

    // Transform conversions

    struct transform_2d
    {
        glm::vec2 position;
        float rotation; // radians
    };

    [[nodiscard]] inline transform_2d to_transform(b2Transform t)
    {
        return { to_glm(t.p), to_radians(t.q) };
    }

    [[nodiscard]] inline b2Transform to_b2_transform(const glm::vec2& position, float rotation)
    {
        return { to_b2(position), to_b2_rot(rotation) };
    }

    // Color conversions (for debug drawing)

    [[nodiscard]] inline glm::vec4 to_glm(b2HexColor hex, float alpha = 1.0f)
    {
        float r = ((hex >> 16) & 0xFF) / 255.0f;
        float g = ((hex >> 8) & 0xFF) / 255.0f;
        float b = (hex & 0xFF) / 255.0f;
        return { r, g, b, alpha };
    }
}
