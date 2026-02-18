#include "debug_draw.h"
#include "world.h"
#include "body.h"
#include "shapes.h"
#include "conversions.h"

namespace lumina::physics
{
    namespace
    {
        glm::vec4 get_body_color(body_type type, bool is_awake)
        {
            if (!is_awake)
                return { 0.5f, 0.5f, 0.5f, 0.8f };

            switch (type)
            {
            case body_type::static_body: return { 0.5f, 0.9f, 0.5f, 0.8f };
            case body_type::kinematic:   return { 0.5f, 0.5f, 0.9f, 0.8f };
            case body_type::dynamic:
            default:                     return { 0.9f, 0.7f, 0.7f, 0.8f };
            }
        }

        void draw_shape(debug_draw_interface& drawer, const shape& s, const body& b, const glm::vec4& color)
        {
            b2Transform xf = b2Body_GetTransform(b.get_native_id());

            switch (s.get_type())
            {
            case shape_type::circle:
            {
                b2Circle circle = b2Shape_GetCircle(s.get_native_id());
                b2Vec2 center = b2TransformPoint(xf, circle.center);
                b2Vec2 axis = b2RotateVector(xf.q, { 1, 0 });
                drawer.draw_solid_circle(to_glm(center), circle.radius, to_glm(axis), color);
                break;
            }
            case shape_type::polygon:
            {
                b2Polygon polygon = b2Shape_GetPolygon(s.get_native_id());
                std::vector<glm::vec2> vertices;
                vertices.reserve(polygon.count);
                for (int i = 0; i < polygon.count; ++i)
                {
                    b2Vec2 v = b2TransformPoint(xf, polygon.vertices[i]);
                    vertices.push_back(to_glm(v));
                }
                drawer.draw_solid_polygon(vertices.data(), static_cast<int>(vertices.size()), color);
                break;
            }
            case shape_type::capsule:
            {
                b2Capsule capsule = b2Shape_GetCapsule(s.get_native_id());
                b2Vec2 p1 = b2TransformPoint(xf, capsule.center1);
                b2Vec2 p2 = b2TransformPoint(xf, capsule.center2);
                drawer.draw_solid_capsule(to_glm(p1), to_glm(p2), capsule.radius, color);
                break;
            }
            case shape_type::segment:
            {
                b2Segment segment = b2Shape_GetSegment(s.get_native_id());
                b2Vec2 p1 = b2TransformPoint(xf, segment.point1);
                b2Vec2 p2 = b2TransformPoint(xf, segment.point2);
                drawer.draw_segment(to_glm(p1), to_glm(p2), color);
                break;
            }
            }
        }
    }

    void debug_draw_world(const world& w, debug_draw_interface& drawer, const debug_draw_flags& flags)
    {
        if (flags.draw_shapes)
        {
            for (const auto& b : w.get_bodies())
            {
                if (!b || !b->is_valid())
                    continue;

                glm::vec4 color = get_body_color(b->get_type(), b->is_awake());

                for (const auto& s : b->get_shapes())
                {
                    if (!s || !s->is_valid())
                        continue;

                    if (s->is_sensor())
                    {
                        glm::vec4 sensor_color = { 0.9f, 0.9f, 0.2f, 0.5f };
                        draw_shape(drawer, *s, *b, sensor_color);
                    }
                    else
                    {
                        draw_shape(drawer, *s, *b, color);
                    }
                }
            }
        }

        if (flags.draw_aabbs)
        {
            for (const auto& b : w.get_bodies())
            {
                if (!b || !b->is_valid())
                    continue;

                for (const auto& s : b->get_shapes())
                {
                    if (!s || !s->is_valid())
                        continue;

                    b2AABB aabb = b2Shape_GetAABB(s->get_native_id());
                    glm::vec2 vertices[4] = {
                        { aabb.lowerBound.x, aabb.lowerBound.y },
                        { aabb.upperBound.x, aabb.lowerBound.y },
                        { aabb.upperBound.x, aabb.upperBound.y },
                        { aabb.lowerBound.x, aabb.upperBound.y }
                    };
                    drawer.draw_polygon(vertices, 4, { 0.9f, 0.3f, 0.9f, 0.5f });
                }
            }
        }

        if (flags.draw_mass)
        {
            for (const auto& b : w.get_bodies())
            {
                if (!b || !b->is_valid())
                    continue;

                if (b->get_type() != body_type::dynamic)
                    continue;

                glm::vec2 com = b->get_center_of_mass();
                drawer.draw_point(com, 5.0f, { 1.0f, 0.0f, 0.0f, 1.0f });

                char mass_text[32];
                snprintf(mass_text, sizeof(mass_text), "%.2f kg", b->get_mass());
                drawer.draw_string(com, mass_text, { 1.0f, 1.0f, 1.0f, 1.0f });
            }
        }
    }
}
