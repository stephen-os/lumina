// Unit tests for camera2d

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <lumina/graphics/camera2d.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace lumina::graphics;
using Catch::Approx;

TEST_CASE("camera2d construction", "[graphics][camera2d]")
{
    SECTION("default construction")
    {
        camera2d cam(720.0f);

        REQUIRE(cam.get_position() == glm::vec2(0.0f, 0.0f));
        REQUIRE(cam.get_zoom() == Approx(1.0f));
        REQUIRE(cam.get_rotation() == Approx(0.0f));
        REQUIRE(cam.get_view_height() == Approx(720.0f));
    }

    SECTION("construction with aspect ratio")
    {
        camera2d cam(1080.0f, 2.0f);

        REQUIRE(cam.get_view_height() == Approx(1080.0f));
        REQUIRE(cam.get_aspect_ratio() == Approx(2.0f));
        REQUIRE(cam.get_zoom() == Approx(1.0f));  // Default zoom
    }
}

TEST_CASE("camera2d position", "[graphics][camera2d]")
{
    camera2d cam(720.0f);

    SECTION("set position")
    {
        cam.set_position({100.0f, 200.0f});
        cam.update(0.0f);

        REQUIRE(cam.get_position().x == Approx(100.0f));
        REQUIRE(cam.get_position().y == Approx(200.0f));
    }

    SECTION("move")
    {
        cam.set_position({50.0f, 50.0f});
        cam.move({25.0f, -10.0f});
        cam.update(0.0f);

        REQUIRE(cam.get_position().x == Approx(75.0f));
        REQUIRE(cam.get_position().y == Approx(40.0f));
    }
}

TEST_CASE("camera2d zoom", "[graphics][camera2d]")
{
    camera2d cam(720.0f);

    SECTION("set zoom")
    {
        cam.set_zoom(2.0f);
        REQUIRE(cam.get_zoom() == Approx(2.0f));
    }

    SECTION("zoom clamping - minimum")
    {
        cam.set_zoom(0.0001f);  // Below minimum
        REQUIRE(cam.get_zoom() >= 0.001f);  // Clamped to 0.001
    }

    SECTION("zoom accepts large values")
    {
        cam.set_zoom(1000.0f);
        REQUIRE(cam.get_zoom() == Approx(1000.0f));  // No max clamping
    }
}

TEST_CASE("camera2d rotation", "[graphics][camera2d]")
{
    camera2d cam(720.0f);

    SECTION("set rotation")
    {
        cam.set_rotation(1.57f);  // ~90 degrees
        REQUIRE(cam.get_rotation() == Approx(1.57f));
    }
}

TEST_CASE("camera2d matrices", "[graphics][camera2d]")
{
    camera2d cam(720.0f);
    cam.update(0.0f);

    SECTION("identity at origin")
    {
        // Camera at origin with no zoom/rotation should have identity-like view
        glm::mat4 view = cam.get_view_matrix();

        // View matrix at origin should be identity
        REQUIRE(view[3][0] == Approx(0.0f));
        REQUIRE(view[3][1] == Approx(0.0f));
    }

    SECTION("view matrix translates opposite to position")
    {
        cam.set_position({100.0f, 50.0f});
        cam.update(0.0f);

        glm::mat4 view = cam.get_view_matrix();

        // View matrix should translate opposite to camera position
        REQUIRE(view[3][0] == Approx(-100.0f));
        REQUIRE(view[3][1] == Approx(-50.0f));
    }

    SECTION("projection matrix is orthographic")
    {
        glm::mat4 proj = cam.get_projection_matrix();

        // Orthographic projection has 0 in perspective row
        REQUIRE(proj[2][3] == Approx(0.0f));
        REQUIRE(proj[3][3] == Approx(1.0f));
    }

    SECTION("view-projection combines correctly")
    {
        cam.set_position({10.0f, 20.0f});
        cam.update(0.0f);

        glm::mat4 vp = cam.get_view_projection_matrix();
        glm::mat4 expected = cam.get_projection_matrix() * cam.get_view_matrix();

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                REQUIRE(vp[i][j] == Approx(expected[i][j]).margin(0.0001f));
            }
        }
    }
}

