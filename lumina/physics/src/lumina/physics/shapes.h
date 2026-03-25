#pragma once

#include <lumina/core/base.h>
#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include <any>
#include <vector>

namespace Lumina
{
    class Body;

    // Collision filtering
    struct CollisionFilter
    {
        uint64_t CategoryBits = 1;
        uint64_t MaskBits = UINT64_MAX;
        int GroupIndex = 0;
    };

    // Shape material properties
    struct ShapeMaterial
    {
        float Friction = 0.6f;
        float Restitution = 0.0f;
        float Density = 1.0f;
    };

    // Base shape definition
    struct ShapeDef
    {
        ShapeMaterial Material;
        CollisionFilter Filter;
        bool IsSensor = false;
        bool EnableContactEvents = false;
        bool EnableSensorEvents = false;
        bool EnableHitEvents = false;
        std::any UserData;
    };

    enum class ShapeType
    {
        Circle,
        Polygon,
        Capsule,
        Segment
    };

    class Shape : public std::enable_shared_from_this<Shape>
    {
    public:
        // Shape properties
        [[nodiscard]] ShapeType GetType() const;
        [[nodiscard]] ref<Body> GetBody() const;

        // Material
        [[nodiscard]] float GetFriction() const;
        void SetFriction(float friction);
        [[nodiscard]] float GetRestitution() const;
        void SetRestitution(float restitution);
        [[nodiscard]] float GetDensity() const;
        void SetDensity(float density);

        // Filtering
        [[nodiscard]] CollisionFilter GetFilter() const;
        void SetFilter(const CollisionFilter& filter);

        // Sensor (can only be set at creation time via ShapeDef::IsSensor)
        [[nodiscard]] bool IsSensor() const;

        // Events
        void EnableContactEvents(bool enable);
        void EnableSensorEvents(bool enable);
        void EnableHitEvents(bool enable);

        // Geometry queries
        [[nodiscard]] bool TestPoint(const glm::vec2& point) const;
        [[nodiscard]] glm::vec2 GetCentroid() const;

        // User data
        template<typename T>
        void SetUserData(T&& data) { m_UserData = std::forward<T>(data); }

        template<typename T>
        [[nodiscard]] T* GetUserData() { return std::any_cast<T>(&m_UserData); }

        template<typename T>
        [[nodiscard]] const T* GetUserData() const { return std::any_cast<T>(&m_UserData); }

        [[nodiscard]] bool HasUserData() const { return m_UserData.has_value(); }

        // Internal
        [[nodiscard]] b2ShapeId GetNativeId() const { return m_ShapeId; }
        [[nodiscard]] bool IsValid() const { return b2Shape_IsValid(m_ShapeId); }

    private:
        friend class Body;
        Shape(b2ShapeId id, ref<Body> parent, const ShapeDef& def);

        b2ShapeId m_ShapeId = {};
        std::weak_ptr<Body> m_Body;
        std::any m_UserData;
    };

    // Shape definition helpers
    struct CircleDef : ShapeDef
    {
        glm::vec2 Center = { 0.0f, 0.0f };
        float Radius = 0.5f;
    };

    struct BoxDef : ShapeDef
    {
        float HalfWidth = 0.5f;
        float HalfHeight = 0.5f;
        glm::vec2 Center = { 0.0f, 0.0f };
        float Rotation = 0.0f;
    };

    struct PolygonDef : ShapeDef
    {
        std::vector<glm::vec2> Vertices;
    };

    struct CapsuleDef : ShapeDef
    {
        glm::vec2 Center1 = { 0.0f, -0.5f };
        glm::vec2 Center2 = { 0.0f, 0.5f };
        float Radius = 0.25f;
    };

    struct SegmentDef : ShapeDef
    {
        glm::vec2 Point1 = { -0.5f, 0.0f };
        glm::vec2 Point2 = { 0.5f, 0.0f };
    };
}
