#pragma once

#include "Lumina/Core/KeyCodes.h"

#include <memory>
#include <functional>

namespace Lumina
{
    class GlobalInputPlayback
    {
    public:
        virtual ~GlobalInputPlayback() = default;

        // Single event simulation
        virtual void SimulateKeyPress(KeyCode key) = 0;
        virtual void SimulateKeyRelease(KeyCode key) = 0;
        virtual void SimulateMouseButtonPress(MouseCode button, int x, int y) = 0;
        virtual void SimulateMouseButtonRelease(MouseCode button, int x, int y) = 0;
        virtual void SimulateMouseMove(int x, int y) = 0;
        virtual void SimulateMouseScroll(int dx, int dy) = 0;

        // Factory
        static std::unique_ptr<GlobalInputPlayback> Create();
    };
}