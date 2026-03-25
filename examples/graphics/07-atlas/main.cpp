// 07-atlas: Texture Atlas and Camera2D Demo
// Demonstrates: texture_atlas, camera2d, sprite rendering with the new optimizations

#include <lumina/core/Core.h>
#include <lumina/core/EntryPoint.h>
#include <lumina/core/Input.h>
#include <lumina/graphics/Graphics.h>
#include <lumina/ui/UI.h>

#include <glm/glm.hpp>
#include <filesystem>

namespace UI = Lumina::UI;
namespace Gfx = Lumina::Graphics;
namespace Input = Lumina::Input;

// Get asset path relative to this source file
static std::string GetAssetPath(const std::string& filename)
{
    std::filesystem::path sourceDir = std::filesystem::path(__FILE__).parent_path();
    return (sourceDir / filename).string();
}

class AtlasLayer : public Lumina::Layer
{
public:
    AtlasLayer() : Layer("Atlas") {}

    void OnAttach() override
    {
        auto& device = Lumina::Application::Get().GetDevice();
        m_Renderer = std::make_unique<Gfx::Renderer2D>(device);
        m_Renderer->Init();

        // Create render target for the viewport (square)
        m_RenderTarget = Gfx::RenderTarget::Create(
            device, 600, 600, Gfx::Format::RGBA8_UNORM
        );

        // Load the factory atlas texture (relative to source file)
        m_AtlasTexture = Gfx::Texture::LoadFromFile(device, GetAssetPath("factory_atlas.png"));
        if (!m_AtlasTexture)
        {
            LUMINA_LOG_ERROR("Failed to load factory_atlas.png");
            return;
        }

        // Create texture atlas from the texture
        m_Atlas = Gfx::TextureAtlas::Create(m_AtlasTexture);
        if (!m_Atlas)
        {
            LUMINA_LOG_ERROR("Failed to create texture atlas");
            return;
        }

        // Get tile size from texture dimensions (16x16 grid)
        uint32_t texWidth = m_AtlasTexture->GetWidth();
        uint32_t texHeight = m_AtlasTexture->GetHeight();
        m_TileSize = glm::vec2(texWidth / 16.0f, texHeight / 16.0f);

        // Add a 16x16 grid of tiles to the atlas
        m_Atlas->AddGrid("tile_", 16, 16, m_TileSize);

        LUMINA_LOG_INFO("Atlas created: {}x{} texture, {:.0f}x{:.0f} tile size, {} regions",
            texWidth, texHeight, m_TileSize.x, m_TileSize.y, m_Atlas->GetRegionCount());

        // Initialize camera (square aspect ratio)
        m_Camera = Gfx::Camera2D(600.0f, 1.0f);
        m_Camera.SetPosition({300.0f, 300.0f});
        m_Camera.Update(0.0f);  // Process dirty flags
    }

    void OnDetach() override
    {
        m_Atlas.reset();
        m_AtlasTexture.reset();
        m_RenderTarget.reset();
        m_Renderer.reset();
    }

    void OnUpdate(float dt) override
    {
        m_Time += dt;

        // Camera controls
        float moveSpeed = 300.0f * dt;

        glm::vec2 moveDir{0.0f};
        if (Input::IsKeyPressed(Input::KeyCode::W) || Input::IsKeyPressed(Input::KeyCode::Up))
            moveDir.y += 1.0f;
        if (Input::IsKeyPressed(Input::KeyCode::S) || Input::IsKeyPressed(Input::KeyCode::Down))
            moveDir.y -= 1.0f;
        if (Input::IsKeyPressed(Input::KeyCode::A) || Input::IsKeyPressed(Input::KeyCode::Left))
            moveDir.x -= 1.0f;
        if (Input::IsKeyPressed(Input::KeyCode::D) || Input::IsKeyPressed(Input::KeyCode::Right))
            moveDir.x += 1.0f;

        if (glm::length(moveDir) > 0.0f)
        {
            moveDir = glm::normalize(moveDir);
            m_Camera.Move(moveDir * moveSpeed / m_Camera.GetZoom());
        }

        // Zoom controls
        if (Input::IsKeyPressed(Input::KeyCode::Q))
            m_Camera.SetZoom(m_Camera.GetZoom() * (1.0f + dt));
        if (Input::IsKeyPressed(Input::KeyCode::E))
            m_Camera.SetZoom(m_Camera.GetZoom() * (1.0f - dt));

        // Rotation controls
        if (Input::IsKeyPressed(Input::KeyCode::Z))
            m_Camera.SetRotation(m_Camera.GetRotation() + dt);
        if (Input::IsKeyPressed(Input::KeyCode::X))
            m_Camera.SetRotation(m_Camera.GetRotation() - dt);

        // Camera shake (space key)
        if (Input::IsKeyPressed(Input::KeyCode::Space) && !m_Camera.IsShaking())
        {
            m_Camera.StartShake(10.0f, 0.3f);
        }

        // Update camera
        m_Camera.Update(dt);
    }

