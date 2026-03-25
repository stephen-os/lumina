#pragma once

#include <Lumina/Core/Base.h>
#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include <vector>
#include <any>

namespace Lumina
{
    class World;
    class Shape;
    struct ShapeDef;

    enum class BodyType
    {
        Static,
        Kinematic,
        Dynamic
    };

    struct BodyDef
    {
        BodyType Type = BodyType::Dynamic;
        glm::vec2 Position = { 0.0f, 0.0f };
        float Rotation = 0.0f;
        glm::vec2 LinearVelocity = { 0.0f, 0.0f };
        float AngularVelocity = 0.0f;
        float LinearDamping = 0.0f;
        float AngularDamping = 0.0f;
        float GravityScale = 1.0f;
        bool IsBullet = false;
        bool IsAwake = true;
        bool EnableSleep = true;
        bool IsEnabled = true;
        std::any UserData;
    };

    class Body : public std::enable_shared_from_this<Body>
    {
    public:
        // Shape attachment
        ref<Shape> AddCircle(float radius, const glm::vec2& offset = {});
        ref<Shape> AddCircle(float radius, const glm::vec2& offset, const ShapeDef& def);
        ref<Shape> AddBox(float halfWidth, float halfHeight, const glm::vec2& offset = {}, float rotation = 0.0f);
        ref<Shape> AddBox(float halfWidth, float halfHeight, const glm::vec2& offset, float rotation, const ShapeDef& def);
        ref<Shape> AddPolygon(const std::vector<glm::vec2>& vertices);
        ref<Shape> AddPolygon(const std::vector<glm::vec2>& vertices, const ShapeDef& def);
        ref<Shape> AddCapsule(const glm::vec2& center1, const glm::vec2& center2, float radius);
        ref<Shape> AddCapsule(const glm::vec2& center1, const glm::vec2& center2, float radius, const ShapeDef& def);
        ref<Shape> AddSegment(const glm::vec2& point1, const glm::vec2& point2);
        ref<Shape> AddSegment(const glm::vec2& point1, const glm::vec2& point2, const ShapeDef& def);

        void RemoveShape(ref<Shape>& s);
        [[nodiscard]] const std::vector<ref<Shape>>& GetShapes() const { return m_Shapes; }

        // Transform
        [[nodiscard]] glm::vec2 GetPosition() const;
        [[nodiscard]] float GetRotation() const;
        void SetTransform(const glm::vec2& position, float rotation);
        void SetPosition(const glm::vec2& position);
        void SetRotation(float rotation);

        // Velocity
        [[nodiscard]] glm::vec2 GetLinearVelocity() const;
        [[nodiscard]] float GetAngularVelocity() const;
        void SetLinearVelocity(const glm::vec2& velocity);
        void SetAngularVelocity(float velocity);

        // Forces and impulses
        void ApplyForce(const glm::vec2& force, const glm::vec2& point, bool wake = true);
        void ApplyForceToCenter(const glm::vec2& force, bool wake = true);
        void ApplyTorque(float torque, bool wake = true);
        void ApplyLinearImpulse(const glm::vec2& impulse, const glm::vec2& point, bool wake = true);
        void ApplyLinearImpulseToCenter(const glm::vec2& impulse, bool wake = true);
        void ApplyAngularImpulse(float impulse, bool wake = true);

        // Mass properties
        [[nodiscard]] float GetMass() const;
        [[nodiscard]] float GetInertia() const;
        [[nodiscard]] glm::vec2 GetCenterOfMass() const;
        void SetMassData(float mass, const glm::vec2& center, float inertia);
        void ResetMassData();

        // Body properties
        [[nodiscard]] BodyType GetType() const;
        void SetType(BodyType type);
        [[nodiscard]] bool IsAwake() const;
        void SetAwake(bool awake);
        [[nodiscard]] bool IsEnabled() const;
        void SetEnabled(bool enabled);
        [[nodiscard]] bool IsBullet() const;
        void SetBullet(bool bullet);
        [[nodiscard]] float GetGravityScale() const;
        void SetGravityScale(float scale);
        [[nodiscard]] float GetLinearDamping() const;
        void SetLinearDamping(float damping);
        [[nodiscard]] float GetAngularDamping() const;
        void SetAngularDamping(float damping);

        // User data
        template<typename T>
        void SetUserData(T&& data) { m_UserData = std::forward<T>(data); }

        template<typename T>
        [[nodiscard]] T* GetUserData() { return std::any_cast<T>(&m_UserData); }

        template<typename T>
        [[nodiscard]] const T* GetUserData() const { return std::any_cast<T>(&m_UserData); }

        [[nodiscard]] bool HasUserData() const { return m_UserData.has_value(); }

        // World coordinate conversion
        [[nodiscard]] glm::vec2 GetWorldPoint(const glm::vec2& localPoint) const;
        [[nodiscard]] glm::vec2 GetLocalPoint(const glm::vec2& worldPoint) const;
        [[nodiscard]] glm::vec2 GetWorldVector(const glm::vec2& localVector) const;
        [[nodiscard]] glm::vec2 GetLocalVector(const glm::vec2& worldVector) const;

        // Internal
        [[nodiscard]] b2BodyId GetNativeId() const { return m_BodyId; }
        [[nodiscard]] bool IsValid() const { return b2Body_IsValid(m_BodyId); }

    private:
        friend class World;
        Body(b2BodyId id, const BodyDef& def);

        b2ShapeDef MakeShapeDef(const ShapeDef& def) const;

        b2BodyId m_BodyId = {};
        std::vector<ref<Shape>> m_Shapes;
        std::any m_UserData;
    };
}
