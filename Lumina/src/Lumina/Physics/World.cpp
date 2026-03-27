#include "World.h"
#include "Body.h"
#include "Conversions.h"

namespace Lumina
{
    World::World(const WorldDef& def)
        : m_Config(def)
    {
        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.gravity = ToB2(def.Gravity);
        worldDef.enableSleep = def.EnableSleep;
        worldDef.enableContinuous = def.EnableContinuous;

        m_WorldId = b2CreateWorld(&worldDef);
    }

    World::~World()
    {
        if (b2World_IsValid(m_WorldId))
        {
            m_Bodies.clear();
            b2DestroyWorld(m_WorldId);
        }
    }

    World::World(World&& other) noexcept
        : m_WorldId(other.m_WorldId)
        , m_Config(other.m_Config)
        , m_Bodies(std::move(other.m_Bodies))
        , m_ContactEvents(std::move(other.m_ContactEvents))
        , m_SensorEvents(std::move(other.m_SensorEvents))
    {
        other.m_WorldId = {};
    }

    World& World::operator=(World&& other) noexcept
    {
        if (this != &other)
        {
            if (b2World_IsValid(m_WorldId))
            {
                m_Bodies.clear();
                b2DestroyWorld(m_WorldId);
            }

            m_WorldId = other.m_WorldId;
            m_Config = other.m_Config;
            m_Bodies = std::move(other.m_Bodies);
            m_ContactEvents = std::move(other.m_ContactEvents);
            m_SensorEvents = std::move(other.m_SensorEvents);

            other.m_WorldId = {};
        }
        return *this;
    }

    void World::Step()
    {
        Step(m_Config.TimeStep, m_Config.SubStepCount);
    }

    void World::Step(float timeStep)
    {
        Step(timeStep, m_Config.SubStepCount);
    }

    void World::Step(float timeStep, int subSteps)
    {
        b2World_Step(m_WorldId, timeStep, subSteps);
        ProcessEvents();
    }

    void World::SetGravity(const glm::vec2& gravity)
    {
        b2World_SetGravity(m_WorldId, ToB2(gravity));
        m_Config.Gravity = gravity;
    }

    glm::vec2 World::GetGravity() const
    {
        return ToGlm(b2World_GetGravity(m_WorldId));
    }

    Ref<Body> World::CreateBody(const BodyDef& def)
    {
        b2BodyDef bd = b2DefaultBodyDef();

        switch (def.Type)
        {
        case BodyType::Static: bd.type = b2_staticBody; break;
        case BodyType::Kinematic:   bd.type = b2_kinematicBody; break;
        case BodyType::Dynamic:     bd.type = b2_dynamicBody; break;
        }

        bd.position = ToB2(def.Position);
        bd.rotation = ToB2Rot(def.Rotation);
        bd.linearVelocity = ToB2(def.LinearVelocity);
        bd.angularVelocity = def.AngularVelocity;
        bd.linearDamping = def.LinearDamping;
        bd.angularDamping = def.AngularDamping;
        bd.gravityScale = def.GravityScale;
        bd.isBullet = def.IsBullet;
        bd.isAwake = def.IsAwake;
        bd.enableSleep = def.EnableSleep;
        bd.isEnabled = def.IsEnabled;

        b2BodyId bodyId = b2CreateBody(m_WorldId, &bd);

        auto b = Ref<Body>(new Body(bodyId, def));
        m_Bodies.push_back(b);
        return b;
    }

    void World::DestroyBody(Ref<Body>& b)
    {
        if (b && b->IsValid())
        {
            b2DestroyBody(b->GetNativeId());
        }
        m_Bodies.erase(std::remove(m_Bodies.begin(), m_Bodies.end(), b), m_Bodies.end());
        b.reset();
    }

    void World::ProcessEvents()
    {
        m_ContactEvents.clear();
        m_SensorEvents.clear();

        // Process contact events
        b2ContactEvents contactEvents = b2World_GetContactEvents(m_WorldId);

        for (int i = 0; i < contactEvents.beginCount; ++i)
        {
            const b2ContactBeginTouchEvent& e = contactEvents.beginEvents[i];
            ContactEvent event;
            event.Type = ContactEventType::Begin;
            event.ShapeIdA = e.shapeIdA;
            event.ShapeIdB = e.shapeIdB;
            event.Point = { 0, 0 };
            event.Normal = { 0, 0 };
            event.ApproachSpeed = 0;
            m_ContactEvents.push_back(event);
        }

        for (int i = 0; i < contactEvents.endCount; ++i)
        {
            const b2ContactEndTouchEvent& e = contactEvents.endEvents[i];
            ContactEvent event;
            event.Type = ContactEventType::End;
            event.ShapeIdA = e.shapeIdA;
            event.ShapeIdB = e.shapeIdB;
            event.Point = { 0, 0 };
            event.Normal = { 0, 0 };
            event.ApproachSpeed = 0;
            m_ContactEvents.push_back(event);
        }

        for (int i = 0; i < contactEvents.hitCount; ++i)
        {
            const b2ContactHitEvent& e = contactEvents.hitEvents[i];
            ContactEvent event;
            event.Type = ContactEventType::Hit;
            event.ShapeIdA = e.shapeIdA;
            event.ShapeIdB = e.shapeIdB;
            event.Point = ToGlm(e.point);
            event.Normal = ToGlm(e.normal);
            event.ApproachSpeed = e.approachSpeed;
            m_ContactEvents.push_back(event);
        }

        // Process sensor events
        b2SensorEvents sensorEvents = b2World_GetSensorEvents(m_WorldId);

        for (int i = 0; i < sensorEvents.beginCount; ++i)
        {
            const b2SensorBeginTouchEvent& e = sensorEvents.beginEvents[i];
            SensorEvent event;
            event.Type = SensorEventType::Begin;
            event.SensorShapeId = e.sensorShapeId;
            event.VisitorShapeId = e.visitorShapeId;
            m_SensorEvents.push_back(event);
        }

        for (int i = 0; i < sensorEvents.endCount; ++i)
        {
            const b2SensorEndTouchEvent& e = sensorEvents.endEvents[i];
            SensorEvent event;
            event.Type = SensorEventType::End;
            event.SensorShapeId = e.sensorShapeId;
            event.VisitorShapeId = e.visitorShapeId;
            m_SensorEvents.push_back(event);
        }
    }

    RayCastResult World::RayCastClosest(glm::vec2 origin, glm::vec2 direction, float maxDistance) const
    {
        glm::vec2 translation = direction * maxDistance;
        b2RayResult result = b2World_CastRayClosest(
            m_WorldId,
            ToB2(origin),
            ToB2(translation),
            b2DefaultQueryFilter()
        );

        RayCastResult r;
        r.Hit = result.hit;
        if (result.hit)
        {
            r.ShapeId = result.shapeId;
            r.Point = ToGlm(result.point);
            r.Normal = ToGlm(result.normal);
            r.Fraction = result.fraction;
        }
        return r;
    }
}
