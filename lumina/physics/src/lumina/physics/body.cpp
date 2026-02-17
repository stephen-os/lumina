#include "body.h"
#include "shapes.h"
#include "conversions.h"

namespace lumina::physics
{
    body::body(b2BodyId id, const body_def& def)
        : m_body_id(id)
        , m_user_data(def.user_data)
    {
    }

    b2ShapeDef body::make_shape_def(const shape_def& def) const
    {
        b2ShapeDef sd = b2DefaultShapeDef();
        sd.friction = def.material.friction;
        sd.restitution = def.material.restitution;
        sd.density = def.material.density;
        sd.filter.categoryBits = def.filter.category_bits;
        sd.filter.maskBits = def.filter.mask_bits;
        sd.filter.groupIndex = def.filter.group_index;
        sd.isSensor = def.is_sensor;
        sd.enableContactEvents = def.enable_contact_events;
        sd.enableSensorEvents = def.enable_sensor_events;
        sd.enableHitEvents = def.enable_hit_events;
        return sd;
    }

    ref<shape> body::add_circle(float radius, const glm::vec2& offset)
    {
        return add_circle(radius, offset, shape_def{});
    }

    ref<shape> body::add_circle(float radius, const glm::vec2& offset, const shape_def& def)
    {
        b2Circle circle;
        circle.center = to_b2(offset);
        circle.radius = radius;

        b2ShapeDef sd = make_shape_def(def);
        b2ShapeId id = b2CreateCircleShape(m_body_id, &sd, &circle);

        auto s = ref<shape>(new shape(id, shared_from_this(), def));
        m_shapes.push_back(s);
        return s;
    }

    ref<shape> body::add_box(float half_width, float half_height, const glm::vec2& offset, float rotation)
    {
        return add_box(half_width, half_height, offset, rotation, shape_def{});
    }

    ref<shape> body::add_box(float half_width, float half_height, const glm::vec2& offset, float rotation, const shape_def& def)
    {
        b2Polygon polygon = b2MakeOffsetBox(half_width, half_height, to_b2(offset), to_b2_rot(rotation));

        b2ShapeDef sd = make_shape_def(def);
        b2ShapeId id = b2CreatePolygonShape(m_body_id, &sd, &polygon);

        auto s = ref<shape>(new shape(id, shared_from_this(), def));
        m_shapes.push_back(s);
        return s;
    }

    ref<shape> body::add_polygon(const std::vector<glm::vec2>& vertices)
    {
        return add_polygon(vertices, shape_def{});
    }

    ref<shape> body::add_polygon(const std::vector<glm::vec2>& vertices, const shape_def& def)
    {
        std::vector<b2Vec2> b2_vertices;
        b2_vertices.reserve(vertices.size());
        for (const auto& v : vertices)
        {
            b2_vertices.push_back(to_b2(v));
        }

        b2Hull hull = b2ComputeHull(b2_vertices.data(), static_cast<int>(b2_vertices.size()));
        b2Polygon polygon = b2MakePolygon(&hull, 0.0f);

        b2ShapeDef sd = make_shape_def(def);
        b2ShapeId id = b2CreatePolygonShape(m_body_id, &sd, &polygon);

        auto s = ref<shape>(new shape(id, shared_from_this(), def));
        m_shapes.push_back(s);
        return s;
    }

    ref<shape> body::add_capsule(const glm::vec2& center1, const glm::vec2& center2, float radius)
    {
        return add_capsule(center1, center2, radius, shape_def{});
    }

    ref<shape> body::add_capsule(const glm::vec2& center1, const glm::vec2& center2, float radius, const shape_def& def)
    {
        b2Capsule capsule;
        capsule.center1 = to_b2(center1);
        capsule.center2 = to_b2(center2);
        capsule.radius = radius;

        b2ShapeDef sd = make_shape_def(def);
        b2ShapeId id = b2CreateCapsuleShape(m_body_id, &sd, &capsule);

        auto s = ref<shape>(new shape(id, shared_from_this(), def));
        m_shapes.push_back(s);
        return s;
    }

    ref<shape> body::add_segment(const glm::vec2& point1, const glm::vec2& point2)
    {
        return add_segment(point1, point2, shape_def{});
    }

    ref<shape> body::add_segment(const glm::vec2& point1, const glm::vec2& point2, const shape_def& def)
    {
        b2Segment segment;
        segment.point1 = to_b2(point1);
        segment.point2 = to_b2(point2);

        b2ShapeDef sd = make_shape_def(def);
        b2ShapeId id = b2CreateSegmentShape(m_body_id, &sd, &segment);

        auto s = ref<shape>(new shape(id, shared_from_this(), def));
        m_shapes.push_back(s);
        return s;
    }

    void body::remove_shape(ref<shape>& s)
    {
        if (s && s->is_valid())
        {
            b2DestroyShape(s->get_native_id(), true);
        }
        m_shapes.erase(std::remove(m_shapes.begin(), m_shapes.end(), s), m_shapes.end());
        s.reset();
    }

    glm::vec2 body::get_position() const
    {
        return to_glm(b2Body_GetPosition(m_body_id));
    }

    float body::get_rotation() const
    {
        return to_radians(b2Body_GetRotation(m_body_id));
    }

    void body::set_transform(const glm::vec2& position, float rotation)
    {
        b2Body_SetTransform(m_body_id, to_b2(position), to_b2_rot(rotation));
    }

