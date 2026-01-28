#pragma once

#include "application.h"

#ifdef LUMINA_PLATFORM_WINDOWS

extern lumina::core::application* lumina::core::create_application(int argc, char** argv);

int main(int argc, char** argv)
{
    auto app = lumina::core::create_application(argc, argv);
    app->run();
    delete app;
    return 0;
}

#else

extern lumina::core::application* lumina::core::create_application(int argc, char** argv);

int main(int argc, char** argv)
{
    auto app = lumina::core::create_application(argc, argv);
    app->run();
    delete app;
    return 0;
}

#endif