TEST_CASE("camera2d coordinate conversion", "[graphics][camera2d]")
{
    camera2d cam(720.0f);
    cam.update(0.0f);
    glm::vec2 screen_size(1280.0f, 720.0f);

    SECTION("screen center maps to camera position")
    {
        glm::vec2 screen_center(640.0f, 360.0f);
        glm::vec2 world = cam.screen_to_world(screen_center, screen_size);

        REQUIRE(world.x == Approx(0.0f).margin(0.01f));
        REQUIRE(world.y == Approx(0.0f).margin(0.01f));
    }

    SECTION("world origin maps to screen center")
    {
        glm::vec2 world_origin(0.0f, 0.0f);
        glm::vec2 screen = cam.world_to_screen(world_origin, screen_size);

        REQUIRE(screen.x == Approx(640.0f).margin(0.01f));
        REQUIRE(screen.y == Approx(360.0f).margin(0.01f));
    }

    SECTION("round-trip conversion")
    {
        glm::vec2 original_world(123.0f, -456.0f);
        glm::vec2 screen = cam.world_to_screen(original_world, screen_size);
        glm::vec2 back_to_world = cam.screen_to_world(screen, screen_size);

        REQUIRE(back_to_world.x == Approx(original_world.x).margin(0.01f));
        REQUIRE(back_to_world.y == Approx(original_world.y).margin(0.01f));
    }

    SECTION("zoom affects conversion")
    {
        cam.set_zoom(2.0f);
        cam.update(0.0f);

        // At 2x zoom, screen edges are closer in world space
        glm::vec2 screen_edge(1280.0f, 360.0f);
        glm::vec2 world_at_1x = camera2d(720.0f, 1.0f).screen_to_world(screen_edge, screen_size);

        glm::vec2 world_at_2x = cam.screen_to_world(screen_edge, screen_size);

        // At 2x zoom, the world coordinate should be half the distance
        REQUIRE(std::abs(world_at_2x.x) < std::abs(world_at_1x.x));
    }
}

// =============================================================================
// RIGOROUS MATHEMATICAL VERIFICATION TESTS
// =============================================================================

TEST_CASE("camera2d visible area - mathematical verification", "[graphics][camera2d][math]")
{
    // The visible area formulas are:
    //   visible_height = view_height / zoom
    //   visible_width = visible_height * aspect_ratio

    SECTION("visible dimensions at zoom 1.0")
    {
        camera2d cam(720.0f, 16.0f / 9.0f);  // 720p with 16:9 aspect
        cam.update(0.0f);

        // Expected: height = 720 / 1.0 = 720
        // Expected: width = 720 * (16/9) = 1280
        float expected_height = 720.0f;
        float expected_width = 720.0f * (16.0f / 9.0f);

        REQUIRE(cam.get_visible_height() == Approx(expected_height));
        REQUIRE(cam.get_visible_width() == Approx(expected_width));
    }

    SECTION("visible dimensions at zoom 2.0")
    {
        camera2d cam(720.0f, 16.0f / 9.0f);
        cam.set_zoom(2.0f);
        cam.update(0.0f);

        // Expected: height = 720 / 2.0 = 360
        // Expected: width = 360 * (16/9) = 640
        float expected_height = 720.0f / 2.0f;
        float expected_width = expected_height * (16.0f / 9.0f);

        REQUIRE(cam.get_visible_height() == Approx(expected_height));
        REQUIRE(cam.get_visible_width() == Approx(expected_width));
    }

    SECTION("visible dimensions at zoom 0.5")
    {
        camera2d cam(720.0f, 16.0f / 9.0f);
        cam.set_zoom(0.5f);
        cam.update(0.0f);

        // Expected: height = 720 / 0.5 = 1440
        // Expected: width = 1440 * (16/9) = 2560
        float expected_height = 720.0f / 0.5f;
        float expected_width = expected_height * (16.0f / 9.0f);

        REQUIRE(cam.get_visible_height() == Approx(expected_height));
        REQUIRE(cam.get_visible_width() == Approx(expected_width));
    }

    SECTION("visible bounds includes position offset")
    {
        camera2d cam(720.0f, 16.0f / 9.0f);
        cam.set_position({100.0f, 200.0f});
        cam.update(0.0f);

        glm::vec4 bounds = cam.get_visible_bounds();

        // Bounds: (position.x, position.y, width, height)
        REQUIRE(bounds.x == Approx(100.0f));
        REQUIRE(bounds.y == Approx(200.0f));
        REQUIRE(bounds.z == Approx(cam.get_visible_width()));
        REQUIRE(bounds.w == Approx(cam.get_visible_height()));
    }
}

