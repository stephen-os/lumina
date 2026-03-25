#include "Shapes.h"
#include "Body.h"
#include "Conversions.h"

namespace Lumina
{
    Shape::Shape(b2ShapeId id, Ref<Body> parent, const ShapeDef& def)
        : m_ShapeId(id)
        , m_Body(parent)
        , m_UserData(def.UserData)
    {
    }

    ShapeType Shape::GetType() const
    {
        b2ShapeType type = b2Shape_GetType(m_ShapeId);
        switch (type)
        {
        case b2_circleShape:  return ShapeType::Circle;
        case b2_capsuleShape: return ShapeType::Capsule;
        case b2_segmentShape: return ShapeType::Segment;
        case b2_polygonShape:
        default:              return ShapeType::Polygon;
        }
    }

    Ref<Body> Shape::GetBody() const
    {
        return m_Body.lock();
    }

    float Shape::GetFriction() const
    {
        return b2Shape_GetFriction(m_ShapeId);
    }

    void Shape::SetFriction(float friction)
    {
        b2Shape_SetFriction(m_ShapeId, friction);
    }

    float Shape::GetRestitution() const
    {
        return b2Shape_GetRestitution(m_ShapeId);
    }

    void Shape::SetRestitution(float restitution)
    {
        b2Shape_SetRestitution(m_ShapeId, restitution);
    }

    float Shape::GetDensity() const
    {
        return b2Shape_GetDensity(m_ShapeId);
    }

    void Shape::SetDensity(float density)
    {
        b2Shape_SetDensity(m_ShapeId, density, true);
    }

    CollisionFilter Shape::GetFilter() const
    {
        b2Filter f = b2Shape_GetFilter(m_ShapeId);
        return { f.categoryBits, f.maskBits, f.groupIndex };
    }

    void Shape::SetFilter(const CollisionFilter& filter)
    {
        b2Filter f;
        f.categoryBits = filter.CategoryBits;
        f.maskBits = filter.MaskBits;
        f.groupIndex = filter.GroupIndex;
        b2Shape_SetFilter(m_ShapeId, f);
    }

    bool Shape::IsSensor() const
    {
        return b2Shape_IsSensor(m_ShapeId);
    }

    void Shape::EnableContactEvents(bool enable)
    {
        b2Shape_EnableContactEvents(m_ShapeId, enable);
    }

    void Shape::EnableSensorEvents(bool enable)
    {
        b2Shape_EnableSensorEvents(m_ShapeId, enable);
    }

    void Shape::EnableHitEvents(bool enable)
    {
        b2Shape_EnableHitEvents(m_ShapeId, enable);
    }

    bool Shape::TestPoint(const glm::vec2& point) const
    {
        return b2Shape_TestPoint(m_ShapeId, ToB2(point));
    }

    glm::vec2 Shape::GetCentroid() const
    {
        b2ShapeType type = b2Shape_GetType(m_ShapeId);
        switch (type)
        {
        case b2_circleShape:
        {
            b2Circle circle = b2Shape_GetCircle(m_ShapeId);
            return ToGlm(circle.center);
        }
        case b2_capsuleShape:
        {
            b2Capsule capsule = b2Shape_GetCapsule(m_ShapeId);
            return ToGlm(b2Lerp(capsule.center1, capsule.center2, 0.5f));
        }
        case b2_polygonShape:
        {
            b2Polygon polygon = b2Shape_GetPolygon(m_ShapeId);
            return ToGlm(polygon.centroid);
        }
        case b2_segmentShape:
        {
            b2Segment segment = b2Shape_GetSegment(m_ShapeId);
            return ToGlm(b2Lerp(segment.point1, segment.point2, 0.5f));
        }
        default:
            return { 0.0f, 0.0f };
        }
    }
}
