#pragma once

#include "Events/Event.h"

#include <string>
#include <vector>
#include <unordered_map>

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

    enum class DockRelation
    {
        Split,      // Normal split from main dockspace
        Tab,        // Tab with another window
        SubSplit    // Split from another window's dock
    };

    class Layer
    {
    public:
        virtual ~Layer() = default;
        virtual void OnAttach() {}
        virtual void OnDetach() {}
        
        virtual void OnUpdate(float ts) {}
        virtual void OnEvent(Event& e) {}
        virtual void OnUIRender() {}

        void ProcessDockingRequests(ImGuiID dockspaceID);
        bool HasDockingRequests() const { return !m_DockRequests.empty(); }

    protected:
        void DockWindowSplit(const std::string& windowName, DockPosition position, float sizeRatio = 1.0f);
        void DockWindowSubSplit(const std::string& windowName, const std::string& splitFromWindow, DockPosition direction, float sizeRatio = 0.5f);
        void DockWindowTabbed(const std::string& windowName, const std::string& tabWithWindow);

    private:
        struct DockRequest
        {
            std::string WindowName;
            DockRelation Relation;
            DockPosition Position;
            float SizeRatio;
            std::string RelativeToWindow;

            bool operator<(const DockRequest& other) const
            {
                if (Relation != other.Relation)
                    return Relation < other.Relation;

                if (Relation == DockRelation::Split)
                {
                    if (Position == DockPosition::Center && other.Position != DockPosition::Center)
                        return false;
                    if (Position != DockPosition::Center && other.Position == DockPosition::Center)
                        return true;
                }

                return false;
            }
        };

        std::vector<DockRequest> m_DockRequests;
        std::unordered_map<std::string, ImGuiID> m_WindowDockIDs;
    };
}