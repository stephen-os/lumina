// 00-profiler-test: Tracy Profiler Integration Test
// Demonstrates: Tracy profiling integration with ImGui debug overlay

#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/graphics/graphics.h>
#include <lumina/ui/ui.h>

#include <glm/glm.hpp>
#include <cmath>
#include <deque>
#include <random>
#include <chrono>

namespace ui = lumina::ui;
namespace gfx = lumina::graphics;

class profiler_test_layer : public lumina::core::layer
{
public:
    profiler_test_layer() : layer("profiler_test") {}

    void on_attach() override
    {
        auto& device = lumina::core::application::get().get_device();
        m_renderer = std::make_unique<gfx::renderer2d>(device);
        m_renderer->init();

        // Create render target
        m_render_target = gfx::render_target::create(
            device, 1280, 720, gfx::format::rgba8_unorm
        );

        // Initialize camera
        m_camera = gfx::camera2d(720.0f, 1.0f);
        m_camera.set_position({0.0f, 0.0f});
        m_camera.update(0.0f);

        // Initialize random positions
        init_random_positions();
    }

    void on_detach() override
    {
        m_render_target.reset();
        m_renderer.reset();
    }

    void on_update(float dt) override
    {
        LUMINA_PROFILE_SCOPE_NC("ProfilerTest::Update", 0x44FF44);

        m_time += dt;
        m_camera.update(dt);

        // Track frame times for display
        m_frame_times.push_back(dt * 1000.0f);
        if (m_frame_times.size() > 120)
            m_frame_times.pop_front();
    }

    void on_render() override
    {
        LUMINA_PROFILE_SCOPE_NC("ProfilerTest::Render", 0x44FF44);

        if (!m_renderer || !m_render_target) return;

        auto start = std::chrono::high_resolution_clock::now();

        m_renderer->begin(m_camera);
        m_renderer->set_render_target(m_render_target);
        m_renderer->clear({0.05f, 0.05f, 0.08f, 1.0f});

        // Run selected stress test
        run_stress_test();

        m_renderer->end();

        auto end = std::chrono::high_resolution_clock::now();
        m_render_time_ms = std::chrono::duration<float, std::milli>(end - start).count();

        // Cache stats before reset
        m_last_stats = m_renderer->get_stats();
        m_renderer->reset_stats();

        render_ui();
    }

private:
    void init_random_positions()
    {
        std::mt19937 rng(42);
        std::uniform_real_distribution<float> dist_x(-600.0f, 600.0f);
        std::uniform_real_distribution<float> dist_y(-350.0f, 350.0f);

        m_positions.resize(m_max_primitives);
        for (auto& pos : m_positions)
        {
            pos = {dist_x(rng), dist_y(rng)};
        }
    }

    void run_stress_test()
    {
        LUMINA_PROFILE_SCOPE_NC("ProfilerTest::StressTest", 0x44FF44);

        if (m_enable_quads)
        {
            LUMINA_PROFILE_SCOPE_NC("ProfilerTest::DrawQuads", 0x44FF44);
            for (int i = 0; i < m_quad_count; i++)
            {
                m_renderer->draw_quad({
                    .position = {m_positions[i].x, m_positions[i].y, 0.0f},
                    .size = {m_primitive_size, m_primitive_size},
                    .color = {0.8f, 0.3f, 0.3f, 0.8f}
                });
            }
        }

        if (m_enable_circles)
        {
            LUMINA_PROFILE_SCOPE_NC("ProfilerTest::DrawCircles", 0x44FF44);
            int offset = m_max_primitives / 4;
            for (int i = 0; i < m_circle_count; i++)
            {
                int idx = (offset + i) % m_max_primitives;
                m_renderer->draw_circle({
                    .position = {m_positions[idx].x, m_positions[idx].y, 0.0f},
                    .radius = {m_primitive_size / 2, m_primitive_size / 2},
                    .color = {0.3f, 0.8f, 0.3f, 0.8f}
                });
            }
        }

        if (m_enable_lines)
        {
            LUMINA_PROFILE_SCOPE_NC("ProfilerTest::DrawLines", 0x44FF44);
            int offset = m_max_primitives / 2;
            for (int i = 0; i < m_line_count; i++)
            {
                int idx = (offset + i) % m_max_primitives;
                m_renderer->draw_line({
                    .start = {m_positions[idx].x, m_positions[idx].y, 0.0f},
                    .end = {m_positions[idx].x + 50.0f, m_positions[idx].y + 30.0f, 0.0f},
                    .color = {0.3f, 0.3f, 0.8f, 0.8f},
                    .thickness = 2.0f
                });
            }
        }

        if (m_enable_text)
        {
            LUMINA_PROFILE_SCOPE_NC("ProfilerTest::DrawText", 0x44FF44);
            int offset = (m_max_primitives * 3) / 4;
            for (int i = 0; i < m_text_count; i++)
            {
                int idx = (offset + i) % m_max_primitives;
                m_renderer->draw_text({
                    .text = "Test",
                    .position = {m_positions[idx].x, m_positions[idx].y, 0.0f},
                    .scale = 0.5f,
                    .color = {0.8f, 0.8f, 0.3f, 0.8f}
                });
            }
        }
    }

