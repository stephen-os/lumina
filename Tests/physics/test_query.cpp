#include <catch2/catch_all.hpp>
#include <Lumina/Physics/physics.h>

using namespace lumina::physics;

TEST_CASE("Raycast closest hit", "[physics][query]")
{
    world w;

    // Create a wall
    auto wall = w.create_body({ .type = body_type::static_body, .position = { 5.0f, 0.0f } });
    wall->add_box(0.5f, 5.0f);

    SECTION("Ray hits wall")
    {
        auto result = w.ray_cast_closest({ 0.0f, 0.0f }, { 1.0f, 0.0f }, 10.0f);

        REQUIRE(result.hit);
        REQUIRE(result.point.x == Catch::Approx(4.5f).margin(0.1f));
        REQUIRE(result.point.y == Catch::Approx(0.0f).margin(0.1f));
        REQUIRE(result.normal.x == Catch::Approx(-1.0f).margin(0.1f));
    }

    SECTION("Ray misses wall")
    {
        auto result = w.ray_cast_closest({ 0.0f, 0.0f }, { 0.0f, 1.0f }, 10.0f);

        REQUIRE_FALSE(result.hit);
    }

    SECTION("Ray too short")
    {
        auto result = w.ray_cast_closest({ 0.0f, 0.0f }, { 1.0f, 0.0f }, 2.0f);

        REQUIRE_FALSE(result.hit);
    }
}

TEST_CASE("Raycast with multiple targets", "[physics][query]")
{
    world w;

    // Create multiple walls at different distances
    auto wall1 = w.create_body({ .type = body_type::static_body, .position = { 3.0f, 0.0f } });
    wall1->add_box(0.5f, 2.0f);

    auto wall2 = w.create_body({ .type = body_type::static_body, .position = { 6.0f, 0.0f } });
    wall2->add_box(0.5f, 2.0f);

    auto wall3 = w.create_body({ .type = body_type::static_body, .position = { 9.0f, 0.0f } });
    wall3->add_box(0.5f, 2.0f);

    // Raycast should hit closest wall first
    auto result = w.ray_cast_closest({ 0.0f, 0.0f }, { 1.0f, 0.0f }, 20.0f);

    REQUIRE(result.hit);
    REQUIRE(result.point.x == Catch::Approx(2.5f).margin(0.1f));
    REQUIRE(result.fraction < 0.2f); // Should be close to origin
}

TEST_CASE("AABB query", "[physics][query]")
{
    world w;

    // Create bodies at different positions
    auto body1 = w.create_body({ .type = body_type::static_body, .position = { 0.0f, 0.0f } });
    body1->add_circle(1.0f);

    auto body2 = w.create_body({ .type = body_type::static_body, .position = { 5.0f, 0.0f } });
    body2->add_circle(1.0f);

    auto body3 = w.create_body({ .type = body_type::static_body, .position = { 10.0f, 0.0f } });
    body3->add_circle(1.0f);

    SECTION("Query includes some bodies")
    {
        int count = 0;
        query::query_aabb(w, { -2.0f, -2.0f }, { 6.0f, 2.0f },
            [&count](b2ShapeId) {
                ++count;
                return true;
            });

        REQUIRE(count == 2); // body1 and body2 are in range
    }

    SECTION("Query includes all bodies")
    {
        int count = 0;
        query::query_aabb(w, { -5.0f, -5.0f }, { 15.0f, 5.0f },
            [&count](b2ShapeId) {
                ++count;
                return true;
            });

        REQUIRE(count == 3);
    }

    SECTION("Query includes no bodies")
    {
        int count = 0;
        query::query_aabb(w, { 20.0f, 20.0f }, { 25.0f, 25.0f },
            [&count](b2ShapeId) {
                ++count;
                return true;
            });

        REQUIRE(count == 0);
    }
}

