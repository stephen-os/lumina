#pragma once

#include <imgui.h>

#include <concepts>

namespace lumina::ui
{
    inline void open_popup(const char* id)
    {
        ImGui::OpenPopup(id);
    }

    inline void close_current_popup()
    {
        ImGui::CloseCurrentPopup();
    }

    inline void confirm_dialog(const char* title, const char* message,
                               std::invocable auto&& on_confirm,
                               std::invocable auto&& on_cancel = []{})
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
                on_confirm();
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                on_cancel();
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    inline void input_dialog(const char* title, const char* prompt,
                             char* buffer, size_t buffer_size,
                             std::invocable auto&& on_submit)
    {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal(title, nullptr,
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
        {
            ImGui::TextUnformatted(prompt);
            ImGui::Spacing();

            bool enter_pressed = ImGui::InputText("##input", buffer, buffer_size,
                                                  ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::Button("OK", ImVec2(120, 0)) || enter_pressed)
            {
                on_submit();
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
