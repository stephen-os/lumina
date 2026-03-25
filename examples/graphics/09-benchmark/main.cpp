// 09-benchmark: Performance Benchmark Suite
// Demonstrates: Stress tests for renderer2d batching, draw calls, and throughput

#include <lumina/core/Core.h>
#include <lumina/core/EntryPoint.h>
#include <lumina/core/Input.h>
#include <lumina/graphics/Graphics.h>
#include <lumina/ui/UI.h>

#include <glm/glm.hpp>
#include <cmath>
#include <random>
#include <chrono>
#include <numeric>
#include <deque>

namespace UI = Lumina::UI;
namespace Gfx = Lumina::Graphics;
namespace Input = Lumina::Input;

// Test scenarios
enum class BenchmarkTest
{
    QuadsSameTexture,         // Best case: all quads use same texture
    Quads32Textures,          // Texture array: 32 different textures
    QuadsAlternatingBlend,    // Worst case: blend mode changes every quad
    QuadsBatchedBlend,        // Grouped by blend mode
    MixedPrimitives,          // Quads + circles + lines + text
    ScissorRegions,           // Multiple scissor regions
    StressMaxPrimitives,      // Maximum primitive count
    Count
};

const char* GetTestName(BenchmarkTest test)
{
    switch (test)
    {
        case BenchmarkTest::QuadsSameTexture:      return "Quads (Same Texture)";
        case BenchmarkTest::Quads32Textures:       return "Quads (32 Textures)";
        case BenchmarkTest::QuadsAlternatingBlend: return "Quads (Alternating Blend)";
        case BenchmarkTest::QuadsBatchedBlend:     return "Quads (Batched Blend)";
        case BenchmarkTest::MixedPrimitives:       return "Mixed Primitives";
        case BenchmarkTest::ScissorRegions:        return "Scissor Regions";
        case BenchmarkTest::StressMaxPrimitives:   return "Stress Test (Max)";
        default: return "Unknown";
    }
}

struct TestResult
{
    BenchmarkTest Test;
    float AvgFps;
    float MinFrameTime;
    float MaxFrameTime;
    float AvgFrameTime;
    float AvgRenderTime;
    uint32_t AvgDrawCalls;
    uint32_t TotalPrimitives;
    float PrimsPerCall;
};

class BenchmarkLayer : public Lumina::Layer
{
public:
    BenchmarkLayer() : Layer("benchmark") {}

    void OnAttach() override
    {
        auto& device = Lumina::Application::Get().GetDevice();
        m_Renderer = std::make_unique<Gfx::Renderer2D>(device);
        m_Renderer->Init();

        // Create render target
        m_RenderTarget = Gfx::RenderTarget::Create(
            device, 1280, 720, Gfx::Format::RGBA8Unorm
        );

        // Initialize camera
        m_Camera = Gfx::Camera2D(720.0f, 1.0f);
        m_Camera.SetPosition({0.0f, 0.0f});
        m_Camera.Update(0.0f);

        // Create test textures (simple colored textures)
        CreateTestTextures(device);

        // Initialize random positions for primitives
        InitRandomPositions();
    }

    void OnDetach() override
    {
        m_TestTextures.clear();
        m_RenderTarget.reset();
        m_Renderer.reset();
    }

    void OnUpdate(float dt) override
    {
        m_Time += dt;
        m_Camera.Update(dt);

        // Track frame times
        m_FrameTimes.push_back(dt * 1000.0f); // Convert to ms
        if (m_FrameTimes.size() > 120) // Keep last 120 frames
            m_FrameTimes.pop_front();
    }

