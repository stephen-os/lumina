#include "docking.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <algorithm>
#include <filesystem>

namespace lumina::ui
{
    void dock_layout::split(const std::string& window_name, dock_position position, float size_ratio)
    {
        m_requests.push_back({window_name, dock_relation::split, position, size_ratio, ""});
    }

    void dock_layout::sub_split(const std::string& window_name, const std::string& split_from,
                                dock_position direction, float size_ratio)
    {
        m_requests.push_back({window_name, dock_relation::sub_split, direction, size_ratio, split_from});
    }

    void dock_layout::tabbed(const std::string& window_name, const std::string& tab_with)
    {
        m_requests.push_back({window_name, dock_relation::tab, dock_position::center, 1.0f, tab_with});
    }

    namespace
    {
        ImGuiDir to_imgui_dir(dock_position pos)
        {
            switch (pos)
            {
            case dock_position::left:   return ImGuiDir_Left;
            case dock_position::right:  return ImGuiDir_Right;
            case dock_position::top:    return ImGuiDir_Up;
            case dock_position::bottom: return ImGuiDir_Down;
            default:                    return ImGuiDir_Left;
            }
        }
    }

    void dock_layout::apply(unsigned int dockspace_id)
    {
        if (m_requests.empty())
            return;

        ImGuiIO& io = ImGui::GetIO();
        if (io.IniFilename && std::filesystem::exists(io.IniFilename))
        {
            m_requests.clear();
            m_window_dock_ids.clear();
            return;
        }

        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

        std::sort(m_requests.begin(), m_requests.end());

        ImGuiID remaining_space = dockspace_id;
        float remaining_ratio = 1.0f;

        for (const auto& request : m_requests)
        {
            switch (request.relation)
            {
            case dock_relation::split:
            {
                if (request.position == dock_position::center)
                {
                    ImGui::DockBuilderDockWindow(request.window_name.c_str(), remaining_space);
                    m_window_dock_ids[request.window_name] = remaining_space;
                }
                else
                {
                    float adjusted_ratio = request.size_ratio / remaining_ratio;
                    adjusted_ratio = std::clamp(adjusted_ratio, 0.01f, 0.95f);

                    ImGuiID dock_id = ImGui::DockBuilderSplitNode(
                        remaining_space, to_imgui_dir(request.position),
                        adjusted_ratio, nullptr, &remaining_space);

                    ImGui::DockBuilderDockWindow(request.window_name.c_str(), dock_id);
                    m_window_dock_ids[request.window_name] = dock_id;

                    remaining_ratio = std::max(0.05f, remaining_ratio - request.size_ratio);
                }
                break;
            }

            case dock_relation::sub_split:
            {
                auto it = m_window_dock_ids.find(request.relative_to);
                if (it != m_window_dock_ids.end())
                {
                    ImGuiID parent_dock_id = it->second;
                    float split_ratio = std::clamp(request.size_ratio, 0.01f, 0.95f);

                    ImGuiID new_dock_id = 0;
                    ImGuiID remaining_dock_id = 0;
                    new_dock_id = ImGui::DockBuilderSplitNode(
                        parent_dock_id, to_imgui_dir(request.position),
                        split_ratio, nullptr, &remaining_dock_id);

                    ImGui::DockBuilderDockWindow(request.window_name.c_str(), new_dock_id);
                    m_window_dock_ids[request.window_name] = new_dock_id;
                    m_window_dock_ids[request.relative_to] = remaining_dock_id;
                }
                break;
            }

            case dock_relation::tab:
            {
                auto it = m_window_dock_ids.find(request.relative_to);
                if (it != m_window_dock_ids.end())
                {
                    ImGuiID tab_dock_id = it->second;
                    ImGui::DockBuilderDockWindow(request.window_name.c_str(), tab_dock_id);
                    m_window_dock_ids[request.window_name] = tab_dock_id;
                }
                break;
            }
            }
        }

        ImGui::DockBuilderFinish(dockspace_id);

        m_requests.clear();
        m_window_dock_ids.clear();
    }
}
