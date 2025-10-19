#pragma once

#include "Lumina/Input/GlobalInputPlayback.h"

#ifdef _WIN32

#include <Windows.h>

namespace Lumina
{
    class WindowsGlobalInputPlayback : public GlobalInputPlayback
    {
    public:
        WindowsGlobalInputPlayback() = default;
        ~WindowsGlobalInputPlayback() override = default;

        void SimulateKeyPress(KeyCode key) override;
        void SimulateKeyRelease(KeyCode key) override;
        void SimulateMouseButtonPress(MouseCode button, int x, int y) override;
        void SimulateMouseButtonRelease(MouseCode button, int x, int y) override;
        void SimulateMouseMove(int x, int y) override;
        void SimulateMouseScroll(int dx, int dy) override;

    private:
        WORD KeyCodeToVirtualKey(KeyCode key);
        void SendMouseButton(MouseCode button, bool pressed, int x, int y);
    };
}

#endif // _WIN32