    void OnRender() override
    {
        if (!m_Renderer || !m_RenderTarget) return;

        auto startTime = std::chrono::high_resolution_clock::now();

        m_Renderer->Begin(m_Camera);
        m_Renderer->SetRenderTarget(m_RenderTarget);
        m_Renderer->Clear({0.05f, 0.05f, 0.08f, 1.0f});

        // Run current test
        switch (m_CurrentTest)
        {
            case BenchmarkTest::QuadsSameTexture:
                RunQuadsSameTexture();
                break;
            case BenchmarkTest::Quads32Textures:
                RunQuads32Textures();
                break;
            case BenchmarkTest::QuadsAlternatingBlend:
                RunQuadsAlternatingBlend();
                break;
            case BenchmarkTest::QuadsBatchedBlend:
                RunQuadsBatchedBlend();
                break;
            case BenchmarkTest::MixedPrimitives:
                RunMixedPrimitives();
                break;
            case BenchmarkTest::ScissorRegions:
                RunScissorRegions();
                break;
            case BenchmarkTest::StressMaxPrimitives:
                RunStressMax();
                break;
            default:
                break;
        }

        m_Renderer->End();

        auto endTime = std::chrono::high_resolution_clock::now();
        m_RenderTimeMs = std::chrono::duration<float, std::milli>(endTime - startTime).count();

        // Cache stats before reset
        m_LastStats = m_Renderer->GetStats();
        m_Renderer->ResetStats();

        // Handle automated benchmark
        if (m_AutoRunning)
        {
            UpdateAutoBenchmark();
        }

        RenderUI();
    }

    void StartAutoBenchmark()
    {
        m_AutoRunning = true;
        m_AutoCurrentTest = 0;
        m_AutoFrameCount = 0;
        m_AutoResults.clear();
        m_AutoFrameTimes.clear();
        m_AutoRenderTimes.clear();
        m_AutoDrawCalls.clear();
        m_CurrentTest = static_cast<BenchmarkTest>(0);
        LUMINA_LOG_INFO("========== AUTOMATED BENCHMARK STARTED ==========");
        LUMINA_LOG_INFO("Warmup frames: {}, Measure frames: {}", m_AutoWarmupFrames, m_AutoMeasureFrames);
        LUMINA_LOG_INFO("Primitives: {}, Quad Size: {:.1f}", m_PrimitiveCount, m_QuadSize);
        LUMINA_LOG_INFO("==================================================");
    }

    void UpdateAutoBenchmark()
    {
        m_AutoFrameCount++;

        // Skip warmup frames
        if (m_AutoFrameCount > m_AutoWarmupFrames)
        {
            // Collect data
            if (!m_FrameTimes.empty())
                m_AutoFrameTimes.push_back(m_FrameTimes.back());
            m_AutoRenderTimes.push_back(m_RenderTimeMs);
            m_AutoDrawCalls.push_back(m_LastStats.DrawCalls);
        }

        // Check if current test is done
        int totalFrames = m_AutoWarmupFrames + m_AutoMeasureFrames;
        if (m_AutoFrameCount >= totalFrames)
        {
            // Calculate results for this test
            TestResult result;
            result.Test = m_CurrentTest;
            result.TotalPrimitives = m_LastStats.GetTotalPrimitives();

            // Frame time stats
            float sumFrame = 0, minFrame = FLT_MAX, maxFrame = 0;
            for (float t : m_AutoFrameTimes)
            {
                sumFrame += t;
                minFrame = std::min(minFrame, t);
                maxFrame = std::max(maxFrame, t);
            }
            result.AvgFrameTime = m_AutoFrameTimes.empty() ? 0 : sumFrame / m_AutoFrameTimes.size();
            result.MinFrameTime = minFrame == FLT_MAX ? 0 : minFrame;
            result.MaxFrameTime = maxFrame;
            result.AvgFps = result.AvgFrameTime > 0 ? 1000.0f / result.AvgFrameTime : 0;

            // Render time stats
            float sumRender = 0;
            for (float t : m_AutoRenderTimes)
                sumRender += t;
            result.AvgRenderTime = m_AutoRenderTimes.empty() ? 0 : sumRender / m_AutoRenderTimes.size();

            // Draw call stats
            uint32_t sumDraws = 0;
            for (uint32_t d : m_AutoDrawCalls)
                sumDraws += d;
            result.AvgDrawCalls = m_AutoDrawCalls.empty() ? 0 : sumDraws / static_cast<uint32_t>(m_AutoDrawCalls.size());
            result.PrimsPerCall = result.AvgDrawCalls > 0 ?
                static_cast<float>(result.TotalPrimitives) / result.AvgDrawCalls : 0;

            m_AutoResults.push_back(result);

            // Log this test's results
            LUMINA_LOG_INFO("[{}] FPS: {:.1f}, Frame: {:.2f}ms, Render: {:.2f}ms, Draws: {}, P/D: {:.1f}",
                GetTestName(result.Test), result.AvgFps, result.AvgFrameTime,
                result.AvgRenderTime, result.AvgDrawCalls, result.PrimsPerCall);

            // Move to next test
            AdvanceToNextTest();
        }
    }