    void render_ui()
    {
        ui::begin_window("Profiler Test Controls");

        // Tracy connection info
        ui::text("Tracy Profiler Integration Test");
        ui::separator();

#ifdef TRACY_ENABLE
        ui::text("Tracy Status: ENABLED");
        ui::text("Connect Tracy Profiler to view detailed zones.");
#else
        ui::text("Tracy Status: DISABLED");
        ui::text("Build with TRACY_ENABLE to enable profiling.");
#endif
        ui::separator();

        // Frame time graph
        ui::text("Frame Time History:");
        if (!m_frame_times.empty())
        {
            std::vector<float> times(m_frame_times.begin(), m_frame_times.end());
            ImGui::PlotLines("##frametime", times.data(), static_cast<int>(times.size()),
                0, nullptr, 0.0f, 33.3f, ImVec2(0, 60));
        }

        // Performance metrics
        float avg_frame_time = 0.0f;
        if (!m_frame_times.empty())
        {
            for (float t : m_frame_times)
                avg_frame_time += t;
            avg_frame_time /= m_frame_times.size();
        }
        float fps = avg_frame_time > 0 ? 1000.0f / avg_frame_time : 0.0f;

        ui::text_fmt("FPS: {:.1f}", fps);
        ui::text_fmt("Frame Time: {:.2f} ms", avg_frame_time);
        ui::text_fmt("Render Time: {:.2f} ms", m_render_time_ms);
        ui::separator();

        // Renderer stats
        ui::text("Renderer Stats:");
        ui::text_fmt("Draw Calls: {}", m_last_stats.draw_calls);
        ui::text_fmt("Total Primitives: {}", m_last_stats.get_total_primitives());
        ui::text_fmt("Quads: {}", m_last_stats.quad_count);
        ui::text_fmt("Circles: {}", m_last_stats.circle_count);
        ui::text_fmt("Lines: {}", m_last_stats.line_count);
        ui::text_fmt("Text Chars: {}", m_last_stats.text_char_count);
        ui::separator();

        // Stress test controls
        ui::text("Stress Test Controls:");

        ImGui::Checkbox("Quads", &m_enable_quads);
        if (m_enable_quads)
        {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(150);
            ImGui::SliderInt("##quads", &m_quad_count, 0, m_max_primitives / 4);
        }

        ImGui::Checkbox("Circles", &m_enable_circles);
        if (m_enable_circles)
        {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(150);
            ImGui::SliderInt("##circles", &m_circle_count, 0, m_max_primitives / 4);
        }

        ImGui::Checkbox("Lines", &m_enable_lines);
        if (m_enable_lines)
        {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(150);
            ImGui::SliderInt("##lines", &m_line_count, 0, m_max_primitives / 4);
        }

        ImGui::Checkbox("Text", &m_enable_text);
        if (m_enable_text)
        {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(150);
            ImGui::SliderInt("##text", &m_text_count, 0, 500);
        }

        ui::separator();
        ImGui::SliderFloat("Primitive Size", &m_primitive_size, 5.0f, 50.0f);

        ui::separator();
        if (ImGui::Button("Reset to Defaults"))
        {
            m_enable_quads = true;
            m_enable_circles = true;
            m_enable_lines = true;
            m_enable_text = false;
            m_quad_count = 1000;
            m_circle_count = 500;
            m_line_count = 500;
            m_text_count = 100;
            m_primitive_size = 20.0f;
        }

        ui::end_window();

        // Viewport
        ui::push_style_var(ImGuiStyleVar_WindowPadding, glm::vec2(0, 0));
        ui::begin_window("Viewport");

        auto content_size = ui::get_content_size();
        if (auto tex = m_render_target->get_color_texture())
        {
            ui::image(tex->get_texture(), content_size);
        }

        ui::end_window();
        ui::pop_style_var();
    }

private:
    lumina::scope<gfx::renderer2d> m_renderer;
    lumina::ref<gfx::render_target> m_render_target;
    gfx::camera2d m_camera;

    float m_time = 0.0f;
    std::deque<float> m_frame_times;
    float m_render_time_ms = 0.0f;
    gfx::renderer2d_stats m_last_stats;

    // Random positions for primitives
    static constexpr int m_max_primitives = 20000;
    std::vector<glm::vec2> m_positions;

    // Stress test controls
    bool m_enable_quads = true;
    bool m_enable_circles = true;
    bool m_enable_lines = true;
    bool m_enable_text = false;
    int m_quad_count = 1000;
    int m_circle_count = 500;
    int m_line_count = 500;
    int m_text_count = 100;
    float m_primitive_size = 20.0f;
};

lumina::core::application* lumina::core::create_application(int argc, char** argv)
{
    application_specifications specs;
    specs.title = "application/00-profiler-test";
    auto* app = new application(specs);
    app->push_layer<profiler_test_layer>();
    return app;
}
