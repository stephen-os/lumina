#include "Lumina/Core/Application.h"
#include "Lumina/Core/EntryPoint.h"

#include "ApplicationLayer.h"

namespace Lumina::Core
{
    Application * CreateApplication(int argc, char** argv)
    {
        ApplicationSpecification spec;
        spec.Name = "Lumina Application";
        spec.Width = 900;
        spec.Height = 900;
    
        Application* app = new Application(spec);
        app->PushLayer<ApplicationLayer>();
    
        return app;
    }
}