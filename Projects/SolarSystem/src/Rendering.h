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

namespace Lumina
{
    struct PlanetData
    {
        std::string Name;
        Ref<Model> Model;
        ModelAttributes Attributes; 

        // Orbital parameters
        float OrbitRadius;
        float OrbitSpeed;
        float RotationSpeed;
        float CurrentOrbitAngle = 0.0f;
        float CurrentRotationAngle = 0.0f;
    };

    struct AsteroidData
    {
        Ref<Model> Model;
        ModelAttributes Attributes;

        // Oribital parameters 
        float OrbitRadius;
        float OrbitSpeed;
        float RotationSpeed;
        float CurrentOrbitAngle;
        float CurrentRotationAngle = 0.0f;
    };

    class Rendering : public Layer
    {
    public:
        
        virtual void OnAttach() override
        {
            // Initialize 3D renderer
            Renderer3D::Init();

            // Initialize model registry
            ModelRegistry::Init();

            // Initialize skybox
            std::vector<std::string> faces =
            {
                "res/skybox/galexy/right.png",
                "res/skybox/galexy/left.png",
                "res/skybox/galexy/top.png",
                "res/skybox/galexy/bottom.png",
                "res/skybox/galexy/front.png",
                "res/skybox/galexy/back.png"
            };
            m_Skybox = Skybox::Create(faces, "Galexy");

            // Load UFO model
            m_UFOModel = Model::Load("res/geometry/ufo.glb");
            LUMINA_ASSERT(m_UFOModel, "Failed to load UFO model");

            // Setup asteroid belt
            auto asteroid1Model = Model::Load("res/geometry/asteroid1.glb", "Small Astriod");
            auto asteroid2Model = Model::Load("res/geometry/asteroid2.glb", "Large Astroid Cluster");
            auto asteroid3Model = Model::Load("res/geometry/asteroid3.glb", "Small Astroid Cluster");
            
            const int asteroidCount = 500;
            const float beltInnerRadius = 1900.0f;
            const float beltOuterRadius = 2100.0f;
            const float astriodScaleBase = 1.0f;
            const int astriodScaleRange = 30; 

            for (int i = 0; i < asteroidCount; ++i)
            {
                AsteroidData asteroid;

                // Randomly select asteroid model
                int modelChoice = i % 3;
                if (modelChoice == 0) asteroid.Model = asteroid1Model;
                else if (modelChoice == 1) asteroid.Model = asteroid2Model;
                else asteroid.Model = asteroid3Model;
                

                // Random position in belt
                float angle = (float)i / asteroidCount * 2.0f * 3.14159f + (rand() % 100) * 0.01f;
                asteroid.OrbitRadius = beltInnerRadius + (rand() % 100) * 0.01f * (beltOuterRadius - beltInnerRadius);
                asteroid.OrbitSpeed = 0.3f + (rand() % 50) * 0.01f;
                asteroid.RotationSpeed = 1.0f + (rand() % 200) * 0.01f;
                asteroid.CurrentOrbitAngle = angle;

                // Set initial position based on angle
                asteroid.Attributes.Position.x = cos(asteroid.CurrentOrbitAngle) * asteroid.OrbitRadius;
                asteroid.Attributes.Position.z = sin(asteroid.CurrentOrbitAngle) * asteroid.OrbitRadius;
                asteroid.Attributes.Position.y = (rand() % 100 - 50) * 2.0f; // Random Y offset
                asteroid.Attributes.Rotation = { 0.0f, 0.0f, 0.0f };
                asteroid.Attributes.Scale = { astriodScaleBase + 0.1f * (rand() % astriodScaleRange), astriodScaleBase + 0.1f * (rand() % astriodScaleRange), astriodScaleBase + 0.1f * (rand() % astriodScaleRange) };
                
                asteroid.Attributes.TintColor = { 0.6f + (rand() % 20) * 0.01f, 0.6f + (rand() % 20) * 0.01f, 0.6f + (rand() % 20) * 0.01f, 1.0f };

                m_Asteroids.push_back(asteroid);
            }

            // Setup Sun (center of solar system)
            PlanetData sun;
            sun.Name = "Sun";
            sun.Model = Model::Load("res/geometry/sun.glb", "Sun");
            sun.OrbitRadius = 0.0f;
            sun.OrbitSpeed = 0.0f;
            sun.RotationSpeed = 0.5f;
            sun.CurrentOrbitAngle = 0.0f;
            sun.Attributes.Position = { 0.0f, 0.0f, 0.0f };
            sun.Attributes.Rotation = { 0.0f, 0.0f, 0.0f };
            sun.Attributes.Scale = { 500.0f, 500.0f, 500.0f };
            sun.Attributes.TintColor = { 1.0f, 1.0f, 0.8f, 1.0f };
            m_Planets.push_back(sun);

            PlanetData mercury;
            mercury.Name = "Mercury";
            mercury.Model = Model::Load("res/geometry/mercury.glb", "Mercury");
            mercury.OrbitRadius = 750.0f;
            mercury.OrbitSpeed = 4.0f;
            mercury.RotationSpeed = 1.0f;
            mercury.CurrentOrbitAngle = 0.0f;
            mercury.Attributes.Position = { 750.0f, 0.0f, 0.0f };
            mercury.Attributes.Rotation = { 0.0f, 0.0f, 0.0f };
            mercury.Attributes.Scale = { 250.0f, 250.0f, 250.0f };
            mercury.Attributes.TintColor = { 0.8f, 0.7f, 0.6f, 1.0f };
            m_Planets.push_back(mercury);

            PlanetData venus;
            venus.Name = "Venus";
            venus.Model = Model::Load("res/geometry/venus.glb", "Venus");
            venus.OrbitRadius = 1000.0f;
            venus.OrbitSpeed = 2.8f;
            venus.RotationSpeed = -0.5f; // Venus rotates backwards
            venus.CurrentOrbitAngle = 1.57f; // Start at 90 degrees
            venus.Attributes.Position = { 0.0f, 0.0f, 1000.0f };
            venus.Attributes.Rotation = { 0.0f, 0.0f, 0.0f };
            venus.Attributes.Scale = { 300.0f, 300.0f, 300.0f };
            venus.Attributes.TintColor = { 1.0f, 0.8f, 0.4f, 1.0f };
            m_Planets.push_back(venus);

            PlanetData earth;
            earth.Name = "Earth";
            earth.Model = Model::Load("res/geometry/earth.glb", "Earth");
            earth.OrbitRadius = 1250.0f;
            earth.OrbitSpeed = 2.0f;
            earth.RotationSpeed = 2.0f;
            earth.CurrentOrbitAngle = 3.14f; // Start at 180 degrees
            earth.Attributes.Position = { -1250.0f, -50.0f, 0.0f };
            earth.Attributes.Rotation = { 0.0f, 0.0f, 0.0f };
            earth.Attributes.Scale = { 0.13f, 0.13f, 0.13f };
            earth.Attributes.TintColor = { 0.6f, 0.8f, 1.0f, 1.0f };
            m_Planets.push_back(earth);

            PlanetData mars;
            mars.Name = "Mars";
            mars.Model = Model::Load("res/geometry/mars.glb", "Mars");
            mars.OrbitRadius = 1500.0f;
            mars.OrbitSpeed = 1.5f;
            mars.RotationSpeed = 1.8f;
            mars.CurrentOrbitAngle = 4.71f; // Start at 270 degrees
            mars.Attributes.Position = { 0.0f, 0.0f, -1500.0f };
            mars.Attributes.Rotation = { 0.0f, 0.0f, 0.0f };
            mars.Attributes.Scale = { 200.0f, 200.0f, 200.0f };
            mars.Attributes.TintColor = { 1.0f, 0.6f, 0.4f , 1.0f };
            m_Planets.push_back(mars);

            PlanetData jupiter;
            jupiter.Name = "Jupiter";
            jupiter.Model = Model::Load("res/geometry/jupiter.glb", "Jupiter");
            jupiter.OrbitRadius = 2300.0f;
            jupiter.OrbitSpeed = 0.8f;
            jupiter.RotationSpeed = 3.0f;
            jupiter.CurrentOrbitAngle = 0.78f; // Start at 45 degrees
            jupiter.Attributes.Position = { 1626.0f, -200.0f, 1626.0f };
            jupiter.Attributes.Rotation = { 0.0f, 0.0f, 0.0f };
            jupiter.Attributes.Scale = { 2.0f, 2.0f, 2.0f };
            jupiter.Attributes.TintColor = { 1.0f, 0.9f, 0.7f, 1.0f };
            m_Planets.push_back(jupiter);

            PlanetData saturn;
            saturn.Name = "Saturn";
            saturn.Model = Model::Load("res/geometry/saturn.glb", "Saturn");
            saturn.OrbitRadius = 3000.0f;
            saturn.OrbitSpeed = 0.6f;
            saturn.RotationSpeed = 2.5f;
            saturn.CurrentOrbitAngle = 2.35f; // Start at 135 degrees
            saturn.Attributes.Position = { -2121.0f, 0.0f, 2121.0f };
            saturn.Attributes.Rotation = { 0.0f, 0.0f, 0.0f };
            saturn.Attributes.Scale = { 200.0f, 200.0f, 200.0f };
            saturn.Attributes.TintColor = { 1.0f, 1.0f, 0.8f, 1.0f };
            m_Planets.push_back(saturn);

            PlanetData uranus;
            uranus.Name = "Uranus";
            uranus.Model = Model::Load("res/geometry/uranus.glb", "Uranus");
            uranus.OrbitRadius = 3600.0f;
            uranus.OrbitSpeed = 0.4f;
            uranus.RotationSpeed = 1.5f;
            uranus.CurrentOrbitAngle = 3.92f; // Start at 225 degrees
            uranus.Attributes.Position = { -2546.0f, 0.0f, -2546.0f };
            uranus.Attributes.Rotation = { 0.0f, 0.0f, 0.0f };
            uranus.Attributes.Scale = { 200.0f, 200.0f, 200.0f };
            uranus.Attributes.TintColor = { 0.6f, 0.8f, 1.0f, 1.0f };
            m_Planets.push_back(uranus);

            // Setup cameras
            m_PerspectiveCamera.SetPosition(glm::vec3(0.0f, 300.0f, 3000.0f));
            m_PerspectiveCamera.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
            m_PerspectiveCamera.SetFOV(90.0f);
            m_PerspectiveCamera.SetClippingPlanes(0.1f, 50000.0f);

            // Setup lighting
            m_DirectionalLight.Direction = glm::normalize(glm::vec3(0.0f, -0.5f, -0.5f));
            m_DirectionalLight.Color = glm::vec3(1.0f, 1.0f, 0.9f);
            m_DirectionalLight.Intensity = 2.0f;
            Renderer3D::SetDirectionalLight(m_DirectionalLight);

            // Sun point light
            PointLight sunLight;
            sunLight.Position = glm::vec3(0.0f, 0.0f, 0.0f);
            sunLight.Color = glm::vec3(1.0f, 0.9f, 0.7f);
            sunLight.Intensity = 10.0f;
            sunLight.Constant = 1.0f;
            sunLight.Linear = 0.014f;
            sunLight.Quadratic = 0.0007f;
            Renderer3D::AddPointLight(sunLight);
            m_PointLights.push_back(sunLight);
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

            // Update time for animations
            if (m_AnimationEnabled)
            {
                m_CurrentTime += ts * m_TimeScale;

                // Update planets
                for (auto& planet : m_Planets)
                {
                    // Update orbital position
                    planet.CurrentOrbitAngle += planet.OrbitSpeed * ts * m_TimeScale;
                    if (planet.CurrentOrbitAngle > 2.0f * 3.14159f)
                        planet.CurrentOrbitAngle -= 2.0f * 3.14159f;

                    // Calculate position based on orbit
                    if (planet.OrbitRadius > 0.0f) // Don't move the sun
                    {
                        planet.Attributes.Position.x = cos(planet.CurrentOrbitAngle) * planet.OrbitRadius;
                        planet.Attributes.Position.z = sin(planet.CurrentOrbitAngle) * planet.OrbitRadius;
                    }

                    // Update rotation
                    planet.CurrentRotationAngle += planet.RotationSpeed * ts * m_TimeScale;
                    if (planet.CurrentRotationAngle > 360.0f)
                        planet.CurrentRotationAngle -= 360.0f;

                    planet.Attributes.Rotation.y = planet.CurrentRotationAngle;
                }

                // Update asteroids
                for (auto& asteroid : m_Asteroids)
                {
                    // Update orbital position
                    asteroid.CurrentOrbitAngle += asteroid.OrbitSpeed * ts * m_TimeScale;
                    if (asteroid.CurrentOrbitAngle > 2.0f * 3.14159f)
                        asteroid.CurrentOrbitAngle -= 2.0f * 3.14159f;

                    // Calculate position
                    asteroid.Attributes.Position.x = cos(asteroid.CurrentOrbitAngle) * asteroid.OrbitRadius;
                    asteroid.Attributes.Position.z = sin(asteroid.CurrentOrbitAngle) * asteroid.OrbitRadius;
                    // Keep Y position with slight variation

                    // Update rotation
                    asteroid.CurrentRotationAngle += asteroid.RotationSpeed * ts * m_TimeScale;
                    if (asteroid.CurrentRotationAngle > 360.0f)
                        asteroid.CurrentRotationAngle -= 360.0f;

                    asteroid.Attributes.Rotation.y = asteroid.CurrentRotationAngle;
                    asteroid.Attributes.Rotation.x = asteroid.CurrentRotationAngle * 0.5f;
                    asteroid.Attributes.Rotation.z = asteroid.CurrentRotationAngle * 0.3f;
                }

                // Update UFO orbit around the system
                if (m_UFOOrbitEnabled)
                {
                    m_UFOOrbitAngle += m_UFOOrbitSpeed * ts * m_TimeScale;
                    if (m_UFOOrbitAngle > 2.0f * 3.14159f)
                        m_UFOOrbitAngle -= 2.0f * 3.14159f;

                    m_UFOAttributes.Position.x = cos(m_UFOOrbitAngle) * m_UFOOrbitRadius;
                    m_UFOAttributes.Position.z = sin(m_UFOOrbitAngle) * m_UFOOrbitRadius;
                    m_UFOAttributes.Position.y = sin(m_UFOOrbitAngle * 2.0f) * 100.0f + 200.0f; // Figure-8 motion

                    // Make UFO face its direction of travel
                    m_UFOAttributes.Rotation.y = m_UFOOrbitAngle + 1.57f; // +90 degrees
                }
            }
        }

