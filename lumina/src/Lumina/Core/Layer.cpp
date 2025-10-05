#include "Layer.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <algorithm>
#include <filesystem>

namespace Lumina
{
    void Layer::DockWindowSplit(const std::string& windowName, DockPosition position, float sizeRatio)
    {
        m_DockRequests.push_back({ windowName, DockRelation::Split, position, sizeRatio, "" });
    }

    void Layer::DockWindowSubSplit(const std::string& windowName, const std::string& splitFromWindow, DockPosition direction, float sizeRatio)
    {
        m_DockRequests.push_back({ windowName, DockRelation::SubSplit, direction, sizeRatio, splitFromWindow });
    }

    void Layer::DockWindowTabbed(const std::string& windowName, const std::string& tabWithWindow)
    {
        m_DockRequests.push_back({ windowName, DockRelation::Tab, DockPosition::Center, 1.0f, tabWithWindow });
    }

    void Layer::ProcessDockingRequests(ImGuiID dockspaceID)
    {
        if (m_DockRequests.empty())
            return;

        ImGuiIO& io = ImGui::GetIO();
        if (io.IniFilename && std::filesystem::exists(io.IniFilename))
        {
            m_DockRequests.clear();
			m_WindowDockIDs.clear();
            return;
        }

        ImGui::DockBuilderRemoveNode(dockspaceID);
        ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspaceID, ImGui::GetMainViewport()->Size);

        std::sort(m_DockRequests.begin(), m_DockRequests.end());

        ImGuiID remainingSpace = dockspaceID;
        float remainingRatio = 1.0f;

        for (const auto& request : m_DockRequests)
        {
            switch (request.Relation)
            {
            case DockRelation::Split:
            {
                if (request.Position == DockPosition::Center)
                {
                    ImGui::DockBuilderDockWindow(request.WindowName.c_str(), remainingSpace);
                    m_WindowDockIDs[request.WindowName] = remainingSpace;
                }
                else
                {
                    float adjustedRatio = request.SizeRatio / remainingRatio;
                    adjustedRatio = std::clamp(adjustedRatio, 0.01f, 0.95f);

                    ImGuiID dockID = 0;
                    ImGuiDir direction;

                    switch (request.Position)
                    {
                    case DockPosition::Left:   direction = ImGuiDir_Left; break;
                    case DockPosition::Right:  direction = ImGuiDir_Right; break;
                    case DockPosition::Top:    direction = ImGuiDir_Up; break;
                    case DockPosition::Bottom: direction = ImGuiDir_Down; break;
                    default: direction = ImGuiDir_Left; break;
                    }

                    dockID = ImGui::DockBuilderSplitNode(remainingSpace, direction, adjustedRatio, nullptr, &remainingSpace);
                    ImGui::DockBuilderDockWindow(request.WindowName.c_str(), dockID);
                    m_WindowDockIDs[request.WindowName] = dockID;

                    remainingRatio = std::max(0.05f, remainingRatio - request.SizeRatio);
                }
                break;
            }

            case DockRelation::SubSplit:
            {
                auto it = m_WindowDockIDs.find(request.RelativeToWindow);
                if (it != m_WindowDockIDs.end())
                {
                    ImGuiID parentDockID = it->second;
                    float splitRatio = std::clamp(request.SizeRatio, 0.01f, 0.95f);

                    ImGuiDir direction;
                    switch (request.Position)
                    {
                    case DockPosition::Left:   direction = ImGuiDir_Left; break;
                    case DockPosition::Right:  direction = ImGuiDir_Right; break;
                    case DockPosition::Top:    direction = ImGuiDir_Up; break;
                    case DockPosition::Bottom: direction = ImGuiDir_Down; break;
                    default: direction = ImGuiDir_Left; break;
                    }

                    ImGuiID newDockID = 0;
                    ImGuiID remainingDockID = 0;
                    newDockID = ImGui::DockBuilderSplitNode(parentDockID, direction, splitRatio, nullptr, &remainingDockID);

                    ImGui::DockBuilderDockWindow(request.WindowName.c_str(), newDockID);
                    m_WindowDockIDs[request.WindowName] = newDockID;

                    m_WindowDockIDs[request.RelativeToWindow] = remainingDockID;
                }
                break;
            }

            case DockRelation::Tab:
            {
                auto it = m_WindowDockIDs.find(request.RelativeToWindow);
                if (it != m_WindowDockIDs.end())
                {
                    ImGuiID tabDockID = it->second;
                    ImGui::DockBuilderDockWindow(request.WindowName.c_str(), tabDockID);
                    m_WindowDockIDs[request.WindowName] = tabDockID;
                }
                break;
            }
            }
        }

        ImGui::DockBuilderFinish(dockspaceID);

        m_DockRequests.clear();
        m_WindowDockIDs.clear();
    }
}