#include "Lumina/Core/Application.h"
#include "Lumina/Core/EntryPoint.h"

#include "SpacialRendering.h"

Lumina::Application* Lumina::CreateApplication(int argc, char** argv)
{
    Lumina::ApplicationSpecification spec;
    spec.Name = "Lumina Application";
    spec.Width = 1920;
    spec.Height = 1080;

    Lumina::Application* app = new Lumina::Application(spec);
    app->PushLayer<SpacialRendering>();

    return app;
}