        virtual void OnUIRender() override
        {
            RenderFPSWindow();
            RenderSceneViewer();
            RenderAnimationControls();
            RenderRenderControls();
            RenderCameraControls();
            RenderLightingControls();
            RenderStatistics();
            RenderPlanetaryControls();
        }

    private:
        void RenderFPSWindow()
        {
            ImGui::Begin("FPS");
            ImGui::Text("FPS: %.2f", m_FPS);
            ImGui::End();
        }

        void RenderSceneViewer()
        {
            ImGui::Begin("Planetarium View");
            ImGui::SetCursorPos({ 0.0f, 0.0f });

            ImVec2 size = ImGui::GetContentRegionAvail();
            m_PerspectiveCamera.SetAspectRatio(size.x / size.y);
            Renderer3D::SetResolution(static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y));
            Renderer3D::SetRenderMode(m_Mode);

            Renderer3D::Begin(m_PerspectiveCamera);
            

            // Draw skybox first
            Renderer3D::Draw(m_Skybox);

            // Draw all planets
            for (const auto& planet : m_Planets)
            {
                Renderer3D::Draw(planet.Model, planet.Attributes);
            }
            // Draw asteroids
            if (m_ShowAsteroids)
            {
                for (const auto& asteroid : m_Asteroids)
                {
                    Renderer3D::Submit(asteroid.Model, asteroid.Attributes);
                }
            }

#if 0 
            // Draw UFO
            if (m_ShowUFO)
            {
                Renderer3D::DrawModel(m_UFOModel, m_UFOAttributes);
            }

#endif
            Renderer3D::End();

