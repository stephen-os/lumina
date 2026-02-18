#include <catch2/catch_all.hpp>
#include <lumina/physics/physics.h>

using namespace lumina::physics;

TEST_CASE("Body creation with default settings", "[physics][body]")
{
    world w;
    auto body = w.create_body({});

    REQUIRE(body->is_valid());
    REQUIRE(body->get_type() == body_type::dynamic);
    REQUIRE(body->get_position().x == Catch::Approx(0.0f));
    REQUIRE(body->get_position().y == Catch::Approx(0.0f));
}

TEST_CASE("Body creation with custom position", "[physics][body]")
{
    world w;

    body_def def;
    def.position = { 5.0f, 10.0f };
    auto body = w.create_body(def);

    REQUIRE(body->get_position().x == Catch::Approx(5.0f));
    REQUIRE(body->get_position().y == Catch::Approx(10.0f));
}

TEST_CASE("Body types", "[physics][body]")
{
    world w;

    SECTION("Static body")
    {
        auto body = w.create_body({ .type = body_type::static_body });
        REQUIRE(body->get_type() == body_type::static_body);
    }

    SECTION("Kinematic body")
    {
        auto body = w.create_body({ .type = body_type::kinematic });
        REQUIRE(body->get_type() == body_type::kinematic);
    }

    SECTION("Dynamic body")
    {
        auto body = w.create_body({ .type = body_type::dynamic });
        REQUIRE(body->get_type() == body_type::dynamic);
    }
}

TEST_CASE("Body transform manipulation", "[physics][body]")
{
    world w;
    auto body = w.create_body({});

    SECTION("Set position")
    {
        body->set_position({ 10.0f, 20.0f });
        REQUIRE(body->get_position().x == Catch::Approx(10.0f));
        REQUIRE(body->get_position().y == Catch::Approx(20.0f));
    }

    SECTION("Set rotation")
    {
        body->set_rotation(1.57f); // ~90 degrees
        REQUIRE(body->get_rotation() == Catch::Approx(1.57f).margin(0.01f));
    }

    SECTION("Set transform")
    {
        body->set_transform({ 5.0f, 15.0f }, 0.785f); // ~45 degrees
        REQUIRE(body->get_position().x == Catch::Approx(5.0f));
        REQUIRE(body->get_position().y == Catch::Approx(15.0f));
        REQUIRE(body->get_rotation() == Catch::Approx(0.785f).margin(0.01f));
    }
}

TEST_CASE("Body velocity", "[physics][body]")
{
    world w;
    auto body = w.create_body({ .type = body_type::dynamic });
    body->add_box(1.0f, 1.0f);

    SECTION("Set linear velocity")
    {
        body->set_linear_velocity({ 5.0f, 10.0f });
        REQUIRE(body->get_linear_velocity().x == Catch::Approx(5.0f));
        REQUIRE(body->get_linear_velocity().y == Catch::Approx(10.0f));
    }

    SECTION("Set angular velocity")
    {
        body->set_angular_velocity(2.0f);
        REQUIRE(body->get_angular_velocity() == Catch::Approx(2.0f));
    }
}

TEST_CASE("Body forces and impulses", "[physics][body]")
{
    world_def world_def;
    world_def.gravity = { 0.0f, 0.0f }; // No gravity for cleaner tests
    world w(world_def);

    auto body = w.create_body({ .type = body_type::dynamic });
    body->add_box(1.0f, 1.0f);

    glm::vec2 initial_pos = body->get_position();

    SECTION("Apply force to center")
    {
        body->apply_force_to_center({ 100.0f, 0.0f });
        w.step(1.0f / 60.0f, 4);

        // Body should have moved in the x direction
        REQUIRE(body->get_position().x > initial_pos.x);
    }

    SECTION("Apply linear impulse to center")
    {
        body->apply_linear_impulse_to_center({ 10.0f, 0.0f });
        w.step(1.0f / 60.0f, 4);

        // Body should have moved in the x direction
        REQUIRE(body->get_position().x > initial_pos.x);
    }

    SECTION("Apply torque")
    {
        body->apply_torque(10.0f);
        w.step(1.0f / 60.0f, 4);

        // Body should have rotated
        REQUIRE(body->get_angular_velocity() != 0.0f);
    }
}

