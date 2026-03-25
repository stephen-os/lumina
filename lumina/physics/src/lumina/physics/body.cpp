#include "body.h"
#include "shapes.h"
#include "conversions.h"

namespace Lumina
{
    Body::Body(b2BodyId id, const BodyDef& def)
        : m_BodyId(id)
        , m_UserData(def.UserData)
    {
    }

    b2ShapeDef Body::MakeShapeDef(const ShapeDef& def) const
    {
        b2ShapeDef sd = b2DefaultShapeDef();
        sd.material.friction = def.Material.Friction;
        sd.material.restitution = def.Material.Restitution;
        sd.density = def.Material.Density;
        sd.filter.categoryBits = def.Filter.CategoryBits;
        sd.filter.maskBits = def.Filter.MaskBits;
        sd.filter.groupIndex = def.Filter.GroupIndex;
        sd.isSensor = def.IsSensor;
        sd.enableContactEvents = def.EnableContactEvents;
        sd.enableSensorEvents = def.EnableSensorEvents;
        sd.enableHitEvents = def.EnableHitEvents;
        return sd;
    }

    Ref<Shape> Body::AddCircle(float radius, const glm::vec2& offset)
    {
        return AddCircle(radius, offset, ShapeDef{});
    }

    Ref<Shape> Body::AddCircle(float radius, const glm::vec2& offset, const ShapeDef& def)
    {
        b2Circle circle;
        circle.center = ToB2(offset);
        circle.radius = radius;

        b2ShapeDef sd = MakeShapeDef(def);
        b2ShapeId id = b2CreateCircleShape(m_BodyId, &sd, &circle);

        auto s = Ref<Shape>(new Shape(id, shared_from_this(), def));
        m_Shapes.push_back(s);
        return s;
    }

    Ref<Shape> Body::AddBox(float halfWidth, float halfHeight, const glm::vec2& offset, float rotation)
    {
        return AddBox(halfWidth, halfHeight, offset, rotation, ShapeDef{});
    }

    Ref<Shape> Body::AddBox(float halfWidth, float halfHeight, const glm::vec2& offset, float rotation, const ShapeDef& def)
    {
        b2Polygon polygon = b2MakeOffsetBox(halfWidth, halfHeight, ToB2(offset), ToB2Rot(rotation));

        b2ShapeDef sd = MakeShapeDef(def);
        b2ShapeId id = b2CreatePolygonShape(m_BodyId, &sd, &polygon);

        auto s = Ref<Shape>(new Shape(id, shared_from_this(), def));
        m_Shapes.push_back(s);
        return s;
    }

    Ref<Shape> Body::AddPolygon(const std::vector<glm::vec2>& vertices)
    {
        return AddPolygon(vertices, ShapeDef{});
    }

    Ref<Shape> Body::AddPolygon(const std::vector<glm::vec2>& vertices, const ShapeDef& def)
    {
        std::vector<b2Vec2> b2Vertices;
        b2Vertices.reserve(vertices.size());
        for (const auto& v : vertices)
        {
            b2Vertices.push_back(ToB2(v));
        }

        b2Hull hull = b2ComputeHull(b2Vertices.data(), static_cast<int>(b2Vertices.size()));
        b2Polygon polygon = b2MakePolygon(&hull, 0.0f);

        b2ShapeDef sd = MakeShapeDef(def);
        b2ShapeId id = b2CreatePolygonShape(m_BodyId, &sd, &polygon);

        auto s = Ref<Shape>(new Shape(id, shared_from_this(), def));
        m_Shapes.push_back(s);
        return s;
    }

    Ref<Shape> Body::AddCapsule(const glm::vec2& center1, const glm::vec2& center2, float radius)
    {
        return AddCapsule(center1, center2, radius, ShapeDef{});
    }

    Ref<Shape> Body::AddCapsule(const glm::vec2& center1, const glm::vec2& center2, float radius, const ShapeDef& def)
    {
        b2Capsule capsule;
        capsule.center1 = ToB2(center1);
        capsule.center2 = ToB2(center2);
        capsule.radius = radius;

        b2ShapeDef sd = MakeShapeDef(def);
        b2ShapeId id = b2CreateCapsuleShape(m_BodyId, &sd, &capsule);

        auto s = Ref<Shape>(new Shape(id, shared_from_this(), def));
        m_Shapes.push_back(s);
        return s;
    }

    Ref<Shape> Body::AddSegment(const glm::vec2& point1, const glm::vec2& point2)
    {
        return AddSegment(point1, point2, ShapeDef{});
    }

    Ref<Shape> Body::AddSegment(const glm::vec2& point1, const glm::vec2& point2, const ShapeDef& def)
    {
        b2Segment segment;
        segment.point1 = ToB2(point1);
        segment.point2 = ToB2(point2);

        b2ShapeDef sd = MakeShapeDef(def);
        b2ShapeId id = b2CreateSegmentShape(m_BodyId, &sd, &segment);

        auto s = Ref<Shape>(new Shape(id, shared_from_this(), def));
        m_Shapes.push_back(s);
        return s;
    }

    void Body::RemoveShape(Ref<Shape>& s)
    {
        if (s && s->IsValid())
        {
            b2DestroyShape(s->GetNativeId(), true);
        }
        m_Shapes.erase(std::remove(m_Shapes.begin(), m_Shapes.end(), s), m_Shapes.end());
        s.reset();
    }

    glm::vec2 Body::GetPosition() const
    {
        return ToGlm(b2Body_GetPosition(m_BodyId));
    }

    float Body::GetRotation() const
    {
        return ToRadians(b2Body_GetRotation(m_BodyId));
    }

    void Body::SetTransform(const glm::vec2& position, float rotation)
    {
        b2Body_SetTransform(m_BodyId, ToB2(position), ToB2Rot(rotation));
    }

