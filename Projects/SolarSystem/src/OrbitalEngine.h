#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <memory>
#include <algorithm>

#include "imgui.h"

#include "Lumina/Core/Layer.h"
#include "Lumina/Core/Log.h"
#include "Lumina/Core/Assert.h"

#include "Lumina/Utils/Timer.h"
#include "Lumina/Graphics/Renderer3D.h"
#include "Lumina/Graphics/Model.h"
#include "Lumina/Graphics/ModelRegistry.h"
#include "Lumina/Graphics/Skybox.h"

#include "Lumina/Graphics/Cameras/OrthographicCamera.h"
#include "Lumina/Graphics/Cameras/PerspectiveCamera.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SolarSystem.h"

namespace Lumina
{
    class OrbitalEngine : public Layer
    {
    public:
        
        virtual void OnAttach() override
        {
            Renderer3D::Init();
            ModelRegistry::Init();

			m_SolarSystem.Init();

			Renderer3D::SetDirectionalLight(m_SolarSystem.GetDirectionalLight());
			auto pointLights = m_SolarSystem.GetPointLights();
            for (const auto& light : pointLights)
            {
				Renderer3D::AddPointLight(light);
            }
        }

        virtual void OnDetach() override
        {
            ModelRegistry::Shutdown();
            Renderer3D::Shutdown();
        }

        virtual void OnUpdate(float ts) override
        {
            Renderer3D::ResetStats();

            m_FPS = 1.0f / m_FrameTimer.Elapsed();
            m_FrameTimer.Reset();

			m_SolarSystem.Update(ts);
        }

