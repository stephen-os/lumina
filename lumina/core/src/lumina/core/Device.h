#pragma once

#include "Base.h"

#include <nvrhi/nvrhi.h>

#include <cstdint>
#include <string>

struct GLFWwindow;

namespace Lumina
{
	enum class GraphicsAPI
	{
		D3D12,
		Vulkan
	};

	struct DeviceDesc
	{
		GLFWwindow* Window = nullptr;
		uint32_t Width = 1600;
		uint32_t Height = 900;
		uint32_t BackbufferCount = 2;
		bool VSync = true;
		bool EnableDebugLayer = false;
		std::string AppName = "Lumina Application";
	};

	// Native handle structs for viewport support (void* to avoid leaking API headers)
	struct VulkanNativeHandles
	{
		void* Instance = nullptr;           // VkInstance
		void* PhysicalDevice = nullptr;     // VkPhysicalDevice
		void* Device = nullptr;             // VkDevice
		void* GraphicsQueue = nullptr;      // VkQueue
		uint32_t GraphicsQueueFamily = 0;
	};

	struct D3D12NativeHandles
	{
		void* Device = nullptr;             // ID3D12Device*
		void* CommandQueue = nullptr;       // ID3D12CommandQueue*
		void* DXGIFactory = nullptr;        // IDXGIFactory6*
	};

	class Device
	{
	public:
		virtual ~Device() = default;

		[[nodiscard]] virtual bool Init(const DeviceDesc& desc) = 0;
		virtual void Shutdown() = 0;

		virtual void BeginFrame() = 0;
		virtual void Present() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		[[nodiscard]] virtual nvrhi::IDevice* GetNvrhiDevice() const = 0;
		[[nodiscard]] virtual nvrhi::ICommandList* GetCommandList() const = 0;
		[[nodiscard]] virtual nvrhi::IFramebuffer* GetCurrentFramebuffer() const = 0;

		[[nodiscard]] virtual uint32_t GetWidth() const = 0;
		[[nodiscard]] virtual uint32_t GetHeight() const = 0;
		[[nodiscard]] virtual uint32_t GetFrameIndex() const = 0;

		[[nodiscard]] virtual GraphicsAPI GetAPI() const = 0;
		[[nodiscard]] virtual nvrhi::Format GetSwapchainFormat() const = 0;

		[[nodiscard]] virtual VulkanNativeHandles GetVulkanHandles() const { return {}; }
		[[nodiscard]] virtual D3D12NativeHandles GetD3D12Handles() const { return {}; }

		[[nodiscard]] static Scope<Device> Create(GraphicsAPI api);
	};
}
