#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace lumina::ui
{
    enum class dock_position
    {
        left,
        right,
        top,
        bottom,
        center
    };

    class dock_layout
    {
    public:
        void split(const std::string& window_name, dock_position position, float size_ratio = 1.0f);
        void sub_split(const std::string& window_name, const std::string& split_from,
                       dock_position direction, float size_ratio = 0.5f);
        void tabbed(const std::string& window_name, const std::string& tab_with);

        void apply(unsigned int dockspace_id);
        bool has_requests() const { return !m_requests.empty(); }

    private:
        enum class dock_relation
        {
            split,
            tab,
            sub_split
        };

        struct dock_request
        {
            std::string window_name;
            dock_relation relation;
            dock_position position;
            float size_ratio;
            std::string relative_to;

            bool operator<(const dock_request& other) const
            {
                if (relation != other.relation)
                    return relation < other.relation;

                if (relation == dock_relation::split)
                {
                    if (position == dock_position::center && other.position != dock_position::center)
                        return false;
                    if (position != dock_position::center && other.position == dock_position::center)
                        return true;
                }

                return false;
            }
        };

        std::vector<dock_request> m_requests;
        std::unordered_map<std::string, unsigned int> m_window_dock_ids;
    };
}
