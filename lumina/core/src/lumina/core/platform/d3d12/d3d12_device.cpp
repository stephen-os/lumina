#include "d3d12_device.h"

#ifdef LUMINA_PLATFORM_WINDOWS

#include "../../log.h"
#include "../../assert.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <nvrhi/validation.h>

namespace lumina::core::platform::d3d12
{
    void d3d12_message_callback::message(nvrhi::MessageSeverity severity, const char* message_text)
    {
        switch (severity)
        {
        case nvrhi::MessageSeverity::Info:
            LUMINA_LOG_INFO("[NVRHI] {}", message_text);
            break;
        case nvrhi::MessageSeverity::Warning:
            LUMINA_LOG_WARN("[NVRHI] {}", message_text);
            break;
        case nvrhi::MessageSeverity::Error:
            LUMINA_LOG_ERROR("[NVRHI] {}", message_text);
            break;
        case nvrhi::MessageSeverity::Fatal:
            LUMINA_LOG_CRITICAL("[NVRHI] {}", message_text);
            break;
        }
    }

    d3d12_device::~d3d12_device()
    {
        shutdown();
    }

    bool d3d12_device::init(const device_desc& desc)
    {
        m_window = desc.window;
        m_width = desc.width;
        m_height = desc.height;
        m_backbuffer_count = desc.backbuffer_count;
        m_vsync = desc.vsync;

        if (!create_device())
        {
            LUMINA_LOG_ERROR("Failed to create D3D12 device");
            return false;
        }

        if (!create_swapchain())
        {
            LUMINA_LOG_ERROR("Failed to create swapchain");
            return false;
        }

        // Create NVRHI device wrapper
        nvrhi::d3d12::DeviceDesc nvrhi_desc;
        nvrhi_desc.errorCB = &m_message_callback;
        nvrhi_desc.pDevice = m_device.Get();
        nvrhi_desc.pGraphicsCommandQueue = m_command_queue.Get();

        m_nvrhi_device = nvrhi::d3d12::createDevice(nvrhi_desc);
        if (!m_nvrhi_device)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI device");
            return false;
        }

#ifdef LUMINA_DEBUG
        // Wrap with validation layer in debug builds
        nvrhi::DeviceHandle validation_layer = nvrhi::validation::createValidationLayer(m_nvrhi_device);
        m_nvrhi_device = validation_layer;
#endif

        // Create command list
        m_command_list = m_nvrhi_device->createCommandList();
        if (!m_command_list)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI command list");
            return false;
        }

        if (!create_framebuffers())
        {
            LUMINA_LOG_ERROR("Failed to create framebuffers");
            return false;
        }

        LUMINA_LOG_INFO("D3D12 graphics device initialized successfully");
        return true;
    }

    void d3d12_device::shutdown()
    {
        if (m_nvrhi_device)
        {
            wait_for_gpu();
        }

        destroy_framebuffers();

        m_command_list = nullptr;
        m_nvrhi_device = nullptr;

        if (m_fence_event)
        {
            CloseHandle(m_fence_event);
            m_fence_event = nullptr;
        }

        m_fence.Reset();
        m_swapchain.Reset();
        m_command_queue.Reset();
        m_device.Reset();
        m_adapter.Reset();
        m_factory.Reset();

        LUMINA_LOG_INFO("D3D12 graphics device shutdown");
    }

    bool d3d12_device::create_device()
    {
        UINT dxgi_factory_flags = 0;

#ifdef LUMINA_DEBUG
        // Enable debug layer
        ComPtr<ID3D12Debug> debug_interface;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_interface))))
        {
            debug_interface->EnableDebugLayer();
            dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
            LUMINA_LOG_INFO("D3D12 debug layer enabled");
        }