    void AdvanceToNextTest()
    {
        m_AutoCurrentTest++;
        m_AutoFrameCount = 0;
        m_AutoFrameTimes.clear();
        m_AutoRenderTimes.clear();
        m_AutoDrawCalls.clear();

        // Skip slow tests if option enabled
        while (m_AutoCurrentTest < static_cast<int>(BenchmarkTest::Count))
        {
            auto test = static_cast<BenchmarkTest>(m_AutoCurrentTest);
            if (m_SkipSlowTests && test == BenchmarkTest::QuadsAlternatingBlend)
            {
                LUMINA_LOG_INFO("[{}] SKIPPED (slow test)", GetTestName(test));
                m_AutoCurrentTest++;
                continue;
            }
            break;
        }

        if (m_AutoCurrentTest >= static_cast<int>(BenchmarkTest::Count))
        {
            FinishAutoBenchmark();
        }
        else
        {
            m_CurrentTest = static_cast<BenchmarkTest>(m_AutoCurrentTest);
        }
    }

    void FinishAutoBenchmark()
    {
        m_AutoRunning = false;

        LUMINA_LOG_INFO("");
        LUMINA_LOG_INFO("============ BENCHMARK RESULTS SUMMARY ============");
        LUMINA_LOG_INFO("| Test                      | FPS    | Draws | P/Draw  |");
        LUMINA_LOG_INFO("|---------------------------|--------|-------|---------|");

        for (const auto& r : m_AutoResults)
        {
            LUMINA_LOG_INFO("| {:<25} | {:>6.1f} | {:>5} | {:>7.1f} |",
                GetTestName(r.Test), r.AvgFps, r.AvgDrawCalls, r.PrimsPerCall);
        }

        LUMINA_LOG_INFO("|---------------------------|--------|-------|---------|");
        LUMINA_LOG_INFO("Primitives: {}, Quad Size: {:.1f}", m_PrimitiveCount, m_QuadSize);
        LUMINA_LOG_INFO("Warmup: {} frames, Measured: {} frames per test", m_AutoWarmupFrames, m_AutoMeasureFrames);
        LUMINA_LOG_INFO("====================================================");
    }

private:
    void CreateTestTextures(Lumina::Device& device)
    {
        // Create 32 colored textures for texture array testing
        m_TestTextures.resize(32);

        for (int i = 0; i < 32; i++)
        {
            // Generate unique color for each texture
            float hue = static_cast<float>(i) / 32.0f;
            glm::vec3 rgb = HsvToRgb(hue, 0.8f, 0.9f);

            // Create 4x4 solid color texture
            uint32_t color =
                (static_cast<uint32_t>(rgb.r * 255) << 0) |
                (static_cast<uint32_t>(rgb.g * 255) << 8) |
                (static_cast<uint32_t>(rgb.b * 255) << 16) |
                (255 << 24);

            std::vector<uint32_t> pixels(16, color);
            m_TestTextures[i] = Gfx::Texture::Create(
                device, 4, 4, Gfx::Format::RGBA8Unorm,
                pixels.data()
            );
        }
    }

