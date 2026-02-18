#include <catch2/catch_all.hpp>
#include <lumina/physics/physics.h>

using namespace lumina::physics;

TEST_CASE("World creation with default settings", "[physics][world]")
{
    world w;

    REQUIRE(w.is_valid());
    REQUIRE(w.get_gravity().x == Catch::Approx(0.0f));
    REQUIRE(w.get_gravity().y == Catch::Approx(-9.81f));
}

TEST_CASE("World creation with custom gravity", "[physics][world]")
{
    world_def def;
    def.gravity = { 0.0f, -20.0f };

    world w(def);

    REQUIRE(w.is_valid());
    REQUIRE(w.get_gravity().x == Catch::Approx(0.0f));
    REQUIRE(w.get_gravity().y == Catch::Approx(-20.0f));
}

TEST_CASE("World gravity can be changed", "[physics][world]")
{
    world w;

    w.set_gravity({ 5.0f, -15.0f });

    REQUIRE(w.get_gravity().x == Catch::Approx(5.0f));
    REQUIRE(w.get_gravity().y == Catch::Approx(-15.0f));
}

TEST_CASE("World step advances simulation", "[physics][world]")
{
    world w;

    // Create a dynamic body
    body_def def;
    def.type = body_type::dynamic;
    def.position = { 0.0f, 10.0f };
    auto body = w.create_body(def);
    body->add_box(1.0f, 1.0f);

    glm::vec2 initial_pos = body->get_position();

    // Step the simulation
    w.step(1.0f / 60.0f, 4);

    glm::vec2 new_pos = body->get_position();

    // Body should have fallen due to gravity
    REQUIRE(new_pos.y < initial_pos.y);
}

TEST_CASE("World move constructor", "[physics][world]")
{
    world w1;
    auto body = w1.create_body({ .type = body_type::dynamic, .position = { 5.0f, 5.0f } });
    body->add_circle(1.0f);

    b2WorldId original_id = w1.get_native_id();

    world w2(std::move(w1));

    REQUIRE(w2.is_valid());
    REQUIRE(w2.get_native_id().index1 == original_id.index1);
    REQUIRE(w2.get_bodies().size() == 1);
}

TEST_CASE("World body creation and destruction", "[physics][world]")
{
    world w;

    REQUIRE(w.get_bodies().empty());

    auto body1 = w.create_body({});
    REQUIRE(w.get_bodies().size() == 1);

    auto body2 = w.create_body({});
    REQUIRE(w.get_bodies().size() == 2);

    w.destroy_body(body1);
    REQUIRE(w.get_bodies().size() == 1);

    w.destroy_body(body2);
    REQUIRE(w.get_bodies().empty());
}

TEST_CASE("World zero gravity", "[physics][world]")
{
    world_def def;
    def.gravity = { 0.0f, 0.0f };

    world w(def);

    body_def body_def;
    body_def.type = body_type::dynamic;
    body_def.position = { 0.0f, 10.0f };
    auto body = w.create_body(body_def);
    body->add_box(1.0f, 1.0f);

    glm::vec2 initial_pos = body->get_position();

    // Step multiple times
    for (int i = 0; i < 60; ++i)
    {
        w.step(1.0f / 60.0f);
    }

    glm::vec2 new_pos = body->get_position();

    // Body should not have moved (no gravity, no velocity)
    REQUIRE(new_pos.x == Catch::Approx(initial_pos.x).margin(0.001f));
    REQUIRE(new_pos.y == Catch::Approx(initial_pos.y).margin(0.001f));
}
