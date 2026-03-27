#include "IndexBuffer.h"

#include <Lumina/Core/Device.h>
#include <Lumina/Core/Log.h>

#include <string>
#include <utility>

namespace Lumina
{
    IndexBuffer::~IndexBuffer() = default;

    IndexBuffer::IndexBuffer(IndexBuffer&& other) noexcept
        : m_Device(other.m_Device)
        , m_Handle(std::move(other.m_Handle))
        , m_Count(other.m_Count)
        , m_IndexSize(other.m_IndexSize)
    {
        other.m_Count = 0;
    }

    Ref<IndexBuffer> IndexBuffer::Create(
        Device& dev,
        const void* data,
        size_t count,
        size_t indexSize,
        std::string_view debugName)
    {
        auto* nvrhiDevice = dev.GetNvrhiDevice();
        if (!nvrhiDevice)
        {
            LUMINA_LOG_ERROR("Failed to create index buffer: no device");
            return nullptr;
        }

        if (indexSize != 2 && indexSize != 4)
        {
            LUMINA_LOG_ERROR("Failed to create index buffer: indexSize must be 2 (uint16) or 4 (uint32)");
            return nullptr;
        }

        if (count == 0)
        {
            LUMINA_LOG_ERROR("Failed to create index buffer: count cannot be zero");
            return nullptr;
        }

        size_t byteSize = count * indexSize;

        nvrhi::BufferDesc desc;
        desc.byteSize = byteSize;
        desc.isIndexBuffer = true;
        desc.debugName = std::string(debugName);
        desc.initialState = nvrhi::ResourceStates::IndexBuffer;
        desc.keepInitialState = true;

        nvrhi::BufferHandle buffer = nvrhiDevice->createBuffer(desc);
        if (!buffer)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI index buffer");
            return nullptr;
        }

        // Upload initial data if provided
        if (data)
        {
            nvrhi::CommandListHandle cmd = nvrhiDevice->createCommandList();
            cmd->open();
            cmd->writeBuffer(buffer, data, byteSize);
            cmd->close();
            nvrhiDevice->executeCommandList(cmd);
            nvrhiDevice->waitForIdle();
        }

        return Ref<IndexBuffer>(new IndexBuffer(dev, std::move(buffer), count, indexSize));
    }
}