    void OnRender() override
    {
        if (!m_Renderer || !m_RenderTarget || !m_Atlas) return;

        // Begin rendering with the camera
        m_Renderer->Begin(m_Camera);
        m_Renderer->SetRenderTarget(m_RenderTarget);

        // Use point filtering for pixel art
        m_Renderer->SetFilterMode(Gfx::FilterMode::Point);

        // Draw background
        m_Renderer->DrawQuad({
            .Position = {400, 300, -0.1f},
            .Size = {2000, 2000},
            .Color = {0.15f, 0.15f, 0.2f, 1.0f},
            .Layer = Gfx::RenderLayer::Background
        });

        // Draw a grid of tiles from the atlas
        float spacing = m_TileSize.x * 1.2f;
        int gridCols = 8;
        int gridRows = 4;
        float startX = 400.0f - (gridCols * spacing) / 2.0f + spacing / 2.0f;
        float startY = 300.0f - (gridRows * spacing) / 2.0f + spacing / 2.0f;

        int tileIndex = 0;
        for (int row = 0; row < gridRows; row++)
        {
            for (int col = 0; col < gridCols; col++)
            {
                float x = startX + col * spacing;
                float y = startY + row * spacing;

                // Use the tile from atlas
                m_Renderer->DrawSprite(*m_Atlas, static_cast<uint32_t>(tileIndex), {
                    .Position = {x, y, 0.0f},
                    .Size = m_TileSize,
                    .Color = {1.0f, 1.0f, 1.0f, 1.0f},
                    .Layer = Gfx::RenderLayer::Sprites
                });

                tileIndex++;
            }
        }

        // Draw some animated sprites
        float animOffset = std::sin(m_Time * 2.0f) * 50.0f;

        // Sprite 1 - bouncing
        m_Renderer->DrawSprite(*m_Atlas, 0, {
            .Position = {150.0f, 150.0f + animOffset, 0.1f},
            .Size = m_TileSize * 2.0f,
            .Color = {1.0f, 0.8f, 0.8f, 1.0f},
            .Rotation = m_Time,
            .Layer = Gfx::RenderLayer::Sprites
        });

        // Sprite 2 - flipped
        m_Renderer->DrawSprite(*m_Atlas, 1, {
            .Position = {650.0f, 150.0f - animOffset, 0.1f},
            .Size = m_TileSize * 2.0f,
            .Color = {0.8f, 1.0f, 0.8f, 1.0f},
            .FlipX = true,
            .Layer = Gfx::RenderLayer::Sprites
        });

        // Sprite 3 - pulsing
        float pulse = 1.0f + std::sin(m_Time * 4.0f) * 0.2f;
        m_Renderer->DrawSprite(*m_Atlas, 16, {
            .Position = {300.0f, 500.0f, 0.1f},
            .Size = m_TileSize * pulse * 1.5f,
            .Color = {0.8f, 0.8f, 1.0f, 1.0f},
            .Layer = Gfx::RenderLayer::Sprites
        });

        // Blend mode demonstrations
        float blendY = 500.0f;

        // Additive blend (glow effect)
        m_Renderer->DrawSprite(*m_Atlas, 17, {
            .Position = {400.0f, blendY, 0.2f},
            .Size = m_TileSize * 1.5f,
            .Color = {1.0f, 0.8f, 0.2f, 0.8f},
            .Layer = Gfx::RenderLayer::Effects,
            .Blend = Gfx::BlendMode::Additive
        });

        // Multiply blend (shadow/darken)
        m_Renderer->DrawSprite(*m_Atlas, 18, {
            .Position = {500.0f, blendY, 0.2f},
            .Size = m_TileSize * 1.5f,
            .Color = {0.5f, 0.5f, 0.8f, 1.0f},
            .Layer = Gfx::RenderLayer::Effects,
            .Blend = Gfx::BlendMode::Multiply
        });

        m_Renderer->End();

        // UI
        UI::BeginWindow("Atlas Demo");
        UI::Text("Texture Atlas & Camera2D Demo");
        UI::Separator();

        UI::Text("Controls:");
        UI::Text("  WASD/Arrows - Pan camera");
        UI::Text("  Q/E - Zoom in/out");
        UI::Text("  Z/X - Rotate camera");
        UI::Text("  Space - Camera shake");
        UI::Separator();

        auto pos = m_Camera.GetPosition();
        UI::TextFmt("Camera Pos: ({:.0f}, {:.0f})", pos.x, pos.y);
        UI::TextFmt("Zoom: {:.2f}x", m_Camera.GetZoom());
        UI::TextFmt("Rotation: {:.1f} deg", glm::degrees(m_Camera.GetRotation()));
        UI::TextFmt("Shaking: {}", m_Camera.IsShaking() ? "Yes" : "No");
        UI::Separator();

        UI::TextFmt("Atlas Regions: {}", m_Atlas->GetRegionCount());
        UI::TextFmt("Tile Size: {:.0f}x{:.0f}", m_TileSize.x, m_TileSize.y);
        UI::Separator();

        UI::Text("Blend Modes:");
        UI::Text("  Yellow sprite - Additive (glow)");
        UI::Text("  Blue sprite - Multiply (darken)");
        UI::Separator();

        const auto& stats = m_Renderer->GetStats();
        UI::TextFmt("Draw Calls: {}", stats.DrawCalls);
        UI::TextFmt("Quads: {}", stats.QuadCount);
        m_Renderer->ResetStats();
        UI::EndWindow();

        // Viewport
        UI::BeginWindow("Viewport");
        auto tex = m_RenderTarget->GetColorTexture();
        if (tex)
        {
            UI::Image(tex->GetTexture(), UI::GetContentSize());
        }
        UI::EndWindow();
    }

private:
    std::unique_ptr<Gfx::Renderer2D> m_Renderer;
    Lumina::Ref<Gfx::RenderTarget> m_RenderTarget;
    Lumina::Ref<Gfx::Texture> m_AtlasTexture;
    Lumina::Ref<Gfx::TextureAtlas> m_Atlas;
    Gfx::Camera2D m_Camera;

    glm::vec2 m_TileSize{32.0f};
    float m_Time = 0.0f;
};

Lumina::Application* Lumina::CreateApplication(int argc, char** argv)
{
    ApplicationSpecifications specs;
    specs.Title = "graphics/07-atlas";
    auto* app = new Application(specs);
    app->PushLayer<AtlasLayer>();
    return app;
}
