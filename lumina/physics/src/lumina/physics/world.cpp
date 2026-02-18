#include "world.h"
#include "body.h"
#include "conversions.h"

namespace lumina::physics
{
    world::world(const world_def& def)
        : m_config(def)
    {
        b2WorldDef world_def = b2DefaultWorldDef();
        world_def.gravity = to_b2(def.gravity);
        world_def.enableSleep = def.enable_sleep;
        world_def.enableContinuous = def.enable_continuous;

        m_world_id = b2CreateWorld(&world_def);
    }

    world::~world()
    {
        if (b2World_IsValid(m_world_id))
        {
            m_bodies.clear();
            b2DestroyWorld(m_world_id);
        }
    }

    world::world(world&& other) noexcept
        : m_world_id(other.m_world_id)
        , m_config(other.m_config)
        , m_bodies(std::move(other.m_bodies))
        , m_contact_events(std::move(other.m_contact_events))
        , m_sensor_events(std::move(other.m_sensor_events))
    {
        other.m_world_id = {};
    }

    world& world::operator=(world&& other) noexcept
    {
        if (this != &other)
        {
            if (b2World_IsValid(m_world_id))
            {
                m_bodies.clear();
                b2DestroyWorld(m_world_id);
            }

            m_world_id = other.m_world_id;
            m_config = other.m_config;
            m_bodies = std::move(other.m_bodies);
            m_contact_events = std::move(other.m_contact_events);
            m_sensor_events = std::move(other.m_sensor_events);

            other.m_world_id = {};
        }
        return *this;
    }

    void world::step()
    {
        step(m_config.time_step, m_config.sub_step_count);
    }

    void world::step(float time_step)
    {
        step(time_step, m_config.sub_step_count);
    }

    void world::step(float time_step, int sub_steps)
    {
        b2World_Step(m_world_id, time_step, sub_steps);
        process_events();
    }

    void world::set_gravity(const glm::vec2& gravity)
    {
        b2World_SetGravity(m_world_id, to_b2(gravity));
        m_config.gravity = gravity;
    }

    glm::vec2 world::get_gravity() const
    {
        return to_glm(b2World_GetGravity(m_world_id));
    }

    ref<body> world::create_body(const body_def& def)
    {
        b2BodyDef bd = b2DefaultBodyDef();

        switch (def.type)
        {
        case body_type::static_body: bd.type = b2_staticBody; break;
        case body_type::kinematic:   bd.type = b2_kinematicBody; break;
        case body_type::dynamic:     bd.type = b2_dynamicBody; break;
        }

        bd.position = to_b2(def.position);
        bd.rotation = to_b2_rot(def.rotation);
        bd.linearVelocity = to_b2(def.linear_velocity);
        bd.angularVelocity = def.angular_velocity;
        bd.linearDamping = def.linear_damping;
        bd.angularDamping = def.angular_damping;
        bd.gravityScale = def.gravity_scale;
        bd.isBullet = def.is_bullet;
        bd.isAwake = def.is_awake;
        bd.enableSleep = def.enable_sleep;
        bd.isEnabled = def.is_enabled;

        b2BodyId body_id = b2CreateBody(m_world_id, &bd);

        auto b = ref<body>(new body(body_id, def));
        m_bodies.push_back(b);
        return b;
    }

    void world::destroy_body(ref<body>& b)
    {
        if (b && b->is_valid())
        {
            b2DestroyBody(b->get_native_id());
        }
        m_bodies.erase(std::remove(m_bodies.begin(), m_bodies.end(), b), m_bodies.end());
        b.reset();
    }

    void world::process_events()
    {
        m_contact_events.clear();
        m_sensor_events.clear();

        // Process contact events
        b2ContactEvents contact_events = b2World_GetContactEvents(m_world_id);

        for (int i = 0; i < contact_events.beginCount; ++i)
        {
            const b2ContactBeginTouchEvent& e = contact_events.beginEvents[i];
            contact_event event;
            event.type = contact_event_type::begin;
            event.shape_id_a = e.shapeIdA;
            event.shape_id_b = e.shapeIdB;
            event.point = { 0, 0 };
            event.normal = { 0, 0 };
            event.approach_speed = 0;
            m_contact_events.push_back(event);
        }

        for (int i = 0; i < contact_events.endCount; ++i)
        {
            const b2ContactEndTouchEvent& e = contact_events.endEvents[i];
            contact_event event;
            event.type = contact_event_type::end;
            event.shape_id_a = e.shapeIdA;
            event.shape_id_b = e.shapeIdB;
            event.point = { 0, 0 };
            event.normal = { 0, 0 };
            event.approach_speed = 0;
            m_contact_events.push_back(event);
        }

        for (int i = 0; i < contact_events.hitCount; ++i)
        {
            const b2ContactHitEvent& e = contact_events.hitEvents[i];
            contact_event event;
            event.type = contact_event_type::hit;
            event.shape_id_a = e.shapeIdA;
            event.shape_id_b = e.shapeIdB;
            event.point = to_glm(e.point);
            event.normal = to_glm(e.normal);
            event.approach_speed = e.approachSpeed;
            m_contact_events.push_back(event);
        }

        // Process sensor events
        b2SensorEvents sensor_events = b2World_GetSensorEvents(m_world_id);

        for (int i = 0; i < sensor_events.beginCount; ++i)
        {
            const b2SensorBeginTouchEvent& e = sensor_events.beginEvents[i];
            sensor_event event;
            event.type = sensor_event_type::begin;
            event.sensor_shape_id = e.sensorShapeId;
            event.visitor_shape_id = e.visitorShapeId;
            m_sensor_events.push_back(event);
        }

        for (int i = 0; i < sensor_events.endCount; ++i)
        {
            const b2SensorEndTouchEvent& e = sensor_events.endEvents[i];
            sensor_event event;
            event.type = sensor_event_type::end;
            event.sensor_shape_id = e.sensorShapeId;
            event.visitor_shape_id = e.visitorShapeId;
            m_sensor_events.push_back(event);
        }
    }

    ray_cast_result world::ray_cast_closest(glm::vec2 origin, glm::vec2 direction, float max_distance) const
    {
        glm::vec2 translation = direction * max_distance;
        b2RayResult result = b2World_CastRayClosest(
            m_world_id,
            to_b2(origin),
            to_b2(translation),
            b2DefaultQueryFilter()
        );

        ray_cast_result r;
        r.hit = result.hit;
        if (result.hit)
        {
            r.shape_id = result.shapeId;
            r.point = to_glm(result.point);
            r.normal = to_glm(result.normal);
            r.fraction = result.fraction;
        }
        return r;
    }
}