            ImGui::Image((void*)(intptr_t)Renderer3D::GetImage(), { size.x, size.y });
            ImGui::End();
        }

        void RenderAnimationControls()
        {
            ImGui::Begin("Planetarium Controls");

            ImGui::Checkbox("Enable Animation", &m_AnimationEnabled);
            ImGui::SameLine();
            if (ImGui::Button("Reset"))
            {
                m_CurrentTime = 0.0f;
                for (auto& planet : m_Planets)
                {
                    planet.CurrentOrbitAngle = 0.0f;
                    planet.CurrentRotationAngle = 0.0f;
                }
                m_UFOOrbitAngle = 0.0f;
            }

            ImGui::SliderFloat("Time Scale", &m_TimeScale, 0.1f, 5.0f);
            ImGui::Text("Simulation Time: %.1f", m_CurrentTime);

            ImGui::Separator();
            ImGui::Checkbox("Show Asteroids", &m_ShowAsteroids);
            ImGui::Checkbox("Show UFO", &m_ShowUFO);
            ImGui::Checkbox("UFO Orbit", &m_UFOOrbitEnabled);

            if (m_UFOOrbitEnabled)
            {
                ImGui::SliderFloat("UFO Orbit Speed", &m_UFOOrbitSpeed, 0.1f, 3.0f);
                ImGui::SliderFloat("UFO Orbit Radius", &m_UFOOrbitRadius, 10.0f, 40.0f);
            }

            ImGui::End();
        }

