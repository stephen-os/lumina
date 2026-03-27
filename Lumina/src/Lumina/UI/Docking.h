#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace Lumina::UI
{
    enum class DockPosition
    {
        Left,
        Right,
        Top,
        Bottom,
        Center
    };

    class DockLayout
    {
    public:
        void Split(const std::string& windowName, DockPosition position, float sizeRatio = 1.0f);
        void SubSplit(const std::string& windowName, const std::string& splitFrom,
                       DockPosition direction, float sizeRatio = 0.5f);
        void Tabbed(const std::string& windowName, const std::string& tabWith);

        void Apply(unsigned int dockspaceId);
        [[nodiscard]] bool HasRequests() const { return !m_Requests.empty(); }

    private:
        enum class DockRelation
        {
            Split,
            Tab,
            SubSplit
        };

        struct DockRequest
        {
            std::string WindowName;
            DockRelation Relation;
            DockPosition Position;
            float SizeRatio;
            std::string RelativeTo;

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

        std::vector<DockRequest> m_Requests;
        std::unordered_map<std::string, unsigned int> m_WindowDockIds;
    };
}
