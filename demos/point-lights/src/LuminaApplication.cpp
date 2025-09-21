#include "Lumina/Core/Application.h"
#include "Lumina/Core/EntryPoint.h"

#include "PointLights.h"

Lumina::Application* Lumina::CreateApplication(int argc, char** argv)
{
    Lumina::ApplicationSpecification spec;
    spec.Name = "Lumina Application";
    spec.Width = 900;
    spec.Height = 900;
    spec.Use2DRenderer = true; 
	spec.Maximized = true;
    
    Lumina::Application* app = new Lumina::Application(spec);
    app->PushLayer<PointLights>();
    
    return app;
}