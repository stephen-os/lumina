#pragma once

#include <utility>
#include <string>
#include <vector>

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
        static std::string MouseCodeToString(MouseCode mousecode); 
    };


    // TODO: Move this to a separate file
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

        bool Contains(KeyCode key) const
        {
            return std::find(Keys.begin(), Keys.end(), key) != Keys.end();
        }

        void Add(KeyCode key)
        {
            if (!Contains(key))
                Keys.push_back(key);
        }

        void Remove(KeyCode key)
        {
            auto it = std::find(Keys.begin(), Keys.end(), key);
            if (it != Keys.end())
                Keys.erase(it);
        }

        void Clear()
        {
            Keys.clear();
        }

        bool IsEmpty() const
        {
            return Keys.empty();
        }

        size_t Size() const
        {
            return Keys.size();
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
}