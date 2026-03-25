// physics/00-hello-physics: Basic Physics Demo
// Demonstrates: Physics world, dynamic/static bodies, basic collision

#include <Lumina/Core/Core.h>
#include <Lumina/Core/EntryPoint.h>
#include <Lumina/Core/Input.h>
#include <Lumina/Graphics/Graphics.h>
#include <Lumina/Physics/Physics.h>
#include <Lumina/UI/UI.h>

#include <glm/glm.hpp>
#include <vector>
#include <random>

namespace UI = Lumina::UI;
namespace Gfx = Lumina::Graphics;
namespace Input = Lumina::Input;
namespace Physics = Lumina::Physics;

// Scale factor: pixels per meter (Box2D works in meters)
constexpr float PIXELS_PER_METER = 50.0f;

// Convert physics coords to render coords
inline glm::vec2 ToRender(const glm::vec2& physicsPos, float renderSize)
{
    return {
        physicsPos.x * PIXELS_PER_METER,
        renderSize - (physicsPos.y * PIXELS_PER_METER)  // Flip Y axis
    };
}

// Convert render coords to physics coords
inline glm::vec2 ToPhysics(const glm::vec2& renderPos, float renderSize)
{
    return {
        renderPos.x / PIXELS_PER_METER,
        (renderSize - renderPos.y) / PIXELS_PER_METER
    };
}

// Simple body wrapper with color
struct ColoredBody
{
    Lumina::Ref<Physics::Body> Body;
    glm::vec4 Color;
    bool IsCircle = false;
    float HalfWidth = 0.5f;
    float HalfHeight = 0.5f;
    float Radius = 0.5f;
};

class HelloPhysicsLayer : public Lumina::Layer
{
public:
    HelloPhysicsLayer() : Layer("HelloPhysicsLayer") {}

    void OnAttach() override
    {
        Gfx::Renderer::Init({.Width = 600, .Height = 600, .MSAA = Gfx::MSAAMode::X4});

        // Create physics world with gravity pointing down
        Physics::WorldDef worldDef;
        worldDef.Gravity = {0.0f, -9.81f};
        m_World = std::make_unique<Physics::World>(worldDef);

        // Create ground (static body)
        CreateGround();

        // Create some initial boxes
        for (int i = 0; i < 5; i++)
        {
            SpawnBox({3.0f + i * 1.2f, 8.0f + i * 0.5f});
        }

        LUMINA_LOG_INFO("Hello Physics demo loaded");
    }

    void OnDetach() override
    {
        m_Bodies.clear();
        m_World.reset();
        Gfx::Renderer::Shutdown();
    }

    void OnUpdate(float dt) override
    {
        // Step physics
        if (!m_Paused)
        {
            m_World->Step(dt);
        }

        // Spawn with mouse click (only when viewport is hovered)
        if (Input::IsMouseButtonPressed(Input::MouseCode::Left))
        {
            if (!m_MouseDown && m_ViewportHovered)
            {
                // Convert viewport-relative position to physics position
                glm::vec2 physicsPos = ToPhysics(m_ViewportMousePos, 600.0f);

                if (m_SpawnCircles)
                    SpawnCircle(physicsPos);
                else
                    SpawnBox(physicsPos);
            }
            m_MouseDown = true;
        }
        else
        {
            m_MouseDown = false;
        }

        // Reset with R key
        if (Input::IsKeyPressed(Input::KeyCode::R))
        {
            if (!m_RDown)
            {
                ResetSimulation();
            }
            m_RDown = true;
        }
        else
        {
            m_RDown = false;
        }

        // Pause with space
        if (Input::IsKeyPressed(Input::KeyCode::Space))
        {
            if (!m_SpaceDown)
            {
                m_Paused = !m_Paused;
            }
            m_SpaceDown = true;
        }
        else
        {
            m_SpaceDown = false;
        }
    }

