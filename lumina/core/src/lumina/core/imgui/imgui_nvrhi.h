#pragma once

#include <nvrhi/nvrhi.h>

struct ImDrawData;

namespace Lumina
{
	class Device;
}

namespace Lumina::ImGuiNvrhi
{
	struct ImGuiNvrhiConfig
	{
		nvrhi::IDevice* Device = nullptr;
		nvrhi::Format RenderTargetFormat = nvrhi::Format::RGBA8_UNORM;
		uint32_t InitialVertexBufferSize = 5000;
		uint32_t InitialIndexBufferSize = 10000;
	};

	bool Init(const ImGuiNvrhiConfig& config);
	void Shutdown();

	void NewFrame();
	void RenderDrawData(nvrhi::ICommandList* commandList, nvrhi::IFramebuffer* framebuffer, ImDrawData* drawData);

	void InitPlatformViewports(Device& dev);
	void ShutdownPlatformViewports();
}
