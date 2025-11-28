#pragma once

#include "Application.h"

#ifdef LUMINA_PLATFORM_WINDOWS

extern Lumina::Core::Application* Lumina::Core::CreateApplication(int argc, char** argv);

namespace Lumina::Core
{
	int Main(int argc, char** argv)
	{
		Application* app = CreateApplication(argc, argv);
		app->Run();
		delete app;

		return 0;
	}
}

#ifdef LUMINA_DIST

#include <Windows.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	return Lumina::Core::Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
	return Lumina::Core::Main(argc, argv); 
}

#endif // LUMINA_DIST

#endif // LUMINA_PLATFORM_WINDOWS
