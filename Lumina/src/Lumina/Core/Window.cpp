#include "Window.h"
#include "Log.h"
#include "Assert.h"
#include "Input.h"
#include "DefaultIcon.h"

#include <GLFW/glfw3.h>

#ifdef LUMINA_PLATFORM_WINDOWS
	#define GLFW_EXPOSE_NATIVE_WIN32
	#include <GLFW/glfw3native.h>
	#include <dwmapi.h>

	#ifndef DWMWA_CAPTION_COLOR
		#define DWMWA_CAPTION_COLOR 35
	#endif
	#ifndef DWMWA_TEXT_COLOR
		#define DWMWA_TEXT_COLOR 36
	#endif
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb_image.h>

namespace Lumina
{
	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		LUMINA_LOG_ERROR("[GLFW] Error {}: {}", error, description);
	}

	Window::Window(const WindowSpec& spec)
		: m_Width(spec.Width)
		, m_Height(spec.Height)
		, m_WindowedWidth(spec.Width)
		, m_WindowedHeight(spec.Height)
		, m_VSync(spec.VSync)
		, m_Fullscreen(spec.Fullscreen)
	{
		if (!s_GLFWInitialized)
		{
			glfwSetErrorCallback(GLFWErrorCallback);

			if (!glfwInit())
			{
				LUMINA_LOG_ERROR("Failed to initialize GLFW");
				return;
			}
			s_GLFWInitialized = true;
		}

		// Window hints
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		glfwWindowHint(GLFW_RESIZABLE, spec.Resizable ? GLFW_TRUE : GLFW_FALSE);
		glfwWindowHint(GLFW_DECORATED, spec.Decorated ? GLFW_TRUE : GLFW_FALSE);

		m_Window = glfwCreateWindow(
			static_cast<int>(spec.Width),
			static_cast<int>(spec.Height),
			spec.Title.c_str(),
			nullptr,
			nullptr
		);

		if (!m_Window)
		{
			LUMINA_LOG_ERROR("Failed to create GLFW window");
			return;
		}

		// Set window icon (custom or default)
		if (!spec.IconPath.empty())
		{
			SetIcon(spec.IconPath);
		}
		else
		{
			SetDefaultIcon();
		}

		SetupCallbacks();

		// Apply initial window state
		if (spec.Fullscreen)
		{
			SetFullscreen(true);
		}
		else if (spec.Maximized)
		{
			Maximize();
		}
		else if (spec.Centered)
		{
			CenterOnMonitor();
		}

		LUMINA_LOG_INFO("Window created: {}x{}", m_Width, m_Height);
	}

	Window::~Window()
	{
		if (m_Window)
		{
			glfwDestroyWindow(m_Window);
			m_Window = nullptr;
		}
	}

	void Window::TerminateGLFW()
	{
		if (s_GLFWInitialized)
		{
			glfwTerminate();
			s_GLFWInitialized = false;
		}
	}

	void Window::SetupCallbacks()
	{
		glfwSetWindowUserPointer(m_Window, this);

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* glfwWindow, int width, int height)
		{
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
			win->m_Width = static_cast<uint32_t>(width);
			win->m_Height = static_cast<uint32_t>(height);

			if (win->m_EventCallback)
			{
				WindowResizeEvent e(width, height);
				win->m_EventCallback(e);
			}
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* glfwWindow)
		{
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
			if (win->m_EventCallback)
			{
				WindowCloseEvent e;
				win->m_EventCallback(e);
			}
		});

		glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* glfwWindow, int focused)
		{
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
			if (win->m_EventCallback)
			{
				if (focused)
				{
					WindowFocusEvent e;
					win->m_EventCallback(e);
				}
				else
				{
					WindowLostFocusEvent e;
					win->m_EventCallback(e);
				}
			}
		});

		glfwSetWindowPosCallback(m_Window, [](GLFWwindow* glfwWindow, int xpos, int ypos)
		{
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
			if (win->m_EventCallback)
			{
				WindowMovedEvent e(xpos, ypos);
				win->m_EventCallback(e);
			}
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* glfwWindow, int key, int scancode, int action, int mods)
		{
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
			if (win->m_EventCallback)
			{
				Input::KeyCode keycode = static_cast<Input::KeyCode>(key);
				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent e(keycode, false);
					win->m_EventCallback(e);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent e(keycode);
					win->m_EventCallback(e);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent e(keycode, true);
					win->m_EventCallback(e);
					break;
				}
				}
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* glfwWindow, unsigned int codepoint)
		{
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
			if (win->m_EventCallback)
			{
				KeyTypedEvent e(codepoint);
				win->m_EventCallback(e);
			}
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* glfwWindow, int button, int action, int mods)
		{
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
			if (win->m_EventCallback)
			{
				Input::MouseCode mouseButton = static_cast<Input::MouseCode>(button);
				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent e(mouseButton);
					win->m_EventCallback(e);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent e(mouseButton);
					win->m_EventCallback(e);
					break;
				}
				}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* glfwWindow, double xOffset, double yOffset)
		{
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
			if (win->m_EventCallback)
			{
				MouseScrolledEvent e(static_cast<float>(xOffset), static_cast<float>(yOffset));
				win->m_EventCallback(e);
			}
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* glfwWindow, double xpos, double ypos)
		{
			Window* win = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
			if (win->m_EventCallback)
			{
				MouseMovedEvent e(static_cast<float>(xpos), static_cast<float>(ypos));
				win->m_EventCallback(e);
			}
		});
	}

	void Window::Update()
	{
		glfwPollEvents();
	}

	bool Window::ShouldClose() const
	{
		return glfwWindowShouldClose(m_Window);
	}

	void Window::SetVSync(bool enabled)
	{
		m_VSync = enabled;
	}

	void Window::SetFullscreen(bool fullscreen)
	{
		if (m_Fullscreen == fullscreen)
			return;

		if (fullscreen)
		{
			// Store windowed position and size before going fullscreen
			int x, y;
			glfwGetWindowPos(m_Window, &x, &y);
			m_WindowedX = x;
			m_WindowedY = y;
			m_WindowedWidth = m_Width;
			m_WindowedHeight = m_Height;

			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			LUMINA_ASSERT(monitor, "Failed to get primary monitor");
			if (!monitor) return;

			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			LUMINA_ASSERT(mode, "Failed to get monitor video mode");

			glfwSetWindowMonitor(m_Window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		}
		else
		{
			glfwSetWindowMonitor(m_Window, nullptr,
				m_WindowedX, m_WindowedY,
				static_cast<int>(m_WindowedWidth), static_cast<int>(m_WindowedHeight), 0);
		}

		m_Fullscreen = fullscreen;
	}

	void Window::SetPosition(int32_t x, int32_t y)
	{
		glfwSetWindowPos(m_Window, x, y);
	}

	void Window::Maximize()
	{
		glfwMaximizeWindow(m_Window);
	}

	void Window::Minimize()
	{
		glfwIconifyWindow(m_Window);
	}

	void Window::Restore()
	{
		glfwRestoreWindow(m_Window);
	}

	void Window::Show()
	{
		glfwShowWindow(m_Window);
	}

	void Window::CenterOnMonitor()
	{
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		if (!monitor)
		{
			LUMINA_LOG_WARN("Failed to get primary monitor for centering");
			return;
		}

		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		if (!mode)
		{
			LUMINA_LOG_WARN("Failed to get video mode for centering");
			return;
		}

		int monitorX, monitorY;
		glfwGetMonitorPos(monitor, &monitorX, &monitorY);

		int windowWidth, windowHeight;
		glfwGetWindowSize(m_Window, &windowWidth, &windowHeight);

		int centerX = monitorX + (mode->width - windowWidth) / 2;
		int centerY = monitorY + (mode->height - windowHeight) / 2;

		glfwSetWindowPos(m_Window, centerX, centerY);
		m_WindowedX = centerX;
		m_WindowedY = centerY;
	}

	bool Window::IsMaximized() const
	{
		return glfwGetWindowAttrib(m_Window, GLFW_MAXIMIZED) == GLFW_TRUE;
	}

	bool Window::IsMinimized() const
	{
		return glfwGetWindowAttrib(m_Window, GLFW_ICONIFIED) == GLFW_TRUE;
	}

	void Window::SetIcon(const std::string& iconPath)
	{
		int width, height, channels;
		unsigned char* pixels = stbi_load(iconPath.c_str(), &width, &height, &channels, 4);

		if (!pixels)
		{
			LUMINA_LOG_ERROR("Failed to load window icon: {}", iconPath);
			return;
		}

		GLFWimage icon;
		icon.width = width;
		icon.height = height;
		icon.pixels = pixels;

		glfwSetWindowIcon(m_Window, 1, &icon);
		stbi_image_free(pixels);

		LUMINA_LOG_INFO("Window icon set: {} ({}x{})", iconPath, width, height);
	}

	void Window::SetDefaultIcon()
	{
		int width, height, channels;
		unsigned char* pixels = stbi_load_from_memory(
			DefaultIconData,
			static_cast<int>(DefaultIconSize),
			&width, &height, &channels, 4
		);

		if (!pixels)
		{
			LUMINA_LOG_WARN("Failed to load default window icon");
			return;
		}

		GLFWimage icon;
		icon.width = width;
		icon.height = height;
		icon.pixels = pixels;

		glfwSetWindowIcon(m_Window, 1, &icon);
		stbi_image_free(pixels);
	}

	void Window::SetTitlebarColor(uint8_t r, uint8_t g, uint8_t b)
	{
#ifdef LUMINA_PLATFORM_WINDOWS
		HWND hwnd = glfwGetWin32Window(m_Window);
		COLORREF color = RGB(r, g, b);
		HRESULT hr = DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &color, sizeof(color));
		if (FAILED(hr))
		{
			LUMINA_LOG_WARN("Failed to set titlebar color (requires Windows 11+)");
		}
#else
		LUMINA_LOG_WARN("Titlebar color customization is only supported on Windows");
		(void)r; (void)g; (void)b;
#endif
	}

	void Window::SetTitlebarTextColor(uint8_t r, uint8_t g, uint8_t b)
	{
#ifdef LUMINA_PLATFORM_WINDOWS
		HWND hwnd = glfwGetWin32Window(m_Window);
		COLORREF color = RGB(r, g, b);
		HRESULT hr = DwmSetWindowAttribute(hwnd, DWMWA_TEXT_COLOR, &color, sizeof(color));
		if (FAILED(hr))
		{
			LUMINA_LOG_WARN("Failed to set titlebar text color (requires Windows 11+)");
		}
#else
		LUMINA_LOG_WARN("Titlebar text color customization is only supported on Windows");
		(void)r; (void)g; (void)b;
#endif
	}
}
