// 00-profiler-test: Tracy Profiler Integration Test
// Demonstrates: Tracy profiling integration with ImGui debug overlay

#include <Lumina/Core/Core.h>
#include <Lumina/Core/EntryPoint.h>
#include <Lumina/Graphics/Graphics.h>
#include <Lumina/UI/UI.h>

#include <glm/glm.hpp>
#include <cmath>
#include <deque>
#include <random>
#include <chrono>

namespace UI = Lumina::UI;
namespace Gfx = Lumina::Graphics;

class ProfilerTestLayer : public Lumina::Layer
{
public:
    ProfilerTestLayer() : Layer("ProfilerTest") {}

    void OnAttach() override
    {
        auto& device = Lumina::Application::Get().GetDevice();
        m_Renderer = std::make_unique<Gfx::Renderer2D>(device);
        m_Renderer->Init();

        // Create render target
        m_RenderTarget = Gfx::RenderTarget::Create(
            device, 1280, 720, Gfx::Format::RGBA8_UNORM
        );

        // Initialize camera
        m_Camera = Gfx::Camera2D(720.0f, 1.0f);
        m_Camera.SetPosition({0.0f, 0.0f});
        m_Camera.Update(0.0f);

        // Initialize random positions
        InitRandomPositions();
    }

    void OnDetach() override
    {
        m_RenderTarget.reset();
        m_Renderer.reset();
    }

    void OnUpdate(float dt) override
    {
        LUMINA_PROFILE_SCOPE_NC("ProfilerTest::Update", 0x44FF44);

        m_Time += dt;
        m_Camera.Update(dt);

        // Track frame times for display
        m_FrameTimes.push_back(dt * 1000.0f);
        if (m_FrameTimes.size() > 120)
            m_FrameTimes.pop_front();
    }

    void OnRender() override
    {
        LUMINA_PROFILE_SCOPE_NC("ProfilerTest::Render", 0x44FF44);

        if (!m_Renderer || !m_RenderTarget) return;

        auto start = std::chrono::high_resolution_clock::now();

        m_Renderer->Begin(m_Camera);
        m_Renderer->SetRenderTarget(m_RenderTarget);
        m_Renderer->Clear({0.05f, 0.05f, 0.08f, 1.0f});

        // Run selected stress test
        RunStressTest();

        m_Renderer->End();

        auto end = std::chrono::high_resolution_clock::now();
        m_RenderTimeMs = std::chrono::duration<float, std::milli>(end - start).count();

        // Cache stats before reset
        m_LastStats = m_Renderer->GetStats();
        m_Renderer->ResetStats();

        RenderUI();
    }

private:
    void InitRandomPositions()
    {
        std::mt19937 rng(42);
        std::uniform_real_distribution<float> distX(-600.0f, 600.0f);
        std::uniform_real_distribution<float> distY(-350.0f, 350.0f);

        m_Positions.resize(m_MaxPrimitives);
        for (auto& pos : m_Positions)
        {
            pos = {distX(rng), distY(rng)};
        }
    }

    void RunStressTest()
    {
        LUMINA_PROFILE_SCOPE_NC("ProfilerTest::StressTest", 0x44FF44);

        if (m_EnableQuads)
        {
            LUMINA_PROFILE_SCOPE_NC("ProfilerTest::DrawQuads", 0x44FF44);
            for (int i = 0; i < m_QuadCount; i++)
            {
                m_Renderer->DrawQuad({
                    .Position = {m_Positions[i].x, m_Positions[i].y, 0.0f},
                    .Size = {m_PrimitiveSize, m_PrimitiveSize},
                    .Color = {0.8f, 0.3f, 0.3f, 0.8f}
                });
            }
        }

        if (m_EnableCircles)
        {
            LUMINA_PROFILE_SCOPE_NC("ProfilerTest::DrawCircles", 0x44FF44);
            int offset = m_MaxPrimitives / 4;
            for (int i = 0; i < m_CircleCount; i++)
            {
                int idx = (offset + i) % m_MaxPrimitives;
                m_Renderer->DrawCircle({
                    .Position = {m_Positions[idx].x, m_Positions[idx].y, 0.0f},
                    .Radius = {m_PrimitiveSize / 2, m_PrimitiveSize / 2},
                    .Color = {0.3f, 0.8f, 0.3f, 0.8f}
                });
            }
        }

        if (m_EnableLines)
        {
            LUMINA_PROFILE_SCOPE_NC("ProfilerTest::DrawLines", 0x44FF44);
            int offset = m_MaxPrimitives / 2;
            for (int i = 0; i < m_LineCount; i++)
            {
                int idx = (offset + i) % m_MaxPrimitives;
                m_Renderer->DrawLine({
                    .Start = {m_Positions[idx].x, m_Positions[idx].y, 0.0f},
                    .End = {m_Positions[idx].x + 50.0f, m_Positions[idx].y + 30.0f, 0.0f},
                    .Color = {0.3f, 0.3f, 0.8f, 0.8f},
                    .Thickness = 2.0f
                });
            }
        }

        if (m_EnableText)
        {
            LUMINA_PROFILE_SCOPE_NC("ProfilerTest::DrawText", 0x44FF44);
            int offset = (m_MaxPrimitives * 3) / 4;
            for (int i = 0; i < m_TextCount; i++)
            {
                int idx = (offset + i) % m_MaxPrimitives;
                m_Renderer->DrawText({
                    .Text = "Test",
                    .Position = {m_Positions[idx].x, m_Positions[idx].y, 0.0f},
                    .Scale = 0.5f,
                    .Color = {0.8f, 0.8f, 0.3f, 0.8f}
                });
            }
        }
    }

