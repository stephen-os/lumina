#pragma once

#include <utility>
#include <string>

#include "KeyCodes.h"

namespace Lumina 
{
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