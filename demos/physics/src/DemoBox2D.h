#include <vector>
#include <iostream>
#include <string>
#include <memory>
#include "imgui.h"
#include "Lumina/Lumina.h"
#include <glm/glm.hpp>
#include <box2d/box2d.h>

namespace Lumina
{
    struct PhysicsQuad
    {
        b2BodyId bodyId;
        glm::vec2 size;
        glm::vec4 color;

        PhysicsQuad(b2WorldId worldId, glm::vec2 position, glm::vec2 size, bool isStatic = false)
            : size(size), color(1.0f, 0.5f, 0.2f, 1.0f)
        {
            // Create body definition
            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = isStatic ? b2_staticBody : b2_dynamicBody;
            bodyDef.position = { position.x, position.y };
            bodyDef.rotation = b2MakeRot(0.0f);

            // Create body
            bodyId = b2CreateBody(worldId, &bodyDef);

            // Create box shape
            b2Polygon boxShape = b2MakeBox(size.x * 0.5f, size.y * 0.5f);

            // Create shape definition
            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = 1.0f;
            shapeDef.material.friction = 0.3f;
            shapeDef.material.restitution = 0.6f;

            b2CreatePolygonShape(bodyId, &shapeDef, &boxShape);
        }

        void Render()
        {
            b2Vec2 position = b2Body_GetPosition(bodyId);
            b2Rot rotation = b2Body_GetRotation(bodyId);
            float angle = b2Rot_GetAngle(rotation);

            Renderer2D::SetQuadPosition({ position.x, position.y, 0.0f });
            Renderer2D::SetQuadRotation({ 0.0f, 0.0f, glm::degrees(angle) });
            Renderer2D::SetQuadSize(size);
            Renderer2D::SetQuadTintColor(color);
            Renderer2D::SetQuadTexture(nullptr);
            Renderer2D::DrawQuad();
        }
    };

    struct PhysicsCircle
    {
        b2BodyId bodyId;
        float radius;
        glm::vec4 color;

        PhysicsCircle(b2WorldId worldId, glm::vec2 position, float radius)
            : radius(radius), color(0.2f, 0.8f, 0.5f, 1.0f)
        {
            // Create body definition
            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = b2_dynamicBody;
            bodyDef.position = { position.x, position.y };

            // Create body
            bodyId = b2CreateBody(worldId, &bodyDef);

            // Create circle shape
            b2Circle circleShape = { {0.0f, 0.0f}, radius };

            // Create shape definition
            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = 1.0f;
            shapeDef.material.friction = 0.3f;
            shapeDef.material.restitution = 0.8f;

            b2CreateCircleShape(bodyId, &shapeDef, &circleShape);
        }

        void Render()
        {
            b2Vec2 position = b2Body_GetPosition(bodyId);

            Renderer2D::SetCirclePosition({ position.x, position.y, 0.0f });
            Renderer2D::SetCircleRadius({ radius * 2, radius * 2 });
            Renderer2D::SetCircleColor(color);
            Renderer2D::SetCircleThickness(1.0f);
            Renderer2D::SetCircleFade(0.0f);
            Renderer2D::DrawCircle();
        }
    };

    class DemoBox2D : public Layer
    {
    public:
        virtual void OnAttach() override
        {
			m_OrthographicCamera = CreateRef<OrthographicCamera>(16.0f, 9.0f, 0.1f, 100.0f);

            m_OrthographicCamera->SetPosition(glm::vec3(0.0f, 0.0f, 10.0f));
            m_OrthographicCamera->LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
            m_OrthographicCamera->SetZoom(50.0f);

            // Create Box2D world with gravity
            b2WorldDef worldDef = b2DefaultWorldDef();
            worldDef.gravity = { 0.0f, -20.0f }; // Strong gravity
            m_WorldId = b2CreateWorld(&worldDef);

            // Create ground (static boxes)
            m_Quads.emplace_back(m_WorldId, glm::vec2(0.0f, -8.0f), glm::vec2(20.0f, 1.0f), true);
            m_Quads.back().color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f); // Gray ground

            // Create walls
            m_Quads.emplace_back(m_WorldId, glm::vec2(-10.0f, 0.0f), glm::vec2(1.0f, 20.0f), true);
            m_Quads.back().color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

            m_Quads.emplace_back(m_WorldId, glm::vec2(10.0f, 0.0f), glm::vec2(1.0f, 20.0f), true);
            m_Quads.back().color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

            // Create some falling boxes
            for (int i = 0; i < 5; ++i)
            {
                float x = (i - 2) * 2.0f;
                m_Quads.emplace_back(m_WorldId, glm::vec2(x, 5.0f + i * 2.0f), glm::vec2(1.0f, 1.0f));
            }

            // Create some bouncy circles
            for (int i = 0; i < 3; ++i)
            {
                float x = (i - 1) * 3.0f;
                m_Circles.emplace_back(m_WorldId, glm::vec2(x, 8.0f + i * 2.0f), 0.5f);
            }
        }

        virtual void OnDetach() override
        {
            m_Quads.clear();
            m_Circles.clear();
            b2DestroyWorld(m_WorldId);
        }

        virtual void OnUpdate(float ts) override
        {
            Renderer2D::ResetStats();

            // Step the physics simulation
            float timeStep = ts;
            if (timeStep > 1.0f / 30.0f) timeStep = 1.0f / 30.0f; // Cap to 30fps minimum

            if (m_SimulationRunning)
            {
                b2World_Step(m_WorldId, timeStep, 4);
            }

            m_FPS = 1.0f / (ts > 0.0f ? ts : 0.016f);
        }

