#include <catch2/catch_all.hpp>
#include <Lumina/Physics/physics.h>

using namespace lumina::physics;

TEST_CASE("Contact events on collision", "[physics][events]")
{
    world w;

    // Create ground
    auto ground = w.create_body({ .type = body_type::static_body, .position = { 0.0f, 0.0f } });
    shape_def ground_shape_def;
    ground_shape_def.enable_contact_events = true;
    ground->add_box(10.0f, 0.5f, {}, 0.0f, ground_shape_def);

    // Create falling box
    auto box = w.create_body({ .type = body_type::dynamic, .position = { 0.0f, 5.0f } });
    shape_def box_shape_def;
    box_shape_def.enable_contact_events = true;
    box->add_box(0.5f, 0.5f, {}, 0.0f, box_shape_def);

    // Simulate until collision
    bool had_contact_begin = false;
    for (int i = 0; i < 300; ++i)
    {
        w.step(1.0f / 60.0f, 4);

        for (const auto& event : w.get_contact_events())
        {
            if (event.type == contact_event_type::begin)
            {
                had_contact_begin = true;
            }
        }

        if (had_contact_begin)
            break;
    }

    REQUIRE(had_contact_begin);
}

TEST_CASE("Hit events on high-speed collision", "[physics][events]")
{
    world w;

    // Create wall
    auto wall = w.create_body({ .type = body_type::static_body, .position = { 0.0f, 0.0f } });
    shape_def wall_shape_def;
    wall_shape_def.enable_hit_events = true;
    wall->add_box(0.5f, 5.0f, {}, 0.0f, wall_shape_def);

    // Create fast-moving projectile
    body_def projectile_def;
    projectile_def.type = body_type::dynamic;
    projectile_def.position = { -5.0f, 0.0f };
    projectile_def.linear_velocity = { 50.0f, 0.0f }; // Fast!
    projectile_def.is_bullet = true;

    auto projectile = w.create_body(projectile_def);
    shape_def proj_shape_def;
    proj_shape_def.enable_hit_events = true;
    projectile->add_circle(0.2f, {}, proj_shape_def);

    // Simulate until hit
    bool had_hit_event = false;
    float approach_speed = 0.0f;

    for (int i = 0; i < 60; ++i)
    {
        w.step(1.0f / 60.0f, 4);

        for (const auto& event : w.get_contact_events())
        {
            if (event.type == contact_event_type::hit)
            {
                had_hit_event = true;
                approach_speed = event.approach_speed;
            }
        }

        if (had_hit_event)
            break;
    }

    REQUIRE(had_hit_event);
    REQUIRE(approach_speed > 10.0f); // Should be a significant speed
}

TEST_CASE("Sensor events", "[physics][events]")
{
    world w;

    // Create trigger zone (sensor)
    auto trigger = w.create_body({ .type = body_type::static_body, .position = { 0.0f, 0.0f } });
    shape_def trigger_def;
    trigger_def.is_sensor = true;
    trigger_def.enable_sensor_events = true;
    trigger->add_box(2.0f, 2.0f, {}, 0.0f, trigger_def);

    // Create moving body that will enter the sensor
    body_def mover_def;
    mover_def.type = body_type::kinematic;
    mover_def.position = { -5.0f, 0.0f };
    mover_def.linear_velocity = { 5.0f, 0.0f };

    auto mover = w.create_body(mover_def);
    shape_def mover_shape_def;
    mover_shape_def.enable_sensor_events = true;
    mover->add_circle(0.5f, {}, mover_shape_def);

    // Simulate until sensor triggered
    bool had_sensor_begin = false;

    for (int i = 0; i < 120; ++i)
    {
        w.step(1.0f / 60.0f, 4);

        for (const auto& event : w.get_sensor_events())
        {
            if (event.type == sensor_event_type::begin)
            {
                had_sensor_begin = true;
            }
        }

        if (had_sensor_begin)
            break;
    }

    REQUIRE(had_sensor_begin);
}

TEST_CASE("No events when events disabled", "[physics][events]")
{
    world w;

    // Create ground without events enabled
    auto ground = w.create_body({ .type = body_type::static_body, .position = { 0.0f, 0.0f } });
    ground->add_box(10.0f, 0.5f);

    // Create falling box without events enabled
    auto box = w.create_body({ .type = body_type::dynamic, .position = { 0.0f, 2.0f } });
    box->add_box(0.5f, 0.5f);

    // Simulate
    int event_count = 0;
    for (int i = 0; i < 120; ++i)
    {
        w.step(1.0f / 60.0f, 4);
        event_count += static_cast<int>(w.get_contact_events().size());
    }

    REQUIRE(event_count == 0);
}

TEST_CASE("Contact events have shape IDs", "[physics][events]")
{
    world w;

    // Create ground
    auto ground = w.create_body({ .type = body_type::static_body, .position = { 0.0f, 0.0f } });
    shape_def shape_def;
    shape_def.enable_contact_events = true;
    auto ground_shape = ground->add_box(10.0f, 0.5f, {}, 0.0f, shape_def);

    // Create falling box
    auto box = w.create_body({ .type = body_type::dynamic, .position = { 0.0f, 2.0f } });
    auto box_shape = box->add_box(0.5f, 0.5f, {}, 0.0f, shape_def);

    // Simulate until collision
    bool verified_shape_ids = false;
    for (int i = 0; i < 120; ++i)
    {
        w.step(1.0f / 60.0f, 4);

        for (const auto& event : w.get_contact_events())
        {
            if (event.type == contact_event_type::begin)
            {
                // Shape IDs should be valid
                REQUIRE(b2Shape_IsValid(event.shape_id_a));
                REQUIRE(b2Shape_IsValid(event.shape_id_b));
                verified_shape_ids = true;
            }
        }

        if (verified_shape_ids)
            break;
    }

    REQUIRE(verified_shape_ids);
}
