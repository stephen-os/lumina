<p align="center">
  <img src="assets/icon.png" alt="Lumina" width="200">
</p>

<h1 align="center">Lumina</h1>

<p align="center">
  A C++ application framework for graphics-focused applications
</p>

---

Lumina is a modular application framework designed for building graphics applications, tools, and games. It provides a complete foundation with windowing, input, 2D rendering, UI, and physics - all with multi-backend GPU support.

## Features

**Core**
- Window management and input handling
- Event system with layer architecture
- Vulkan and D3D12 backends via NVRHI

**Graphics**
- Batched 2D renderer with texture arrays
- Primitives: quads, circles, lines, triangles, text, grids
- Camera system with zoom, rotation, shake, and follow
- Texture atlases and sprite sheets
- Per-primitive blend modes and scissor clipping
- Render targets with MSAA support
- 2D lighting with multiple attenuation models

**UI**
- ImGui integration with docking and viewports
- Node editor for visual graphs
- File dialogs
- Notification system

**Physics**
- Box2D integration for 2D physics simulation

## Requirements

- Windows 10/11
- Visual Studio 2022
- Vulkan SDK (for Vulkan backend)

## Building

```bash
git clone --recursive https://github.com/stephen-os/Lumina.git
cd Lumina/scripts
setup-project-files.bat
```

Open the generated `.sln` file in Visual Studio and build.

## Quick Start

```cpp
#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/graphics/graphics.h>

class game_layer : public lumina::core::layer
{
public:
    void on_attach() override
    {
        auto& device = lumina::core::application::get().get_device();
        m_renderer = std::make_unique<lumina::graphics::renderer2d>(device);
        m_renderer->init();
    }

    void on_render() override
    {
        m_renderer->begin(m_camera);
        m_renderer->draw_quad({
            .position = {100, 100, 0},
            .size = {50, 50},
            .color = {1, 0, 0, 1}
        });
        m_renderer->end();
    }

private:
    std::unique_ptr<lumina::graphics::renderer2d> m_renderer;
    lumina::graphics::camera2d m_camera{600.0f, 1.0f};
};

lumina::core::application* lumina::core::create_application(int argc, char** argv)
{
    application_specifications specs;
    specs.title = "My Game";
    auto* app = new application(specs);
    app->push_layer<game_layer>();
    return app;
}
```

## Dependencies

- [GLFW](https://github.com/glfw/glfw) - Windowing and input
- [NVRHI](https://github.com/NVIDIAGameWorks/nvrhi) - Graphics abstraction
- [ImGui](https://github.com/ocornut/imgui) - Immediate mode UI
- [GLM](https://github.com/g-truc/glm) - Mathematics
- [Box2D](https://github.com/erincatto/box2d) - 2D physics
- [spdlog](https://github.com/gabime/spdlog) - Logging

## License

MIT
