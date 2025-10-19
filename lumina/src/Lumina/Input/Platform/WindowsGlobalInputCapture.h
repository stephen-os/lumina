#pragma once

#include "Lumina/Input/GlobalInputCapture.h"

#ifdef _WIN32

#include <Windows.h>
#include <mutex>

namespace Lumina
{
    class WindowsGlobalInputCapture : public GlobalInputCapture
    {
    public:
        WindowsGlobalInputCapture();
        ~WindowsGlobalInputCapture() override;

        bool Start() override;
        void Stop() override;
        bool IsActive() const override { return m_IsActive; }

    private:
        static LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);

        static KeyCode VirtualKeyToKeyCode(DWORD vkCode);
        static MouseCode ButtonToMouseCode(WPARAM wParam);

        std::mutex m_CallbackMutex;

        HHOOK m_KeyboardHook = nullptr;
        HHOOK m_MouseHook = nullptr;
        bool m_IsActive = false;

        static WindowsGlobalInputCapture* s_Instance;
    };
}

#endif // _WIN32