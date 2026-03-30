#pragma once

#include "Application.h"

#include <memory>

extern Lumina::Application* Lumina::CreateApplication(int argc, char** argv);

int main(int argc, char** argv)
{
	std::unique_ptr<Lumina::Application> app(Lumina::CreateApplication(argc, argv));
	app->Create();
	app->Run();
	app->Destroy();
	return 0;
}
