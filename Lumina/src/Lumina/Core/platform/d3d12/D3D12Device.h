#pragma once

#include "../../Device.h"

#ifdef LUMINA_PLATFORM_WINDOWS

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <nvrhi/d3d12.h>

#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl/client.h>

// Include profiler and Tracy D3D12 GPU header
#include "../../Profiler.h"
#ifdef TRACY_ENABLE
    #include <tracy/TracyD3D12.hpp>
#endif

#include <vector>

namespace Lumina
{
    using Microsoft::WRL::ComPtr;

    class D3D12MessageCallback : public nvrhi::IMessageCallback
    {
    public:
        void message(nvrhi::MessageSeverity severity, const char* messageText) override;
    };

    class D3D12Device : public Device
    {
    public:
        D3D12Device() = default;
        ~D3D12Device() override;

        bool Init(const DeviceDesc& desc) override;
        void Shutdown() override;

        void BeginFrame() override;
        void Present() override;

        void Resize(uint32_t width, uint32_t height) override;

        nvrhi::IDevice* GetNvrhiDevice() const override { return m_NvrhiDevice.Get(); }
        nvrhi::ICommandList* GetCommandList() const override { return m_CommandList.Get(); }
        nvrhi::IFramebuffer* GetCurrentFramebuffer() const override;

        uint32_t GetWidth() const override { return m_Width; }
        uint32_t GetHeight() const override { return m_Height; }
        uint32_t GetFrameIndex() const override { return m_FrameIndex; }

        GraphicsAPI GetAPI() const override { return GraphicsAPI::D3D12; }
        nvrhi::Format GetSwapchainFormat() const override { return nvrhi::Format::RGBA8_UNORM; }

        D3D12NativeHandles GetD3D12Handles() const override
        {
            return { m_Device.Get(), m_CommandQueue.Get(), m_Factory.Get() };
        }

    private:
        bool CreateDevice();
        bool CreateSwapchain();
        bool CreateFramebuffers();
        void DestroyFramebuffers();

        void WaitForGPU();
        void MoveToNextFrame();

        GLFWwindow* m_Window = nullptr;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        uint32_t m_BackbufferCount = 2;
        bool m_VSync = true;

        // D3D12 objects
        ComPtr<IDXGIFactory6> m_Factory;
        ComPtr<IDXGIAdapter1> m_Adapter;
        ComPtr<ID3D12Device> m_Device;
        ComPtr<ID3D12CommandQueue> m_CommandQueue;
        ComPtr<IDXGISwapChain4> m_Swapchain;

        // Synchronization
        ComPtr<ID3D12Fence> m_Fence;
        std::vector<uint64_t> m_FenceValues;
        uint64_t m_CurrentFenceValue = 0;
        HANDLE m_FenceEvent = nullptr;
        uint32_t m_FrameIndex = 0;

        // NVRHI objects
        D3D12MessageCallback m_MessageCallback;
        nvrhi::DeviceHandle m_NvrhiDevice;
        nvrhi::CommandListHandle m_CommandList;

        // Swapchain textures and framebuffers
        std::vector<nvrhi::TextureHandle> m_SwapchainTextures;
        std::vector<nvrhi::FramebufferHandle> m_SwapchainFramebuffers;

#ifdef TRACY_ENABLE
        TracyD3D12Ctx m_TracyCtx = nullptr;
#endif
    };
}

#endif // LUMINA_PLATFORM_WINDOWS