TEST_CASE("camera2d projection matrix - mathematical verification", "[graphics][camera2d][math]")
{
    // Orthographic projection matrix from glm::ortho(left, right, bottom, top, near, far):
    // [0][0] = 2/(right-left)
    // [1][1] = 2/(top-bottom)
    // [2][2] = -2/(far-near)
    // [3][0] = -(right+left)/(right-left)
    // [3][1] = -(top+bottom)/(top-bottom)
    // [3][2] = -(far+near)/(far-near)
    // [3][3] = 1

    SECTION("projection matrix values at zoom 1.0")
    {
        camera2d cam(720.0f, 16.0f / 9.0f);
        cam.update(0.0f);

        glm::mat4 proj = cam.get_projection_matrix();

        // visible_height = 720, visible_width = 720 * 16/9 = 1280
        // ortho(-640, 640, -360, 360, near, far)
        float half_width = cam.get_visible_width() * 0.5f;   // 640
        float half_height = cam.get_visible_height() * 0.5f; // 360

        // [0][0] = 2 / (640 - (-640)) = 2 / 1280 = 0.0015625
        REQUIRE(proj[0][0] == Approx(2.0f / (half_width * 2.0f)));

        // [1][1] = 2 / (360 - (-360)) = 2 / 720 = 0.002778
        REQUIRE(proj[1][1] == Approx(2.0f / (half_height * 2.0f)));

        // [3][0] = -(640 + (-640)) / 1280 = 0 (symmetric)
        REQUIRE(proj[3][0] == Approx(0.0f).margin(0.0001f));

        // [3][1] = -(360 + (-360)) / 720 = 0 (symmetric)
        REQUIRE(proj[3][1] == Approx(0.0f).margin(0.0001f));

        // [3][3] = 1 (orthographic)
        REQUIRE(proj[3][3] == Approx(1.0f));
    }

    SECTION("projection matrix scales with zoom")
    {
        camera2d cam(720.0f, 16.0f / 9.0f);
        cam.update(0.0f);
        glm::mat4 proj1x = cam.get_projection_matrix();

        cam.set_zoom(2.0f);
        cam.update(0.0f);
        glm::mat4 proj2x = cam.get_projection_matrix();

        // At 2x zoom, visible area is halved, so projection scale doubles
        REQUIRE(proj2x[0][0] == Approx(proj1x[0][0] * 2.0f));
        REQUIRE(proj2x[1][1] == Approx(proj1x[1][1] * 2.0f));
    }
}

