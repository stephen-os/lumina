// 11-benchmark: Performance Benchmark Suite
// Stress tests for renderer2d batching, draw calls, and throughput

#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/core/input.h>
#include <lumina/graphics/graphics.h>
#include <lumina/ui/ui.h>

#include <glm/glm.hpp>
#include <cmath>
#include <random>
#include <chrono>
#include <numeric>
#include <deque>

namespace ui = lumina::ui;
namespace gfx = lumina::graphics;
namespace input = lumina::core::input;

// Test scenarios
enum class benchmark_test
{
    quads_same_texture,         // Best case: all quads use same texture
    quads_32_textures,          // Texture array: 32 different textures
    quads_alternating_blend,    // Worst case: blend mode changes every quad
    quads_batched_blend,        // Grouped by blend mode
    mixed_primitives,           // Quads + circles + lines + text
    scissor_regions,            // Multiple scissor regions
    stress_max_primitives,      // Maximum primitive count
    count
};

const char* get_test_name(benchmark_test test)
{
    switch (test)
    {
        case benchmark_test::quads_same_texture:      return "Quads (Same Texture)";
        case benchmark_test::quads_32_textures:       return "Quads (32 Textures)";
        case benchmark_test::quads_alternating_blend: return "Quads (Alternating Blend)";
        case benchmark_test::quads_batched_blend:     return "Quads (Batched Blend)";
        case benchmark_test::mixed_primitives:        return "Mixed Primitives";
        case benchmark_test::scissor_regions:         return "Scissor Regions";
        case benchmark_test::stress_max_primitives:   return "Stress Test (Max)";
        default: return "Unknown";
    }
}

struct test_result
{
    benchmark_test test;
    float avg_fps;
    float min_frame_time;
    float max_frame_time;
    float avg_frame_time;
    float avg_render_time;
    uint32_t avg_draw_calls;
    uint32_t total_primitives;
    float prims_per_call;
};

class benchmark_layer : public lumina::core::layer
{
public:
    benchmark_layer() : layer("benchmark") {}

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

        // Create test textures (simple colored textures)
        create_test_textures(device);

