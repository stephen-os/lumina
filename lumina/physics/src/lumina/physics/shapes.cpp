#include "shapes.h"
#include "body.h"
#include "conversions.h"

namespace lumina::physics
{
    shape::shape(b2ShapeId id, ref<body> parent, const shape_def& def)
        : m_shape_id(id)
        , m_body(parent)
        , m_user_data(def.user_data)
    {
    }

    shape_type shape::get_type() const
    {
        b2ShapeType type = b2Shape_GetType(m_shape_id);
        switch (type)
        {
        case b2_circleShape:  return shape_type::circle;
        case b2_capsuleShape: return shape_type::capsule;
        case b2_segmentShape: return shape_type::segment;
        case b2_polygonShape:
        default:              return shape_type::polygon;
        }
    }

    ref<body> shape::get_body() const
    {
        return m_body.lock();
    }

    float shape::get_friction() const
    {
        return b2Shape_GetFriction(m_shape_id);
    }

    void shape::set_friction(float friction)
    {
        b2Shape_SetFriction(m_shape_id, friction);
    }

    float shape::get_restitution() const
    {
        return b2Shape_GetRestitution(m_shape_id);
    }

    void shape::set_restitution(float restitution)
    {
        b2Shape_SetRestitution(m_shape_id, restitution);
    }

    float shape::get_density() const
    {
        return b2Shape_GetDensity(m_shape_id);
    }

    void shape::set_density(float density)
    {
        b2Shape_SetDensity(m_shape_id, density, true);
    }

    collision_filter shape::get_filter() const
    {
        b2Filter f = b2Shape_GetFilter(m_shape_id);
        return { f.categoryBits, f.maskBits, f.groupIndex };
    }

    void shape::set_filter(const collision_filter& filter)
    {
        b2Filter f;
        f.categoryBits = filter.category_bits;
        f.maskBits = filter.mask_bits;
        f.groupIndex = filter.group_index;
        b2Shape_SetFilter(m_shape_id, f);
    }

    bool shape::is_sensor() const
    {
        return b2Shape_IsSensor(m_shape_id);
    }

    void shape::set_sensor(bool sensor)
    {
        b2Shape_SetSensor(m_shape_id, sensor);
    }

    void shape::enable_contact_events(bool enable)
    {
        b2Shape_EnableContactEvents(m_shape_id, enable);
    }

    void shape::enable_sensor_events(bool enable)
    {
        b2Shape_EnableSensorEvents(m_shape_id, enable);
    }

    void shape::enable_hit_events(bool enable)
    {
        b2Shape_EnableHitEvents(m_shape_id, enable);
    }

    bool shape::test_point(const glm::vec2& point) const
    {
        return b2Shape_TestPoint(m_shape_id, to_b2(point));
    }

    glm::vec2 shape::get_centroid() const
    {
        b2ShapeType type = b2Shape_GetType(m_shape_id);
        switch (type)
        {
        case b2_circleShape:
        {
            b2Circle circle = b2Shape_GetCircle(m_shape_id);
            return to_glm(circle.center);
        }
        case b2_capsuleShape:
        {
            b2Capsule capsule = b2Shape_GetCapsule(m_shape_id);
            return to_glm(b2Lerp(capsule.center1, capsule.center2, 0.5f));
        }
        case b2_polygonShape:
        {
            b2Polygon polygon = b2Shape_GetPolygon(m_shape_id);
            return to_glm(polygon.centroid);
        }
        case b2_segmentShape:
        {
            b2Segment segment = b2Shape_GetSegment(m_shape_id);
            return to_glm(b2Lerp(segment.point1, segment.point2, 0.5f));
        }
        default:
            return { 0.0f, 0.0f };
        }
    }
}
