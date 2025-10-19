#include "GlobalInputPlayback.h"

#ifdef _WIN32
#include "Platform/WindowsGlobalInputPlayback.h"
#endif

namespace Lumina
{
    std::unique_ptr<GlobalInputPlayback> GlobalInputPlayback::Create()
    {
#ifdef _WIN32
        return std::make_unique<WindowsGlobalInputPlayback>();
#else
        return nullptr;
#endif
    }
}