    void Body::SetPosition(const glm::vec2& position)
    {
        SetTransform(position, GetRotation());
    }

    void Body::SetRotation(float rotation)
    {
        SetTransform(GetPosition(), rotation);
    }

    glm::vec2 Body::GetLinearVelocity() const
    {
        return ToGlm(b2Body_GetLinearVelocity(m_BodyId));
    }

    float Body::GetAngularVelocity() const
    {
        return b2Body_GetAngularVelocity(m_BodyId);
    }

    void Body::SetLinearVelocity(const glm::vec2& velocity)
    {
        b2Body_SetLinearVelocity(m_BodyId, ToB2(velocity));
    }

    void Body::SetAngularVelocity(float velocity)
    {
        b2Body_SetAngularVelocity(m_BodyId, velocity);
    }

    void Body::ApplyForce(const glm::vec2& force, const glm::vec2& point, bool wake)
    {
        b2Body_ApplyForce(m_BodyId, ToB2(force), ToB2(point), wake);
    }

    void Body::ApplyForceToCenter(const glm::vec2& force, bool wake)
    {
        b2Body_ApplyForceToCenter(m_BodyId, ToB2(force), wake);
    }

    void Body::ApplyTorque(float torque, bool wake)
    {
        b2Body_ApplyTorque(m_BodyId, torque, wake);
    }

    void Body::ApplyLinearImpulse(const glm::vec2& impulse, const glm::vec2& point, bool wake)
    {
        b2Body_ApplyLinearImpulse(m_BodyId, ToB2(impulse), ToB2(point), wake);
    }

    void Body::ApplyLinearImpulseToCenter(const glm::vec2& impulse, bool wake)
    {
        b2Body_ApplyLinearImpulseToCenter(m_BodyId, ToB2(impulse), wake);
    }

    void Body::ApplyAngularImpulse(float impulse, bool wake)
    {
        b2Body_ApplyAngularImpulse(m_BodyId, impulse, wake);
    }

    float Body::GetMass() const
    {
        return b2Body_GetMass(m_BodyId);
    }

    float Body::GetInertia() const
    {
        return b2Body_GetRotationalInertia(m_BodyId);
    }

    glm::vec2 Body::GetCenterOfMass() const
    {
        return ToGlm(b2Body_GetWorldCenterOfMass(m_BodyId));
    }

    void Body::SetMassData(float mass, const glm::vec2& center, float inertia)
    {
        b2MassData data;
        data.mass = mass;
        data.center = ToB2(center);
        data.rotationalInertia = inertia;
        b2Body_SetMassData(m_BodyId, data);
    }

    void Body::ResetMassData()
    {
        b2Body_ApplyMassFromShapes(m_BodyId);
    }

    BodyType Body::GetType() const
    {
        b2BodyType t = b2Body_GetType(m_BodyId);
        switch (t)
        {
        case b2_staticBody:    return BodyType::StaticBody;
        case b2_kinematicBody: return BodyType::Kinematic;
        case b2_dynamicBody:
        default:               return BodyType::Dynamic;
        }
    }

    void Body::SetType(BodyType type)
    {
        b2BodyType t = b2_dynamicBody;
        switch (type)
        {
        case BodyType::StaticBody: t = b2_staticBody; break;
        case BodyType::Kinematic:   t = b2_kinematicBody; break;
        case BodyType::Dynamic:     t = b2_dynamicBody; break;
        }
        b2Body_SetType(m_BodyId, t);
    }

    bool Body::IsAwake() const
    {
        return b2Body_IsAwake(m_BodyId);
    }

    void Body::SetAwake(bool awake)
    {
        b2Body_SetAwake(m_BodyId, awake);
    }

    bool Body::IsEnabled() const
    {
        return b2Body_IsEnabled(m_BodyId);
    }

    void Body::SetEnabled(bool enabled)
    {
        if (enabled)
            b2Body_Enable(m_BodyId);
        else
            b2Body_Disable(m_BodyId);
    }

    bool Body::IsBullet() const
    {
        return b2Body_IsBullet(m_BodyId);
    }

    void Body::SetBullet(bool bullet)
    {
        b2Body_SetBullet(m_BodyId, bullet);
    }

    float Body::GetGravityScale() const
    {
        return b2Body_GetGravityScale(m_BodyId);
    }

    void Body::SetGravityScale(float scale)
    {
        b2Body_SetGravityScale(m_BodyId, scale);
    }

    float Body::GetLinearDamping() const
    {
        return b2Body_GetLinearDamping(m_BodyId);
    }

    void Body::SetLinearDamping(float damping)
    {
        b2Body_SetLinearDamping(m_BodyId, damping);
    }

    float Body::GetAngularDamping() const
    {
        return b2Body_GetAngularDamping(m_BodyId);
    }

    void Body::SetAngularDamping(float damping)
    {
        b2Body_SetAngularDamping(m_BodyId, damping);
    }

    glm::vec2 Body::GetWorldPoint(const glm::vec2& localPoint) const
    {
        return ToGlm(b2Body_GetWorldPoint(m_BodyId, ToB2(localPoint)));
    }

    glm::vec2 Body::GetLocalPoint(const glm::vec2& worldPoint) const
    {
        return ToGlm(b2Body_GetLocalPoint(m_BodyId, ToB2(worldPoint)));
    }

    glm::vec2 Body::GetWorldVector(const glm::vec2& localVector) const
    {
        return ToGlm(b2Body_GetWorldVector(m_BodyId, ToB2(localVector)));
    }

    glm::vec2 Body::GetLocalVector(const glm::vec2& worldVector) const
    {
        return ToGlm(b2Body_GetLocalVector(m_BodyId, ToB2(worldVector)));
    }
}