        void RenderPlanetaryControls()
        {
            ImGui::Begin("Planetary System");

            for (size_t i = 0; i < m_Planets.size(); ++i)
            {
                ImGui::PushID(static_cast<int>(i));

                if (ImGui::CollapsingHeader(m_Planets[i].Name.c_str()))
                {
                    ImGui::DragFloat3("Position", glm::value_ptr(m_Planets[i].Attributes.Position), 0.1f);
                    ImGui::DragFloat3("Rotation", glm::value_ptr(m_Planets[i].Attributes.Rotation), 0.1f); 
                    ImGui::DragFloat3("Scale", glm::value_ptr(m_Planets[i].Attributes.Scale), 0.1f);
                    ImGui::SliderFloat("Orbit Speed", &m_Planets[i].OrbitSpeed, 0.0f, 10.0f);
                    ImGui::SliderFloat("Rotation Speed", &m_Planets[i].RotationSpeed, -5.0f, 5.0f);
                    ImGui::ColorEdit4("Tint", glm::value_ptr(m_Planets[i].Attributes.TintColor)); 
                }

                ImGui::PopID();
            }

            ImGui::End();
        }

        void RenderRenderControls()
        {
            ImGui::Begin("Render Controls");
            if (ImGui::BeginCombo("Render Mode",
                m_Mode == RenderMode::Normal ? "Normal" :
                m_Mode == RenderMode::Wireframe ? "Wireframe" : "Points"))
            {
                if (ImGui::Selectable("Normal", m_Mode == RenderMode::Normal))
                    m_Mode = RenderMode::Normal;
                if (ImGui::Selectable("Wireframe", m_Mode == RenderMode::Wireframe))
                    m_Mode = RenderMode::Wireframe;
                if (ImGui::Selectable("Points", m_Mode == RenderMode::Points))
                    m_Mode = RenderMode::Points;
                ImGui::EndCombo();
            }
            ImGui::End();
        }

