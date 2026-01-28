#include "notifications.h"

#include <imgui.h>
#include <lumina/core/theme.h>

#include <vector>
#include <string>
#include <chrono>

namespace lumina::ui
{
    namespace
    {
        struct notification_entry
        {
            std::string message;
            notification_type type;
            float duration;
            std::chrono::steady_clock::time_point creation_time;
        };

        std::vector<notification_entry> s_notifications;

        ImVec4 get_notification_color(notification_type type)
        {
            switch (type)
            {
            case notification_type::info:    return lumina::core::color::text_secondary;
            case notification_type::success: return lumina::core::color::success;
            case notification_type::warning: return lumina::core::color::warning;
            case notification_type::error:   return lumina::core::color::error;
            default:                         return lumina::core::color::text_secondary;
            }
        }

        const char* get_notification_prefix(notification_type type)
        {
            switch (type)
            {
            case notification_type::info:    return "[INFO]";
            case notification_type::success: return "[OK]";
            case notification_type::warning: return "[WARN]";
            case notification_type::error:   return "[ERROR]";
            default:                         return "[INFO]";
            }
        }
    }

    void notify(const std::string& message, notification_type type, float duration_seconds)
    {
        s_notifications.push_back({
            message,
            type,
            duration_seconds,
            std::chrono::steady_clock::now()
        });
    }

    void render_notifications()
    {
        if (s_notifications.empty())
            return;

        auto now = std::chrono::steady_clock::now();

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        const float padding = 16.0f;
        const float notification_width = 300.0f;
        float y_offset = padding;

        for (int i = static_cast<int>(s_notifications.size()) - 1; i >= 0; --i)
        {
            auto& entry = s_notifications[i];
            float elapsed = std::chrono::duration<float>(now - entry.creation_time).count();

            if (elapsed >= entry.duration)
            {
                s_notifications.erase(s_notifications.begin() + i);
                continue;
            }

            // Fade out in the last 0.5 seconds
            float alpha = 1.0f;
            float fade_start = entry.duration - 0.5f;
            if (elapsed > fade_start)
                alpha = 1.0f - (elapsed - fade_start) / 0.5f;

            ImVec2 window_pos = ImVec2(
                viewport->WorkPos.x + viewport->WorkSize.x - notification_width - padding,
                viewport->WorkPos.y + viewport->WorkSize.y - y_offset - 50.0f
            );

            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(notification_width, 0));
            ImGui::SetNextWindowBgAlpha(0.85f * alpha);

            char window_name[64];
            snprintf(window_name, sizeof(window_name), "##notification_%d", i);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f, 8.0f));
            ImGui::PushStyleColor(ImGuiCol_WindowBg, lumina::core::color::medium_gray);

            ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration
                                   | ImGuiWindowFlags_NoInputs
                                   | ImGuiWindowFlags_NoNav
                                   | ImGuiWindowFlags_NoFocusOnAppearing
                                   | ImGuiWindowFlags_NoSavedSettings
                                   | ImGuiWindowFlags_AlwaysAutoResize;

            if (ImGui::Begin(window_name, nullptr, flags))
            {
                ImVec4 color = get_notification_color(entry.type);
                color.w *= alpha;

                ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::TextUnformatted(get_notification_prefix(entry.type));
                ImGui::PopStyleColor();

                ImGui::SameLine();

                ImVec4 text_color = lumina::core::color::text_primary;
                text_color.w *= alpha;
                ImGui::PushStyleColor(ImGuiCol_Text, text_color);
                ImGui::TextWrapped("%s", entry.message.c_str());
                ImGui::PopStyleColor();

                y_offset += ImGui::GetWindowHeight() + 4.0f;
            }
            ImGui::End();

            ImGui::PopStyleColor();
            ImGui::PopStyleVar(2);
        }
    }
}
