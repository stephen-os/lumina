#pragma once

#include <string>
#include <vector>

#include "Planet.h"
#include "Asteroid.h"

#include "Lumina/Graphics/Skybox.h"
#include "Lumina/Graphics/Model.h"
#include "Lumina/Graphics/Renderer3D.h"

#include "Lumina/Graphics/Cameras/PerspectiveCamera.h"

namespace Lumina
{
	class SolarSystem
	{
	public:
		SolarSystem() = default;
	
		void Init(); 

		void Update(float ts);
		void Render();

		DirectionalLight& GetDirectionalLight() { return m_DirectionalLight; }
		std::vector<PointLight>& GetPointLights() { return m_PointLights; }
		std::vector<Planet>& GetPlanets() { return m_Planets; }
		std::vector<Asteroid>& GetAsteroids() { return m_Asteroids; }
		PerspectiveCamera& GetCamera() { return m_PerspectiveCamera; }
		bool& GetAnimationEnabled() { return m_AnimationEnabled; }
		float& GetTimeScale() { return m_TimeScale; }
		float GetCurrentTime() const { return m_CurrentTime; }

		SkyboxAttributes& GetSkyboxAttributes() { return m_SkyboxAttributes; }
		const SkyboxAttributes& GetSkyboxAttributes() const { return m_SkyboxAttributes; }


		bool& GetShowAsteroids() { return m_ShowAsteroids; }
		bool& GetShowPlanets() { return m_ShowPlanets; }

		void SetViewportSize(float width, float height) { m_ViewportSize = { width, height }; }

		void Reset()
		{
			m_CurrentTime = 0.0f;
			for (auto& planet : m_Planets)
			{
				planet.SetCurrentOrbitAngle(0.0f);
				planet.SetCurrentRotationAngle(0.0f);
			}
		}
	private:
		// Controls
		bool m_AnimationEnabled = true;
		float m_TimeScale = 1.0f;
		float m_CurrentTime = 0.0f;

		bool m_ShowAsteroids = true;
		bool m_ShowPlanets = true;

		// Scene Controls
		glm::vec2 m_ViewportSize = { 1280.0f, 720.0f };

		// Lights
		DirectionalLight m_DirectionalLight;
		std::vector<PointLight> m_PointLights;

		// Models
		std::vector<Planet> m_Planets;
		std::vector<Asteroid> m_Asteroids;
		
		Ref<Skybox> m_Skybox;
		SkyboxAttributes m_SkyboxAttributes;

		Ref<Model> m_UFOModel;

		// Camera
		PerspectiveCamera m_PerspectiveCamera;
	};
}