        virtual void OnUIRender() override
        {   
            ImGui::Begin("FPS");
            ImGui::Text("FPS: %.2f", m_FPS);
            ImGui::End();

            {
                ImGui::Begin("Planetarium View");
                ImGui::SetCursorPos({ 0.0f, 0.0f });
                ImVec2 size = ImGui::GetContentRegionAvail();
                m_SolarSystem.SetViewportSize(size.x, size.y);
                m_SolarSystem.Render();
                ImGui::Image((void*)(intptr_t)Renderer3D::GetImage(), { size.x, size.y });
                ImGui::End();
            }

            {
                ImGui::Begin("Render Controls");
                auto renderMode = Renderer3D::GetRenderMode();
                if (ImGui::BeginCombo("Render Mode",
                    renderMode == RenderMode::Normal ? "Normal" :
                    renderMode == RenderMode::Wireframe ? "Wireframe" : "Points"))
                {
                    if (ImGui::Selectable("Normal", renderMode == RenderMode::Normal))
                        Renderer3D::SetRenderMode(RenderMode::Normal);
                    if (ImGui::Selectable("Wireframe", renderMode == RenderMode::Wireframe))
                        Renderer3D::SetRenderMode(RenderMode::Wireframe);
                    if (ImGui::Selectable("Points", renderMode == RenderMode::Points))
                        Renderer3D::SetRenderMode(RenderMode::Points);
                    ImGui::EndCombo();
                }
                ImGui::End();
            }

            {
                ImGui::Begin("Planetarium Controls");
                ImGui::Checkbox("Enable Animation", &m_SolarSystem.GetAnimationEnabled());
                ImGui::SameLine();
                if (ImGui::Button("Reset"))
                {
                    m_SolarSystem.Reset();
                }
                ImGui::SliderFloat("Time Scale", &m_SolarSystem.GetTimeScale(), 0.1f, 5.0f);
                ImGui::Text("Simulation Time: %.1f", m_SolarSystem.GetCurrentTime());
                ImGui::Separator();
                ImGui::Checkbox("Show Asteroids", &m_SolarSystem.GetShowAsteroids());
                ImGui::Checkbox("Show Planets", &m_SolarSystem.GetShowPlanets());
                ImGui::End();
            }

            {
                ImGui::Begin("Camera Controls");
                static int cameraMode = 0;
                static float followDistance = 1500.0f;
                static float followHeight = 500.0f;
                static bool lookAtPlanet = true;
                static bool orbitWithPlanet = true;

                ImGui::Text("Camera Mode:");
                if (ImGui::RadioButton("Free Camera", cameraMode == 0))
                    cameraMode = 0;
                auto& planets = m_SolarSystem.GetPlanets();
                for (size_t i = 0; i < planets.size(); i++)
                {
                    if (ImGui::RadioButton(planets[i].GetName().c_str(), cameraMode == (int)(i + 1)))
                        cameraMode = (int)(i + 1);
                }
                if (cameraMode > 0)
                {
                    ImGui::Separator();
                    ImGui::Text("Following Settings:");
                    ImGui::SliderFloat("Follow Distance", &followDistance, 500.0f, 5000.0f);
                    ImGui::SliderFloat("Height Offset", &followHeight, -1000.0f, 2000.0f);
                    ImGui::Checkbox("Look at Planet", &lookAtPlanet);
                    ImGui::Checkbox("Orbit with Planet", &orbitWithPlanet);
                }
                auto& camera = m_SolarSystem.GetCamera();
                if (cameraMode > 0 && cameraMode <= (int)planets.size())
                {
                    const auto& followPlanet = planets[cameraMode - 1];
                    glm::vec3 planetPos = followPlanet.GetAttributes().Position;
                    glm::vec3 sunPos = glm::vec3(0.0f, 0.0f, 0.0f);
                    glm::vec3 cameraPos;
                    if (orbitWithPlanet && followPlanet.GetOrbitRadius() > 0.0f)
                    {
                        glm::vec3 sunToPlanet = glm::normalize(planetPos - sunPos);
                        cameraPos = planetPos + (sunToPlanet * followDistance) + glm::vec3(0.0f, followHeight, 0.0f);
                    }
                    else
                    {
                        cameraPos = planetPos + glm::vec3(followDistance, followHeight, followDistance * 0.5f);
                    }

                    camera.SetPosition(cameraPos);
                    if (lookAtPlanet)
                    {
                        glm::vec3 lookDirection = glm::normalize(sunPos - cameraPos);
                        camera.LookAt(cameraPos + lookDirection);
                    }
                }
                else
                {
                    glm::vec3 position = camera.GetPosition();
                    if (ImGui::DragFloat3("Position", glm::value_ptr(position), 1.0f))
                        camera.SetPosition(position);
                    glm::quat rotation = camera.GetRotation();
                    if (ImGui::DragFloat4("Rotation", glm::value_ptr(rotation), 0.05f))
                        camera.SetRotation(rotation);
                }
                ImGui::Separator();
                ImGui::Text("Quick Camera Positions:");
                if (ImGui::Button("Reset to Overview"))
                {
                    cameraMode = 0;
                    camera.SetPosition(glm::vec3(0.0f, 12000.0f, 12000.0f));
                    camera.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
                }
                ImGui::SameLine();
                if (ImGui::Button("Top-Down View"))
                {
                    cameraMode = 0;
                    camera.SetPosition(glm::vec3(0.0f, 15000.0f, 1.0f));
                    camera.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
                }
                ImGui::SameLine();
                if (ImGui::Button("Inner System"))
                {
                    cameraMode = 0;
                    camera.SetPosition(glm::vec3(0.0f, 3000.0f, 6000.0f));
                    camera.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
                }
                if (ImGui::Button("Outer System"))
                {
                    cameraMode = 0; // Switch to free camera
                    camera.SetPosition(glm::vec3(0.0f, 8000.0f, 20000.0f));
                    camera.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
                }
                ImGui::SameLine();
                if (ImGui::Button("Asteroid Belt"))
                {
                    cameraMode = 0; // Switch to free camera
                    camera.SetPosition(glm::vec3(5500.0f, 1000.0f, 5500.0f));
                    camera.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
                }
                ImGui::SameLine();
                if (ImGui::Button("Sun Close-up"))
                {
                    cameraMode = 0; // Switch to free camera
                    camera.SetPosition(glm::vec3(1500.0f, 800.0f, 1500.0f));
                    camera.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
                }
                ImGui::Separator();
                ImGui::Text("Camera Info:");
                glm::vec3 pos = camera.GetPosition();
                ImGui::Text("Position: %.1f, %.1f, %.1f", pos.x, pos.y, pos.z);
                if (cameraMode > 0)
                {
                    ImGui::Text("Following: %s", planets[cameraMode - 1].GetName().c_str());
                    float distToTarget = glm::length(pos - planets[cameraMode - 1].GetAttributes().Position);
                    ImGui::Text("Distance to Target: %.1f", distToTarget);
                }
                ImGui::End();
            }

            {
                ImGui::Begin("Lighting Controls");
                bool lightUpdated = false;
                DirectionalLight& directionalLight = m_SolarSystem.GetDirectionalLight();
                lightUpdated |= ImGui::DragFloat3("Sun Direction", glm::value_ptr(directionalLight.Direction), 0.01f);
                lightUpdated |= ImGui::ColorEdit3("Sun Color", glm::value_ptr(directionalLight.Color));
                lightUpdated |= ImGui::DragFloat("Sun Intensity", &directionalLight.Intensity, 0.1f, 0.0f, 10.0f);
                if (lightUpdated)
                {
                    directionalLight.Direction = glm::normalize(directionalLight.Direction);
                    Renderer3D::SetDirectionalLight(directionalLight);
                }
                ImGui::End();
            }

            {
                ImGui::Begin("Statistics");
                Renderer3D::Statistics stats = Renderer3D::GetStats();
                ImGui::Text("Draw Calls: %d", stats.DrawCalls);
                ImGui::Text("Models: %d", stats.ModelCount);
                ImGui::Text("Triangles: %d", stats.TriangleCount);
                ImGui::Text("Vertices: %d", stats.VertexCount);
                ImGui::End();
            }
            
            {
                ImGui::Begin("Planetary System");
                auto& planets = m_SolarSystem.GetPlanets();
                for (size_t i = 0; i < planets.size(); i++)
                {
                    ImGui::PushID(static_cast<int>(i));
                    if (ImGui::CollapsingHeader(planets[i].GetName().c_str()))
                    {
                        ModelAttributes& modelAttributes = planets[i].GetAttributes();
                        OrbitalAttributes& orbitalAttributes = planets[i].GetOrbitalAttributes();
                        ImGui::DragFloat3("Position", glm::value_ptr(modelAttributes.Position), 0.1f);
                        ImGui::DragFloat3("Rotation", glm::value_ptr(modelAttributes.Rotation), 0.1f);
                        ImGui::DragFloat3("Scale", glm::value_ptr(modelAttributes.Scale), 0.1f);
                        ImGui::SliderFloat("Orbit Speed", &orbitalAttributes.OrbitSpeed, 0.0f, 10.0f);
                        ImGui::SliderFloat("Rotation Speed", &orbitalAttributes.RotationSpeed, -5.0f, 5.0f);
                        ImGui::ColorEdit4("Tint", glm::value_ptr(modelAttributes.TintColor));
                    }
                    ImGui::PopID();
                }
                ImGui::End();
            }

            {
                ImGui::Begin("Skybox Controls");

                auto& skyboxAttribs = m_SolarSystem.GetSkyboxAttributes();

                ImGui::Text("=== Appearance ===");
                ImGui::SliderFloat("Intensity", &skyboxAttribs.Intensity, 0.0f, 3.0f);
                ImGui::ColorEdit3("Tint", glm::value_ptr(skyboxAttribs.Tint));
                ImGui::SliderFloat("Exposure", &skyboxAttribs.Exposure, 0.1f, 3.0f);
                ImGui::SliderFloat("Saturation", &skyboxAttribs.Saturation, 0.0f, 2.0f);
                ImGui::SliderFloat("Contrast", &skyboxAttribs.Contrast, 0.5f, 2.0f);
                ImGui::SliderFloat("Alpha", &skyboxAttribs.Alpha, 0.0f, 1.0f);

                ImGui::Separator();
                ImGui::Text("=== Rotation ===");
                ImGui::Checkbox("Enable Auto-Rotation", &skyboxAttribs.EnableRotation);

                if (skyboxAttribs.EnableRotation)
                {
                    ImGui::SliderFloat("Rotation Speed", &skyboxAttribs.RotationSpeed, -1.0f, 1.0f);
                }

                ImGui::SliderFloat("Manual Rotation", &skyboxAttribs.Rotation, 0.0f, glm::two_pi<float>());
                ImGui::DragFloat3("Rotation Axis", glm::value_ptr(skyboxAttribs.RotationAxis), 0.01f, -1.0f, 1.0f);

                if (ImGui::Button("Reset Rotation"))
                {
                    skyboxAttribs.Rotation = 0.0f;
                    skyboxAttribs.RotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
                }

                ImGui::Separator();
                ImGui::Text("=== Time of Day (Experimental) ===");
                ImGui::Checkbox("Use Time of Day", &skyboxAttribs.UseTimeOfDay);

                if (skyboxAttribs.UseTimeOfDay)
                {
                    ImGui::SliderFloat("Time of Day", &skyboxAttribs.TimeOfDay, 0.0f, 1.0f);
                    ImGui::Text("%.1f = Midnight, 0.5 = Noon, 1.0 = Midnight", skyboxAttribs.TimeOfDay);
                }

                ImGui::Separator();
                ImGui::Text("=== Presets ===");

                if (ImGui::Button("Space (Default)"))
                {
                    skyboxAttribs.Intensity = 1.2f;
                    skyboxAttribs.Tint = glm::vec3(0.9f, 0.95f, 1.0f);
                    skyboxAttribs.Saturation = 1.1f;
                    skyboxAttribs.Contrast = 1.0f;
                    skyboxAttribs.EnableRotation = false;
                }

                ImGui::SameLine();
                if (ImGui::Button("Warm Galaxy"))
                {
                    skyboxAttribs.Intensity = 1.5f;
                    skyboxAttribs.Tint = glm::vec3(1.2f, 1.0f, 0.8f);
                    skyboxAttribs.Saturation = 1.3f;
                    skyboxAttribs.Contrast = 1.1f;
                }

                ImGui::SameLine();
                if (ImGui::Button("Cool Nebula"))
                {
                    skyboxAttribs.Intensity = 0.8f;
                    skyboxAttribs.Tint = glm::vec3(0.7f, 0.9f, 1.3f);
                    skyboxAttribs.Saturation = 1.4f;
                    skyboxAttribs.Contrast = 1.2f;
                }

                if (ImGui::Button("Dim Space"))
                {
                    skyboxAttribs.Intensity = 0.5f;
                    skyboxAttribs.Tint = glm::vec3(0.8f, 0.8f, 1.0f);
                    skyboxAttribs.Saturation = 0.8f;
                    skyboxAttribs.Contrast = 0.9f;
                }

                ImGui::SameLine();
                if (ImGui::Button("Bright Stars"))
                {
                    skyboxAttribs.Intensity = 2.0f;
                    skyboxAttribs.Tint = glm::vec3(1.0f, 1.0f, 1.0f);
                    skyboxAttribs.Saturation = 1.5f;
                    skyboxAttribs.Contrast = 1.3f;
                }

                ImGui::SameLine();
                if (ImGui::Button("Rotating Galaxy"))
                {
                    skyboxAttribs.EnableRotation = true;
                    skyboxAttribs.RotationSpeed = 0.1f;
                    skyboxAttribs.Intensity = 1.3f;
                    skyboxAttribs.Saturation = 1.2f;
                }

                ImGui::End();
            }
        }
    private:
        SolarSystem m_SolarSystem; 
        Timer m_FrameTimer;
        float m_FPS = 0.0f;
    };
}