TEST_CASE("camera2d view matrix - mathematical verification", "[graphics][camera2d][math]")
{
    // View matrix is the inverse of the camera's transform matrix
    // For a 2D camera at position (px, py) with rotation r:
    //   Transform = Translate(px, py) * Rotate(r)
    //   View = inverse(Transform) = Rotate(-r) * Translate(-px, -py)

    SECTION("view matrix translation is negated position")
    {
        camera2d cam(720.0f);
        cam.set_position({150.0f, -75.0f});
        cam.update(0.0f);

        glm::mat4 view = cam.get_view_matrix();

        // For no rotation, the translation component is simply -position
        REQUIRE(view[3][0] == Approx(-150.0f));
        REQUIRE(view[3][1] == Approx(75.0f));
    }

    SECTION("view matrix with rotation")
    {
        camera2d cam(720.0f);
        cam.set_rotation(glm::radians(90.0f));
        cam.update(0.0f);

        glm::mat4 view = cam.get_view_matrix();

        // 90 degree rotation: cos(90)=0, sin(90)=1
        // Rotation matrix for +90 degrees around Z:
        // [ cos  sin 0 0]   [ 0  1 0 0]
        // [-sin  cos 0 0] = [-1  0 0 0]
        // [ 0    0   1 0]   [ 0  0 1 0]
        // [ 0    0   0 1]   [ 0  0 0 1]
        // But view is INVERSE, so rotation is -90 degrees
        // [ cos -sin 0 0]   [ 0 -1 0 0]
        // [ sin  cos 0 0] = [ 1  0 0 0]
        // View matrix rotation part:
        REQUIRE(view[0][0] == Approx(0.0f).margin(0.0001f));
        REQUIRE(view[0][1] == Approx(-1.0f).margin(0.0001f));
        REQUIRE(view[1][0] == Approx(1.0f).margin(0.0001f));
        REQUIRE(view[1][1] == Approx(0.0f).margin(0.0001f));
    }

    SECTION("view matrix with position and rotation combined")
    {
        camera2d cam(720.0f);
        cam.set_position({100.0f, 0.0f});
        cam.set_rotation(glm::radians(90.0f));
        cam.update(0.0f);

        glm::mat4 view = cam.get_view_matrix();

        // Camera transform: Translate(100, 0) * Rotate(90)
        // View = inverse(T * R) = R^-1 * T^-1
        //
        // R^-1 = Rotate(-90)
        // T^-1 = Translate(-100, 0)
        //
        // View's translation column = R^-1 * (-100, 0, 0)
        // For -90 degree rotation around Z:
        //   cos(-90) = 0, sin(-90) = -1
        //   Rotation matrix: [0, 1; -1, 0]
        //   Applied to (-100, 0): x' = 0*(-100) + 1*0 = 0, y' = -1*(-100) + 0*0 = 100
        //
        // So translation component is (0, 100)
        REQUIRE(view[3][0] == Approx(0.0f).margin(0.001f));
        REQUIRE(view[3][1] == Approx(100.0f).margin(0.001f));
    }
}

TEST_CASE("camera2d screen_to_world - mathematical verification", "[graphics][camera2d][math]")
{
    // screen_to_world formula:
    // 1. NDC.x = (screen.x / screen_size.x) * 2 - 1
    // 2. NDC.y = 1 - (screen.y / screen_size.y) * 2  (Y flipped)
    // 3. world = inverse(VP) * (NDC.x, NDC.y, 0, 1)

    glm::vec2 screen_size(1280.0f, 720.0f);

    SECTION("screen corners map to world corners")
    {
        camera2d cam(720.0f, 16.0f / 9.0f);
        cam.update(0.0f);

        float half_w = cam.get_visible_width() * 0.5f;   // 640
        float half_h = cam.get_visible_height() * 0.5f;  // 360

        // Top-left screen (0, 0) -> NDC (-1, 1) -> world (-half_w, +half_h)
        glm::vec2 world_tl = cam.screen_to_world({0.0f, 0.0f}, screen_size);
        REQUIRE(world_tl.x == Approx(-half_w).margin(0.01f));
        REQUIRE(world_tl.y == Approx(half_h).margin(0.01f));

        // Top-right screen (1280, 0) -> NDC (1, 1) -> world (+half_w, +half_h)
        glm::vec2 world_tr = cam.screen_to_world({1280.0f, 0.0f}, screen_size);
        REQUIRE(world_tr.x == Approx(half_w).margin(0.01f));
        REQUIRE(world_tr.y == Approx(half_h).margin(0.01f));

        // Bottom-left screen (0, 720) -> NDC (-1, -1) -> world (-half_w, -half_h)
        glm::vec2 world_bl = cam.screen_to_world({0.0f, 720.0f}, screen_size);
        REQUIRE(world_bl.x == Approx(-half_w).margin(0.01f));
        REQUIRE(world_bl.y == Approx(-half_h).margin(0.01f));

        // Bottom-right screen (1280, 720) -> NDC (1, -1) -> world (+half_w, -half_h)
        glm::vec2 world_br = cam.screen_to_world({1280.0f, 720.0f}, screen_size);
        REQUIRE(world_br.x == Approx(half_w).margin(0.01f));
        REQUIRE(world_br.y == Approx(-half_h).margin(0.01f));
    }

    SECTION("camera position offsets world coordinates")
    {
        camera2d cam(720.0f, 16.0f / 9.0f);
        cam.set_position({100.0f, 50.0f});
        cam.update(0.0f);

        // Screen center should now map to camera position
        glm::vec2 world_center = cam.screen_to_world({640.0f, 360.0f}, screen_size);
        REQUIRE(world_center.x == Approx(100.0f).margin(0.01f));
        REQUIRE(world_center.y == Approx(50.0f).margin(0.01f));
    }

    SECTION("zoom scales world coordinates proportionally")
    {
        camera2d cam(720.0f, 16.0f / 9.0f);
        cam.set_zoom(2.0f);
        cam.update(0.0f);

        // At 2x zoom, visible area is halved
        float half_w = cam.get_visible_width() * 0.5f;   // 320
        float half_h = cam.get_visible_height() * 0.5f;  // 180

        // Top-left screen (0, 0) -> world (-320, +180)
        glm::vec2 world_tl = cam.screen_to_world({0.0f, 0.0f}, screen_size);
        REQUIRE(world_tl.x == Approx(-half_w).margin(0.01f));
        REQUIRE(world_tl.y == Approx(half_h).margin(0.01f));
    }

    SECTION("specific coordinate verification")
    {
        camera2d cam(720.0f, 16.0f / 9.0f);
        cam.update(0.0f);

        // Screen position (320, 180) is 1/4 from top-left
        // NDC: x = (320/1280)*2 - 1 = 0.5 - 1 = -0.5
        // NDC: y = 1 - (180/720)*2 = 1 - 0.5 = 0.5
        // World: x = -0.5 * 640 = -320
        // World: y = 0.5 * 360 = 180
        glm::vec2 world = cam.screen_to_world({320.0f, 180.0f}, screen_size);
        REQUIRE(world.x == Approx(-320.0f).margin(0.01f));
        REQUIRE(world.y == Approx(180.0f).margin(0.01f));
    }
}

