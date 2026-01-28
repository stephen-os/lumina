#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>

#include <imgui.h>

class sandbox_layer : public lumina::core::layer
{
public:
    sandbox_layer() : layer("sandbox") {}

    void on_attach() override
    {
        LUMINA_LOG_INFO("Sandbox layer attached");
    }

    void on_detach() override
    {
        LUMINA_LOG_INFO("Sandbox layer detached");
    }

    void on_update(float dt) override
    {
        // Update logic here
    }

    void on_render() override
    {
        // ImGui rendering
        ImGui::Begin("Sandbox");
        ImGui::Text("Welcome to Lumina!");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
            1000.0f / ImGui::GetIO().Framerate,
            ImGui::GetIO().Framerate);
        ImGui::End();

        ImGui::ShowDemoWindow();
    }

    void on_event(lumina::core::event& e) override
    {
        lumina::core::event_dispatcher dispatcher(e);

        dispatcher.dispatch<lumina::core::key_pressed_event>([](lumina::core::key_pressed_event& e)
        {
            if (e.get_key() == lumina::core::input::key_code::escape)
            {
                lumina::core::application::get().shutdown();
                return true;
            }
            return false;
        });
    }
};

lumina::core::application* lumina::core::create_application(int argc, char** argv)
{
    lumina::core::application_spec spec;
    spec.name = "Lumina Sandbox";
    spec.width = 1280;
    spec.height = 720;
	// spec.api = lumina::core::graphics_api::d3d12;

    auto* app = new lumina::core::application(spec);
    app->push_layer<sandbox_layer>();

    return app;
}