TEST_CASE("Circle query", "[physics][query]")
{
    world w;

    // Create bodies at different positions
    auto body1 = w.create_body({ .type = body_type::static_body, .position = { 0.0f, 0.0f } });
    body1->add_circle(0.5f);

    auto body2 = w.create_body({ .type = body_type::static_body, .position = { 2.0f, 0.0f } });
    body2->add_circle(0.5f);

    auto body3 = w.create_body({ .type = body_type::static_body, .position = { 5.0f, 0.0f } });
    body3->add_circle(0.5f);

    SECTION("Circle query finds nearby bodies")
    {
        int count = 0;
        query::query_circle(w, { 0.0f, 0.0f }, 3.0f,
            [&count](b2ShapeId) {
                ++count;
                return true;
            });

        REQUIRE(count == 2); // body1 and body2 are within radius 3
    }

    SECTION("Small circle query")
    {
        int count = 0;
        query::query_circle(w, { 0.0f, 0.0f }, 1.0f,
            [&count](b2ShapeId) {
                ++count;
                return true;
            });

        REQUIRE(count == 1); // Only body1
    }
}

TEST_CASE("Raycast with callback", "[physics][query]")
{
    world w;

    // Create multiple walls
    auto wall1 = w.create_body({ .type = body_type::static_body, .position = { 3.0f, 0.0f } });
    wall1->add_box(0.5f, 2.0f);

    auto wall2 = w.create_body({ .type = body_type::static_body, .position = { 6.0f, 0.0f } });
    wall2->add_box(0.5f, 2.0f);

    SECTION("Collect all hits")
    {
        std::vector<glm::vec2> hit_points;

        query::ray_cast(w, { 0.0f, 0.0f }, { 1.0f, 0.0f }, 20.0f,
            [&hit_points](b2ShapeId, glm::vec2 point, glm::vec2, float) {
                hit_points.push_back(point);
                return 1.0f; // Continue
            });

        REQUIRE(hit_points.size() == 2);
    }

    SECTION("Stop at first hit")
    {
        int hit_count = 0;

        query::ray_cast(w, { 0.0f, 0.0f }, { 1.0f, 0.0f }, 20.0f,
            [&hit_count](b2ShapeId, glm::vec2, glm::vec2, float) {
                ++hit_count;
                return 0.0f; // Stop immediately
            });

        REQUIRE(hit_count == 1);
    }
}

TEST_CASE("Query filter by category", "[physics][query]")
{
    world w;

    // Create body with category 1
    auto body1 = w.create_body({ .type = body_type::static_body, .position = { 3.0f, 0.0f } });
    shape_def def1;
    def1.filter.category_bits = 0x0001;
    body1->add_box(0.5f, 2.0f, {}, 0.0f, def1);

    // Create body with category 2
    auto body2 = w.create_body({ .type = body_type::static_body, .position = { 6.0f, 0.0f } });
    shape_def def2;
    def2.filter.category_bits = 0x0002;
    body2->add_box(0.5f, 2.0f, {}, 0.0f, def2);

    SECTION("Query only category 1")
    {
        int count = 0;
        query_filter filter;
        filter.mask_bits = 0x0001;

        query::query_aabb(w, { -10.0f, -10.0f }, { 10.0f, 10.0f },
            [&count](b2ShapeId) {
                ++count;
                return true;
            }, filter);

        REQUIRE(count == 1);
    }

    SECTION("Query only category 2")
    {
        int count = 0;
        query_filter filter;
        filter.mask_bits = 0x0002;

        query::query_aabb(w, { -10.0f, -10.0f }, { 10.0f, 10.0f },
            [&count](b2ShapeId) {
                ++count;
                return true;
            }, filter);

        REQUIRE(count == 1);
    }

    SECTION("Query both categories")
    {
        int count = 0;
        query_filter filter;
        filter.mask_bits = 0x0003;

        query::query_aabb(w, { -10.0f, -10.0f }, { 10.0f, 10.0f },
            [&count](b2ShapeId) {
                ++count;
                return true;
            }, filter);

        REQUIRE(count == 2);
    }
}
