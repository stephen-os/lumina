#pragma once

#include "Base.h"
#include "Window.h"
#include "Layer.h"
#include "Event.h"
#include "Device.h"
#include "Input.h"

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <optional>

namespace Lumina
{
	enum class WindowStartMode
	{
		Normal,   // Default position
		Centered, // Center on primary monitor
		Maximized // Start maximized
	};

	struct ApplicationSpecifications
	{
		// Identity (immutable after creation)
		std::string Title = "Lumina Application";
		std::filesystem::path IconPath;
		std::filesystem::path WorkingDirectory;

		// Window configuration
		uint32_t Width = 1600;
		uint32_t Height = 900;
		WindowStartMode StartMode = WindowStartMode::Centered;
		bool Fullscreen = false;
		bool Resizable = true;
		bool Decorated = true;

		// Rendering
		bool VSync = true;
		bool EnableImGui = true;
		GraphicsAPI API = GraphicsAPI::Vulkan;
		glm::vec4 ClearColor = {0.1f, 0.1f, 0.1f, 1.0f};

		// Optional titlebar theming (Windows only)
		struct TitlebarTheme
		{
			glm::vec3 Background = {0.118f, 0.118f, 0.118f};
			glm::vec3 Text = {1.0f, 1.0f, 1.0f};
		};
		std::optional<TitlebarTheme> Titlebar;
	};

	class Application
	{
	public:
		Application(const ApplicationSpecifications& specifications);
		~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		void Run();
		void Shutdown() { m_Running = false; }

		// Layer management
		template<typename T>
		void PushLayer()
		{
			static_assert(std::is_base_of_v<Layer, T>, "T must derive from Layer");
			m_LayerStack.emplace_back(std::make_shared<T>())->OnAttach();
		}

		void PushLayer(std::unique_ptr<Layer> layer)
		{
			m_LayerStack.emplace_back(std::move(layer));
			LayerPtr->OnAttach();
		}

		// Events
		void OnEvent(Event& e);
		void PostEvent(Event& e);
		void QueueEvent(std::unique_ptr<Event> e);

		// Runtime window operations
		void SetFullscreen(bool fullscreen);
		void SetVSync(bool enabled);
		void SetPosition(int32_t x, int32_t y);
		void Maximize();
		void Minimize();
		void Restore();

		// Window state
		[[nodiscard]] uint32_t GetWidth() const;
		[[nodiscard]] uint32_t GetHeight() const;
		[[nodiscard]] bool IsFullscreen() const;
		[[nodiscard]] bool IsVSync() const;
		[[nodiscard]] bool IsMaximized() const;
		[[nodiscard]] bool IsMinimized() const;

		// Device access
		[[nodiscard]] Device& GetDevice() { return *m_Device; }
		[[nodiscard]] nvrhi::IDevice* GetNvrhiDevice() { return m_Device->GetNvrhiDevice(); }

		// Specifications
		[[nodiscard]] const ApplicationSpecifications& GetSpecifications() const { return m_Specs; }

		[[nodiscard]] static Application& Get();
		[[nodiscard]] static float GetTime();

	private:
		void InitImGui();
		void ShutdownImGui();
		void BeginFrame();
		void EndFrame();
		void OnResize(uint32_t width, uint32_t height);

		// Internal window access for input system
		[[nodiscard]] GLFWwindow* GetWindowHandle() const { return m_Window->GetNativeWindow(); }
		friend bool Input::IsKeyPressed(Input::KeyCode);
		friend bool Input::IsMouseButtonPressed(Input::MouseCode);
		friend void Input::SetCursorMode(Input::CursorMode);
		friend Input::MousePosition Input::GetMousePosition();

		ApplicationSpecifications m_Specs;
		Scope<Window> m_Window;
		Scope<Device> m_Device;

		bool m_Running = true;
		std::vector<Ref<Layer>> m_LayerStack;
		std::vector<std::unique_ptr<Event>> m_EventQueue;
	};

	// User must implement this function
	Application* CreateApplication(int argc, char** argv);
}