    void InitRandomPositions()
    {
        std::mt19937 rng(42); // Fixed seed for reproducibility
        std::uniform_real_distribution<float> distX(-600.0f, 600.0f);
        std::uniform_real_distribution<float> distY(-350.0f, 350.0f);

        m_Positions.resize(m_MaxPrimitives);
        for (auto& pos : m_Positions)
        {
            pos = {distX(rng), distY(rng)};
        }
    }

    // ========================================================================
    // Test Implementations
    // ========================================================================

    void RunQuadsSameTexture()
    {
        // Best case: all quads use the same texture -> minimal draw calls
        auto& tex = m_TestTextures[0];

        for (int i = 0; i < m_PrimitiveCount; i++)
        {
            m_Renderer->DrawQuad({
                .Position = {m_Positions[i].x, m_Positions[i].y, 0.0f},
                .Size = {m_QuadSize, m_QuadSize},
                .Color = {1.0f, 1.0f, 1.0f, 0.8f},
                .Texture = tex
            });
        }
    }

    void RunQuads32Textures()
    {
        // Texture array test: cycle through 32 textures
        for (int i = 0; i < m_PrimitiveCount; i++)
        {
            int texIdx = i % 32;
            m_Renderer->DrawQuad({
                .Position = {m_Positions[i].x, m_Positions[i].y, 0.0f},
                .Size = {m_QuadSize, m_QuadSize},
                .Color = {1.0f, 1.0f, 1.0f, 0.8f},
                .Texture = m_TestTextures[texIdx]
            });
        }
    }

    void RunQuadsAlternatingBlend()
    {
        // Worst case: blend mode changes every quad -> many draw calls
        // Cap at 1000 to avoid freezing (this test is intentionally pathological)
        int count = std::min(m_PrimitiveCount, 1000);

        Gfx::BlendMode modes[] = {
            Gfx::BlendMode::Alpha,
            Gfx::BlendMode::Additive,
            Gfx::BlendMode::Multiply,
            Gfx::BlendMode::Opaque
        };

        for (int i = 0; i < count; i++)
        {
            m_Renderer->DrawQuad({
                .Position = {m_Positions[i].x, m_Positions[i].y, 0.0f},
                .Size = {m_QuadSize, m_QuadSize},
                .Color = {1.0f, 0.8f, 0.2f, 0.7f},
                .Blend = modes[i % 4]
            });
        }
    }

    void RunQuadsBatchedBlend()
    {
        // Better: group by blend mode -> fewer draw calls
        Gfx::BlendMode modes[] = {
            Gfx::BlendMode::Alpha,
            Gfx::BlendMode::Additive,
            Gfx::BlendMode::Multiply,
            Gfx::BlendMode::Opaque
        };

        int perMode = m_PrimitiveCount / 4;

        for (int m = 0; m < 4; m++)
        {
            for (int i = 0; i < perMode; i++)
            {
                int idx = m * perMode + i;
                if (idx >= m_PrimitiveCount) break;

                m_Renderer->DrawQuad({
                    .Position = {m_Positions[idx].x, m_Positions[idx].y, 0.0f},
                    .Size = {m_QuadSize, m_QuadSize},
                    .Color = {1.0f, 0.8f, 0.2f, 0.7f},
                    .Blend = modes[m]
                });
            }
        }
    }