        void RenderCameraControls()
        {
            ImGui::Begin("Camera Controls");

            if (ImGui::CollapsingHeader("Perspective Camera"))
            {
                glm::vec3 position = m_PerspectiveCamera.GetPosition();
                if (ImGui::DragFloat3("Position", glm::value_ptr(position), 0.5f))
                    m_PerspectiveCamera.SetPosition(position);

                glm::quat rotation = m_PerspectiveCamera.GetRotation();
                if (ImGui::DragFloat4("Rotation", glm::value_ptr(rotation), 0.5f))
                    m_PerspectiveCamera.SetRotation(rotation);

                if (ImGui::Button("Reset to Overview"))
                {
                    m_PerspectiveCamera.SetPosition(glm::vec3(0.0f, 300.0f, 3000.0f));
                    m_PerspectiveCamera.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
                }
                ImGui::SameLine();
                if (ImGui::Button("Top-Down View"))
                {
                    // Bug. When the camera is looking straight down the perspective is 
                    // Ruined. No image, not even the skybox, is rendererd. 
                    // Temp fix is to offset the camera so that the view matrix is not
                    // Zeroed out when calling LookAt. 
                    m_PerspectiveCamera.SetPosition(glm::vec3(0.0f, 3000.0f, 1.0f));
                    m_PerspectiveCamera.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
                }
            }

            ImGui::End();
        }

