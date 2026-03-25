#include "d3d12_device.h"

#ifdef LUMINA_PLATFORM_WINDOWS

#include "../../Log.h"
#include "../../Assert.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <nvrhi/validation.h>

namespace Lumina
{
    void D3D12MessageCallback::message(nvrhi::MessageSeverity severity, const char* messageText)
    {
        switch (severity)
        {
        case nvrhi::MessageSeverity::Info:
            LUMINA_LOG_INFO("[NVRHI] {}", messageText);
            break;
        case nvrhi::MessageSeverity::Warning:
            LUMINA_LOG_WARN("[NVRHI] {}", messageText);
            break;
        case nvrhi::MessageSeverity::Error:
            LUMINA_LOG_ERROR("[NVRHI] {}", messageText);
            break;
        case nvrhi::MessageSeverity::Fatal:
            LUMINA_LOG_CRITICAL("[NVRHI] {}", messageText);
            break;
        }
    }

    D3D12Device::~D3D12Device()
    {
        Shutdown();
    }

    bool D3D12Device::Init(const DeviceDesc& desc)
    {
        m_Window = desc.Window;
        m_Width = desc.Width;
        m_Height = desc.Height;
        m_BackbufferCount = desc.BackbufferCount;
        m_VSync = desc.VSync;

        if (!CreateDevice())
        {
            LUMINA_LOG_ERROR("Failed to create D3D12 device");
            return false;
        }

        if (!CreateSwapchain())
        {
            LUMINA_LOG_ERROR("Failed to create swapchain");
            return false;
        }

        // Create NVRHI device wrapper
        nvrhi::d3d12::DeviceDesc nvrhiDesc;
        nvrhiDesc.errorCB = &m_MessageCallback;
        nvrhiDesc.pDevice = m_Device.Get();
        nvrhiDesc.pGraphicsCommandQueue = m_CommandQueue.Get();

        m_NvrhiDevice = nvrhi::d3d12::createDevice(nvrhiDesc);
        if (!m_NvrhiDevice)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI device");
            return false;
        }

#ifdef LUMINA_DEBUG
        // Wrap with validation layer in debug builds
        nvrhi::DeviceHandle validationLayer = nvrhi::validation::createValidationLayer(m_NvrhiDevice);
        m_NvrhiDevice = validationLayer;
#endif

        // Create command list
        m_CommandList = m_NvrhiDevice->createCommandList();
        if (!m_CommandList)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI command list");
            return false;
        }

        if (!CreateFramebuffers())
        {
            LUMINA_LOG_ERROR("Failed to create framebuffers");
            return false;
        }

#ifdef TRACY_ENABLE
        m_TracyCtx = LUMINA_PROFILE_GPU_D3D12_CONTEXT(m_Device.Get(), m_CommandQueue.Get());
        LUMINA_PROFILE_GPU_D3D12_CONTEXT_NAME(m_TracyCtx, "D3D12 Main Queue");
#endif

        LUMINA_LOG_INFO("D3D12 graphics device initialized successfully");
        return true;
    }

    void D3D12Device::Shutdown()
    {
        if (m_NvrhiDevice)
        {
            WaitForGPU();
        }

#ifdef TRACY_ENABLE
        if (m_TracyCtx)
        {
            LUMINA_PROFILE_GPU_D3D12_DESTROY(m_TracyCtx);
            m_TracyCtx = nullptr;
        }
#endif

        DestroyFramebuffers();

        m_CommandList = nullptr;
        m_NvrhiDevice = nullptr;

        if (m_FenceEvent)
        {
            CloseHandle(m_FenceEvent);
            m_FenceEvent = nullptr;
        }

        m_Fence.Reset();
        m_Swapchain.Reset();
        m_CommandQueue.Reset();
        m_Device.Reset();
        m_Adapter.Reset();
        m_Factory.Reset();

        LUMINA_LOG_INFO("D3D12 graphics device shutdown");
    }

    bool D3D12Device::CreateDevice()
    {
        UINT dxgiFactoryFlags = 0;

#ifdef LUMINA_DEBUG
        // Enable debug layer
        ComPtr<ID3D12Debug> debugInterface;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface))))
        {
            debugInterface->EnableDebugLayer();
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
            LUMINA_LOG_INFO("D3D12 debug layer enabled");
        }