    void RenderUI()
    {
        UI::BeginWindow("Profiler Test Controls");

        // Tracy connection info
        UI::Text("Tracy Profiler Integration Test");
        UI::Separator();

#ifdef TRACY_ENABLE
        UI::Text("Tracy Status: ENABLED");
        UI::Text("Connect Tracy Profiler to view detailed zones.");
#else
        UI::Text("Tracy Status: DISABLED");
        UI::Text("Build with TRACY_ENABLE to enable profiling.");
#endif
        UI::Separator();

        // Frame time graph
        UI::Text("Frame Time History:");
        if (!m_FrameTimes.empty())
        {
            std::vector<float> times(m_FrameTimes.begin(), m_FrameTimes.end());
            ImGui::PlotLines("##frametime", times.data(), static_cast<int>(times.size()),
                0, nullptr, 0.0f, 33.3f, ImVec2(0, 60));
        }

        // Performance metrics
        float avgFrameTime = 0.0f;
        if (!m_FrameTimes.empty())
        {
            for (float t : m_FrameTimes)
                avgFrameTime += t;
            avgFrameTime /= m_FrameTimes.size();
        }
        float fps = avgFrameTime > 0 ? 1000.0f / avgFrameTime : 0.0f;

        UI::TextFmt("FPS: {:.1f}", fps);
        UI::TextFmt("Frame Time: {:.2f} ms", avgFrameTime);
        UI::TextFmt("Render Time: {:.2f} ms", m_RenderTimeMs);
        UI::Separator();

        // Renderer stats
        UI::Text("Renderer Stats:");
        UI::TextFmt("Draw Calls: {}", m_LastStats.DrawCalls);
        UI::TextFmt("Total Primitives: {}", m_LastStats.GetTotalPrimitives());
        UI::TextFmt("Quads: {}", m_LastStats.QuadCount);
        UI::TextFmt("Circles: {}", m_LastStats.CircleCount);
        UI::TextFmt("Lines: {}", m_LastStats.LineCount);
        UI::TextFmt("Text Chars: {}", m_LastStats.TextCharCount);
        UI::Separator();

        // Stress test controls
        UI::Text("Stress Test Controls:");

        ImGui::Checkbox("Quads", &m_EnableQuads);
        if (m_EnableQuads)
        {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(150);
            ImGui::SliderInt("##quads", &m_QuadCount, 0, m_MaxPrimitives / 4);
        }

        ImGui::Checkbox("Circles", &m_EnableCircles);
        if (m_EnableCircles)
        {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(150);
            ImGui::SliderInt("##circles", &m_CircleCount, 0, m_MaxPrimitives / 4);
        }

        ImGui::Checkbox("Lines", &m_EnableLines);
        if (m_EnableLines)
        {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(150);
            ImGui::SliderInt("##lines", &m_LineCount, 0, m_MaxPrimitives / 4);
        }

        ImGui::Checkbox("Text", &m_EnableText);
        if (m_EnableText)
        {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(150);
            ImGui::SliderInt("##text", &m_TextCount, 0, 500);
        }

        UI::Separator();
        ImGui::SliderFloat("Primitive Size", &m_PrimitiveSize, 5.0f, 50.0f);

        UI::Separator();
        if (ImGui::Button("Reset to Defaults"))
        {
            m_EnableQuads = true;
            m_EnableCircles = true;
            m_EnableLines = true;
            m_EnableText = false;
            m_QuadCount = 1000;
            m_CircleCount = 500;
            m_LineCount = 500;
            m_TextCount = 100;
            m_PrimitiveSize = 20.0f;
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

private:
    Lumina::Scope<Gfx::Renderer2D> m_Renderer;
    Lumina::Ref<Gfx::RenderTarget> m_RenderTarget;
    Gfx::Camera2D m_Camera;

    float m_Time = 0.0f;
    std::deque<float> m_FrameTimes;
    float m_RenderTimeMs = 0.0f;
    Gfx::Renderer2DStats m_LastStats;

    // Random positions for primitives
    static constexpr int m_MaxPrimitives = 20000;
    std::vector<glm::vec2> m_Positions;

    // Stress test controls
    bool m_EnableQuads = true;
    bool m_EnableCircles = true;
    bool m_EnableLines = true;
    bool m_EnableText = false;
    int m_QuadCount = 1000;
    int m_CircleCount = 500;
    int m_LineCount = 500;
    int m_TextCount = 100;
    float m_PrimitiveSize = 20.0f;
};

Lumina::Application* Lumina::CreateApplication(int argc, char** argv)
{
    ApplicationSpecifications specs;
    specs.Title = "application/00-profiler-test";
    auto* app = new Application(specs);
    app->PushLayer<ProfilerTestLayer>();
    return app;
}
