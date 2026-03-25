// graphics/10-animation: Sprite Animation Demo
// Demonstrates: Frame-based animation, animation states, procedural animation

#include <lumina/core/Core.h>
#include <lumina/core/EntryPoint.h>
#include <lumina/core/Input.h>
#include <lumina/graphics/Graphics.h>
#include <lumina/ui/UI.h>

#include <glm/glm.hpp>
#include <filesystem>
#include <vector>
#include <cmath>

namespace UI = Lumina::UI;
namespace Gfx = Lumina::Graphics;
namespace Input = Lumina::Input;

// Simple animation class
class SpriteAnimation
{
public:
    SpriteAnimation() = default;

    void AddFrame(uint32_t regionIndex)
    {
        m_Frames.push_back(regionIndex);
    }

    void SetFrameDuration(float duration)
    {
        m_FrameDuration = duration;
    }

    void Update(float dt)
    {
        if (m_Frames.empty() || m_Paused) return;

        m_Time += dt * m_Speed;
        while (m_Time >= m_FrameDuration)
        {
            m_Time -= m_FrameDuration;
            m_CurrentFrame = (m_CurrentFrame + 1) % m_Frames.size();
        }
    }

    [[nodiscard]] uint32_t GetCurrentRegion() const
    {
        if (m_Frames.empty()) return 0;
        return m_Frames[m_CurrentFrame];
    }

    [[nodiscard]] size_t GetFrameCount() const { return m_Frames.size(); }
    [[nodiscard]] size_t GetCurrentFrameIndex() const { return m_CurrentFrame; }

    void SetSpeed(float speed) { m_Speed = speed; }
    [[nodiscard]] float GetSpeed() const { return m_Speed; }

    void SetPaused(bool paused) { m_Paused = paused; }
    [[nodiscard]] bool IsPaused() const { return m_Paused; }

    void Reset()
    {
        m_CurrentFrame = 0;
        m_Time = 0.0f;
    }

private:
    std::vector<uint32_t> m_Frames;
    float m_FrameDuration = 0.1f;
    float m_Time = 0.0f;
    float m_Speed = 1.0f;
    size_t m_CurrentFrame = 0;
    bool m_Paused = false;
};

// Get asset path relative to source file
static std::string GetAssetPath(const std::string& filename)
{
    std::filesystem::path sourceDir = std::filesystem::path(__FILE__).parent_path();
    // Use the atlas from 07-atlas example
    return (sourceDir / ".." / "07-atlas" / filename).string();
}

class AnimationLayer : public Lumina::Layer
{
public:
    AnimationLayer() : Layer("animation") {}

    void OnAttach() override
    {
        Gfx::Renderer::Init({.Width = 600, .Height = 400});

        auto& device = Lumina::Application::Get().GetDevice();

        // Load atlas texture
        m_AtlasTexture = Gfx::Texture::LoadFromFile(device, GetAssetPath("factory_atlas.png"));
        if (!m_AtlasTexture)
        {
            LUMINA_LOG_ERROR("Failed to load atlas texture");
            return;
        }

        // Create atlas with grid
        m_Atlas = Gfx::TextureAtlas::Create(m_AtlasTexture);
        uint32_t texWidth = m_AtlasTexture->GetWidth();
        uint32_t texHeight = m_AtlasTexture->GetHeight();
        m_TileSize = glm::vec2(texWidth / 16.0f, texHeight / 16.0f);
        m_Atlas->AddGrid("tile_", 16, 16, m_TileSize);

        // Create animations using atlas tiles as frames
        // Animation 1: Cycle through first row (frames 0-7)
        m_AnimRow.SetFrameDuration(0.15f);
        for (int i = 0; i < 8; i++)
            m_AnimRow.AddFrame(i);

        // Animation 2: Cycle through first column (frames 0, 16, 32, 48)
        m_AnimCol.SetFrameDuration(0.2f);
        for (int i = 0; i < 4; i++)
            m_AnimCol.AddFrame(i * 16);

        // Animation 3: Diagonal pattern
        m_AnimDiag.SetFrameDuration(0.12f);
        for (int i = 0; i < 8; i++)
            m_AnimDiag.AddFrame(i * 17);  // 0, 17, 34, 51...

        LUMINA_LOG_INFO("Animation demo loaded");
    }

    void OnDetach() override
    {
        m_Atlas.reset();
        m_AtlasTexture.reset();
        Gfx::Renderer::Shutdown();
    }