    void body::set_position(const glm::vec2& position)
    {
        set_transform(position, get_rotation());
    }

    void body::set_rotation(float rotation)
    {
        set_transform(get_position(), rotation);
    }

    glm::vec2 body::get_linear_velocity() const
    {
        return to_glm(b2Body_GetLinearVelocity(m_body_id));
    }

    float body::get_angular_velocity() const
    {
        return b2Body_GetAngularVelocity(m_body_id);
    }

    void body::set_linear_velocity(const glm::vec2& velocity)
    {
        b2Body_SetLinearVelocity(m_body_id, to_b2(velocity));
    }

    void body::set_angular_velocity(float velocity)
    {
        b2Body_SetAngularVelocity(m_body_id, velocity);
    }

    void body::apply_force(const glm::vec2& force, const glm::vec2& point, bool wake)
    {
        b2Body_ApplyForce(m_body_id, to_b2(force), to_b2(point), wake);
    }

    void body::apply_force_to_center(const glm::vec2& force, bool wake)
    {
        b2Body_ApplyForceToCenter(m_body_id, to_b2(force), wake);
    }

    void body::apply_torque(float torque, bool wake)
    {
        b2Body_ApplyTorque(m_body_id, torque, wake);
    }

    void body::apply_linear_impulse(const glm::vec2& impulse, const glm::vec2& point, bool wake)
    {
        b2Body_ApplyLinearImpulse(m_body_id, to_b2(impulse), to_b2(point), wake);
    }

    void body::apply_linear_impulse_to_center(const glm::vec2& impulse, bool wake)
    {
        b2Body_ApplyLinearImpulseToCenter(m_body_id, to_b2(impulse), wake);
    }

    void body::apply_angular_impulse(float impulse, bool wake)
    {
        b2Body_ApplyAngularImpulse(m_body_id, impulse, wake);
    }

    float body::get_mass() const
    {
        return b2Body_GetMass(m_body_id);
    }

    float body::get_inertia() const
    {
        return b2Body_GetRotationalInertia(m_body_id);
    }

    glm::vec2 body::get_center_of_mass() const
    {
        return to_glm(b2Body_GetWorldCenterOfMass(m_body_id));
    }

    void body::set_mass_data(float mass, const glm::vec2& center, float inertia)
    {
        b2MassData data;
        data.mass = mass;
        data.center = to_b2(center);
        data.rotationalInertia = inertia;
        b2Body_SetMassData(m_body_id, data);
    }

    void body::reset_mass_data()
    {
        b2Body_ApplyMassFromShapes(m_body_id);
    }

    body_type body::get_type() const
    {
        b2BodyType t = b2Body_GetType(m_body_id);
        switch (t)
        {
        case b2_staticBody:    return body_type::static_body;
        case b2_kinematicBody: return body_type::kinematic;
        case b2_dynamicBody:
        default:               return body_type::dynamic;
        }
    }

    void body::set_type(body_type type)
    {
        b2BodyType t = b2_dynamicBody;
        switch (type)
        {
        case body_type::static_body: t = b2_staticBody; break;
        case body_type::kinematic:   t = b2_kinematicBody; break;
        case body_type::dynamic:     t = b2_dynamicBody; break;
        }
        b2Body_SetType(m_body_id, t);
    }

    bool body::is_awake() const
    {
        return b2Body_IsAwake(m_body_id);
    }

    void body::set_awake(bool awake)
    {
        b2Body_SetAwake(m_body_id, awake);
    }

    bool body::is_enabled() const
    {
        return b2Body_IsEnabled(m_body_id);
    }

    void body::set_enabled(bool enabled)
    {
        if (enabled)
            b2Body_Enable(m_body_id);
        else
            b2Body_Disable(m_body_id);
    }

    bool body::is_bullet() const
    {
        return b2Body_IsBullet(m_body_id);
    }

    void body::set_bullet(bool bullet)
    {
        b2Body_SetBullet(m_body_id, bullet);
    }

    float body::get_gravity_scale() const
    {
        return b2Body_GetGravityScale(m_body_id);
    }

    void body::set_gravity_scale(float scale)
    {
        b2Body_SetGravityScale(m_body_id, scale);
    }

    float body::get_linear_damping() const
    {
        return b2Body_GetLinearDamping(m_body_id);
    }

    void body::set_linear_damping(float damping)
    {
        b2Body_SetLinearDamping(m_body_id, damping);
    }

    float body::get_angular_damping() const
    {
        return b2Body_GetAngularDamping(m_body_id);
    }

    void body::set_angular_damping(float damping)
    {
        b2Body_SetAngularDamping(m_body_id, damping);
    }

    glm::vec2 body::get_world_point(const glm::vec2& local_point) const
    {
        return to_glm(b2Body_GetWorldPoint(m_body_id, to_b2(local_point)));
    }

    glm::vec2 body::get_local_point(const glm::vec2& world_point) const
    {
        return to_glm(b2Body_GetLocalPoint(m_body_id, to_b2(world_point)));
    }

    glm::vec2 body::get_world_vector(const glm::vec2& local_vector) const
    {
        return to_glm(b2Body_GetWorldVector(m_body_id, to_b2(local_vector)));
    }

    glm::vec2 body::get_local_vector(const glm::vec2& world_vector) const
    {
        return to_glm(b2Body_GetLocalVector(m_body_id, to_b2(world_vector)));
    }
}