    void OnRender() override
    {
        const float renderSize = 600.0f;

        Gfx::Renderer::Begin();
        Gfx::Renderer::Clear({0.1f, 0.1f, 0.12f, 1.0f});

        // Draw ground
        glm::vec2 groundRender = ToRender(m_GroundPos, renderSize);
        Gfx::Renderer::DrawQuad({
            .Position = {groundRender.x, groundRender.y, 0},
            .Size = {m_GroundWidth * PIXELS_PER_METER * 2, m_GroundHeight * PIXELS_PER_METER * 2},
            .Color = {0.3f, 0.3f, 0.35f, 1.0f}
        });

        // Draw walls
        glm::vec2 leftWallRender = ToRender(m_LeftWallPos, renderSize);
        Gfx::Renderer::DrawQuad({
            .Position = {leftWallRender.x, leftWallRender.y, 0},
            .Size = {m_WallThickness * PIXELS_PER_METER * 2, m_WallHeight * PIXELS_PER_METER * 2},
            .Color = {0.3f, 0.3f, 0.35f, 1.0f}
        });

        glm::vec2 rightWallRender = ToRender(m_RightWallPos, renderSize);
        Gfx::Renderer::DrawQuad({
            .Position = {rightWallRender.x, rightWallRender.y, 0},
            .Size = {m_WallThickness * PIXELS_PER_METER * 2, m_WallHeight * PIXELS_PER_METER * 2},
            .Color = {0.3f, 0.3f, 0.35f, 1.0f}
        });

        // Draw dynamic bodies
        for (const auto& cb : m_Bodies)
        {
            if (!cb.Body || !cb.Body->IsValid()) continue;

            glm::vec2 pos = cb.Body->GetPosition();
            float rotation = cb.Body->GetRotation();
            glm::vec2 renderPos = ToRender(pos, renderSize);

            if (cb.IsCircle)
            {
                Gfx::Renderer::DrawCircle({
                    .Position = {renderPos.x, renderPos.y, 0},
                    .Radius = {cb.Radius * PIXELS_PER_METER, cb.Radius * PIXELS_PER_METER},
                    .Color = cb.Color
                });

                // Draw a line to show rotation
                float lineLen = cb.Radius * PIXELS_PER_METER * 0.8f;
                glm::vec2 lineEnd = {
                    renderPos.x + std::cos(-rotation) * lineLen,
                    renderPos.y + std::sin(-rotation) * lineLen
                };
                Gfx::Renderer::DrawLine({
                    .Start = {renderPos.x, renderPos.y, 0.1f},
                    .End = {lineEnd.x, lineEnd.y, 0.1f},
                    .Color = {1, 1, 1, 0.8f},
                    .Thickness = 2.0f
                });
            }
            else
            {
                Gfx::Renderer::DrawQuad({
                    .Position = {renderPos.x, renderPos.y, 0},
                    .Size = {cb.HalfWidth * PIXELS_PER_METER * 2, cb.HalfHeight * PIXELS_PER_METER * 2},
                    .Color = cb.Color,
                    .Rotation = -rotation  // Flip rotation for render coords
                });
            }
        }

        // Draw title
        Gfx::Renderer::DrawText({
            .Text = "Hello Physics!",
            .Position = {300, 30, 0},
            .Scale = 1.5f,
            .Color = {1, 1, 1, 1},
            .Alignment = Gfx::TextAlignment::Center
        });

        // Draw instructions
        Gfx::Renderer::DrawText({
            .Text = "Click to spawn | R = Reset | Space = Pause",
            .Position = {300, 580, 0},
            .Scale = 0.9f,
            .Color = {0.6f, 0.6f, 0.6f, 1},
            .Alignment = Gfx::TextAlignment::Center
        });

        Gfx::Renderer::End();

        // UI
        RenderUI();
    }

    void RenderUI()
    {
        UI::BeginWindow("Physics Controls");
        UI::Text("Hello Physics Demo");
        UI::Separator();

        UI::TextFmt("Bodies: {}", m_Bodies.size());
        UI::TextFmt("Paused: {}", m_Paused ? "Yes" : "No");
        UI::Separator();

        UI::Text("Spawn Settings:");
        UI::Checkbox("Spawn Circles", m_SpawnCircles);
        UI::PropertySlider("Box Size", m_SpawnSize, 0.3f, 1.5f);
        UI::PropertySlider("Restitution", m_SpawnRestitution, 0.0f, 1.0f);
        UI::Separator();

        UI::Text("World Settings:");
        glm::vec2 gravity = m_World->GetGravity();
        if (UI::PropertySlider("Gravity Y", gravity.y, -20.0f, 20.0f))
        {
            m_World->SetGravity(gravity);
        }
        UI::Separator();

        if (UI::Button("Spawn Box"))
        {
            SpawnBox({6.0f, 10.0f});
        }
        UI::SameLine();
        if (UI::Button("Spawn Circle"))
        {
            SpawnCircle({6.0f, 10.0f});
        }

        if (UI::Button("Spawn 10 Random"))
        {
            for (int i = 0; i < 10; i++)
            {
                float x = 2.0f + m_Rng() % 80 / 10.0f;
                float y = 8.0f + m_Rng() % 30 / 10.0f;
                if (m_Rng() % 2 == 0)
                    SpawnBox({x, y});
                else
                    SpawnCircle({x, y});
            }
        }
        UI::SameLine();
        if (UI::Button("Clear All"))
        {
            m_Bodies.clear();
        }

        UI::Separator();
        const auto& stats = Gfx::Renderer::GetStats();
        UI::TextFmt("Draw Calls: {}", stats.DrawCalls);
        Gfx::Renderer::ResetStats();

        UI::EndWindow();

        // Viewport - maintain 1:1 aspect ratio
        UI::BeginWindow("Viewport");
        auto tex = Gfx::Renderer::GetTexture();
        if (tex)
        {
            auto contentSize = UI::GetContentSize();
            float size = std::min(contentSize.x, contentSize.y);
            auto imgRect = UI::ImageWithRect(tex->GetTexture(), size, size);

            // Track viewport hover and mouse position for spawning
            m_ViewportHovered = UI::IsItemHovered();
            if (m_ViewportHovered)
            {
                auto mousePos = UI::GetMousePos();
                // Calculate position relative to viewport image, scaled to render target size
                float scale = 600.0f / size;
                m_ViewportMousePos = {
                    (mousePos.x - imgRect.Pos.x) * scale,
                    (mousePos.y - imgRect.Pos.y) * scale
                };
            }
        }
        UI::EndWindow();
    }

private:
    void CreateGround()
    {
        // Ground
        Physics::BodyDef groundDef;
        groundDef.Type = Physics::BodyType::StaticBody;
        groundDef.Position = m_GroundPos;
        auto ground = m_World->CreateBody(groundDef);
        ground->AddBox(m_GroundWidth, m_GroundHeight);

        // Left wall
        Physics::BodyDef leftWallDef;
        leftWallDef.Type = Physics::BodyType::StaticBody;
        leftWallDef.Position = m_LeftWallPos;
        auto leftWall = m_World->CreateBody(leftWallDef);
        leftWall->AddBox(m_WallThickness, m_WallHeight);

        // Right wall
        Physics::BodyDef rightWallDef;
        rightWallDef.Type = Physics::BodyType::StaticBody;
        rightWallDef.Position = m_RightWallPos;
        auto rightWall = m_World->CreateBody(rightWallDef);
        rightWall->AddBox(m_WallThickness, m_WallHeight);
    }

