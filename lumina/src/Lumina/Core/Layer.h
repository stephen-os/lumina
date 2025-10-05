#pragma once

#include <string>
#include <vector>

#include <imgui.h>

namespace Lumina 
{
	enum class DockPosition
	{
		Left,
		Right,
		Top,
		Bottom,
		Center
	};

	class Layer
	{
	public:
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnUpdate(float ts) {}
		virtual void OnUIRender() {}


		void ProcessDockingRequests(ImGuiID dockspaceID);
		bool HasDockingRequests() const { return !m_DockRequests.empty(); }

	protected: 
		void DockWindow(const std::string& panelName, DockPosition position, float ratio = 1.0f);

	private:
		struct DockRequest
		{
			std::string WindowName;
			DockPosition Position;
			float SizeRatio;

			bool operator<(const DockRequest& other) const
			{
				if (Position == DockPosition::Center && other.Position != DockPosition::Center)
					return false;
				if (Position != DockPosition::Center && other.Position == DockPosition::Center)
					return true;

				return false;
			}
		};

		std::vector<DockRequest> m_DockRequests;
	};
}