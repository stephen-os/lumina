#include "StorageBuffer.h"

#include <Lumina/Core/Device.h>
#include <Lumina/Core/Assert.h>
#include <Lumina/Core/Log.h>

#include <string>
#include <utility>

namespace Lumina
{
    StorageBuffer::~StorageBuffer() = default;

    StorageBuffer::StorageBuffer(StorageBuffer&& other) noexcept
        : m_Device(other.m_Device)
        , m_Handle(std::move(other.m_Handle))
        , m_Size(other.m_Size)
        , m_Stride(other.m_Stride)
        , m_PendingData(std::move(other.m_PendingData))
    {
        other.m_Size = 0;
        other.m_Stride = 0;
    }

    Ref<StorageBuffer> StorageBuffer::Create(
        Device& dev,
        size_t size,
        size_t stride,
        std::string_view debugName)
    {
        auto* nvrhiDevice = dev.GetNvrhiDevice();
        if (!nvrhiDevice)
        {
            LUMINA_LOG_ERROR("Failed to create storage buffer: no device");
            return nullptr;
        }

        if (size == 0)
        {
            LUMINA_LOG_ERROR("Failed to create storage buffer: size cannot be zero");
            return nullptr;
        }

        if (stride == 0)
        {
            LUMINA_LOG_ERROR("Failed to create storage buffer: stride cannot be zero");
            return nullptr;
        }

        nvrhi::BufferDesc desc;
        desc.byteSize = size;
        desc.structStride = static_cast<uint32_t>(stride);
        desc.debugName = std::string(debugName);
        desc.initialState = nvrhi::ResourceStates::Common;
        desc.keepInitialState = true;
        desc.canHaveUAVs = true;

        nvrhi::BufferHandle buffer = nvrhiDevice->createBuffer(desc);
        if (!buffer)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI storage buffer");
            return nullptr;
        }

        return Ref<StorageBuffer>(new StorageBuffer(dev, std::move(buffer), size, stride));
    }

    void StorageBuffer::Update(const void* data, size_t size, nvrhi::ICommandList* cmdList)
    {
        LUMINA_ASSERT(data != nullptr, "Cannot update storage buffer with null data");
        LUMINA_ASSERT(cmdList != nullptr, "Command list is required for storage buffer update");
        LUMINA_ASSERT(m_Handle != nullptr, "Storage buffer handle is null");

        if (!data || !cmdList || !m_Handle)
        {
            LUMINA_LOG_ERROR("StorageBuffer::Update called with invalid state");
            return;
        }

        if (size > m_Size)
        {
            LUMINA_LOG_WARN("Update size {} exceeds buffer size {}", size, m_Size);
            size = m_Size;
        }

        cmdList->writeBuffer(m_Handle.Get(), data, size);
        // Let NVRHI handle state transitions based on binding usage (SRV or UAV)
    }

    void StorageBuffer::UploadPending(nvrhi::ICommandList* cmdList)
    {
        if (m_PendingData.empty())
        {
            return;
        }

        Update(m_PendingData.data(), m_PendingData.size(), cmdList);
        m_PendingData.clear();
        m_PendingData.shrink_to_fit();
    }
}
