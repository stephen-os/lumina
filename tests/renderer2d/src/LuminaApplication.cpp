#include "Lumina/Core/Application.h"
#include "Lumina/Core/EntryPoint.h"

#include "TestRenderer2D.h"

Lumina::Application* Lumina::CreateApplication(int argc, char** argv)
{
    Lumina::ApplicationSpecification spec;
    spec.Name = "Test Renderer2D";
	spec.Maximized = true;

    Lumina::Application* app = new Lumina::Application(spec);
    app->PushLayer<TestRenderer2D>();

    return app;
}