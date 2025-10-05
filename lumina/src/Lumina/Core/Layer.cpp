#include "Layer.h"

#include <imgui_internal.h> 

#include <algorithm>
#include <filesystem>

namespace Lumina
{
    void Layer::DockWindow(const std::string& windowName, DockPosition position, float sizeRatio)
    {
        m_DockRequests.push_back({ windowName, position, sizeRatio });
    }

    void Layer::ProcessDockingRequests(ImGuiID dockspaceID)
    {
        if (m_DockRequests.empty())
            return;

        ImGuiIO& io = ImGui::GetIO(); 
        if (io.IniFilename && std::filesystem::exists(io.IniFilename))
        {
            m_DockRequests.clear();
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
            if (request.Position == DockPosition::Center)
            {
                ImGui::DockBuilderDockWindow(request.WindowName.c_str(), remainingSpace);
                continue; 
            }
            
            float adjustedRatio = request.SizeRatio / remainingRatio;
            adjustedRatio = std::clamp(adjustedRatio, 0.01f, 0.95f);

			ImGuiID dockID = 0;

            switch (request.Position)
            {
            case DockPosition::Left:
                dockID = ImGui::DockBuilderSplitNode(remainingSpace, ImGuiDir_Left, adjustedRatio, nullptr, &remainingSpace);
                break;

            case DockPosition::Right:
                dockID = ImGui::DockBuilderSplitNode(remainingSpace, ImGuiDir_Right, adjustedRatio, nullptr, &remainingSpace);
                break;

            case DockPosition::Top:
                dockID = ImGui::DockBuilderSplitNode(remainingSpace, ImGuiDir_Up, adjustedRatio, nullptr, &remainingSpace);
                break;

            case DockPosition::Bottom:
                dockID = ImGui::DockBuilderSplitNode(remainingSpace, ImGuiDir_Down, adjustedRatio, nullptr, &remainingSpace);
                break;
            }

            ImGui::DockBuilderDockWindow(request.WindowName.c_str(), dockID);
            remainingRatio = std::max(0.05f, remainingRatio - request.SizeRatio);
        }

        ImGui::DockBuilderFinish(dockspaceID);
        m_DockRequests.clear();
    }
}