    void RunMixedPrimitives()
    {
        // Mix of different primitive types
        int quads = m_PrimitiveCount / 4;
        int circles = m_PrimitiveCount / 4;
        int lines = m_PrimitiveCount / 4;
        int triangles = m_PrimitiveCount / 4;

        // Quads
        for (int i = 0; i < quads; i++)
        {
            m_Renderer->DrawQuad({
                .Position = {m_Positions[i].x, m_Positions[i].y, 0.0f},
                .Size = {m_QuadSize, m_QuadSize},
                .Color = {0.8f, 0.3f, 0.3f, 0.8f}
            });
        }

        // Circles
        for (int i = 0; i < circles; i++)
        {
            int idx = quads + i;
            m_Renderer->DrawCircle({
                .Position = {m_Positions[idx].x, m_Positions[idx].y, 0.0f},
                .Radius = {m_QuadSize / 2, m_QuadSize / 2},
                .Color = {0.3f, 0.8f, 0.3f, 0.8f}
            });
        }

        // Lines
        for (int i = 0; i < lines; i++)
        {
            int idx = quads + circles + i;
            m_Renderer->DrawLine({
                .Start = {m_Positions[idx].x, m_Positions[idx].y, 0.0f},
                .End = {m_Positions[idx].x + 50.0f, m_Positions[idx].y + 30.0f, 0.0f},
                .Color = {0.3f, 0.3f, 0.8f, 0.8f},
                .Thickness = 2.0f
            });
        }

        // Triangles
        for (int i = 0; i < triangles; i++)
        {
            int idx = quads + circles + lines + i;
            float x = m_Positions[idx].x;
            float y = m_Positions[idx].y;
            float s = m_QuadSize / 2;

            m_Renderer->DrawTriangle({
                .P0 = {x, y + s, 0.0f},
                .P1 = {x - s, y - s, 0.0f},
                .P2 = {x + s, y - s, 0.0f},
                .Color = {0.8f, 0.8f, 0.3f, 0.8f}
            });
        }
    }

    void RunScissorRegions()
    {
        // Test scissor overhead with multiple regions
        int regions = 4;
        int perRegion = m_PrimitiveCount / regions;

        float regionW = 300.0f;
        float regionH = 300.0f;

        glm::vec2 regionCenters[] = {
            {320.0f, 180.0f},
            {960.0f, 180.0f},
            {320.0f, 540.0f},
            {960.0f, 540.0f}
        };

        for (int r = 0; r < regions; r++)
        {
            float sx = regionCenters[r].x - regionW / 2;
            float sy = regionCenters[r].y - regionH / 2;

            m_Renderer->PushScissor(sx, sy, regionW, regionH);

            for (int i = 0; i < perRegion; i++)
            {
                int idx = r * perRegion + i;
                if (idx >= m_PrimitiveCount) break;

                // Random position within world, scissor will clip
                m_Renderer->DrawQuad({
                    .Position = {m_Positions[idx].x, m_Positions[idx].y, 0.0f},
                    .Size = {m_QuadSize, m_QuadSize},
                    .Color = {0.2f + r * 0.2f, 0.8f - r * 0.15f, 0.5f, 0.8f}
                });
            }

            m_Renderer->PopScissor();
        }
    }

    void RunStressMax()
    {
        // Maximum stress: all primitives, all textures
        for (int i = 0; i < m_PrimitiveCount; i++)
        {
            int texIdx = i % 32;
            float z = static_cast<float>(i) / m_PrimitiveCount;

            m_Renderer->DrawQuad({
                .Position = {m_Positions[i].x, m_Positions[i].y, 0.0f},
                .Size = {m_QuadSize, m_QuadSize},
                .Color = {1.0f, 1.0f, 1.0f, 0.5f},
                .Texture = m_TestTextures[texIdx],
                .Z = z
            });
        }
    }

    // ========================================================================
    // UI
    // ========================================================================