#endif

        // Create DXGI factory
        HRESULT hr = CreateDXGIFactory2(dxgi_factory_flags, IID_PPV_ARGS(&m_factory));
        if (FAILED(hr))
        {
            LUMINA_LOG_ERROR("Failed to create DXGI factory");
            return false;
        }

        // Find a suitable adapter
        ComPtr<IDXGIAdapter1> adapter;
        for (UINT i = 0; m_factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND; ++i)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            // Skip software adapter
            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                continue;

            // Try to create device
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
            {
                m_adapter = adapter;

                char adapter_name[256];
                WideCharToMultiByte(CP_UTF8, 0, desc.Description, -1, adapter_name, sizeof(adapter_name), nullptr, nullptr);
                LUMINA_LOG_INFO("Using GPU: {}", adapter_name);
                break;
            }
        }

        if (!m_adapter)
        {
            LUMINA_LOG_ERROR("No suitable GPU found");
            return false;
        }

        // Create the device
        hr = D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device));
        if (FAILED(hr))
        {
            LUMINA_LOG_ERROR("Failed to create D3D12 device");
            return false;
        }

        // Create command queue
        D3D12_COMMAND_QUEUE_DESC queue_desc = {};
        queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

        hr = m_device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&m_command_queue));
        if (FAILED(hr))
        {
            LUMINA_LOG_ERROR("Failed to create command queue");
            return false;
        }

        // Create fence for synchronization
        // Initialize fence values to 0 - these frames haven't been submitted yet
        m_fence_values.resize(m_backbuffer_count, 0);
        m_current_fence_value = 1;

        hr = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
        if (FAILED(hr))
        {
            LUMINA_LOG_ERROR("Failed to create fence");
            return false;
        }

        m_fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (!m_fence_event)
        {
            LUMINA_LOG_ERROR("Failed to create fence event");
            return false;
        }

        return true;
    }

    bool d3d12_device::create_swapchain()
    {
        HWND hwnd = glfwGetWin32Window(m_window);

        DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {};
        swapchain_desc.Width = m_width;
        swapchain_desc.Height = m_height;
        swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapchain_desc.Stereo = FALSE;
        swapchain_desc.SampleDesc.Count = 1;
        swapchain_desc.SampleDesc.Quality = 0;
        swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapchain_desc.BufferCount = m_backbuffer_count;
        swapchain_desc.Scaling = DXGI_SCALING_STRETCH;
        swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapchain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        swapchain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

        ComPtr<IDXGISwapChain1> swapchain1;
        HRESULT hr = m_factory->CreateSwapChainForHwnd(
            m_command_queue.Get(),
            hwnd,
            &swapchain_desc,
            nullptr,
            nullptr,
            &swapchain1);

        if (FAILED(hr))
        {
            LUMINA_LOG_ERROR("Failed to create swapchain");
            return false;
        }

        // Disable Alt+Enter fullscreen toggle
        m_factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

        hr = swapchain1.As(&m_swapchain);
        if (FAILED(hr))
        {
            LUMINA_LOG_ERROR("Failed to get IDXGISwapChain4 interface");
            return false;
        }

        m_frame_index = m_swapchain->GetCurrentBackBufferIndex();

        return true;
    }

    bool d3d12_device::create_framebuffers()
    {
        m_swapchain_textures.resize(m_backbuffer_count);
        m_swapchain_framebuffers.resize(m_backbuffer_count);

        for (UINT i = 0; i < m_backbuffer_count; ++i)
        {
            ComPtr<ID3D12Resource> backbuffer;
            HRESULT hr = m_swapchain->GetBuffer(i, IID_PPV_ARGS(&backbuffer));
            if (FAILED(hr))
            {
                LUMINA_LOG_ERROR("Failed to get swapchain buffer {}", i);
                return false;
            }

            nvrhi::TextureDesc texture_desc;
            texture_desc.dimension = nvrhi::TextureDimension::Texture2D;
            texture_desc.format = nvrhi::Format::RGBA8_UNORM;
            texture_desc.width = m_width;
            texture_desc.height = m_height;
            texture_desc.isRenderTarget = true;
            texture_desc.debugName = "Swapchain Texture " + std::to_string(i);
            texture_desc.initialState = nvrhi::ResourceStates::Present;
            texture_desc.keepInitialState = true;

            m_swapchain_textures[i] = m_nvrhi_device->createHandleForNativeTexture(
                nvrhi::ObjectTypes::D3D12_Resource,
                nvrhi::Object(backbuffer.Get()),
                texture_desc);

            if (!m_swapchain_textures[i])
            {
                LUMINA_LOG_ERROR("Failed to create NVRHI texture for swapchain buffer {}", i);
                return false;
            }

            nvrhi::FramebufferDesc fb_desc;
            fb_desc.addColorAttachment(m_swapchain_textures[i]);

            m_swapchain_framebuffers[i] = m_nvrhi_device->createFramebuffer(fb_desc);
            if (!m_swapchain_framebuffers[i])
            {
                LUMINA_LOG_ERROR("Failed to create framebuffer for swapchain buffer {}", i);
                return false;
            }
        }

        return true;
    }

    void d3d12_device::destroy_framebuffers()
    {
        m_swapchain_framebuffers.clear();
        m_swapchain_textures.clear();
    }

    void d3d12_device::begin_frame()
    {
        m_frame_index = m_swapchain->GetCurrentBackBufferIndex();

        // Wait for the previous frame to complete
        if (m_fence->GetCompletedValue() < m_fence_values[m_frame_index])
        {
            m_fence->SetEventOnCompletion(m_fence_values[m_frame_index], m_fence_event);
            WaitForSingleObjectEx(m_fence_event, INFINITE, FALSE);
        }

        m_command_list->open();
    }

    void d3d12_device::present()
    {
        m_command_list->close();
        m_nvrhi_device->executeCommandList(m_command_list);

        // Present
        UINT sync_interval = m_vsync ? 1 : 0;
        UINT present_flags = m_vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING;
        m_swapchain->Present(sync_interval, present_flags);

        // Signal and advance
        move_to_next_frame();
    }

    void d3d12_device::resize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0)
            return;

        if (width == m_width && height == m_height)
            return;

        wait_for_gpu();
        destroy_framebuffers();

        m_width = width;
        m_height = height;

        DXGI_SWAP_CHAIN_DESC desc;
        m_swapchain->GetDesc(&desc);
        m_swapchain->ResizeBuffers(m_backbuffer_count, width, height, desc.BufferDesc.Format, desc.Flags);

        m_frame_index = m_swapchain->GetCurrentBackBufferIndex();
        create_framebuffers();

        LUMINA_LOG_INFO("Swapchain resized to {}x{}", width, height);
    }

    nvrhi::IFramebuffer* d3d12_device::get_current_framebuffer() const
    {
        return m_swapchain_framebuffers[m_frame_index].Get();
    }

    void d3d12_device::wait_for_gpu()
    {
        // Schedule a signal command with the next fence value
        const uint64_t fence_value = m_current_fence_value;
        m_command_queue->Signal(m_fence.Get(), fence_value);
        m_current_fence_value++;

        // Wait for fence
        if (m_fence->GetCompletedValue() < fence_value)
        {
            m_fence->SetEventOnCompletion(fence_value, m_fence_event);
            WaitForSingleObjectEx(m_fence_event, INFINITE, FALSE);
        }

        // Reset all frame fence values since we've flushed everything
        for (auto& value : m_fence_values)
        {
            value = m_fence->GetCompletedValue();
        }
    }

    void d3d12_device::move_to_next_frame()
    {
        // Signal the fence with the current fence value for this frame
        const uint64_t fence_value_for_signal = m_current_fence_value;
        m_command_queue->Signal(m_fence.Get(), fence_value_for_signal);

        // Store the fence value for this frame so we can wait on it later
        m_fence_values[m_frame_index] = fence_value_for_signal;

        // Increment the fence value for the next signal
        m_current_fence_value++;

        // Get the next frame index
        m_frame_index = m_swapchain->GetCurrentBackBufferIndex();
    }
}

#endif // LUMINA_PLATFORM_WINDOWS