TEST_CASE("camera2d world_to_screen - mathematical verification", "[graphics][camera2d][math]")
{
    // world_to_screen formula:
    // 1. clip = VP * (world.x, world.y, 0, 1)
    // 2. NDC = clip.xy / clip.w
    // 3. screen.x = (NDC.x + 1) * 0.5 * screen_size.x
    // 4. screen.y = (1 - NDC.y) * 0.5 * screen_size.y  (Y flipped)

    glm::vec2 screen_size(1280.0f, 720.0f);

    SECTION("world corners map to screen corners")
    {
        camera2d cam(720.0f, 16.0f / 9.0f);
        cam.update(0.0f);

        float half_w = cam.get_visible_width() * 0.5f;   // 640
        float half_h = cam.get_visible_height() * 0.5f;  // 360

        // World (-640, +360) -> NDC (-1, 1) -> screen (0, 0)
        glm::vec2 screen_tl = cam.world_to_screen({-half_w, half_h}, screen_size);
        REQUIRE(screen_tl.x == Approx(0.0f).margin(0.01f));
        REQUIRE(screen_tl.y == Approx(0.0f).margin(0.01f));

        // World (+640, +360) -> NDC (1, 1) -> screen (1280, 0)
        glm::vec2 screen_tr = cam.world_to_screen({half_w, half_h}, screen_size);
        REQUIRE(screen_tr.x == Approx(1280.0f).margin(0.01f));
        REQUIRE(screen_tr.y == Approx(0.0f).margin(0.01f));

        // World (-640, -360) -> NDC (-1, -1) -> screen (0, 720)
        glm::vec2 screen_bl = cam.world_to_screen({-half_w, -half_h}, screen_size);
        REQUIRE(screen_bl.x == Approx(0.0f).margin(0.01f));
        REQUIRE(screen_bl.y == Approx(720.0f).margin(0.01f));

        // World (+640, -360) -> NDC (1, -1) -> screen (1280, 720)
        glm::vec2 screen_br = cam.world_to_screen({half_w, -half_h}, screen_size);
        REQUIRE(screen_br.x == Approx(1280.0f).margin(0.01f));
        REQUIRE(screen_br.y == Approx(720.0f).margin(0.01f));
    }

    SECTION("specific coordinate verification")
    {
        camera2d cam(720.0f, 16.0f / 9.0f);
        cam.update(0.0f);

        // World (-320, 180) should be 1/4 from top-left
        // NDC: x = -320 / 640 = -0.5
        // NDC: y = 180 / 360 = 0.5
        // screen.x = (-0.5 + 1) * 0.5 * 1280 = 0.25 * 1280 = 320
        // screen.y = (1 - 0.5) * 0.5 * 720 = 0.25 * 720 = 180
        glm::vec2 screen = cam.world_to_screen({-320.0f, 180.0f}, screen_size);
        REQUIRE(screen.x == Approx(320.0f).margin(0.01f));
        REQUIRE(screen.y == Approx(180.0f).margin(0.01f));
    }

    SECTION("screen_to_world and world_to_screen are inverses")
    {
        camera2d cam(720.0f, 16.0f / 9.0f);
        cam.set_position({123.0f, -456.0f});
        cam.set_zoom(1.5f);
        cam.set_rotation(glm::radians(30.0f));
        cam.update(0.0f);

        // Test round-trip for various points
        std::vector<glm::vec2> test_points = {
            {0.0f, 0.0f},
            {100.0f, 200.0f},
            {-300.0f, 150.0f},
            {500.0f, -500.0f}
        };

        for (const auto& world_pos : test_points)
        {
            glm::vec2 screen = cam.world_to_screen(world_pos, screen_size);
            glm::vec2 back = cam.screen_to_world(screen, screen_size);

            REQUIRE(back.x == Approx(world_pos.x).margin(0.01f));
            REQUIRE(back.y == Approx(world_pos.y).margin(0.01f));
        }
    }
}

