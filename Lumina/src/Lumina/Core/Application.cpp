#include "Application.h"
#include "Log.h"
#include "Assert.h"
#include "Theme.h"
#include "Profiler.h"
#include "imgui/ImGuiNvrhi.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>

#include <GLFW/glfw3.h>

#include <nvrhi/utils.h>

#include <algorithm>

namespace Lumina
{
	static Application* s_Instance = nullptr;

	Application& Application::Get()
	{
		LUMINA_ASSERT(s_Instance, "Application instance is null");
		return *s_Instance;
	}

	Application::Application(const ApplicationSpecifications& specifications)
		: m_Specs(specifications)
	{
		// TODO: Different loggers for different subsystems
		// For example, we should have a core logger and a client logger.
		Log::Init("Lumina");

		LUMINA_ASSERT(!s_Instance, "Application already exists");
		s_Instance = this;

		// Set working directory if specified
		if (!m_Specs.WorkingDirectory.empty())
		{
			std::filesystem::current_path(m_Specs.WorkingDirectory);
		}

		LUMINA_LOG_INFO("Starting Lumina Application: {}", m_Specs.Title);

		// Build window spec from application specifications
		WindowSpec winSpec;
		winSpec.Title = m_Specs.Title;
		winSpec.IconPath = m_Specs.IconPath.string();
		winSpec.Width = m_Specs.Width;
		winSpec.Height = m_Specs.Height;
		winSpec.Fullscreen = m_Specs.Fullscreen;
		winSpec.Maximized = (m_Specs.StartMode == WindowStartMode::Maximized);
		winSpec.Centered = (m_Specs.StartMode == WindowStartMode::Centered);
		winSpec.Resizable = m_Specs.Resizable;
		winSpec.Decorated = m_Specs.Decorated;
		winSpec.VSync = m_Specs.VSync;

		m_Window = MakeScope<Window>(winSpec);
		m_Window->SetEventCallback([this](Event& e) { OnEvent(e); });
		LUMINA_ASSERT(m_Window, "Failed to create application window");

		// Apply titlebar theme if specified (Windows only)
		if (m_Specs.Titlebar.has_value())
		{
			const auto& theme = m_Specs.Titlebar.value();
			m_Window->SetTitlebarColor(
				static_cast<uint8_t>(theme.Background.r * 255.0f),
				static_cast<uint8_t>(theme.Background.g * 255.0f),
				static_cast<uint8_t>(theme.Background.b * 255.0f)
			);
			m_Window->SetTitlebarTextColor(
				static_cast<uint8_t>(theme.Text.r * 255.0f),
				static_cast<uint8_t>(theme.Text.g * 255.0f),
				static_cast<uint8_t>(theme.Text.b * 255.0f)
			);
		}

		// Initialize device
		m_Device = Device::Create(m_Specs.API);
		LUMINA_ASSERT(m_Device, "Failed to create device");

		DeviceDesc devDesc;
		devDesc.Window = m_Window->GetNativeWindow();
		devDesc.Width = m_Window->GetWidth();
		devDesc.Height = m_Window->GetHeight();
		devDesc.VSync = m_Specs.VSync;
		devDesc.AppName = m_Specs.Title.c_str();
#ifdef LUMINA_DEBUG
		devDesc.EnableDebugLayer = true;
#endif

		bool devInit = m_Device->Init(devDesc);
		LUMINA_ASSERT(devInit, "Failed to initialize device");

		if (m_Specs.EnableImGui)
		{
			InitImGui();
		}
	}

	Application::~Application()
	{
		m_EventQueue.clear();

		if (m_Specs.EnableImGui)
		{
			ShutdownImGui();
		}

		m_Device->Shutdown();
		m_Device.reset();

		m_Window.reset();
		Window::TerminateGLFW();

		Log::Shutdown();
		s_Instance = nullptr;
	}

	void Application::Create()
	{
		OnCreate();
	}

	void Application::Destroy()
	{
		for (auto& layer : m_LayerStack)
			layer->OnDetach();
		 
		m_LayerStack.clear();

		OnDestroy();
	}

	void Application::InitImGui()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// Initialize GLFW backend for ImGui (handles input)
		ImGui_ImplGlfw_InitForOther(m_Window->GetNativeWindow(), true);

		// Initialize NVRHI backend for ImGui rendering
		ImGuiNvrhi::ImGuiNvrhiConfig imguiConfig;
		imguiConfig.Device = m_Device->GetNvrhiDevice();
		imguiConfig.RenderTargetFormat = m_Device->GetSwapchainFormat();

		bool imguiInit = ImGuiNvrhi::Init(imguiConfig);
		LUMINA_ASSERT(imguiInit, "Failed to initialize ImGui NVRHI backend");

		ImGuiNvrhi::InitPlatformViewports(*m_Device);

		Theme::ApplyLuminaTheme();

