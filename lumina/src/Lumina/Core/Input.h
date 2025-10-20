#pragma once

#include <utility>
#include <string>

#include "KeyCodes.h"

namespace Lumina 
{
    struct KeyCombo
    {
        std::vector<KeyCode> Keys;

        bool IsPressed() const
        {
            for (const auto& key : Keys)
            {
                if (!Input::IsKeyPressed(key))
                    return false;
            }
            return true;
		}

        bool operator==(const KeyCombo& other) const
        {
            if (Keys.size() != other.Keys.size())
                return false;
            for (const auto& key : Keys)
            {
                if (std::find(other.Keys.begin(), other.Keys.end(), key) == other.Keys.end())
                    return false;
            }
            return true;
		}
    };

    class Input
    {
    public:
        static bool IsKeyPressed(KeyCode keycode);
        static bool IsMouseButtonPressed(MouseCode mousecode);
        static std::pair<float, float> GetMousePosition();
        static float GetMouseX();
        static float GetMouseY();

        static void SetCursorMode(CursorMode mode);

        static bool IsShiftPressed();
        static bool IsCtrlPressed();
        static bool IsAltPressed();
        static bool IsSuperPressed();
        static bool IsCapsLockActive();

        static std::string KeyCodeToString(KeyCode keycode);
    };
}