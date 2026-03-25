#include "DebugDraw.h"
#include "World.h"
#include "Body.h"
#include "Shapes.h"
#include "Conversions.h"

namespace Lumina
{
    namespace
    {
        glm::vec4 GetBodyColor(BodyType type, bool isAwake)
        {
            if (!isAwake)
                return { 0.5f, 0.5f, 0.5f, 0.8f };

            switch (type)
            {
            case BodyType::StaticBody: return { 0.5f, 0.9f, 0.5f, 0.8f };
            case BodyType::Kinematic:   return { 0.5f, 0.5f, 0.9f, 0.8f };
            case BodyType::Dynamic:
            default:                     return { 0.9f, 0.7f, 0.7f, 0.8f };
            }
        }

        void DrawShape(DebugDrawInterface& drawer, const Shape& s, const Body& b, const glm::vec4& color)
        {
            b2Transform xf = b2Body_GetTransform(b.GetNativeId());

            switch (s.GetType())
            {
            case ShapeType::Circle:
            {
                b2Circle circle = b2Shape_GetCircle(s.GetNativeId());
                b2Vec2 center = b2TransformPoint(xf, circle.center);
                b2Vec2 axis = b2RotateVector(xf.q, { 1, 0 });
                drawer.DrawSolidCircle(ToGlm(center), circle.radius, ToGlm(axis), color);
                break;
            }
            case ShapeType::Polygon:
            {
                b2Polygon polygon = b2Shape_GetPolygon(s.GetNativeId());
                std::vector<glm::vec2> vertices;
                vertices.reserve(polygon.count);
                for (int i = 0; i < polygon.count; ++i)
                {
                    b2Vec2 v = b2TransformPoint(xf, polygon.vertices[i]);
                    vertices.push_back(ToGlm(v));
                }
                drawer.DrawSolidPolygon(vertices.data(), static_cast<int>(vertices.size()), color);
                break;
            }
            case ShapeType::Capsule:
            {
                b2Capsule capsule = b2Shape_GetCapsule(s.GetNativeId());
                b2Vec2 p1 = b2TransformPoint(xf, capsule.center1);
                b2Vec2 p2 = b2TransformPoint(xf, capsule.center2);
                drawer.DrawSolidCapsule(ToGlm(p1), ToGlm(p2), capsule.radius, color);
                break;
            }
            case ShapeType::Segment:
            {
                b2Segment segment = b2Shape_GetSegment(s.GetNativeId());
                b2Vec2 p1 = b2TransformPoint(xf, segment.point1);
                b2Vec2 p2 = b2TransformPoint(xf, segment.point2);
                drawer.DrawSegment(ToGlm(p1), ToGlm(p2), color);
                break;
            }
            }
        }
    }

    void DebugDrawWorld(const World& w, DebugDrawInterface& drawer, const DebugDrawFlags& flags)
    {
        if (flags.DrawShapes)
        {
            for (const auto& b : w.GetBodies())
            {
                if (!b || !b->IsValid())
                    continue;

                glm::vec4 color = GetBodyColor(b->GetType(), b->IsAwake());

                for (const auto& s : b->GetShapes())
                {
                    if (!s || !s->IsValid())
                        continue;

                    if (s->IsSensor())
                    {
                        glm::vec4 sensorColor = { 0.9f, 0.9f, 0.2f, 0.5f };
                        DrawShape(drawer, *s, *b, sensorColor);
                    }
                    else
                    {
                        DrawShape(drawer, *s, *b, color);
                    }
                }
            }
        }

        if (flags.DrawAABBs)
        {
            for (const auto& b : w.GetBodies())
            {
                if (!b || !b->IsValid())
                    continue;

                for (const auto& s : b->GetShapes())
                {
                    if (!s || !s->IsValid())
                        continue;

                    b2AABB aabb = b2Shape_GetAABB(s->GetNativeId());
                    glm::vec2 vertices[4] = {
                        { aabb.lowerBound.x, aabb.lowerBound.y },
                        { aabb.upperBound.x, aabb.lowerBound.y },
                        { aabb.upperBound.x, aabb.upperBound.y },
                        { aabb.lowerBound.x, aabb.upperBound.y }
                    };
                    drawer.DrawPolygon(vertices, 4, { 0.9f, 0.3f, 0.9f, 0.5f });
                }
            }
        }

        if (flags.DrawMass)
        {
            for (const auto& b : w.GetBodies())
            {
                if (!b || !b->IsValid())
                    continue;

                if (b->GetType() != BodyType::Dynamic)
                    continue;

                glm::vec2 com = b->GetCenterOfMass();
                drawer.DrawPoint(com, 5.0f, { 1.0f, 0.0f, 0.0f, 1.0f });

                char massText[32];
                snprintf(massText, sizeof(massText), "%.2f kg", b->GetMass());
                drawer.DrawString(com, massText, { 1.0f, 1.0f, 1.0f, 1.0f });
            }
        }
    }
}