		LUMINA_LOG_INFO("ImGui initialized");
	}

	void Application::ShutdownImGui()
	{
		ImGuiNvrhi::ShutdownPlatformViewports();
		ImGuiNvrhi::Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void Application::BeginFrame()
	{
		LUMINA_PROFILE_SCOPE_NC("Application::BeginFrame", Profiler::Colors::Frame);

		m_Device->BeginFrame();

		// Clear the framebuffer
		auto cmd = m_Device->GetCommandList();
		auto fb = m_Device->GetCurrentFramebuffer();

		nvrhi::utils::ClearColorAttachment(cmd, fb, 0, nvrhi::Color(
			m_Specs.ClearColor.r,
			m_Specs.ClearColor.g,
			m_Specs.ClearColor.b,
			m_Specs.ClearColor.a
		));

		if (m_Specs.EnableImGui)
		{
			ImGuiNvrhi::NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}
	}

	void Application::EndFrame()
	{
		LUMINA_PROFILE_SCOPE_NC("Application::EndFrame", Profiler::Colors::Frame);

		if (m_Specs.EnableImGui)
		{
			ImGui::Render();

			// Render ImGui draw data using NVRHI
			auto cmd = m_Device->GetCommandList();
			auto fb = m_Device->GetCurrentFramebuffer();
			ImGuiNvrhi::RenderDrawData(cmd, fb, ImGui::GetDrawData());
		}

		// Present the main window first — this closes and executes the main command list.
		// Viewport rendering must happen after, since NVRHI only allows one immediate
		// command list open at a time.
		m_Device->Present();

		if (m_Specs.EnableImGui)
		{
			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		// Handle window resize
		dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& resizeEvent) {
			OnResize(resizeEvent.GetWidth(), resizeEvent.GetHeight());
			return false;
		});

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (e.IsHandled())
				break;
			(*it)->OnEvent(e);
		}
	}

	void Application::OnResize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0)
			return;

		m_Device->Resize(width, height);
	}

	void Application::PostEvent(Event& e)
	{
		OnEvent(e);
	}

	void Application::QueueEvent(std::unique_ptr<Event> e)
	{
		m_EventQueue.push_back(std::move(e));
	}

	void Application::Run()
	{
		m_Window->Show();

		float lastTime = GetTime();

		while (m_Running)
		{
			LUMINA_PROFILE_SCOPE_NC("Application::Frame", Profiler::Colors::Frame);

			m_Window->Update();

			if (m_Window->ShouldClose())
			{
				Stop();
				break;
			}

			// Process queued events
			{
				LUMINA_PROFILE_SCOPE_N("Application::ProcessEvents");
				for (auto& e : m_EventQueue)
					OnEvent(*e);
				m_EventQueue.clear();
			}

			float currentTime = GetTime();
			float timestep = std::clamp(currentTime - lastTime, 0.001f, 0.1f);
			lastTime = currentTime;

			// Update layers
			{
				LUMINA_PROFILE_SCOPE_N("Application::UpdateLayers");
				for (auto& layer : m_LayerStack)
					layer->OnUpdate(timestep);
			}

			BeginFrame();

			if (m_Specs.EnableImGui)
			{
				// Setup dockspace
				ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImGui::SetNextWindowPos(viewport->WorkPos);
				ImGui::SetNextWindowSize(viewport->WorkSize);
				ImGui::SetNextWindowViewport(viewport->ID);

				ImGuiWindowFlags windowFlags =
					ImGuiWindowFlags_NoTitleBar |
					ImGuiWindowFlags_NoCollapse |
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoMove |
					ImGuiWindowFlags_NoBringToFrontOnFocus |
					ImGuiWindowFlags_NoNavFocus |
					ImGuiWindowFlags_NoBackground;

				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

				ImGui::Begin("lumina_dockspace", nullptr, windowFlags);
				ImGui::PopStyleVar(3);

				ImGuiID dockspaceId = ImGui::GetID("lumina_dockspace_id");
				ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
			}

			// Render layers
			{
				LUMINA_PROFILE_SCOPE_N("Application::RenderLayers");
				for (auto& layer : m_LayerStack)
					layer->OnRender();
			}

			if (m_Specs.EnableImGui)
			{
				ImGui::End();
			}

			EndFrame();

			LUMINA_PROFILE_FRAME();
		}
	}

	void Application::SetFullscreen(bool fullscreen)
	{
		m_Window->SetFullscreen(fullscreen);
	}

	void Application::SetVSync(bool enabled)
	{
		m_Window->SetVSync(enabled);
	}

	void Application::SetPosition(int32_t x, int32_t y)
	{
		m_Window->SetPosition(x, y);
	}

	void Application::Maximize()
	{
		m_Window->Maximize();
	}

	void Application::Minimize()
	{
		m_Window->Minimize();
	}

	void Application::Restore()
	{
		m_Window->Restore();
	}

	uint32_t Application::GetWidth() const
	{
		return m_Window->GetWidth();
	}

	uint32_t Application::GetHeight() const
	{
		return m_Window->GetHeight();
	}

	bool Application::IsFullscreen() const
	{
		return m_Window->IsFullscreen();
	}

	bool Application::IsVSync() const
	{
		return m_Window->IsVSync();
	}

	bool Application::IsMaximized() const
	{
		return m_Window->IsMaximized();
	}

	bool Application::IsMinimized() const
	{
		return m_Window->IsMinimized();
	}

	float Application::GetTime()
	{
		return static_cast<float>(glfwGetTime());
	}
}
