#pragma once

#include <utility>

#include "KeyCode.h"

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
    };
}