#endif

        // Create DXGI factory
        HRESULT hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_Factory));
        if (FAILED(hr))
        {
            LUMINA_LOG_ERROR("Failed to create DXGI factory");
            return false;
        }

        // Find a suitable adapter
        ComPtr<IDXGIAdapter1> adapter;
        for (UINT i = 0; m_Factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND; ++i)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            // Skip software adapter
            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                continue;

            // Try to create device
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
            {
                m_Adapter = adapter;

                char adapterName[256];
                WideCharToMultiByte(CP_UTF8, 0, desc.Description, -1, adapterName, sizeof(adapterName), nullptr, nullptr);
                LUMINA_LOG_INFO("Using GPU: {}", adapterName);
                break;
            }
        }

        if (!m_Adapter)
        {
            LUMINA_LOG_ERROR("No suitable GPU found");
            return false;
        }

        // Create the device
        hr = D3D12CreateDevice(m_Adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_Device));
        if (FAILED(hr))
        {
            LUMINA_LOG_ERROR("Failed to create D3D12 device");
            return false;
        }

        // Create command queue
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

        hr = m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue));
        if (FAILED(hr))
        {
            LUMINA_LOG_ERROR("Failed to create command queue");
            return false;
        }

        // Create fence for synchronization
        // Initialize fence values to 0 - these frames haven't been submitted yet
        m_FenceValues.resize(m_BackbufferCount, 0);
        m_CurrentFenceValue = 1;

        hr = m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
        if (FAILED(hr))
        {
            LUMINA_LOG_ERROR("Failed to create fence");
            return false;
        }

        m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (!m_FenceEvent)
        {
            LUMINA_LOG_ERROR("Failed to create fence event");
            return false;
        }

        return true;
    }

    bool D3D12Device::CreateSwapchain()
    {
        HWND hwnd = glfwGetWin32Window(m_Window);

        DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
        swapchainDesc.Width = m_Width;
        swapchainDesc.Height = m_Height;
        swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapchainDesc.Stereo = FALSE;
        swapchainDesc.SampleDesc.Count = 1;
        swapchainDesc.SampleDesc.Quality = 0;
        swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapchainDesc.BufferCount = m_BackbufferCount;
        swapchainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

        ComPtr<IDXGISwapChain1> swapchain1;
        HRESULT hr = m_Factory->CreateSwapChainForHwnd(
            m_CommandQueue.Get(),
            hwnd,
            &swapchainDesc,
            nullptr,
            nullptr,
            &swapchain1);

        if (FAILED(hr))
        {
            LUMINA_LOG_ERROR("Failed to create swapchain");
            return false;
        }

        // Disable Alt+Enter fullscreen toggle
        m_Factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

        hr = swapchain1.As(&m_Swapchain);
        if (FAILED(hr))
        {
            LUMINA_LOG_ERROR("Failed to get IDXGISwapChain4 interface");
            return false;
        }

        m_FrameIndex = m_Swapchain->GetCurrentBackBufferIndex();

        return true;
    }

    bool D3D12Device::CreateFramebuffers()
    {
        m_SwapchainTextures.resize(m_BackbufferCount);
        m_SwapchainFramebuffers.resize(m_BackbufferCount);

        for (UINT i = 0; i < m_BackbufferCount; ++i)
        {
            ComPtr<ID3D12Resource> backbuffer;
            HRESULT hr = m_Swapchain->GetBuffer(i, IID_PPV_ARGS(&backbuffer));
            if (FAILED(hr))
            {
                LUMINA_LOG_ERROR("Failed to get swapchain buffer {}", i);
                return false;
            }

            nvrhi::TextureDesc textureDesc;
            textureDesc.dimension = nvrhi::TextureDimension::Texture2D;
            textureDesc.format = nvrhi::Format::RGBA8_UNORM;
            textureDesc.width = m_Width;
            textureDesc.height = m_Height;
            textureDesc.isRenderTarget = true;
            textureDesc.debugName = "Swapchain Texture " + std::to_string(i);
            textureDesc.initialState = nvrhi::ResourceStates::Present;
            textureDesc.keepInitialState = true;

            m_SwapchainTextures[i] = m_NvrhiDevice->createHandleForNativeTexture(
                nvrhi::ObjectTypes::D3D12_Resource,
                nvrhi::Object(backbuffer.Get()),
                textureDesc);

            if (!m_SwapchainTextures[i])
            {
                LUMINA_LOG_ERROR("Failed to create NVRHI texture for swapchain buffer {}", i);
                return false;
            }

            nvrhi::FramebufferDesc fbDesc;
            fbDesc.addColorAttachment(m_SwapchainTextures[i]);

            m_SwapchainFramebuffers[i] = m_NvrhiDevice->createFramebuffer(fbDesc);
            if (!m_SwapchainFramebuffers[i])
            {
                LUMINA_LOG_ERROR("Failed to create framebuffer for swapchain buffer {}", i);
                return false;
            }
        }

        return true;
    }

    void D3D12Device::DestroyFramebuffers()
    {
        m_SwapchainFramebuffers.clear();
        m_SwapchainTextures.clear();
    }

    void D3D12Device::BeginFrame()
    {
        LUMINA_PROFILE_SCOPE_NC("D3D12::BeginFrame", Profiler::Colors::GPU);

        m_FrameIndex = m_Swapchain->GetCurrentBackBufferIndex();

        // Wait for the previous frame to complete
        if (m_Fence->GetCompletedValue() < m_FenceValues[m_FrameIndex])
        {
            m_Fence->SetEventOnCompletion(m_FenceValues[m_FrameIndex], m_FenceEvent);
            WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);
        }

