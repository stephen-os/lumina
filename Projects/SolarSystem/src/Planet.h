#pragma once

#include <string>

#include "Lumina/Core/Ref.h"
#include "Lumina/Graphics/Model.h"

namespace Lumina
{
	struct OrbitalAttributes
	{
		float OrbitRadius = 0.0f;
		float OrbitSpeed = 1.0f;
		float RotationSpeed = 0.0f;
		float CurrentOrbitAngle = 0.0f;
		float CurrentRotationAngle = 0.0f;
	};

	class Planet
	{
	public:
		Planet(const std::string& name, Ref<Model> model, const ModelAttributes& attributes, const OrbitalAttributes& orbitalAttributes)
			: m_Name(name), m_Model(model), m_Attributes(attributes), m_OrbitalAttributes(orbitalAttributes)
		{
			m_Model->SetName(name);
		}

		// Getters
		const std::string& GetName() const { return m_Name; }
		Ref<Model> GetModel() const { return m_Model; }
		
		// Const
		const ModelAttributes& GetAttributes() const { return m_Attributes; }
		const OrbitalAttributes& GetOrbitalAttributes() const { return m_OrbitalAttributes; }

		// Non-Const
		Lumina::ModelAttributes& GetAttributes() { return m_Attributes; }
		OrbitalAttributes& GetOrbitalAttributes() { return m_OrbitalAttributes; }

		void SetAttributes(const ModelAttributes& attributes) { m_Attributes = attributes; }
		void SetOrbitalAttributes(const OrbitalAttributes& orbitalAttributes) { m_OrbitalAttributes = orbitalAttributes; }

		// Individual orbital attribute getters
		float GetOrbitRadius() const { return m_OrbitalAttributes.OrbitRadius; }
		float GetOrbitSpeed() const { return m_OrbitalAttributes.OrbitSpeed; }
		float GetRotationSpeed() const { return m_OrbitalAttributes.RotationSpeed; }
		float GetCurrentOrbitAngle() const { return m_OrbitalAttributes.CurrentOrbitAngle; }
		float GetCurrentRotationAngle() const { return m_OrbitalAttributes.CurrentRotationAngle; }

		// Individual orbital attribute setters
		void SetOrbitRadius(float radius) { m_OrbitalAttributes.OrbitRadius = radius; }
		void SetOrbitSpeed(float speed) { m_OrbitalAttributes.OrbitSpeed = speed; }
		void SetRotationSpeed(float speed) { m_OrbitalAttributes.RotationSpeed = speed; }
		void SetCurrentOrbitAngle(float angle) { m_OrbitalAttributes.CurrentOrbitAngle = angle; }
		void SetCurrentRotationAngle(float angle) { m_OrbitalAttributes.CurrentRotationAngle = angle; }

	private:
		std::string m_Name = "Unnamed";
		Ref<Model> m_Model;
		ModelAttributes m_Attributes;
		OrbitalAttributes m_OrbitalAttributes;
	};
}