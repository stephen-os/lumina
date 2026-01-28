#pragma once

#include <string>

namespace lumina::ui
{
    enum class notification_type
    {
        info,
        success,
        warning,
        error
    };

    void notify(const std::string& message, notification_type type = notification_type::info,
                float duration_seconds = 3.0f);

    void render_notifications();
}