        // Initialize random positions for primitives
        init_random_positions();
    }

    void on_detach() override
    {
        m_test_textures.clear();
        m_render_target.reset();
        m_renderer.reset();
    }

    void on_update(float dt) override
    {
        m_time += dt;
        m_camera.update(dt);

        // Track frame times
        m_frame_times.push_back(dt * 1000.0f); // Convert to ms
        if (m_frame_times.size() > 120) // Keep last 120 frames
            m_frame_times.pop_front();
    }

    void on_render() override
    {
        if (!m_renderer || !m_render_target) return;

        auto start_time = std::chrono::high_resolution_clock::now();

        m_renderer->begin(m_camera);
        m_renderer->set_render_target(m_render_target);
        m_renderer->clear({0.05f, 0.05f, 0.08f, 1.0f});

        // Run current test
        switch (m_current_test)
        {
            case benchmark_test::quads_same_texture:
                run_quads_same_texture();
                break;
            case benchmark_test::quads_32_textures:
                run_quads_32_textures();
                break;
            case benchmark_test::quads_alternating_blend:
                run_quads_alternating_blend();
                break;
            case benchmark_test::quads_batched_blend:
                run_quads_batched_blend();
                break;
            case benchmark_test::mixed_primitives:
                run_mixed_primitives();
                break;
            case benchmark_test::scissor_regions:
                run_scissor_regions();
                break;
            case benchmark_test::stress_max_primitives:
                run_stress_max();
                break;
            default:
                break;
        }

        m_renderer->end();

        auto end_time = std::chrono::high_resolution_clock::now();
        m_render_time_ms = std::chrono::duration<float, std::milli>(end_time - start_time).count();

        // Cache stats before reset
        m_last_stats = m_renderer->get_stats();
        m_renderer->reset_stats();

        // Handle automated benchmark
        if (m_auto_running)
        {
            update_auto_benchmark();
        }

        render_ui();
    }

    void start_auto_benchmark()
    {
        m_auto_running = true;
        m_auto_current_test = 0;
        m_auto_frame_count = 0;
        m_auto_results.clear();
        m_auto_frame_times.clear();
        m_auto_render_times.clear();
        m_auto_draw_calls.clear();
        m_current_test = static_cast<benchmark_test>(0);
        LUMINA_LOG_INFO("========== AUTOMATED BENCHMARK STARTED ==========");
        LUMINA_LOG_INFO("Warmup frames: {}, Measure frames: {}", m_auto_warmup_frames, m_auto_measure_frames);
        LUMINA_LOG_INFO("Primitives: {}, Quad Size: {:.1f}", m_primitive_count, m_quad_size);
        LUMINA_LOG_INFO("==================================================");
    }

    void update_auto_benchmark()
    {
        m_auto_frame_count++;

        // Skip warmup frames
        if (m_auto_frame_count > m_auto_warmup_frames)
        {
            // Collect data
            if (!m_frame_times.empty())
                m_auto_frame_times.push_back(m_frame_times.back());
            m_auto_render_times.push_back(m_render_time_ms);
            m_auto_draw_calls.push_back(m_last_stats.draw_calls);
        }

        // Check if current test is done
        int total_frames = m_auto_warmup_frames + m_auto_measure_frames;
        if (m_auto_frame_count >= total_frames)
        {
            // Calculate results for this test
            test_result result;
            result.test = m_current_test;
            result.total_primitives = m_last_stats.get_total_primitives();

            // Frame time stats
            float sum_frame = 0, min_frame = FLT_MAX, max_frame = 0;
            for (float t : m_auto_frame_times)
            {
                sum_frame += t;
                min_frame = std::min(min_frame, t);
                max_frame = std::max(max_frame, t);
            }
            result.avg_frame_time = m_auto_frame_times.empty() ? 0 : sum_frame / m_auto_frame_times.size();
            result.min_frame_time = min_frame == FLT_MAX ? 0 : min_frame;
            result.max_frame_time = max_frame;
            result.avg_fps = result.avg_frame_time > 0 ? 1000.0f / result.avg_frame_time : 0;

            // Render time stats
            float sum_render = 0;
            for (float t : m_auto_render_times)
                sum_render += t;
            result.avg_render_time = m_auto_render_times.empty() ? 0 : sum_render / m_auto_render_times.size();

            // Draw call stats
            uint32_t sum_draws = 0;
            for (uint32_t d : m_auto_draw_calls)
                sum_draws += d;
            result.avg_draw_calls = m_auto_draw_calls.empty() ? 0 : sum_draws / static_cast<uint32_t>(m_auto_draw_calls.size());
            result.prims_per_call = result.avg_draw_calls > 0 ?
                static_cast<float>(result.total_primitives) / result.avg_draw_calls : 0;

            m_auto_results.push_back(result);

            // Log this test's results
            LUMINA_LOG_INFO("[{}] FPS: {:.1f}, Frame: {:.2f}ms, Render: {:.2f}ms, Draws: {}, P/D: {:.1f}",
                get_test_name(result.test), result.avg_fps, result.avg_frame_time,
                result.avg_render_time, result.avg_draw_calls, result.prims_per_call);

            // Move to next test
            advance_to_next_test();
        }
    }

    void advance_to_next_test()
    {
        m_auto_current_test++;
        m_auto_frame_count = 0;
        m_auto_frame_times.clear();
        m_auto_render_times.clear();
        m_auto_draw_calls.clear();

        // Skip slow tests if option enabled
        while (m_auto_current_test < static_cast<int>(benchmark_test::count))
        {
            auto test = static_cast<benchmark_test>(m_auto_current_test);
            if (m_skip_slow_tests && test == benchmark_test::quads_alternating_blend)
            {
                LUMINA_LOG_INFO("[{}] SKIPPED (slow test)", get_test_name(test));
                m_auto_current_test++;
                continue;
            }
            break;
        }

        if (m_auto_current_test >= static_cast<int>(benchmark_test::count))
        {
            finish_auto_benchmark();
        }
        else
        {
            m_current_test = static_cast<benchmark_test>(m_auto_current_test);
        }
    }

    void finish_auto_benchmark()
    {
        m_auto_running = false;

        LUMINA_LOG_INFO("");
        LUMINA_LOG_INFO("============ BENCHMARK RESULTS SUMMARY ============");
        LUMINA_LOG_INFO("| Test                      | FPS    | Draws | P/Draw  |");
        LUMINA_LOG_INFO("|---------------------------|--------|-------|---------|");

        for (const auto& r : m_auto_results)
        {
            LUMINA_LOG_INFO("| {:<25} | {:>6.1f} | {:>5} | {:>7.1f} |",
                get_test_name(r.test), r.avg_fps, r.avg_draw_calls, r.prims_per_call);
        }

        LUMINA_LOG_INFO("|---------------------------|--------|-------|---------|");
        LUMINA_LOG_INFO("Primitives: {}, Quad Size: {:.1f}", m_primitive_count, m_quad_size);
        LUMINA_LOG_INFO("Warmup: {} frames, Measured: {} frames per test", m_auto_warmup_frames, m_auto_measure_frames);
        LUMINA_LOG_INFO("====================================================");
    }

