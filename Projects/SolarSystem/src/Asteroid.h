#pragma once
#include <string>
#include "Lumina/Core/Ref.h"
#include "Lumina/Graphics/Model.h"

namespace Lumina
{
	struct AsteroidAttributes
	{
		float OrbitRadius = 0.0f;
		float OrbitSpeed = 1.0f;
		float RotationSpeed = 0.0f;
		float CurrentOrbitAngle = 0.0f;
		float CurrentRotationAngle = 0.0f;
	};

	class Asteroid
	{
	public:
		Asteroid(const std::string& name, const Ref<Model>& model, const ModelAttributes& attributes, const AsteroidAttributes& asteroidAttributes)
			: m_Name(name), m_Model(model), m_Attributes(attributes), m_AsteroidAttributes(asteroidAttributes)
		{
			m_Model->SetName(name);
		}

		// Getters
		const std::string& GetName() const { return m_Name; }
		const Ref<Model>& GetModel() const { return m_Model; }
		const ModelAttributes& GetAttributes() const { return m_Attributes; }
		const AsteroidAttributes& GetAsteroidAttributes() const { return m_AsteroidAttributes; }

		void SetAttributes(const ModelAttributes& attributes)
		{
			m_Attributes = attributes;
		}

		void SetAsteroidAttributes(const AsteroidAttributes& asteroidAttributes)
		{
			m_AsteroidAttributes = asteroidAttributes;
		}

		// Individual asteroid attribute getters
		float GetOrbitRadius() const { return m_AsteroidAttributes.OrbitRadius; }
		float GetOrbitSpeed() const { return m_AsteroidAttributes.OrbitSpeed; }
		float GetRotationSpeed() const { return m_AsteroidAttributes.RotationSpeed; }
		float GetCurrentOrbitAngle() const { return m_AsteroidAttributes.CurrentOrbitAngle; }
		float GetCurrentRotationAngle() const { return m_AsteroidAttributes.CurrentRotationAngle; }

		// Individual asteroid attribute setters
		void SetOrbitRadius(float radius) { m_AsteroidAttributes.OrbitRadius = radius; }
		void SetOrbitSpeed(float speed) { m_AsteroidAttributes.OrbitSpeed = speed; }
		void SetRotationSpeed(float speed) { m_AsteroidAttributes.RotationSpeed = speed; }
		void SetCurrentOrbitAngle(float angle) { m_AsteroidAttributes.CurrentOrbitAngle = angle; }
		void SetCurrentRotationAngle(float angle) { m_AsteroidAttributes.CurrentRotationAngle = angle; }

	private:
		std::string m_Name = "Unnamed";
		Ref<Model> m_Model;
		ModelAttributes m_Attributes;
		AsteroidAttributes m_AsteroidAttributes;
	};
}