    void OnUpdate(float dt) override
    {
        m_Time += dt;

        // Update animations
        m_AnimRow.Update(dt);
        m_AnimCol.Update(dt);
        m_AnimDiag.Update(dt);

        // Speed controls
        if (Input::IsKeyPressed(Input::KeyCode::Up))
            m_AnimationSpeed = std::min(m_AnimationSpeed + dt * 2.0f, 5.0f);
        if (Input::IsKeyPressed(Input::KeyCode::Down))
            m_AnimationSpeed = std::max(m_AnimationSpeed - dt * 2.0f, 0.1f);

        m_AnimRow.SetSpeed(m_AnimationSpeed);
        m_AnimCol.SetSpeed(m_AnimationSpeed);
        m_AnimDiag.SetSpeed(m_AnimationSpeed);

        // Pause toggle
        if (Input::IsKeyPressed(Input::KeyCode::Space))
        {
            if (!m_SpaceHeld)
            {
                m_Paused = !m_Paused;
                m_AnimRow.SetPaused(m_Paused);
                m_AnimCol.SetPaused(m_Paused);
                m_AnimDiag.SetPaused(m_Paused);
            }
            m_SpaceHeld = true;
        }
        else
        {
            m_SpaceHeld = false;
        }
    }

    void OnRender() override
    {
        if (!m_Atlas) return;

        Gfx::Renderer::Begin();
        Gfx::Renderer::Clear({0.12f, 0.12f, 0.15f, 1.0f});
        Gfx::Renderer::SetFilterMode(Gfx::FilterMode::Point);

        float yOffset = 80.0f;
        float spriteScale = 2.0f;
        glm::vec2 displaySize = m_TileSize * spriteScale;

        // Row animation
        DrawAnimationDemo("Row Animation", {100.0f, yOffset}, m_AnimRow, displaySize);

        // Column animation
        DrawAnimationDemo("Column Animation", {300.0f, yOffset}, m_AnimCol, displaySize);

        // Diagonal animation
        DrawAnimationDemo("Diagonal Animation", {500.0f, yOffset}, m_AnimDiag, displaySize);

        // Procedural animation section
        float procY = 220.0f;

        // Bouncing circle
        float bounce = std::abs(std::sin(m_Time * 3.0f)) * 30.0f;
        Gfx::Renderer::DrawCircle({
            .Position = {100.0f, procY + 50.0f - bounce, 0},
            .Radius = {20, 20},
            .Color = {0.2f, 0.8f, 0.4f, 1.0f}
        });
        Gfx::Renderer::DrawText({
            .Text = "Bounce",
            .Position = {100.0f, procY + 90.0f, 0},
            .Scale = 0.8f,
            .Color = {0.7f, 0.7f, 0.7f, 1.0f},
            .Alignment = Gfx::TextAlignment::Center
        });

        // Pulsing square
        float pulse = 1.0f + std::sin(m_Time * 4.0f) * 0.3f;
        Gfx::Renderer::DrawQuad({
            .Position = {250.0f, procY + 50.0f, 0},
            .Size = {30.0f * pulse, 30.0f * pulse},
            .Color = {0.8f, 0.4f, 0.2f, 1.0f}
        });
        Gfx::Renderer::DrawText({
            .Text = "Pulse",
            .Position = {250.0f, procY + 90.0f, 0},
            .Scale = 0.8f,
            .Color = {0.7f, 0.7f, 0.7f, 1.0f},
            .Alignment = Gfx::TextAlignment::Center
        });

        // Rotating square
        Gfx::Renderer::DrawQuad({
            .Position = {400.0f, procY + 50.0f, 0},
            .Size = {35.0f, 35.0f},
            .Color = {0.4f, 0.4f, 0.9f, 1.0f},
            .Rotation = m_Time * 2.0f
        });
        Gfx::Renderer::DrawText({
            .Text = "Rotate",
            .Position = {400.0f, procY + 90.0f, 0},
            .Scale = 0.8f,
            .Color = {0.7f, 0.7f, 0.7f, 1.0f},
            .Alignment = Gfx::TextAlignment::Center
        });

        // Color cycling
        float hue = std::fmod(m_Time * 0.5f, 1.0f);
        glm::vec4 rainbow = HsvToRgb(hue, 0.8f, 1.0f);
        Gfx::Renderer::DrawQuad({
            .Position = {550.0f, procY + 50.0f, 0},
            .Size = {35.0f, 35.0f},
            .Color = rainbow
        });
        Gfx::Renderer::DrawText({
            .Text = "Color",
            .Position = {550.0f, procY + 90.0f, 0},
            .Scale = 0.8f,
            .Color = {0.7f, 0.7f, 0.7f, 1.0f},
            .Alignment = Gfx::TextAlignment::Center
        });

        // Section labels
        Gfx::Renderer::DrawText({
            .Text = "Frame-based Animation (Atlas Tiles)",
            .Position = {300.0f, 20.0f, 0},
            .Scale = 1.2f,
            .Color = {1.0f, 0.9f, 0.5f, 1.0f},
            .Alignment = Gfx::TextAlignment::Center
        });

        Gfx::Renderer::DrawText({
            .Text = "Procedural Animation",
            .Position = {300.0f, procY - 20.0f, 0},
            .Scale = 1.2f,
            .Color = {1.0f, 0.9f, 0.5f, 1.0f},
            .Alignment = Gfx::TextAlignment::Center
        });

        // Controls hint
        Gfx::Renderer::DrawText({
            .Text = "Up/Down: Speed | Space: Pause",
            .Position = {300.0f, 380.0f, 0},
            .Scale = 0.9f,
            .Color = {0.5f, 0.5f, 0.5f, 1.0f},
            .Alignment = Gfx::TextAlignment::Center
        });

        Gfx::Renderer::End();

        // UI
        RenderUI();
    }

