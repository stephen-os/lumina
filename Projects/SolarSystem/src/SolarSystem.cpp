#include "SolarSystem.h"

#include "Lumina/Utils/Random.h"

#include "Lumina/Core/Log.h"

namespace Lumina
{
    void SolarSystem::Init()
    {
        m_Planets.reserve(8);

        // Sun - Massive center of the solar system
        {
            ModelAttributes sunModelAttributes;
            sunModelAttributes.Position = { 0.0f, 0.0f, 0.0f };
            sunModelAttributes.Rotation = { 0.0f, 0.0f, 0.0f };
            sunModelAttributes.Scale = { 800.0f, 800.0f, 800.0f }; // Increased from 500
            sunModelAttributes.TintColor = { 1.0f, 1.0f, 0.8f, 1.0f };
            sunModelAttributes.PointSize = 1.0f;

            OrbitalAttributes sunOrbitalAttributes;
            sunOrbitalAttributes.OrbitRadius = 0.0f;
            sunOrbitalAttributes.OrbitSpeed = 0.0f;
            sunOrbitalAttributes.RotationSpeed = 0.3f; // Slower rotation
            sunOrbitalAttributes.CurrentOrbitAngle = 0.0f;
            sunOrbitalAttributes.CurrentRotationAngle = 0.0f;

            auto sunModel = Model::Load("res/geometry/sun.glb", "Sun");
            m_Planets.push_back(Planet("Sun", sunModel, sunModelAttributes, sunOrbitalAttributes));
        }

        // Mercury - Closest and smallest planet
        {
            ModelAttributes mercuryModelAttributes;
            mercuryModelAttributes.Position = { 1400.0f, 0.0f, 0.0f }; // Increased distance
            mercuryModelAttributes.Rotation = { 0.0f, 0.0f, 0.0f };
            mercuryModelAttributes.Scale = { 150.0f, 150.0f, 150.0f }; // Smaller relative to sun
            mercuryModelAttributes.TintColor = { 0.8f, 0.7f, 0.6f, 1.0f };
            mercuryModelAttributes.PointSize = 1.0f;

            OrbitalAttributes mercuryOrbitalAttributes;
            mercuryOrbitalAttributes.OrbitRadius = 1400.0f;
            mercuryOrbitalAttributes.OrbitSpeed = 4.0f; // Fastest orbit
            mercuryOrbitalAttributes.RotationSpeed = 1.5f;
            mercuryOrbitalAttributes.CurrentOrbitAngle = 0.0f;
            mercuryOrbitalAttributes.CurrentRotationAngle = 0.0f;

            auto mercuryModel = Model::Load("res/geometry/mercury.glb", "Mercury");
            m_Planets.push_back(Planet("Mercury", mercuryModel, mercuryModelAttributes, mercuryOrbitalAttributes));
        }

        // Venus - Second planet, similar size to Earth
        {
            ModelAttributes venusModelAttributes;
            venusModelAttributes.Position = { 0.0f, 0.0f, 2100.0f }; // Increased distance
            venusModelAttributes.Rotation = { 0.0f, 0.0f, 0.0f };
            venusModelAttributes.Scale = { 380.0f, 380.0f, 380.0f }; // Slightly smaller than Earth
            venusModelAttributes.TintColor = { 1.0f, 0.8f, 0.4f, 1.0f };
            venusModelAttributes.PointSize = 1.0f;

            OrbitalAttributes venusOrbitalAttributes;
            venusOrbitalAttributes.OrbitRadius = 2100.0f;
            venusOrbitalAttributes.OrbitSpeed = 2.8f;
            venusOrbitalAttributes.RotationSpeed = -0.3f; // Venus rotates backwards and slowly
            venusOrbitalAttributes.CurrentOrbitAngle = 1.57f;
            venusOrbitalAttributes.CurrentRotationAngle = 0.0f;

            auto venusModel = Model::Load("res/geometry/venus.glb", "Venus");
            m_Planets.push_back(Planet("Venus", venusModel, venusModelAttributes, venusOrbitalAttributes));
        }

        // Earth - Our home planet
        {
            ModelAttributes earthModelAttributes;
            earthModelAttributes.Position = { -2800.0f, -50.0f, 0.0f }; // Much further from sun
            earthModelAttributes.Rotation = { 0.0f, 0.0f, 0.0f };
            earthModelAttributes.Scale = { 0.20f, 0.20f, 0.20f };
            earthModelAttributes.TintColor = { 0.6f, 0.8f, 1.0f, 1.0f };
            earthModelAttributes.PointSize = 1.0f;

            OrbitalAttributes earthOrbitalAttributes;
            earthOrbitalAttributes.OrbitRadius = 2800.0f;
            earthOrbitalAttributes.OrbitSpeed = 2.0f;
            earthOrbitalAttributes.RotationSpeed = 2.0f;
            earthOrbitalAttributes.CurrentOrbitAngle = 3.14f;
            earthOrbitalAttributes.CurrentRotationAngle = 0.0f;

            auto earthModel = Model::Load("res/geometry/earth.glb", "Earth");
            m_Planets.push_back(Planet("Earth", earthModel, earthModelAttributes, earthOrbitalAttributes));
        }

        // Mars - The red planet
        {
            ModelAttributes marsModelAttributes;
            marsModelAttributes.Position = { 0.0f, 0.0f, -3600.0f }; // Increased distance
            marsModelAttributes.Rotation = { 0.0f, 0.0f, 0.0f };
            marsModelAttributes.Scale = { 320.0f, 320.0f, 320.0f }; // Increased size
            marsModelAttributes.TintColor = { 1.0f, 0.6f, 0.4f, 1.0f };
            marsModelAttributes.PointSize = 1.0f;

            OrbitalAttributes marsOrbitalAttributes;
            marsOrbitalAttributes.OrbitRadius = 3600.0f;
            marsOrbitalAttributes.OrbitSpeed = 1.5f;
            marsOrbitalAttributes.RotationSpeed = 1.8f;
            marsOrbitalAttributes.CurrentOrbitAngle = 4.71f;
            marsOrbitalAttributes.CurrentRotationAngle = 0.0f;

            auto marsModel = Model::Load("res/geometry/mars.glb", "Mars");
            m_Planets.push_back(Planet("Mars", marsModel, marsModelAttributes, marsOrbitalAttributes));
        }

        // Jupiter - Gas giant, largest planet
        {
            ModelAttributes jupiterModelAttributes;
            jupiterModelAttributes.Position = { 4950.0f, -200.0f, 4950.0f }; // Much further out
            jupiterModelAttributes.Rotation = { 0.0f, 0.0f, 0.0f };
            jupiterModelAttributes.Scale = { 2.0f, 2.0f, 2.0f };
            jupiterModelAttributes.TintColor = { 1.0f, 0.9f, 0.7f, 1.0f };
            jupiterModelAttributes.PointSize = 1.0f;

            OrbitalAttributes jupiterOrbitalAttributes;
            jupiterOrbitalAttributes.OrbitRadius = 7000.0f; // Much further from sun
            jupiterOrbitalAttributes.OrbitSpeed = 0.8f;
            jupiterOrbitalAttributes.RotationSpeed = 3.0f; // Fast rotation
            jupiterOrbitalAttributes.CurrentOrbitAngle = 0.78f;
            jupiterOrbitalAttributes.CurrentRotationAngle = 0.0f;

            auto jupiterModel = Model::Load("res/geometry/jupiter.glb", "Jupiter");
            m_Planets.push_back(Planet("Jupiter", jupiterModel, jupiterModelAttributes, jupiterOrbitalAttributes));
        }

        // Saturn - Ringed gas giant
        {
            ModelAttributes saturnModelAttributes;
            saturnModelAttributes.Position = { -7778.0f, 0.0f, 7778.0f }; // Very far out
            saturnModelAttributes.Rotation = { 0.0f, 0.0f, 0.0f };
            saturnModelAttributes.Scale = { 400.0f, 400.0f, 400.0f }; // Large gas giant
            saturnModelAttributes.TintColor = { 1.0f, 1.0f, 0.8f, 1.0f };
            saturnModelAttributes.PointSize = 1.0f;

            OrbitalAttributes saturnOrbitalAttributes;
            saturnOrbitalAttributes.OrbitRadius = 11000.0f; // Very distant
            saturnOrbitalAttributes.OrbitSpeed = 0.6f;
            saturnOrbitalAttributes.RotationSpeed = 2.5f;
            saturnOrbitalAttributes.CurrentOrbitAngle = 2.35f;
            saturnOrbitalAttributes.CurrentRotationAngle = 0.0f;

            auto saturnModel = Model::Load("res/geometry/saturn.glb", "Saturn");
            m_Planets.push_back(Planet("Saturn", saturnModel, saturnModelAttributes, saturnOrbitalAttributes));
        }

        // Uranus - Ice giant at the edge
        {
            ModelAttributes uranusModelAttributes;
            uranusModelAttributes.Position = { -10607.0f, 0.0f, -10607.0f }; // Extremely distant
            uranusModelAttributes.Rotation = { 0.0f, 0.0f, 0.0f };
            uranusModelAttributes.Scale = { 700.0f, 700.0f, 700.0f }; // Ice giant size
            uranusModelAttributes.TintColor = { 0.6f, 0.8f, 1.0f, 1.0f };
            uranusModelAttributes.PointSize = 1.0f;

            OrbitalAttributes uranusOrbitalAttributes;
            uranusOrbitalAttributes.OrbitRadius = 15000.0f; // Outer solar system
            uranusOrbitalAttributes.OrbitSpeed = 0.4f;
            uranusOrbitalAttributes.RotationSpeed = 1.5f;
            uranusOrbitalAttributes.CurrentOrbitAngle = 3.92f;
            uranusOrbitalAttributes.CurrentRotationAngle = 0.0f;

            auto uranusModel = Model::Load("res/geometry/uranus.glb", "Uranus");
            m_Planets.push_back(Planet("Uranus", uranusModel, uranusModelAttributes, uranusOrbitalAttributes));
        }

        // Asteroids - Between Mars and Jupiter
        {
            const std::vector<Ref<Model>> asteroidModels
            {
                Model::Load("res/geometry/asteroid1.glb", "Small Asteroid"),
                Model::Load("res/geometry/asteroid2.glb", "Large Asteroid"),
                Model::Load("res/geometry/asteroid3.glb", "Asteroid Cluster")
            };

            // Asteroid belt configuration - positioned between Mars and Jupiter
            constexpr int asteroidCount = 500; // Reduced for performance
            constexpr float beltInnerRadius = 4500.0f; // After Mars orbit
            constexpr float beltOuterRadius = 6200.0f; // Before Jupiter orbit
            constexpr float asteroidScaleBase = 3.0f; // Increased base size
            constexpr float asteroidScaleVariation = 8.0f; // More size variation
            constexpr float colorBaseIntensity = 0.6f;
            constexpr float colorVariation = 0.2f;

            Random rng(42);
            m_Asteroids.reserve(asteroidCount);

            for (uint32_t i = 0; i < asteroidCount; i++)
            {
                // Calculate base orbital angle with slight randomization
                const float baseAngle = (static_cast<float>(i) / asteroidCount) * glm::two_pi<float>();
                const float angle = baseAngle + rng.Float(0.0f, 0.1f); // More variation

                // Orbital parameters with realistic variation
                const float orbitRadius = rng.Float(beltInnerRadius, beltOuterRadius);
                const float orbitSpeed = rng.Float(0.2f, 0.6f); // Slower to match distance
                const float rotationSpeed = rng.Float(1.0f, 4.0f);

                // Random model selection
                auto asteroidModel = rng.Choose(asteroidModels);

                // Calculate initial orbital position using glm
                const glm::vec2 orbitalPos = glm::vec2(glm::cos(angle), glm::sin(angle)) * orbitRadius;

                // Model attributes with enhanced variation
                ModelAttributes asteroidModelAttributes;
                asteroidModelAttributes.Position = glm::vec3(
                    orbitalPos.x,
                    rng.Float(-200.0f, 200.0f), // Increased Y variation
                    orbitalPos.y
                );
                asteroidModelAttributes.Rotation = glm::vec3(0.0f);

                // Varied scale on all axes for more natural look
                asteroidModelAttributes.Scale = rng.Vec3(
                    glm::vec3(asteroidScaleBase),
                    glm::vec3(asteroidScaleBase + asteroidScaleVariation)
                );

                // Enhanced color variation for more realistic asteroids
                const float colorIntensity = colorBaseIntensity + rng.Float(0.0f, colorVariation);
                asteroidModelAttributes.TintColor = glm::vec4(
                    colorIntensity,                     // Red
                    colorIntensity * 0.9f,              // Green (slightly less)
                    colorIntensity * 0.8f,              // Blue (even less for brownish tint)
                    1.0f                                // Alpha
                );
                asteroidModelAttributes.PointSize = 1.0f;

                // Asteroid orbital attributes
                AsteroidAttributes asteroidAttributes;
                asteroidAttributes.OrbitRadius = orbitRadius;
                asteroidAttributes.OrbitSpeed = orbitSpeed;
                asteroidAttributes.RotationSpeed = rotationSpeed;
                asteroidAttributes.CurrentOrbitAngle = angle;
                asteroidAttributes.CurrentRotationAngle = 0.0f;

                // Create and add asteroid to collection
                m_Asteroids.emplace_back(
                    "Asteroid_" + std::to_string(i),
                    asteroidModel,
                    asteroidModelAttributes,
                    asteroidAttributes
                );
            }

            // Skybox
            {
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

                m_SkyboxAttributes.Intensity = 1.2f;                                    // Slightly brighter
                m_SkyboxAttributes.Tint = glm::vec3(0.9f, 0.95f, 1.0f);                 // Cool blue tint
                m_SkyboxAttributes.Rotation = 0.0f;                                     // Start at 0 rotation
                m_SkyboxAttributes.RotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);          // Rotate around Y-axis
                m_SkyboxAttributes.Exposure = 1.0f;                                     // Standard exposure
                m_SkyboxAttributes.Saturation = 1.1f;                                   // Slightly more saturated
                m_SkyboxAttributes.Contrast = 1.0f;                                     // Standard contrast
                m_SkyboxAttributes.EnableRotation = true;                               // Disabled by default
                m_SkyboxAttributes.RotationSpeed = 0.05f;                               // Slow rotation
                m_SkyboxAttributes.Alpha = 1.0f;                                        // Fully opaque
                m_SkyboxAttributes.TimeOfDay = 0.5f;                                    // Noon
                m_SkyboxAttributes.UseTimeOfDay = false;
            }

            // Create UFO
            m_UFOModel = Model::Load("res/geometry/ufo.glb", "UFO");

            // Setup lighting
            m_DirectionalLight.Direction = glm::normalize(glm::vec3(0.0f, -0.5f, -0.5f));
            m_DirectionalLight.Color = glm::vec3(1.0f, 1.0f, 0.9f);
            m_DirectionalLight.Intensity = 2.0f;

            // Sun point light - increased intensity for larger distances
            PointLight sunLight;
            sunLight.Position = glm::vec3(0.0f, 0.0f, 0.0f);
            sunLight.Color = glm::vec3(1.0f, 0.9f, 0.7f);
            sunLight.Intensity = 25.0f; // Increased for larger solar system
            sunLight.Constant = 1.0f;
            sunLight.Linear = 0.0014f; // Reduced for larger distances
            sunLight.Quadratic = 0.00007f; // Reduced for larger distances
            m_PointLights.push_back(sunLight);

            // Camera - positioned further back to view the larger solar system
            m_PerspectiveCamera.SetPosition(glm::vec3(0.0f, 12000.0f, 12000.0f)); // Further back
            m_PerspectiveCamera.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
            m_PerspectiveCamera.SetFOV(90.0f); // Wider field of view
            m_PerspectiveCamera.SetClippingPlanes(1.0f, 30000.0f); // Extended far plane
        }
    }

    void SolarSystem::Update(float ts)
    {
        if (m_AnimationEnabled)
        {
            m_CurrentTime += ts * m_TimeScale;

            for (auto& planet : m_Planets)
            {
                float newOrbitAngle = planet.GetCurrentOrbitAngle() + planet.GetOrbitSpeed() * ts * m_TimeScale;
                if (newOrbitAngle > glm::two_pi<float>())
                    newOrbitAngle -= glm::two_pi<float>();
                planet.SetCurrentOrbitAngle(newOrbitAngle);

                if (planet.GetOrbitRadius() > 0.0f) // Don't move the sun
                {
                    auto attributes = planet.GetAttributes();
                    attributes.Position.x = cos(newOrbitAngle) * planet.GetOrbitRadius();
                    attributes.Position.z = sin(newOrbitAngle) * planet.GetOrbitRadius();
                    planet.SetAttributes(attributes);
                }

                float newRotationAngle = planet.GetCurrentRotationAngle() + planet.GetRotationSpeed() * ts * m_TimeScale;
                if (newRotationAngle > 360.0f)
                    newRotationAngle -= 360.0f;
                planet.SetCurrentRotationAngle(newRotationAngle);

                auto attributes = planet.GetAttributes();
                attributes.Rotation.y = newRotationAngle;
                planet.SetAttributes(attributes);
            }

            for (auto& asteroid : m_Asteroids)
            {
                float newOrbitAngle = asteroid.GetCurrentOrbitAngle() + asteroid.GetOrbitSpeed() * ts * m_TimeScale;
                if (newOrbitAngle > glm::two_pi<float>())
                    newOrbitAngle -= glm::two_pi<float>();
                asteroid.SetCurrentOrbitAngle(newOrbitAngle);

                auto attributes = asteroid.GetAttributes();
                attributes.Position.x = cos(newOrbitAngle) * asteroid.GetOrbitRadius();
                attributes.Position.z = sin(newOrbitAngle) * asteroid.GetOrbitRadius();

                float newRotationAngle = asteroid.GetCurrentRotationAngle() + asteroid.GetRotationSpeed() * ts * m_TimeScale;
                if (newRotationAngle > 360.0f)
                    newRotationAngle -= 360.0f;
                asteroid.SetCurrentRotationAngle(newRotationAngle);

                attributes.Rotation.y = newRotationAngle;
                attributes.Rotation.x = newRotationAngle * 0.5f;
                attributes.Rotation.z = newRotationAngle * 0.3f;

                asteroid.SetAttributes(attributes);
            }

			if (m_SkyboxAttributes.EnableRotation)
			{
                m_SkyboxAttributes.Rotation += m_SkyboxAttributes.RotationSpeed * ts * m_TimeScale;

                if (m_SkyboxAttributes.Rotation > glm::two_pi<float>())
                    m_SkyboxAttributes.Rotation -= glm::two_pi<float>();
                else if (m_SkyboxAttributes.Rotation < 0.0f)
                    m_SkyboxAttributes.Rotation += glm::two_pi<float>();
			}
        }
    }

	void SolarSystem::Render()
	{
        m_PerspectiveCamera.SetAspectRatio(m_ViewportSize.x / m_ViewportSize.y);

        Renderer3D::SetResolution(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
        // Renderer3D::SetRenderMode(m_RenderMode);

        Renderer3D::Begin(m_PerspectiveCamera);

        Renderer3D::Draw(m_Skybox, m_SkyboxAttributes);

        if (m_ShowPlanets)
        {
            for (const auto& planet : m_Planets)
            {
                Renderer3D::Draw(planet.GetModel(), planet.GetAttributes());
            }
        }

        if (m_ShowAsteroids)
        {
            for (const auto& asteroid : m_Asteroids)
            {
                Renderer3D::Submit(asteroid.GetModel(), asteroid.GetAttributes());
            }
        }
        
        Renderer3D::End();
	}
}