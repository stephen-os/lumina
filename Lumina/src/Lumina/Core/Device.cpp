#include "Device.h"
#include "Log.h"

#ifdef LUMINA_PLATFORM_WINDOWS
#include "platform/d3d12/D3D12Device.h"
#endif

#include "platform/vulkan/VulkanDevice.h"

namespace Lumina
{
	Scope<Device> Device::Create(GraphicsAPI api)
	{
		switch (api)
		{
		case GraphicsAPI::D3D12:
#ifdef LUMINA_PLATFORM_WINDOWS
			return MakeScope<D3D12Device>();
#else
			LUMINA_LOG_ERROR("D3D12 is only supported on Windows");
			return nullptr;
#endif

		case GraphicsAPI::Vulkan:
			return MakeScope<VulkanDevice>();

		default:
			LUMINA_LOG_ERROR("Unknown graphics API");
			return nullptr;
		}
	}
}
