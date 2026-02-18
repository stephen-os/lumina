#pragma once

#include "application.h"

#include <memory>

extern lumina::core::application* lumina::core::create_application(int argc, char** argv);

int main(int argc, char** argv)
{
    std::unique_ptr<lumina::core::application> app(lumina::core::create_application(argc, argv));
    app->run();
    return 0;
}