    void DrawAnimationDemo(const char* label, glm::vec2 pos, const SpriteAnimation& anim, glm::vec2 size)
    {
        // Draw current frame
        Gfx::Renderer::DrawSprite(*m_Atlas, anim.GetCurrentRegion(), {
            .Position = {pos.x, pos.y, 0},
            .Size = size,
            .Color = {1.0f, 1.0f, 1.0f, 1.0f}
        });

        // Draw label
        Gfx::Renderer::DrawText({
            .Text = label,
            .Position = {pos.x, pos.y + size.y / 2.0f + 20.0f, 0},
            .Scale = 0.8f,
            .Color = {0.7f, 0.7f, 0.7f, 1.0f},
            .Alignment = Gfx::TextAlignment::Center
        });

        // Draw frame indicator
        std::string frameText = std::to_string(anim.GetCurrentFrameIndex() + 1) + "/" +
                                  std::to_string(anim.GetFrameCount());
        Gfx::Renderer::DrawText({
            .Text = frameText.c_str(),
            .Position = {pos.x, pos.y + size.y / 2.0f + 40.0f, 0},
            .Scale = 0.7f,
            .Color = {0.5f, 0.5f, 0.5f, 1.0f},
            .Alignment = Gfx::TextAlignment::Center
        });
    }

    void RenderUI()
    {
        UI::BeginWindow("Animation Demo");
        UI::Text("Sprite Animation Demo");
        UI::Separator();

        UI::TextFmt("Speed: {:.1f}x", m_AnimationSpeed);
        UI::TextFmt("Paused: {}", m_Paused ? "Yes" : "No");
        UI::Separator();

        UI::Text("Controls:");
        UI::Text("  Up/Down - Adjust speed");
        UI::Text("  Space - Pause/Resume");
        UI::Separator();

        const auto& stats = Gfx::Renderer::GetStats();
        UI::TextFmt("Draw Calls: {}", stats.DrawCalls);
        UI::TextFmt("Quads: {}", stats.QuadCount);
        Gfx::Renderer::ResetStats();

        UI::EndWindow();

        // Viewport
        UI::BeginWindow("Viewport");
        auto tex = Gfx::Renderer::GetTexture();
        if (tex)
        {
            UI::Image(tex->GetTexture(), UI::GetContentSize());
        }
        UI::EndWindow();
    }

private:
    glm::vec4 HsvToRgb(float h, float s, float v)
    {
        float c = v * s;
        float x = c * (1 - std::abs(std::fmod(h * 6.0f, 2.0f) - 1));
        float m = v - c;
        float r, g, b;
        if (h < 1.0f/6.0f) { r = c; g = x; b = 0; }
        else if (h < 2.0f/6.0f) { r = x; g = c; b = 0; }
        else if (h < 3.0f/6.0f) { r = 0; g = c; b = x; }
        else if (h < 4.0f/6.0f) { r = 0; g = x; b = c; }
        else if (h < 5.0f/6.0f) { r = x; g = 0; b = c; }
        else { r = c; g = 0; b = x; }
        return {r + m, g + m, b + m, 1.0f};
    }

    Lumina::Ref<Gfx::Texture> m_AtlasTexture;
    Lumina::Ref<Gfx::TextureAtlas> m_Atlas;
    glm::vec2 m_TileSize{32.0f};

    SpriteAnimation m_AnimRow;
    SpriteAnimation m_AnimCol;
    SpriteAnimation m_AnimDiag;

    float m_Time = 0.0f;
    float m_AnimationSpeed = 1.0f;
    bool m_Paused = false;
    bool m_SpaceHeld = false;
};

Lumina::Application* Lumina::CreateApplication(int argc, char** argv)
{
    ApplicationSpecifications specs;
    specs.Title = "graphics/10-animation";
    auto* app = new Application(specs);
    app->PushLayer<AnimationLayer>();
    return app;
}