#ifdef TRACY_ENABLE
        LUMINA_PROFILE_GPU_D3D12_COLLECT(m_TracyCtx);
#endif

        // Run garbage collection to free staging buffers from previous frames
        // This must be called after GPU sync to safely release resources
        m_NvrhiDevice->runGarbageCollection();

        m_CommandList->open();
    }

    void D3D12Device::Present()
    {
        LUMINA_PROFILE_SCOPE_NC("D3D12::Present", Profiler::Colors::GPU);

        m_CommandList->close();
        m_NvrhiDevice->executeCommandList(m_CommandList);

        // Present
        UINT syncInterval = m_VSync ? 1 : 0;
        UINT presentFlags = m_VSync ? 0 : DXGI_PRESENT_ALLOW_TEARING;
        m_Swapchain->Present(syncInterval, presentFlags);

        // Signal and advance
        MoveToNextFrame();
    }

    void D3D12Device::Resize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0)
            return;

        if (width == m_Width && height == m_Height)
            return;

        WaitForGPU();
        DestroyFramebuffers();

        m_Width = width;
        m_Height = height;

        DXGI_SWAP_CHAIN_DESC desc;
        m_Swapchain->GetDesc(&desc);
        m_Swapchain->ResizeBuffers(m_BackbufferCount, width, height, desc.BufferDesc.Format, desc.Flags);

        m_FrameIndex = m_Swapchain->GetCurrentBackBufferIndex();
        CreateFramebuffers();

        LUMINA_LOG_INFO("Swapchain resized to {}x{}", width, height);
    }

    nvrhi::IFramebuffer* D3D12Device::GetCurrentFramebuffer() const
    {
        return m_SwapchainFramebuffers[m_FrameIndex].Get();
    }

    void D3D12Device::WaitForGPU()
    {
        // Schedule a signal command with the next fence value
        const uint64_t fenceValue = m_CurrentFenceValue;
        m_CommandQueue->Signal(m_Fence.Get(), fenceValue);
        m_CurrentFenceValue++;

        // Wait for fence
        if (m_Fence->GetCompletedValue() < fenceValue)
        {
            m_Fence->SetEventOnCompletion(fenceValue, m_FenceEvent);
            WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);
        }

        // Reset all frame fence values since we've flushed everything
        for (auto& value : m_FenceValues)
        {
            value = m_Fence->GetCompletedValue();
        }
    }

    void D3D12Device::MoveToNextFrame()
    {
        // Signal the fence with the current fence value for this frame
        const uint64_t fenceValueForSignal = m_CurrentFenceValue;
        m_CommandQueue->Signal(m_Fence.Get(), fenceValueForSignal);

        // Store the fence value for this frame so we can wait on it later
        m_FenceValues[m_FrameIndex] = fenceValueForSignal;

        // Increment the fence value for the next signal
        m_CurrentFenceValue++;

        // Get the next frame index
        m_FrameIndex = m_Swapchain->GetCurrentBackBufferIndex();
    }
}

#endif // LUMINA_PLATFORM_WINDOWS
