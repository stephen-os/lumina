#include "Lumina/Core/Application.h"
#include "Lumina/Core/EntryPoint.h"

#include "DemoBasicRendering.h"

Lumina::Application* Lumina::CreateApplication(int argc, char** argv)
{
    Lumina::ApplicationSpecification spec;
    spec.Name = "Basic Rendering";
    spec.Width = 1200;
    spec.Height = 900;
	spec.Use2DRenderer = true;

    Lumina::Application* app = new Lumina::Application(spec);
    app->PushLayer<DemoBasicRendering>();

    return app;
}