private:
    void create_test_textures(lumina::core::device& device)
    {
        // Create 32 colored textures for texture array testing
        m_test_textures.resize(32);

        for (int i = 0; i < 32; i++)
        {
            // Generate unique color for each texture
            float hue = static_cast<float>(i) / 32.0f;
            glm::vec3 rgb = hsv_to_rgb(hue, 0.8f, 0.9f);

            // Create 4x4 solid color texture
            uint32_t color =
                (static_cast<uint32_t>(rgb.r * 255) << 0) |
                (static_cast<uint32_t>(rgb.g * 255) << 8) |
                (static_cast<uint32_t>(rgb.b * 255) << 16) |
                (255 << 24);

            std::vector<uint32_t> pixels(16, color);
            m_test_textures[i] = gfx::texture::create(
                device, 4, 4, gfx::format::rgba8_unorm,
                pixels.data()
            );
        }
    }

    void init_random_positions()
    {
        std::mt19937 rng(42); // Fixed seed for reproducibility
        std::uniform_real_distribution<float> dist_x(-600.0f, 600.0f);
        std::uniform_real_distribution<float> dist_y(-350.0f, 350.0f);

        m_positions.resize(m_max_primitives);
        for (auto& pos : m_positions)
        {
            pos = {dist_x(rng), dist_y(rng)};
        }
    }

    // ========================================================================
    // Test Implementations
    // ========================================================================

    void run_quads_same_texture()
    {
        // Best case: all quads use the same texture -> minimal draw calls
        auto& tex = m_test_textures[0];

        for (int i = 0; i < m_primitive_count; i++)
        {
            m_renderer->draw_quad({
                .position = {m_positions[i].x, m_positions[i].y, 0.0f},
                .size = {m_quad_size, m_quad_size},
                .color = {1.0f, 1.0f, 1.0f, 0.8f},
                .texture = tex
            });
        }
    }

    void run_quads_32_textures()
    {
        // Texture array test: cycle through 32 textures
        for (int i = 0; i < m_primitive_count; i++)
        {
            int tex_idx = i % 32;
            m_renderer->draw_quad({
                .position = {m_positions[i].x, m_positions[i].y, 0.0f},
                .size = {m_quad_size, m_quad_size},
                .color = {1.0f, 1.0f, 1.0f, 0.8f},
                .texture = m_test_textures[tex_idx]
            });
        }
    }

    void run_quads_alternating_blend()
    {
        // Worst case: blend mode changes every quad -> many draw calls
        // Cap at 1000 to avoid freezing (this test is intentionally pathological)
        int count = std::min(m_primitive_count, 1000);

        gfx::blend_mode modes[] = {
            gfx::blend_mode::alpha,
            gfx::blend_mode::additive,
            gfx::blend_mode::multiply,
            gfx::blend_mode::opaque
        };

        for (int i = 0; i < count; i++)
        {
            m_renderer->draw_quad({
                .position = {m_positions[i].x, m_positions[i].y, 0.0f},
                .size = {m_quad_size, m_quad_size},
                .color = {1.0f, 0.8f, 0.2f, 0.7f},
                .blend = modes[i % 4]
            });
        }
    }

    void run_quads_batched_blend()
    {
        // Better: group by blend mode -> fewer draw calls
        gfx::blend_mode modes[] = {
            gfx::blend_mode::alpha,
            gfx::blend_mode::additive,
            gfx::blend_mode::multiply,
            gfx::blend_mode::opaque
        };

        int per_mode = m_primitive_count / 4;

        for (int m = 0; m < 4; m++)
        {
            for (int i = 0; i < per_mode; i++)
            {
                int idx = m * per_mode + i;
                if (idx >= m_primitive_count) break;

                m_renderer->draw_quad({
                    .position = {m_positions[idx].x, m_positions[idx].y, 0.0f},
                    .size = {m_quad_size, m_quad_size},
                    .color = {1.0f, 0.8f, 0.2f, 0.7f},
                    .blend = modes[m]
                });
            }
        }
    }

    void run_mixed_primitives()
    {
        // Mix of different primitive types
        int quads = m_primitive_count / 4;
        int circles = m_primitive_count / 4;
        int lines = m_primitive_count / 4;
        int triangles = m_primitive_count / 4;

        // Quads
        for (int i = 0; i < quads; i++)
        {
            m_renderer->draw_quad({
                .position = {m_positions[i].x, m_positions[i].y, 0.0f},
                .size = {m_quad_size, m_quad_size},
                .color = {0.8f, 0.3f, 0.3f, 0.8f}
            });
        }

        // Circles
        for (int i = 0; i < circles; i++)
        {
            int idx = quads + i;
            m_renderer->draw_circle({
                .position = {m_positions[idx].x, m_positions[idx].y, 0.0f},
                .radius = {m_quad_size / 2, m_quad_size / 2},
                .color = {0.3f, 0.8f, 0.3f, 0.8f}
            });
        }

        // Lines
        for (int i = 0; i < lines; i++)
        {
            int idx = quads + circles + i;
            m_renderer->draw_line({
                .start = {m_positions[idx].x, m_positions[idx].y, 0.0f},
                .end = {m_positions[idx].x + 50.0f, m_positions[idx].y + 30.0f, 0.0f},
                .color = {0.3f, 0.3f, 0.8f, 0.8f},
                .thickness = 2.0f
            });
        }

        // Triangles
        for (int i = 0; i < triangles; i++)
        {
            int idx = quads + circles + lines + i;
            float x = m_positions[idx].x;
            float y = m_positions[idx].y;
            float s = m_quad_size / 2;

            m_renderer->draw_triangle({
                .p0 = {x, y + s, 0.0f},
                .p1 = {x - s, y - s, 0.0f},
                .p2 = {x + s, y - s, 0.0f},
                .color = {0.8f, 0.8f, 0.3f, 0.8f}
            });
        }
    }

    void run_scissor_regions()
    {
        // Test scissor overhead with multiple regions
        int regions = 4;
        int per_region = m_primitive_count / regions;

        float region_w = 300.0f;
        float region_h = 300.0f;

        glm::vec2 region_centers[] = {
            {320.0f, 180.0f},
            {960.0f, 180.0f},
            {320.0f, 540.0f},
            {960.0f, 540.0f}
        };

        for (int r = 0; r < regions; r++)
        {
            float sx = region_centers[r].x - region_w / 2;
            float sy = region_centers[r].y - region_h / 2;

            m_renderer->push_scissor(sx, sy, region_w, region_h);

            for (int i = 0; i < per_region; i++)
            {
                int idx = r * per_region + i;
                if (idx >= m_primitive_count) break;

                // Random position within world, scissor will clip
                m_renderer->draw_quad({
                    .position = {m_positions[idx].x, m_positions[idx].y, 0.0f},
                    .size = {m_quad_size, m_quad_size},
                    .color = {0.2f + r * 0.2f, 0.8f - r * 0.15f, 0.5f, 0.8f}
                });
            }

            m_renderer->pop_scissor();
        }
    }

    void run_stress_max()
    {
        // Maximum stress: all primitives, all textures
        for (int i = 0; i < m_primitive_count; i++)
        {
            int tex_idx = i % 32;
            float z = static_cast<float>(i) / m_primitive_count;

            m_renderer->draw_quad({
                .position = {m_positions[i].x, m_positions[i].y, 0.0f},
                .size = {m_quad_size, m_quad_size},
                .color = {1.0f, 1.0f, 1.0f, 0.5f},
                .texture = m_test_textures[tex_idx],
                .z = z
            });
        }
    }

    // ========================================================================
    // UI
    // ========================================================================

    void render_ui()
    {
        ui::begin_window("Benchmark Controls");

        // Test selection
        ui::text("Select Test:");
        for (int i = 0; i < static_cast<int>(benchmark_test::count); i++)
        {
            auto test = static_cast<benchmark_test>(i);
            if (ImGui::RadioButton(get_test_name(test), m_current_test == test))
            {
                m_current_test = test;
            }
        }
        ui::separator();

        // Primitive count slider
        ui::text("Primitive Count:");
        ImGui::SliderInt("##count", &m_primitive_count, 100, m_max_primitives);
        ImGui::SliderFloat("Quad Size", &m_quad_size, 5.0f, 50.0f);
        ui::separator();

        // Performance metrics
        ui::text("Performance Metrics:");

        float avg_frame_time = 0.0f;
        float min_frame_time = FLT_MAX;
        float max_frame_time = 0.0f;

        if (!m_frame_times.empty())
        {
            for (float t : m_frame_times)
            {
                avg_frame_time += t;
                min_frame_time = std::min(min_frame_time, t);
                max_frame_time = std::max(max_frame_time, t);
            }
            avg_frame_time /= m_frame_times.size();
        }

        float fps = avg_frame_time > 0 ? 1000.0f / avg_frame_time : 0.0f;

        ui::text_fmt("FPS: {:.1f}", fps);
        ui::text_fmt("Frame Time: {:.2f} ms (min: {:.2f}, max: {:.2f})",
            avg_frame_time, min_frame_time, max_frame_time);
        ui::text_fmt("Render Time: {:.2f} ms", m_render_time_ms);
        ui::separator();

        // Renderer stats
        ui::text("Renderer Stats:");
        ui::text_fmt("Draw Calls: {}", m_last_stats.draw_calls);
        ui::text_fmt("Quads: {}", m_last_stats.quad_count);
        ui::text_fmt("Circles: {}", m_last_stats.circle_count);
        ui::text_fmt("Lines: {}", m_last_stats.line_count);
        ui::text_fmt("Triangles: {}", m_last_stats.triangle_count);
        ui::text_fmt("Total Primitives: {}", m_last_stats.get_total_primitives());
        ui::separator();

        // Batch efficiency
        float prims_per_call = 0.0f;
        float efficiency = 0.0f;
        if (m_last_stats.draw_calls > 0)
        {
            prims_per_call = static_cast<float>(m_last_stats.get_total_primitives()) / m_last_stats.draw_calls;
            ui::text_fmt("Primitives/Draw Call: {:.1f}", prims_per_call);

            // Theoretical max for quads with same texture = 10000 (config default)
            efficiency = (prims_per_call / 10000.0f) * 100.0f;
            efficiency = std::min(efficiency, 100.0f);
            ui::text_fmt("Batch Efficiency: {:.1f}%%", efficiency);
        }
        ui::separator();

        // Automated benchmark button
        if (m_auto_running)
        {
            ui::text_fmt("Running Test {}/{}: {}",
                m_auto_current_test + 1,
                static_cast<int>(benchmark_test::count),
                get_test_name(m_current_test));
            ui::text_fmt("Frame {}/{}",
                m_auto_frame_count,
                m_auto_warmup_frames + m_auto_measure_frames);

            if (ImGui::Button("Cancel Benchmark"))
            {
                m_auto_running = false;
                LUMINA_LOG_INFO("Benchmark cancelled by user");
            }
        }
        else
        {
            if (ImGui::Button("Run All Tests (Auto)"))
            {
                start_auto_benchmark();
            }
            ImGui::SameLine();
            ImGui::SetNextItemWidth(80);
            ImGui::InputInt("Frames", &m_auto_measure_frames);
            m_auto_measure_frames = std::max(30, std::min(600, m_auto_measure_frames));

            ImGui::Checkbox("Skip Slow Tests", &m_skip_slow_tests);
            if (m_skip_slow_tests)
            {
                ImGui::SameLine();
                ui::text("(Alternating Blend)");
            }
        }
        ui::separator();

        // Snapshot button
        if (ImGui::Button("Log Snapshot to Console"))
        {
            LUMINA_LOG_INFO("========== BENCHMARK SNAPSHOT ==========");
            LUMINA_LOG_INFO("Test: {}", get_test_name(m_current_test));
            LUMINA_LOG_INFO("Primitives: {}", m_primitive_count);
            LUMINA_LOG_INFO("Quad Size: {:.1f}", m_quad_size);
            LUMINA_LOG_INFO("-----------------------------------------");
            LUMINA_LOG_INFO("FPS: {:.1f}", fps);
            LUMINA_LOG_INFO("Frame Time: {:.2f} ms (min: {:.2f}, max: {:.2f})", avg_frame_time, min_frame_time, max_frame_time);
            LUMINA_LOG_INFO("Render Time: {:.2f} ms", m_render_time_ms);
            LUMINA_LOG_INFO("-----------------------------------------");
            LUMINA_LOG_INFO("Draw Calls: {}", m_last_stats.draw_calls);
            LUMINA_LOG_INFO("Quads: {}", m_last_stats.quad_count);
            LUMINA_LOG_INFO("Circles: {}", m_last_stats.circle_count);
            LUMINA_LOG_INFO("Lines: {}", m_last_stats.line_count);
            LUMINA_LOG_INFO("Triangles: {}", m_last_stats.triangle_count);
            LUMINA_LOG_INFO("Total Primitives: {}", m_last_stats.get_total_primitives());
            LUMINA_LOG_INFO("-----------------------------------------");
            LUMINA_LOG_INFO("Primitives/Draw Call: {:.1f}", prims_per_call);
            LUMINA_LOG_INFO("Batch Efficiency: {:.1f}%", efficiency);
            LUMINA_LOG_INFO("=========================================");
        }

        ui::end_window();

        // Viewport
        ui::push_style_var(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ui::begin_window("Viewport");

        auto content_size = ui::get_content_size();
        if (auto tex = m_render_target->get_color_texture())
        {
            ui::image(tex->get_texture(), content_size);
        }

        ui::end_window();
        ui::pop_style_var();
    }

    // ========================================================================
    // Helpers
    // ========================================================================

    glm::vec3 hsv_to_rgb(float h, float s, float v)
    {
        float c = v * s;
        float x = c * (1 - std::abs(std::fmod(h * 6.0f, 2.0f) - 1));
        float m = v - c;

        glm::vec3 rgb;
        if (h < 1.0f/6.0f)      rgb = {c, x, 0};
        else if (h < 2.0f/6.0f) rgb = {x, c, 0};
        else if (h < 3.0f/6.0f) rgb = {0, c, x};
        else if (h < 4.0f/6.0f) rgb = {0, x, c};
        else if (h < 5.0f/6.0f) rgb = {x, 0, c};
        else                    rgb = {c, 0, x};

        return rgb + glm::vec3(m);
    }

private:
    lumina::scope<gfx::renderer2d> m_renderer;
    lumina::ref<gfx::render_target> m_render_target;
    gfx::camera2d m_camera;
    std::vector<lumina::ref<gfx::texture>> m_test_textures;

    float m_time = 0.0f;
    benchmark_test m_current_test = benchmark_test::quads_same_texture;

    // Test parameters
    static constexpr int m_max_primitives = 50000;
    int m_primitive_count = 10000;
    float m_quad_size = 20.0f;
    std::vector<glm::vec2> m_positions;

    // Performance tracking
    std::deque<float> m_frame_times;
    float m_render_time_ms = 0.0f;
    gfx::renderer2d_stats m_last_stats;

    // Automated benchmark mode
    bool m_auto_running = false;
    bool m_skip_slow_tests = true;      // Skip pathological tests by default
    int m_auto_current_test = 0;
    int m_auto_frame_count = 0;
    int m_auto_warmup_frames = 30;      // Skip first N frames for warmup
    int m_auto_measure_frames = 120;    // Measure over N frames
    std::vector<float> m_auto_frame_times;
    std::vector<float> m_auto_render_times;
    std::vector<uint32_t> m_auto_draw_calls;
    std::vector<test_result> m_auto_results;
};

lumina::core::application* lumina::core::create_application(int argc, char** argv)
{
    auto* app = new application(graphics_api::vulkan);
    app->set_title("11 - Benchmark");
    app->push_layer<benchmark_layer>();
    return app;
}