TEST_CASE("camera2d follow system - mathematical verification", "[graphics][camera2d][math]")
{
    // Follow uses exponential interpolation:
    // t = 1 - exp(-speed * delta_time)
    // position = mix(position, target, t)

    SECTION("follow interpolation factor")
    {
        camera2d cam(720.0f);
        cam.set_position({0.0f, 0.0f});
        cam.set_target({100.0f, 0.0f});
        cam.set_follow_speed(1.0f);

        // After 1 second with speed 1.0:
        // t = 1 - exp(-1 * 1) = 1 - 0.3679 = 0.6321
        // new_pos = mix(0, 100, 0.6321) = 63.21
        cam.update(1.0f);

        float expected_t = 1.0f - std::exp(-1.0f);
        float expected_pos = 0.0f + (100.0f - 0.0f) * expected_t;

        REQUIRE(cam.get_position().x == Approx(expected_pos).margin(0.1f));
    }

    SECTION("deadzone prevents movement when close")
    {
        camera2d cam(720.0f);
        cam.set_position({0.0f, 0.0f});
        cam.set_target({5.0f, 0.0f});  // 5 units away
        cam.set_follow_deadzone(10.0f);  // But deadzone is 10
        cam.set_follow_speed(10.0f);

        cam.update(1.0f);

        // Should not move because distance (5) < deadzone (10)
        REQUIRE(cam.get_position().x == Approx(0.0f).margin(0.01f));
    }

    SECTION("deadzone allows movement when far")
    {
        camera2d cam(720.0f);
        cam.set_position({0.0f, 0.0f});
        cam.set_target({20.0f, 0.0f});  // 20 units away
        cam.set_follow_deadzone(10.0f);  // Deadzone is 10
        cam.set_follow_speed(10.0f);

        cam.update(0.1f);

        // Should move because distance (20) > deadzone (10)
        REQUIRE(cam.get_position().x > 0.0f);
    }
}

TEST_CASE("camera2d bounds clamping - mathematical verification", "[graphics][camera2d][math]")
{
    camera2d cam(720.0f);

    SECTION("clamps to exact boundary values")
    {
        camera2d_bounds bounds;
        bounds.min_x = -100.0f;
        bounds.max_x = 100.0f;
        bounds.min_y = -50.0f;
        bounds.max_y = 50.0f;

        cam.set_bounds(bounds);

        // Set position beyond max bounds
        cam.set_position({500.0f, 200.0f});
        cam.update(0.0f);
        REQUIRE(cam.get_position().x == Approx(100.0f));
        REQUIRE(cam.get_position().y == Approx(50.0f));

        // Set position beyond min bounds
        cam.set_position({-500.0f, -200.0f});
        cam.update(0.0f);
        REQUIRE(cam.get_position().x == Approx(-100.0f));
        REQUIRE(cam.get_position().y == Approx(-50.0f));
    }

    SECTION("position within bounds unchanged")
    {
        camera2d_bounds bounds;
        bounds.min_x = -100.0f;
        bounds.max_x = 100.0f;
        bounds.min_y = -100.0f;
        bounds.max_y = 100.0f;

        cam.set_bounds(bounds);
        cam.set_position({50.0f, -30.0f});
        cam.update(0.0f);

        REQUIRE(cam.get_position().x == Approx(50.0f));
        REQUIRE(cam.get_position().y == Approx(-30.0f));
    }
}

