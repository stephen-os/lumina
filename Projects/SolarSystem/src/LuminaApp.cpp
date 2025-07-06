#include "Lumina/Core/Application.h"
#include "Lumina/Core/EntryPoint.h"

#include "OrbitalEngine.h"

Lumina::Application* Lumina::CreateApplication(int argc, char** argv)
{
    Lumina::ApplicationSpecification spec;
    spec.Name = "Lumina Application";
    spec.Width = 1920;
    spec.Height = 1080;

    Lumina::Application* app = new Lumina::Application(spec);
    app->PushLayer<OrbitalEngine>();

    return app;
}