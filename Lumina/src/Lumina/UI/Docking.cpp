#include "Docking.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <algorithm>
#include <filesystem>

namespace Lumina::UI
{
    void DockLayout::Split(const std::string& windowName, DockPosition position, float sizeRatio)
    {
        m_Requests.push_back({windowName, DockRelation::Split, position, sizeRatio, ""});
    }

    void DockLayout::SubSplit(const std::string& windowName, const std::string& splitFrom,
                                DockPosition direction, float sizeRatio)
    {
        m_Requests.push_back({windowName, DockRelation::SubSplit, direction, sizeRatio, splitFrom});
    }

    void DockLayout::Tabbed(const std::string& windowName, const std::string& tabWith)
    {
        m_Requests.push_back({windowName, DockRelation::Tab, DockPosition::Center, 1.0f, tabWith});
    }

    namespace
    {
        ImGuiDir ToImGuiDir(DockPosition pos)
        {
            switch (pos)
            {
            case DockPosition::Left:   return ImGuiDir_Left;
            case DockPosition::Right:  return ImGuiDir_Right;
            case DockPosition::Top:    return ImGuiDir_Up;
            case DockPosition::Bottom: return ImGuiDir_Down;
            default:                   return ImGuiDir_Left;
            }
        }
    }

    void DockLayout::Apply(unsigned int dockspaceId)
    {
        if (m_Requests.empty())
            return;

        ImGuiIO& io = ImGui::GetIO();
        if (io.IniFilename && std::filesystem::exists(io.IniFilename))
        {
            m_Requests.clear();
            m_WindowDockIds.clear();
            return;
        }

        ImGui::DockBuilderRemoveNode(dockspaceId);
        ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspaceId, ImGui::GetMainViewport()->Size);

        std::sort(m_Requests.begin(), m_Requests.end());

        ImGuiID remainingSpace = dockspaceId;
        float remainingRatio = 1.0f;

        for (const auto& request : m_Requests)
        {
            switch (request.Relation)
            {
            case DockRelation::Split:
            {
                if (request.Position == DockPosition::Center)
                {
                    ImGui::DockBuilderDockWindow(request.WindowName.c_str(), remainingSpace);
                    m_WindowDockIds[request.WindowName] = remainingSpace;
                }
                else
                {
                    float adjustedRatio = request.SizeRatio / remainingRatio;
                    adjustedRatio = std::clamp(adjustedRatio, 0.01f, 0.95f);

                    ImGuiID dockId = ImGui::DockBuilderSplitNode(
                        remainingSpace, ToImGuiDir(request.Position),
                        adjustedRatio, nullptr, &remainingSpace);

                    ImGui::DockBuilderDockWindow(request.WindowName.c_str(), dockId);
                    m_WindowDockIds[request.WindowName] = dockId;

                    remainingRatio = std::max(0.05f, remainingRatio - request.SizeRatio);
                }
                break;
            }

            case DockRelation::SubSplit:
            {
                auto it = m_WindowDockIds.find(request.RelativeTo);
                if (it != m_WindowDockIds.end())
                {
                    ImGuiID parentDockId = it->second;
                    float splitRatio = std::clamp(request.SizeRatio, 0.01f, 0.95f);

                    ImGuiID newDockId = 0;
                    ImGuiID remainingDockId = 0;
                    newDockId = ImGui::DockBuilderSplitNode(
                        parentDockId, ToImGuiDir(request.Position),
                        splitRatio, nullptr, &remainingDockId);

                    ImGui::DockBuilderDockWindow(request.WindowName.c_str(), newDockId);
                    m_WindowDockIds[request.WindowName] = newDockId;
                    m_WindowDockIds[request.RelativeTo] = remainingDockId;
                }
                break;
            }

            case DockRelation::Tab:
            {
                auto it = m_WindowDockIds.find(request.RelativeTo);
                if (it != m_WindowDockIds.end())
                {
                    ImGuiID tabDockId = it->second;
                    ImGui::DockBuilderDockWindow(request.WindowName.c_str(), tabDockId);
                    m_WindowDockIds[request.WindowName] = tabDockId;
                }
                break;
            }
            }
        }

        ImGui::DockBuilderFinish(dockspaceId);

        m_Requests.clear();
        m_WindowDockIds.clear();
    }
}