        virtual void OnUIRender() override
        {
            // Main rendering viewport
            ImGui::Begin("Physics Demo");
            ImGui::SetCursorPos({ 0.0f, 0.0f });
            ImVec2 size = ImGui::GetContentRegionAvail();

            m_OrthographicCamera->SetSize(size.x, size.y);

            Renderer2D::SetResolution(size.x, size.y);
            Renderer2D::SetRenderMode(PolygonMode::Fill);
            Renderer2D::Begin(m_OrthographicCamera);

            // Render all physics objects
            for (auto& quad : m_Quads)
            {
                quad.Render();
            }

            for (auto& circle : m_Circles)
            {
                circle.Render();
            }

            Renderer2D::End();

            ImGui::Image((void*)(intptr_t)Renderer2D::GetImage(), { size.x, size.y });
            ImGui::End();

            // Controls panel
            RenderControlsPanel();

            // Statistics panel
            RenderStatsPanel();
        }

    private:
        void RenderControlsPanel()
        {
            ImGui::Begin("Physics Controls");

            ImGui::Text("Simulation");
            if (ImGui::Button(m_SimulationRunning ? "Pause" : "Play"))
            {
                m_SimulationRunning = !m_SimulationRunning;
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset"))
            {
                ResetSimulation();
            }

            ImGui::Separator();

            ImGui::Text("Add Objects");
            if (ImGui::Button("Add Box"))
            {
                AddRandomBox();
            }
            ImGui::SameLine();
            if (ImGui::Button("Add Circle"))
            {
                AddRandomCircle();
            }

            ImGui::Separator();

            ImGui::Text("World Settings");
            b2Vec2 gravity = b2World_GetGravity(m_WorldId);
            float gravityY = gravity.y;
            if (ImGui::SliderFloat("Gravity", &gravityY, -50.0f, 10.0f))
            {
                b2World_SetGravity(m_WorldId, { 0.0f, gravityY });
            }

            ImGui::Separator();

            ImGui::Text("Object Counts");
            ImGui::Text("Boxes: %zu", m_Quads.size());
            ImGui::Text("Circles: %zu", m_Circles.size());

            if (ImGui::Button("Clear Dynamic Objects"))
            {
                ClearDynamicObjects();
            }

            ImGui::End();
        }

        void RenderStatsPanel()
        {
            ImGui::Begin("Renderer Statistics");

            auto stats = Renderer2D::GetStats();

            ImGui::Text("Performance:");
            ImGui::Text("FPS: %.1f", m_FPS);
            ImGui::Text("Draw Calls: %u", stats.DrawCalls);
            ImGui::Text("Data Size: %u bytes", stats.DataSize);

            ImGui::Separator();
            ImGui::Text("Geometry Count:");
            ImGui::Text("Quads: %u", stats.QuadCount);
            ImGui::Text("Circles: %u", stats.CircleCount);

            ImGui::Separator();
            ImGui::Text("Total Vertices: %u", stats.GetTotalVertexCount());
            ImGui::Text("Total Indices: %u", stats.GetTotalIndexCount());

            ImGui::End();
        }

        void ResetSimulation()
        {
            m_Quads.clear();
            m_Circles.clear();
            b2DestroyWorld(m_WorldId);

            // Recreate world
            b2WorldDef worldDef = b2DefaultWorldDef();
            worldDef.gravity = { 0.0f, -20.0f };
            m_WorldId = b2CreateWorld(&worldDef);

            // Recreate ground and walls
            m_Quads.emplace_back(m_WorldId, glm::vec2(0.0f, -8.0f), glm::vec2(20.0f, 1.0f), true);
            m_Quads.back().color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

            m_Quads.emplace_back(m_WorldId, glm::vec2(-10.0f, 0.0f), glm::vec2(1.0f, 20.0f), true);
            m_Quads.back().color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

            m_Quads.emplace_back(m_WorldId, glm::vec2(10.0f, 0.0f), glm::vec2(1.0f, 20.0f), true);
            m_Quads.back().color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        }

        void AddRandomBox()
        {
            float x = ((rand() % 100) / 100.0f - 0.5f) * 10.0f;
            float y = 10.0f + (rand() % 100) / 100.0f * 5.0f;
            float size = 0.5f + (rand() % 100) / 100.0f * 1.5f;

            m_Quads.emplace_back(m_WorldId, glm::vec2(x, y), glm::vec2(size, size));

            // Random color
            m_Quads.back().color = glm::vec4(
                (rand() % 100) / 100.0f,
                (rand() % 100) / 100.0f,
                (rand() % 100) / 100.0f,
                1.0f
            );
        }

        void AddRandomCircle()
        {
            float x = ((rand() % 100) / 100.0f - 0.5f) * 10.0f;
            float y = 10.0f + (rand() % 100) / 100.0f * 5.0f;
            float radius = 0.3f + (rand() % 100) / 100.0f * 0.7f;

            m_Circles.emplace_back(m_WorldId, glm::vec2(x, y), radius);

            // Random color
            m_Circles.back().color = glm::vec4(
                (rand() % 100) / 100.0f,
                (rand() % 100) / 100.0f,
                (rand() % 100) / 100.0f,
                1.0f
            );
        }

        void ClearDynamicObjects()
        {
            for (size_t i = 3; i < m_Quads.size(); ++i)
            {
                b2DestroyBody(m_Quads[i].bodyId);
            }

            for (auto& circle : m_Circles)
            {
                b2DestroyBody(circle.bodyId);
            }

            m_Quads.erase(m_Quads.begin() + 3, m_Quads.end());
            m_Circles.clear();
        }

    private:
        Ref<OrthographicCamera> m_OrthographicCamera;
        b2WorldId m_WorldId;
        std::vector<PhysicsQuad> m_Quads;
        std::vector<PhysicsCircle> m_Circles;

        bool m_SimulationRunning = true;
        float m_FPS = 0.0f;
    };
}