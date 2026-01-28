#pragma once

#include "../../graphics_device.h"

#ifdef LUMINA_PLATFORM_WINDOWS

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <nvrhi/d3d12.h>

#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl/client.h>

#include <vector>

namespace lumina::core::platform::d3d12
{
    using Microsoft::WRL::ComPtr;

    class d3d12_message_callback : public nvrhi::IMessageCallback
    {
    public:
        void message(nvrhi::MessageSeverity severity, const char* message_text) override;
    };

    class d3d12_device : public graphics_device
    {
    public:
        d3d12_device() = default;
        ~d3d12_device() override;

        bool init(const graphics_device_desc& desc) override;
        void shutdown() override;

        void begin_frame() override;
        void present() override;

        void resize(uint32_t width, uint32_t height) override;

        nvrhi::IDevice* get_device() const override { return m_nvrhi_device.Get(); }
        nvrhi::ICommandList* get_command_list() const override { return m_command_list.Get(); }
        nvrhi::IFramebuffer* get_current_framebuffer() const override;

        uint32_t get_width() const override { return m_width; }
        uint32_t get_height() const override { return m_height; }
        uint32_t get_frame_index() const override { return m_frame_index; }

        graphics_api get_api() const override { return graphics_api::d3d12; }
        nvrhi::Format get_swapchain_format() const override { return nvrhi::Format::RGBA8_UNORM; }

        d3d12_native_handles get_d3d12_handles() const override
        {
            return { m_device.Get(), m_command_queue.Get(), m_factory.Get() };
        }

    private:
        bool create_device();
        bool create_swapchain();
        bool create_framebuffers();
        void destroy_framebuffers();

        void wait_for_gpu();
        void move_to_next_frame();

        GLFWwindow* m_window = nullptr;
        uint32_t m_width = 0;
        uint32_t m_height = 0;
        uint32_t m_backbuffer_count = 2;
        bool m_vsync = true;

        // D3D12 objects
        ComPtr<IDXGIFactory6> m_factory;
        ComPtr<IDXGIAdapter1> m_adapter;
        ComPtr<ID3D12Device> m_device;
        ComPtr<ID3D12CommandQueue> m_command_queue;
        ComPtr<IDXGISwapChain4> m_swapchain;

        // Synchronization
        ComPtr<ID3D12Fence> m_fence;
        std::vector<uint64_t> m_fence_values;
        uint64_t m_current_fence_value = 0;
        HANDLE m_fence_event = nullptr;
        uint32_t m_frame_index = 0;

        // NVRHI objects
        d3d12_message_callback m_message_callback;
        nvrhi::DeviceHandle m_nvrhi_device;
        nvrhi::CommandListHandle m_command_list;

        // Swapchain textures and framebuffers
        std::vector<nvrhi::TextureHandle> m_swapchain_textures;
        std::vector<nvrhi::FramebufferHandle> m_swapchain_framebuffers;
    };
}

#endif // LUMINA_PLATFORM_WINDOWS
