#pragma once

#include <lumina/core/base.h>
#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include <vector>

#include "events.h"

namespace Lumina
{
    class Body;
    struct BodyDef;

    // World configuration
    struct WorldDef
    {
        glm::vec2 Gravity = { 0.0f, -9.81f };
        float TimeStep = 1.0f / 60.0f;
        int SubStepCount = 4;
        bool EnableSleep = true;
        bool EnableContinuous = true;
    };

    // Raycast result
    struct RayCastResult
    {
        b2ShapeId ShapeId;
        glm::vec2 Point;
        glm::vec2 Normal;
        float Fraction;
        bool Hit = false;

        explicit operator bool() const { return Hit; }
    };

    // Physics world - owns the simulation
    class World
    {
    public:
        explicit World(const WorldDef& def = {});
        ~World();

        // Non-copyable, movable
        World(const World&) = delete;
        World& operator=(const World&) = delete;
        World(World&& other) noexcept;
        World& operator=(World&& other) noexcept;

        // Simulation control
        void Step();
        void Step(float timeStep);
        void Step(float timeStep, int subSteps);

        // World properties
        void SetGravity(const glm::vec2& gravity);
        [[nodiscard]] glm::vec2 GetGravity() const;

        // Body creation/destruction
        ref<Body> CreateBody(const BodyDef& def);
        void DestroyBody(ref<Body>& b);

        // Event access (call after step())
        [[nodiscard]] const std::vector<ContactEvent>& GetContactEvents() const { return m_ContactEvents; }
        [[nodiscard]] const std::vector<SensorEvent>& GetSensorEvents() const { return m_SensorEvents; }

        // Queries
        [[nodiscard]] RayCastResult RayCastClosest(glm::vec2 origin, glm::vec2 direction, float maxDistance) const;

        // Internal access
        [[nodiscard]] b2WorldId GetNativeId() const { return m_WorldId; }
        [[nodiscard]] bool IsValid() const { return b2World_IsValid(m_WorldId); }

        // Get all bodies
        [[nodiscard]] const std::vector<ref<Body>>& GetBodies() const { return m_Bodies; }

    private:
        void ProcessEvents();

        b2WorldId m_WorldId = {};
        WorldDef m_Config;

        std::vector<ref<Body>> m_Bodies;
        std::vector<ContactEvent> m_ContactEvents;
        std::vector<SensorEvent> m_SensorEvents;
    };
}
