#include "index_buffer.h"
#include "device.h"

#include <lumina/core/log.h>

#include <nvrhi/nvrhi.h>

namespace lumina::graphics
{
    index_buffer::~index_buffer()
    {
        if (m_handle)
        {
            m_handle->Release();
            m_handle = nullptr;
        }
    }

    index_buffer::index_buffer(index_buffer&& other) noexcept
        : m_device(other.m_device)
        , m_handle(other.m_handle)
        , m_count(other.m_count)
        , m_index_size(other.m_index_size)
    {
        other.m_handle = nullptr;
        other.m_count = 0;
    }

    ref<index_buffer> index_buffer::create(device& dev, const void* data, size_t count, size_t index_size)
    {
        auto* nvrhi_device = dev.get_nvrhi_device();
        if (!nvrhi_device)
        {
            LUMINA_LOG_ERROR("Failed to create index buffer: no device");
            return nullptr;
        }

        if (index_size != 2 && index_size != 4)
        {
            LUMINA_LOG_ERROR("Index size must be 2 (uint16) or 4 (uint32)");
            return nullptr;
        }

        size_t byte_size = count * index_size;

        nvrhi::BufferDesc desc;
        desc.byteSize = byte_size;
        desc.isIndexBuffer = true;
        desc.debugName = "Lumina Index Buffer";
        desc.initialState = nvrhi::ResourceStates::IndexBuffer;
        desc.keepInitialState = true;

        nvrhi::BufferHandle buffer = nvrhi_device->createBuffer(desc);
        if (!buffer)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI index buffer");
            return nullptr;
        }

        if (data)
        {
            nvrhi::CommandListHandle cmd = nvrhi_device->createCommandList();
            cmd->open();
            cmd->writeBuffer(buffer, data, byte_size);
            cmd->close();
            nvrhi_device->executeCommandList(cmd);
            nvrhi_device->waitForIdle();
        }

        buffer->AddRef();

        return ref<index_buffer>(new index_buffer(dev, buffer.Get(), count, index_size));
    }
}
