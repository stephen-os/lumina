#pragma once

#include "raw.h"

#include <concepts>

namespace Lumina::UI
{
    inline void ConfirmDialog(const char* title, const char* message,
                               std::invocable auto&& onConfirm,
                               std::invocable auto&& onCancel = []{})
    {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal(title, nullptr,
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
        {
            ImGui::TextUnformatted(message);
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                onConfirm();
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                onCancel();
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    inline void InputDialog(const char* title, const char* prompt,
                             char* buffer, size_t bufferSize,
                             std::invocable auto&& onSubmit)
    {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal(title, nullptr,
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
        {
            ImGui::TextUnformatted(prompt);
            ImGui::Spacing();

            bool enterPressed = ImGui::InputText("##input", buffer, bufferSize,
                                                  ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::Button("OK", ImVec2(120, 0)) || enterPressed)
            {
                onSubmit();
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}