        void RenderLightingControls()
        {
            ImGui::Begin("Lighting Controls");

            bool lightUpdated = false;
            lightUpdated |= ImGui::DragFloat3("Sun Direction", glm::value_ptr(m_DirectionalLight.Direction), 0.01f);
            lightUpdated |= ImGui::ColorEdit3("Sun Color", glm::value_ptr(m_DirectionalLight.Color));
            lightUpdated |= ImGui::DragFloat("Sun Intensity", &m_DirectionalLight.Intensity, 0.1f, 0.0f, 10.0f);

            if (lightUpdated)
            {
                m_DirectionalLight.Direction = glm::normalize(m_DirectionalLight.Direction);
                Renderer3D::SetDirectionalLight(m_DirectionalLight);
            }

            ImGui::End();
        }

        void RenderStatistics()
        {
            ImGui::Begin("Statistics");
            Renderer3D::Statistics stats = Renderer3D::GetStats();
            ImGui::Text("Draw Calls: %d", stats.DrawCalls);
            ImGui::Text("Models: %d", stats.ModelCount);
            ImGui::Text("Triangles: %d", stats.TriangleCount);
            ImGui::Text("Vertices: %d", stats.VertexCount);
            ImGui::End();
        }

    private:
        // Models
        Ref<Model> m_UFOModel;
        Ref<Skybox> m_Skybox;

        // Planetary system
        std::vector<PlanetData> m_Planets;
        std::vector<AsteroidData> m_Asteroids;
        ModelAttributes m_UFOAttributes;

        // Animation
        bool m_AnimationEnabled = true;
        float m_TimeScale = 1.0f;
        float m_CurrentTime = 0.0f;

        // UFO controls
        bool m_ShowUFO = true;
        bool m_UFOOrbitEnabled = true;
        float m_UFOOrbitSpeed = 0.5f;
        float m_UFOOrbitRadius = 30.0f;
        float m_UFOOrbitAngle = 0.0f;

        // Display options
        bool m_ShowAsteroids = true;

        // Cameras
        PerspectiveCamera m_PerspectiveCamera;

        // Lighting
        DirectionalLight m_DirectionalLight;
        std::vector<PointLight> m_PointLights;

        // Render settings
        RenderMode m_Mode = RenderMode::Normal;

        // Utility
        Timer m_FrameTimer;
        float m_FPS = 0.0f;
    };
}