    void RenderUI()
    {
        UI::BeginWindow("Benchmark Controls");

        // Test selection
        UI::Text("Select Test:");
        for (int i = 0; i < static_cast<int>(BenchmarkTest::Count); i++)
        {
            auto test = static_cast<BenchmarkTest>(i);
            if (ImGui::RadioButton(GetTestName(test), m_CurrentTest == test))
            {
                m_CurrentTest = test;
            }
        }
        UI::Separator();

        // Primitive count slider
        UI::Text("Primitive Count:");
        ImGui::SliderInt("##count", &m_PrimitiveCount, 100, m_MaxPrimitives);
        ImGui::SliderFloat("Quad Size", &m_QuadSize, 5.0f, 50.0f);
        UI::Separator();

        // Performance metrics
        UI::Text("Performance Metrics:");

        float avgFrameTime = 0.0f;
        float minFrameTime = FLT_MAX;
        float maxFrameTime = 0.0f;

        if (!m_FrameTimes.empty())
        {
            for (float t : m_FrameTimes)
            {
                avgFrameTime += t;
                minFrameTime = std::min(minFrameTime, t);
                maxFrameTime = std::max(maxFrameTime, t);
            }
            avgFrameTime /= m_FrameTimes.size();
        }

        float fps = avgFrameTime > 0 ? 1000.0f / avgFrameTime : 0.0f;

        UI::TextFmt("FPS: {:.1f}", fps);
        UI::TextFmt("Frame Time: {:.2f} ms (min: {:.2f}, max: {:.2f})",
            avgFrameTime, minFrameTime, maxFrameTime);
        UI::TextFmt("Render Time: {:.2f} ms", m_RenderTimeMs);
        UI::Separator();

        // Renderer stats
        UI::Text("Renderer Stats:");
        UI::TextFmt("Draw Calls: {}", m_LastStats.DrawCalls);
        UI::TextFmt("Quads: {}", m_LastStats.QuadCount);
        UI::TextFmt("Circles: {}", m_LastStats.CircleCount);
        UI::TextFmt("Lines: {}", m_LastStats.LineCount);
        UI::TextFmt("Triangles: {}", m_LastStats.TriangleCount);
        UI::TextFmt("Total Primitives: {}", m_LastStats.GetTotalPrimitives());
        UI::Separator();

        // Batch efficiency
        float primsPerCall = 0.0f;
        float efficiency = 0.0f;
        if (m_LastStats.DrawCalls > 0)
        {
            primsPerCall = static_cast<float>(m_LastStats.GetTotalPrimitives()) / m_LastStats.DrawCalls;
            UI::TextFmt("Primitives/Draw Call: {:.1f}", primsPerCall);

            // Theoretical max for quads with same texture = 10000 (config default)
            efficiency = (primsPerCall / 10000.0f) * 100.0f;
            efficiency = std::min(efficiency, 100.0f);
            UI::TextFmt("Batch Efficiency: {:.1f}%%", efficiency);
        }
        UI::Separator();

        // Automated benchmark button
        if (m_AutoRunning)
        {
            UI::TextFmt("Running Test {}/{}: {}",
                m_AutoCurrentTest + 1,
                static_cast<int>(BenchmarkTest::Count),
                GetTestName(m_CurrentTest));
            UI::TextFmt("Frame {}/{}",
                m_AutoFrameCount,
                m_AutoWarmupFrames + m_AutoMeasureFrames);

            if (ImGui::Button("Cancel Benchmark"))
            {
                m_AutoRunning = false;
                LUMINA_LOG_INFO("Benchmark cancelled by user");
            }
        }
        else
        {
            if (ImGui::Button("Run All Tests (Auto)"))
            {
                StartAutoBenchmark();
            }
            ImGui::SameLine();
            ImGui::SetNextItemWidth(80);
            ImGui::InputInt("Frames", &m_AutoMeasureFrames);
            m_AutoMeasureFrames = std::max(30, std::min(600, m_AutoMeasureFrames));

            ImGui::Checkbox("Skip Slow Tests", &m_SkipSlowTests);
            if (m_SkipSlowTests)
            {
                ImGui::SameLine();
                UI::Text("(Alternating Blend)");
            }
        }
        UI::Separator();

        // Snapshot button
        if (ImGui::Button("Log Snapshot to Console"))
        {
            LUMINA_LOG_INFO("========== BENCHMARK SNAPSHOT ==========");
            LUMINA_LOG_INFO("Test: {}", GetTestName(m_CurrentTest));
            LUMINA_LOG_INFO("Primitives: {}", m_PrimitiveCount);
            LUMINA_LOG_INFO("Quad Size: {:.1f}", m_QuadSize);
            LUMINA_LOG_INFO("-----------------------------------------");
            LUMINA_LOG_INFO("FPS: {:.1f}", fps);
            LUMINA_LOG_INFO("Frame Time: {:.2f} ms (min: {:.2f}, max: {:.2f})", avgFrameTime, minFrameTime, maxFrameTime);
            LUMINA_LOG_INFO("Render Time: {:.2f} ms", m_RenderTimeMs);
            LUMINA_LOG_INFO("-----------------------------------------");
            LUMINA_LOG_INFO("Draw Calls: {}", m_LastStats.DrawCalls);
            LUMINA_LOG_INFO("Quads: {}", m_LastStats.QuadCount);
            LUMINA_LOG_INFO("Circles: {}", m_LastStats.CircleCount);
            LUMINA_LOG_INFO("Lines: {}", m_LastStats.LineCount);
            LUMINA_LOG_INFO("Triangles: {}", m_LastStats.TriangleCount);
            LUMINA_LOG_INFO("Total Primitives: {}", m_LastStats.GetTotalPrimitives());
            LUMINA_LOG_INFO("-----------------------------------------");
            LUMINA_LOG_INFO("Primitives/Draw Call: {:.1f}", primsPerCall);
            LUMINA_LOG_INFO("Batch Efficiency: {:.1f}%", efficiency);
            LUMINA_LOG_INFO("=========================================");
        }

        UI::EndWindow();

        // Viewport
        UI::PushStyleVar(ImGuiStyleVar_WindowPadding, glm::vec2(0, 0));
        UI::BeginWindow("Viewport");

        auto contentSize = UI::GetContentSize();
        if (auto tex = m_RenderTarget->GetColorTexture())
        {
            UI::Image(tex->GetTexture(), contentSize);
        }

        UI::EndWindow();
        UI::PopStyleVar();
    }