    void SpawnBox(const glm::vec2& pos)
    {
        Physics::BodyDef def;
        def.Type = Physics::BodyType::Dynamic;
        def.Position = pos;
        def.Rotation = (m_Rng() % 100) / 100.0f * 0.5f;  // Small random rotation

        auto body = m_World->CreateBody(def);

        Physics::ShapeDef shapeDef;
        shapeDef.Material.Restitution = m_SpawnRestitution;
        shapeDef.Material.Friction = 0.5f;
        shapeDef.Material.Density = 1.0f;

        float halfSize = m_SpawnSize * 0.5f;
        body->AddBox(halfSize, halfSize, {}, 0.0f, shapeDef);

        ColoredBody cb;
        cb.Body = body;
        cb.Color = RandomColor();
        cb.IsCircle = false;
        cb.HalfWidth = halfSize;
        cb.HalfHeight = halfSize;

        m_Bodies.push_back(cb);
    }

    void SpawnCircle(const glm::vec2& pos)
    {
        Physics::BodyDef def;
        def.Type = Physics::BodyType::Dynamic;
        def.Position = pos;

        auto body = m_World->CreateBody(def);

        Physics::ShapeDef shapeDef;
        shapeDef.Material.Restitution = m_SpawnRestitution;
        shapeDef.Material.Friction = 0.5f;
        shapeDef.Material.Density = 1.0f;

        float radius = m_SpawnSize * 0.5f;
        body->AddCircle(radius, {}, shapeDef);

        ColoredBody cb;
        cb.Body = body;
        cb.Color = RandomColor();
        cb.IsCircle = true;
        cb.Radius = radius;

        m_Bodies.push_back(cb);
    }

    void ResetSimulation()
    {
        m_Bodies.clear();

        // Recreate world
        Physics::WorldDef worldDef;
        worldDef.Gravity = m_World->GetGravity();
        m_World = std::make_unique<Physics::World>(worldDef);

        CreateGround();

        // Spawn initial boxes
        for (int i = 0; i < 5; i++)
        {
            SpawnBox({3.0f + i * 1.2f, 8.0f + i * 0.5f});
        }
    }

    glm::vec4 RandomColor()
    {
        float h = (m_Rng() % 100) / 100.0f;
        float s = 0.6f + (m_Rng() % 40) / 100.0f;
        float v = 0.7f + (m_Rng() % 30) / 100.0f;

        // HSV to RGB
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

    std::unique_ptr<Physics::World> m_World;
    std::vector<ColoredBody> m_Bodies;

    // Ground dimensions (in meters) - 12m x 12m world
    glm::vec2 m_GroundPos = {6.0f, 0.5f};
    float m_GroundWidth = 5.5f;
    float m_GroundHeight = 0.5f;

    // Wall dimensions
    glm::vec2 m_LeftWallPos = {0.5f, 6.0f};
    glm::vec2 m_RightWallPos = {11.5f, 6.0f};
    float m_WallThickness = 0.5f;
    float m_WallHeight = 6.0f;

    // Spawn settings
    bool m_SpawnCircles = false;
    float m_SpawnSize = 0.8f;
    float m_SpawnRestitution = 0.3f;

    // State
    bool m_Paused = false;
    bool m_MouseDown = false;
    bool m_RDown = false;
    bool m_SpaceDown = false;

    // Viewport tracking
    bool m_ViewportHovered = false;
    glm::vec2 m_ViewportMousePos{0, 0};

    std::minstd_rand m_Rng{42};
};

Lumina::Application* Lumina::CreateApplication(int argc, char** argv)
{
    ApplicationSpecifications specs;
    specs.Title = "physics/00-hello-physics";
    auto* app = new Application(specs);
    app->PushLayer<HelloPhysicsLayer>();
    return app;
}
