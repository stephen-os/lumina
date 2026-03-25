#pragma once

#include <string>

namespace Lumina::UI
{
    enum class NotificationType
    {
        Info,
        Success,
        Warning,
        Error
    };

    void Notify(const std::string& message, NotificationType type = NotificationType::Info,
                float durationSeconds = 3.0f);

    void RenderNotifications();
}