    // ========================================================================
    // Helpers
    // ========================================================================

    glm::vec3 HsvToRgb(float h, float s, float v)
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
    Lumina::Scope<Gfx::Renderer2D> m_Renderer;
    Lumina::Ref<Gfx::RenderTarget> m_RenderTarget;
    Gfx::Camera2D m_Camera;
    std::vector<Lumina::Ref<Gfx::Texture>> m_TestTextures;

    float m_Time = 0.0f;
    BenchmarkTest m_CurrentTest = BenchmarkTest::QuadsSameTexture;

    // Test parameters
    static constexpr int m_MaxPrimitives = 50000;
    int m_PrimitiveCount = 10000;
    float m_QuadSize = 20.0f;
    std::vector<glm::vec2> m_Positions;

    // Performance tracking
    std::deque<float> m_FrameTimes;
    float m_RenderTimeMs = 0.0f;
    Gfx::Renderer2DStats m_LastStats;

    // Automated benchmark mode
    bool m_AutoRunning = false;
    bool m_SkipSlowTests = true;      // Skip pathological tests by default
    int m_AutoCurrentTest = 0;
    int m_AutoFrameCount = 0;
    int m_AutoWarmupFrames = 30;      // Skip first N frames for warmup
    int m_AutoMeasureFrames = 120;    // Measure over N frames
    std::vector<float> m_AutoFrameTimes;
    std::vector<float> m_AutoRenderTimes;
    std::vector<uint32_t> m_AutoDrawCalls;
    std::vector<TestResult> m_AutoResults;
};

Lumina::Application* Lumina::CreateApplication(int argc, char** argv)
{
    ApplicationSpecifications specs;
    specs.Title = "graphics/09-benchmark";
    auto* app = new Application(specs);
    app->PushLayer<BenchmarkLayer>();
    return app;
}