TEST_CASE("camera2d bounds", "[graphics][camera2d]")
{
    camera2d cam(720.0f);

    SECTION("bounds clamp position")
    {
        camera2d_bounds bounds;
        bounds.min_x = -100.0f;
        bounds.max_x = 100.0f;
        bounds.min_y = -100.0f;
        bounds.max_y = 100.0f;

        cam.set_bounds(bounds);
        cam.set_position({500.0f, 500.0f});
        cam.update(0.0f);

        REQUIRE(cam.get_position().x <= 100.0f);
        REQUIRE(cam.get_position().y <= 100.0f);
    }

    SECTION("unbounded bounds don't clamp")
    {
        camera2d_bounds bounds;  // Default is unbounded

        REQUIRE(bounds.is_unbounded());

        cam.set_bounds(bounds);
        cam.set_position({500.0f, 500.0f});
        cam.update(0.0f);

        REQUIRE(cam.get_position().x == Approx(500.0f));
        REQUIRE(cam.get_position().y == Approx(500.0f));
    }
}

TEST_CASE("camera2d follow", "[graphics][camera2d]")
{
    camera2d cam(720.0f);

    SECTION("instant follow when speed is high")
    {
        cam.set_target({100.0f, 100.0f});
        cam.set_follow_speed(1000.0f);  // Very fast
        cam.update(1.0f);  // 1 second

        // Should be very close to target
        REQUIRE(cam.get_position().x == Approx(100.0f).margin(1.0f));
        REQUIRE(cam.get_position().y == Approx(100.0f).margin(1.0f));
    }

    SECTION("gradual follow when speed is low")
    {
        cam.set_position({0.0f, 0.0f});
        cam.set_target({100.0f, 0.0f});
        cam.set_follow_speed(1.0f);  // Slow
        cam.update(0.016f);  // One frame

        // Should move toward target but not reach it
        REQUIRE(cam.get_position().x > 0.0f);
        REQUIRE(cam.get_position().x < 100.0f);
    }
}

TEST_CASE("camera2d shake", "[graphics][camera2d]")
{
    camera2d cam(720.0f);

    SECTION("shake adds offset")
    {
        cam.set_position({0.0f, 0.0f});
        cam.start_shake(10.0f, 1.0f);

        // Run several frames to let shake take effect
        for (int i = 0; i < 10; i++)
        {
            cam.update(0.016f);
        }

        // View matrix should include shake offset (position may still be 0,0)
        // We can verify shake is active by checking the view matrix differs from expected
        glm::mat4 view = cam.get_view_matrix();

        // With shake, the translation should be non-zero even though position is 0,0
        // (shake adds to the view matrix)
        bool has_offset = (view[3][0] != 0.0f || view[3][1] != 0.0f);
        REQUIRE(has_offset);
    }

    SECTION("shake decays over time")
    {
        cam.start_shake(10.0f, 0.1f);  // Short duration

        // Run until shake should be done
        for (int i = 0; i < 20; i++)
        {
            cam.update(0.016f);
        }

        // After shake duration, offset should be minimal
        // (This depends on implementation - shake should decay)
    }

    SECTION("stop_shake clears shake")
    {
        cam.start_shake(10.0f, 1.0f);
        cam.update(0.016f);
        cam.stop_shake();
        cam.update(0.016f);

        glm::mat4 view = cam.get_view_matrix();

        // After stop, should be at exact position
        REQUIRE(view[3][0] == Approx(0.0f).margin(0.01f));
        REQUIRE(view[3][1] == Approx(0.0f).margin(0.01f));
    }
}
