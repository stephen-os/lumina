#pragma once

#include "Event.h"

#include <string>

namespace Lumina
{
	class Layer
	{
	public:
		Layer(const std::string& name = "unnamed_layer") : m_Name(name) {}
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float dt) {}
		virtual void OnRender() {}
		virtual void OnEvent(Event& e) {}

		[[nodiscard]] const std::string& GetName() const { return m_Name; }

	private:
		std::string m_Name;
	};
}
