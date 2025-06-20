#include "Lumina/Core/Application.h"
#include "Lumina/Core/EntryPoint.h"

// #include "Example.h"
#include "FileReading.h"
#include "Rendering.h"

Lumina::Application* Lumina::CreateApplication(int argc, char** argv)
{
    Lumina::ApplicationSpecification spec;
    spec.Name = "Lumina Application";
    spec.Width = 900;
    spec.Height = 900;

    Lumina::Application* app = new Lumina::Application(spec);
    app->PushLayer<Rendering>();
    // app->PushLayer<FileReading>();

    return app;
}