TEST_CASE("Body mass properties", "[physics][body]")
{
    world w;
    auto body = w.create_body({ .type = body_type::dynamic });

    // Add a shape with density
    shape_def shape_def;
    shape_def.material.density = 2.0f;
    body->add_box(1.0f, 1.0f, {}, 0.0f, shape_def);

    REQUIRE(body->get_mass() > 0.0f);
    REQUIRE(body->get_inertia() > 0.0f);
}

TEST_CASE("Body properties", "[physics][body]")
{
    world w;
    auto body = w.create_body({});
    body->add_box(1.0f, 1.0f);

    SECTION("Awake state")
    {
        REQUIRE(body->is_awake());
        body->set_awake(false);
        REQUIRE_FALSE(body->is_awake());
    }

    SECTION("Enabled state")
    {
        REQUIRE(body->is_enabled());
        body->set_enabled(false);
        REQUIRE_FALSE(body->is_enabled());
    }

    SECTION("Bullet mode")
    {
        REQUIRE_FALSE(body->is_bullet());
        body->set_bullet(true);
        REQUIRE(body->is_bullet());
    }

    SECTION("Gravity scale")
    {
        body->set_gravity_scale(0.5f);
        REQUIRE(body->get_gravity_scale() == Catch::Approx(0.5f));
    }

    SECTION("Linear damping")
    {
        body->set_linear_damping(0.5f);
        REQUIRE(body->get_linear_damping() == Catch::Approx(0.5f));
    }

    SECTION("Angular damping")
    {
        body->set_angular_damping(0.3f);
        REQUIRE(body->get_angular_damping() == Catch::Approx(0.3f));
    }
}

TEST_CASE("Body coordinate conversion", "[physics][body]")
{
    world w;
    auto body = w.create_body({ .position = { 10.0f, 10.0f }, .rotation = 0.0f });

    SECTION("World to local point")
    {
        glm::vec2 world_point = { 15.0f, 10.0f };
        glm::vec2 local_point = body->get_local_point(world_point);
        REQUIRE(local_point.x == Catch::Approx(5.0f).margin(0.01f));
        REQUIRE(local_point.y == Catch::Approx(0.0f).margin(0.01f));
    }

    SECTION("Local to world point")
    {
        glm::vec2 local_point = { 5.0f, 0.0f };
        glm::vec2 world_point = body->get_world_point(local_point);
        REQUIRE(world_point.x == Catch::Approx(15.0f).margin(0.01f));
        REQUIRE(world_point.y == Catch::Approx(10.0f).margin(0.01f));
    }
}

TEST_CASE("Body user data", "[physics][body]")
{
    world w;
    auto body = w.create_body({});

    SECTION("No user data initially")
    {
        REQUIRE_FALSE(body->has_user_data());
    }

    SECTION("Set and get int user data")
    {
        body->set_user_data(42);
        REQUIRE(body->has_user_data());
        auto* data = body->get_user_data<int>();
        REQUIRE(data != nullptr);
        REQUIRE(*data == 42);
    }

    SECTION("Set and get struct user data")
    {
        struct MyData
        {
            int id;
            float value;
        };

        body->set_user_data(MyData{ 123, 3.14f });
        auto* data = body->get_user_data<MyData>();
        REQUIRE(data != nullptr);
        REQUIRE(data->id == 123);
        REQUIRE(data->value == Catch::Approx(3.14f));
    }
}

TEST_CASE("Body shape management", "[physics][body]")
{
    world w;
    auto body = w.create_body({});

    REQUIRE(body->get_shapes().empty());

    auto circle = body->add_circle(1.0f);
    REQUIRE(body->get_shapes().size() == 1);

    auto box = body->add_box(1.0f, 1.0f);
    REQUIRE(body->get_shapes().size() == 2);

    body->remove_shape(circle);
    REQUIRE(body->get_shapes().size() == 1);

    body->remove_shape(box);
    REQUIRE(body->get_shapes().empty());
}
