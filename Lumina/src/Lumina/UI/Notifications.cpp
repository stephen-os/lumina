#include "Notifications.h"

#include <imgui.h>
#include <Lumina/Core/theme.h>

#include <vector>
#include <string>
#include <chrono>
#include <mutex>

namespace Lumina::UI
{
    namespace
    {
        struct NotificationEntry
        {
            std::string message;
            NotificationType type;
            float duration;
            std::chrono::steady_clock::time_point creationTime;
        };

        std::vector<NotificationEntry> s_Notifications;
        std::mutex s_NotificationsMutex;

        ImVec4 GetNotificationColor(NotificationType type)
        {
            switch (type)
            {
            case NotificationType::Info:    return Lumina::Color::TextSecondary;
            case NotificationType::Success: return Lumina::Color::Success;
            case NotificationType::Warning: return Lumina::Color::Warning;
            case NotificationType::Error:   return Lumina::Color::Error;
            default:                        return Lumina::Color::TextSecondary;
            }
        }

        const char* GetNotificationPrefix(NotificationType type)
        {
            switch (type)
            {
            case NotificationType::Info:    return "[INFO]";
            case NotificationType::Success: return "[OK]";
            case NotificationType::Warning: return "[WARN]";
            case NotificationType::Error:   return "[ERROR]";
            default:                        return "[INFO]";
            }
        }
    }

    void Notify(const std::string& message, NotificationType type, float durationSeconds)
    {
        std::lock_guard<std::mutex> lock(s_NotificationsMutex);
        s_Notifications.push_back({
            message,
            type,
            durationSeconds,
            std::chrono::steady_clock::now()
        });
    }

    void RenderNotifications()
    {
        std::lock_guard<std::mutex> lock(s_NotificationsMutex);

        if (s_Notifications.empty())
            return;

        auto now = std::chrono::steady_clock::now();

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        const float padding = 16.0f;
        const float notificationWidth = 300.0f;
        float yOffset = padding;

        for (int i = static_cast<int>(s_Notifications.size()) - 1; i >= 0; --i)
        {
            auto& entry = s_Notifications[i];
            float elapsed = std::chrono::duration<float>(now - entry.creationTime).count();

            if (elapsed >= entry.duration)
            {
                s_Notifications.erase(s_Notifications.begin() + i);
                continue;
            }

            // Fade out in the last 0.5 seconds
            float alpha = 1.0f;
            float fadeStart = entry.duration - 0.5f;
            if (elapsed > fadeStart)
                alpha = 1.0f - (elapsed - fadeStart) / 0.5f;

            ImVec2 windowPos = ImVec2(
                viewport->WorkPos.x + viewport->WorkSize.x - notificationWidth - padding,
                viewport->WorkPos.y + viewport->WorkSize.y - yOffset - 50.0f
            );

            ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(notificationWidth, 0));
            ImGui::SetNextWindowBgAlpha(0.85f * alpha);

            char windowName[64];
            snprintf(windowName, sizeof(windowName), "##notification_%d", i);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f, 8.0f));
            ImGui::PushStyleColor(ImGuiCol_WindowBg, Lumina::Color::MediumGray);

            ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration
                                   | ImGuiWindowFlags_NoInputs
                                   | ImGuiWindowFlags_NoNav
                                   | ImGuiWindowFlags_NoFocusOnAppearing
                                   | ImGuiWindowFlags_NoSavedSettings
                                   | ImGuiWindowFlags_AlwaysAutoResize;

            if (ImGui::Begin(windowName, nullptr, flags))
            {
                ImVec4 color = GetNotificationColor(entry.type);
                color.w *= alpha;

                ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::TextUnformatted(GetNotificationPrefix(entry.type));
                ImGui::PopStyleColor();

                ImGui::SameLine();

                ImVec4 textColor = Lumina::Color::TextPrimary;
                textColor.w *= alpha;
                ImGui::PushStyleColor(ImGuiCol_Text, textColor);
                ImGui::TextWrapped("%s", entry.message.c_str());
                ImGui::PopStyleColor();

                yOffset += ImGui::GetWindowHeight() + 4.0f;
            }
            ImGui::End();

            ImGui::PopStyleColor();
            ImGui::PopStyleVar(2);
        }
    }
}
