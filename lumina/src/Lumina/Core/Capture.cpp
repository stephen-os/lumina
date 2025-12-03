#include "Capture.h"
#include "Log.h"
#include "Event.h"
#include "Input.h"

#include <thread>
#include <atomic>

namespace Lumina::Core
{
    struct Capture::PlatformData
    {
        std::thread hookThread;
        std::atomic<bool> shouldStop{ false };
    };
    static Capture* s_CaptureInstance = nullptr;

    static void dispatch_proc(uiohook_event* const event, void* user_data)
    {
        if (!s_CaptureInstance)
            return;

        auto& spec = s_CaptureInstance->GetSpecifications();

        if (spec.Keyboard && event->type == EVENT_KEY_PRESSED)
        {
            Input::KeyCode keyCode = static_cast<Input::KeyCode>(event->data.keyboard.keycode);
            GlobalKeyPressedEvent keyEvent(keyCode);

            if (s_CaptureInstance->m_CaptureEventCallback)
                s_CaptureInstance->m_CaptureEventCallback(keyEvent);
        }
    }

    Capture::Capture(const CaptureSpecifications& specifications)
        : m_Specifications(specifications)
        , m_PlatformData(CreateScope<PlatformData>())
    {
        s_CaptureInstance = this;
        LUMINA_LOG_INFO("Capture created");
    }

    Capture::~Capture()
    {
        if (m_Capturing)
            Stop();

        if (s_CaptureInstance == this)
            s_CaptureInstance = nullptr;

        LUMINA_LOG_INFO("Capture destroyed");
    }

    void Capture::Start()
    {
        if (m_Capturing)
        {
            LUMINA_LOG_WARN("Capture already running");
            return;
        }

        LUMINA_LOG_INFO("Starting global capture...");

        hook_set_dispatch_proc(dispatch_proc, nullptr);

        m_PlatformData->shouldStop = false;
        m_PlatformData->hookThread = std::thread([this]()
            {
                LUMINA_LOG_INFO("uiohook thread starting...");

                // This blocks until hook_stop() is called
                int status = hook_run();

                if (status != UIOHOOK_SUCCESS)
                {
                    const char* error = "Unknown error";
                    switch (status)
                    {
                    case UIOHOOK_ERROR_OUT_OF_MEMORY:
                        error = "Out of memory";
                        break;
                    case UIOHOOK_ERROR_X_OPEN_DISPLAY:
                        error = "Failed to open X11 display";
                        break;
                    case UIOHOOK_ERROR_X_RECORD_NOT_FOUND:
                        error = "X Record extension not found";
                        break;
                    case UIOHOOK_ERROR_X_RECORD_ALLOC_RANGE:
                        error = "Failed to allocate X Record range";
                        break;
                    case UIOHOOK_ERROR_X_RECORD_CREATE_CONTEXT:
                        error = "Failed to create X Record context";
                        break;
                    case UIOHOOK_ERROR_X_RECORD_ENABLE_CONTEXT:
                        error = "Failed to enable X Record context";
                        break;
                    case UIOHOOK_ERROR_SET_WINDOWS_HOOK_EX:
                        error = "Failed to set Windows hook";
                        break;
                    case UIOHOOK_ERROR_CREATE_EVENT_PORT:
                        error = "Failed to create macOS event port";
                        break;
                    case UIOHOOK_ERROR_CREATE_RUN_LOOP_SOURCE:
                        error = "Failed to create macOS run loop source";
                        break;
                    case UIOHOOK_ERROR_GET_RUNLOOP:
                        error = "Failed to get macOS run loop";
                        break;
                    case UIOHOOK_ERROR_CREATE_OBSERVER:
                        error = "Failed to create macOS observer";
                        break;
                    }

                    LUMINA_LOG_ERROR("uiohook failed to start: {} ({})", error, status);
                }
                else
                {
                    LUMINA_LOG_INFO("uiohook thread stopped cleanly");
                }
            });

        m_Capturing = true;
        LUMINA_LOG_INFO("Global capture started");
    }

    void Capture::Stop()
    {
        if (!m_Capturing)
        {
            LUMINA_LOG_WARN("Capture not running");
            return;
        }

        LUMINA_LOG_INFO("Stopping global capture...");

        int status = hook_stop();

        if (status != UIOHOOK_SUCCESS)
        {
            LUMINA_LOG_ERROR("Failed to stop uiohook: {}", status);
        }

        if (m_PlatformData->hookThread.joinable())
        {
            m_PlatformData->hookThread.join();
        }

        m_Capturing = false;
        LUMINA_LOG_INFO("Global capture stopped");
    }

    void Capture::ApplySpecifications(const CaptureSpecifications& specifications)
    {
        bool wasCapturing = m_Capturing;

        if (wasCapturing)
            Stop();

        m_Specifications = specifications;
        LUMINA_LOG_INFO("Capture specifications updated");

        if (wasCapturing)
            Start();
    }
}