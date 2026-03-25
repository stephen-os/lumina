#include <catch2/catch_all.hpp>
#include <Lumina/Physics/physics.h>

using namespace lumina::physics;

TEST_CASE("Circle shape creation", "[physics][shapes]")
{
    world w;
    auto body = w.create_body({});

    auto shape = body->add_circle(2.0f);

    REQUIRE(shape->is_valid());
    REQUIRE(shape->get_type() == shape_type::circle);
    REQUIRE(shape->get_body() == body);
}

TEST_CASE("Circle shape with offset", "[physics][shapes]")
{
    world w;
    auto body = w.create_body({});

    auto shape = body->add_circle(1.0f, { 5.0f, 3.0f });

    REQUIRE(shape->is_valid());
    glm::vec2 centroid = shape->get_centroid();
    REQUIRE(centroid.x == Catch::Approx(5.0f));
    REQUIRE(centroid.y == Catch::Approx(3.0f));
}

TEST_CASE("Box shape creation", "[physics][shapes]")
{
    world w;
    auto body = w.create_body({});

    auto shape = body->add_box(2.0f, 1.0f);

    REQUIRE(shape->is_valid());
    REQUIRE(shape->get_type() == shape_type::polygon);
}

TEST_CASE("Box shape with offset and rotation", "[physics][shapes]")
{
    world w;
    auto body = w.create_body({});

    auto shape = body->add_box(1.0f, 1.0f, { 2.0f, 3.0f }, 0.785f);

    REQUIRE(shape->is_valid());
    glm::vec2 centroid = shape->get_centroid();
    REQUIRE(centroid.x == Catch::Approx(2.0f).margin(0.1f));
    REQUIRE(centroid.y == Catch::Approx(3.0f).margin(0.1f));
}

TEST_CASE("Polygon shape creation", "[physics][shapes]")
{
    world w;
    auto body = w.create_body({});

    std::vector<glm::vec2> vertices = {
        { 0.0f, 0.0f },
        { 2.0f, 0.0f },
        { 1.0f, 2.0f }
    };

    auto shape = body->add_polygon(vertices);

    REQUIRE(shape->is_valid());
    REQUIRE(shape->get_type() == shape_type::polygon);
}

TEST_CASE("Capsule shape creation", "[physics][shapes]")
{
    world w;
    auto body = w.create_body({});

    auto shape = body->add_capsule({ 0.0f, -1.0f }, { 0.0f, 1.0f }, 0.5f);

    REQUIRE(shape->is_valid());
    REQUIRE(shape->get_type() == shape_type::capsule);
}

TEST_CASE("Segment shape creation", "[physics][shapes]")
{
    world w;
    auto body = w.create_body({});

    auto shape = body->add_segment({ -5.0f, 0.0f }, { 5.0f, 0.0f });

    REQUIRE(shape->is_valid());
    REQUIRE(shape->get_type() == shape_type::segment);
}

TEST_CASE("Shape material properties", "[physics][shapes]")
{
    world w;
    auto body = w.create_body({});

    shape_def def;
    def.material.friction = 0.8f;
    def.material.restitution = 0.5f;
    def.material.density = 2.0f;

    auto shape = body->add_circle(1.0f, {}, def);

    REQUIRE(shape->get_friction() == Catch::Approx(0.8f));
    REQUIRE(shape->get_restitution() == Catch::Approx(0.5f));
    REQUIRE(shape->get_density() == Catch::Approx(2.0f));
}

TEST_CASE("Shape material modification", "[physics][shapes]")
{
    world w;
    auto body = w.create_body({});
    auto shape = body->add_circle(1.0f);

    shape->set_friction(0.3f);
    REQUIRE(shape->get_friction() == Catch::Approx(0.3f));

    shape->set_restitution(0.9f);
    REQUIRE(shape->get_restitution() == Catch::Approx(0.9f));

    shape->set_density(5.0f);
    REQUIRE(shape->get_density() == Catch::Approx(5.0f));
}

TEST_CASE("Shape collision filter", "[physics][shapes]")
{
    world w;
    auto body = w.create_body({});

    shape_def def;
    def.filter.category_bits = 0x0002;
    def.filter.mask_bits = 0x0004;
    def.filter.group_index = -1;

    auto shape = body->add_circle(1.0f, {}, def);

    collision_filter filter = shape->get_filter();
    REQUIRE(filter.category_bits == 0x0002);
    REQUIRE(filter.mask_bits == 0x0004);
    REQUIRE(filter.group_index == -1);
}

TEST_CASE("Shape filter modification", "[physics][shapes]")
{
    world w;
    auto body = w.create_body({});
    auto shape = body->add_circle(1.0f);

    collision_filter new_filter;
    new_filter.category_bits = 0x0008;
    new_filter.mask_bits = 0x0010;
    new_filter.group_index = 5;

    shape->set_filter(new_filter);

    collision_filter result = shape->get_filter();
    REQUIRE(result.category_bits == 0x0008);
    REQUIRE(result.mask_bits == 0x0010);
    REQUIRE(result.group_index == 5);
}

TEST_CASE("Shape sensor", "[physics][shapes]")
{
    world w;
    auto body = w.create_body({});

    SECTION("Default is not sensor")
    {
        auto shape = body->add_circle(1.0f);
        REQUIRE_FALSE(shape->is_sensor());
    }

    SECTION("Create as sensor")
    {
        shape_def def;
        def.is_sensor = true;
        auto shape = body->add_circle(1.0f, {}, def);
        REQUIRE(shape->is_sensor());
    }
}

TEST_CASE("Shape point test", "[physics][shapes]")
{
    world w;
    auto body = w.create_body({ .position = { 0.0f, 0.0f } });
    auto shape = body->add_circle(1.0f);

    SECTION("Point inside circle")
    {
        REQUIRE(shape->test_point({ 0.0f, 0.0f }));
        REQUIRE(shape->test_point({ 0.5f, 0.0f }));
        REQUIRE(shape->test_point({ 0.0f, 0.5f }));
    }

    SECTION("Point outside circle")
    {
        REQUIRE_FALSE(shape->test_point({ 2.0f, 0.0f }));
        REQUIRE_FALSE(shape->test_point({ 0.0f, 2.0f }));
    }
}

TEST_CASE("Shape user data", "[physics][shapes]")
{
    world w;
    auto body = w.create_body({});
    auto shape = body->add_circle(1.0f);

    SECTION("No user data initially")
    {
        REQUIRE_FALSE(shape->has_user_data());
    }

    SECTION("Set and get user data")
    {
        shape->set_user_data(std::string("test_shape"));
        REQUIRE(shape->has_user_data());
        auto* data = shape->get_user_data<std::string>();
        REQUIRE(data != nullptr);
        REQUIRE(*data == "test_shape");
    }
}

TEST_CASE("Multiple shapes on one body", "[physics][shapes]")
{
    world w;
    auto body = w.create_body({});

    auto circle = body->add_circle(1.0f, { -2.0f, 0.0f });
    auto box = body->add_box(1.0f, 1.0f, { 2.0f, 0.0f });
    auto capsule = body->add_capsule({ 0.0f, -1.0f }, { 0.0f, 1.0f }, 0.5f);

    REQUIRE(body->get_shapes().size() == 3);
    REQUIRE(circle->get_type() == shape_type::circle);
    REQUIRE(box->get_type() == shape_type::polygon);
    REQUIRE(capsule->get_type() == shape_type::capsule);
}
