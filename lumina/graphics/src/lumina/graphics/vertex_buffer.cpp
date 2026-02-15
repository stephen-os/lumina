#include "vertex_buffer.h"

#include <lumina/core/device.h>

#include <lumina/core/assert.h>
#include <lumina/core/log.h>

#include <nvrhi/nvrhi.h>

namespace lumina::graphics
{
    vertex_buffer::~vertex_buffer()
    {
        if (m_handle)
        {
            m_handle->Release();
            m_handle = nullptr;
        }
    }

    vertex_buffer::vertex_buffer(vertex_buffer&& other) noexcept
        : m_device(other.m_device)
        , m_handle(other.m_handle)
        , m_size(other.m_size)
        , m_stride(other.m_stride)
        , m_usage(other.m_usage)
    {
        other.m_handle = nullptr;
        other.m_size = 0;
        other.m_stride = 0;
    }

    ref<vertex_buffer> vertex_buffer::create(core::device& dev, const void* data, size_t size, size_t stride, buffer_usage usage)
    {
        auto* nvrhi_device = dev.get_nvrhi_device();
        if (!nvrhi_device)
        {
            LUMINA_LOG_ERROR("Failed to create vertex buffer: no device");
            return nullptr;
        }

        nvrhi::BufferDesc desc;
        desc.byteSize = size;
        desc.isVertexBuffer = true;
        desc.debugName = "Lumina Vertex Buffer";

        if (usage == buffer_usage::dynamic)
        {
            desc.cpuAccess = nvrhi::CpuAccessMode::Write;
            desc.initialState = nvrhi::ResourceStates::CopyDest;
            desc.keepInitialState = true;
        }
        else
        {
            desc.cpuAccess = nvrhi::CpuAccessMode::None;
            desc.initialState = nvrhi::ResourceStates::VertexBuffer;
            desc.keepInitialState = true;
        }

        nvrhi::BufferHandle buffer = nvrhi_device->createBuffer(desc);
        if (!buffer)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI vertex buffer");
            return nullptr;
        }

        if (data)
        {
            if (usage == buffer_usage::immutable)
            {
                nvrhi::CommandListHandle cmd = nvrhi_device->createCommandList();
                cmd->open();
                cmd->writeBuffer(buffer, data, size);
                cmd->close();
                nvrhi_device->executeCommandList(cmd);
                nvrhi_device->waitForIdle();
            }
            else
            {
                void* mapped = nvrhi_device->mapBuffer(buffer, nvrhi::CpuAccessMode::Write);
                if (mapped)
                {
                    std::memcpy(mapped, data, size);
                    nvrhi_device->unmapBuffer(buffer);
                }
                else
                {
                    LUMINA_LOG_ERROR("Failed to map dynamic vertex buffer for initial data upload");
                }
            }
        }

        buffer->AddRef();

        return ref<vertex_buffer>(new vertex_buffer(dev, buffer.Get(), size, stride, usage));
    }

    void vertex_buffer::update(const void* data, size_t size)
    {
        LUMINA_ASSERT(data != nullptr, "Cannot update vertex buffer with null data");
        LUMINA_ASSERT(m_handle != nullptr, "Vertex buffer handle is null");

        if (!data || !m_handle)
        {
            LUMINA_LOG_ERROR("vertex_buffer::update called with invalid state");
            return;
        }

        if (!is_dynamic())
        {
            LUMINA_LOG_WARN("Cannot update immutable vertex buffer");
            return;
        }

        if (size > m_size)
        {
            LUMINA_LOG_WARN("Update size {} exceeds buffer size {}", size, m_size);
            size = m_size;
        }

        auto* nvrhi_device = m_device.get_nvrhi_device();

        void* mapped = nvrhi_device->mapBuffer(m_handle, nvrhi::CpuAccessMode::Write);
        if (!mapped)
        {
            LUMINA_LOG_ERROR("Failed to map vertex buffer for update");
            return;
        }

        std::memcpy(mapped, data, size);
        nvrhi_device->unmapBuffer(m_handle);
    }

    void vertex_buffer::update(const void* data, size_t size, nvrhi::ICommandList* cmd)
    {
        LUMINA_ASSERT(data != nullptr, "Cannot update vertex buffer with null data");
        LUMINA_ASSERT(m_handle != nullptr, "Vertex buffer handle is null");

        if (!data || !m_handle)
        {
            LUMINA_LOG_ERROR("vertex_buffer::update called with invalid state");
            return;
        }

        if (size > m_size)
        {
            LUMINA_LOG_WARN("Update size {} exceeds buffer size {}", size, m_size);
            size = m_size;
        }

        if (is_dynamic())
        {
            // Dynamic buffers use CPU mapping
            auto* nvrhi_device = m_device.get_nvrhi_device();
            void* mapped = nvrhi_device->mapBuffer(m_handle, nvrhi::CpuAccessMode::Write);
            if (mapped)
            {
                std::memcpy(mapped, data, size);
                nvrhi_device->unmapBuffer(m_handle);
            }
            else
            {
                LUMINA_LOG_ERROR("Failed to map vertex buffer for update");
            }
        }
        else
        {
            // Immutable buffers use writeBuffer through command list
            if (!cmd)
            {
                LUMINA_LOG_ERROR("Command list required for update on non-dynamic buffer");
                return;
            }
            cmd->writeBuffer(m_handle, data, size);
        }
    }

    void vertex_buffer::update_at_offset(const void* data, size_t size, size_t offset_bytes, nvrhi::ICommandList* cmd)
    {
        LUMINA_ASSERT(data != nullptr, "Cannot update vertex buffer with null data");
        LUMINA_ASSERT(m_handle != nullptr, "Vertex buffer handle is null");

        if (!data || !m_handle)
        {
            LUMINA_LOG_ERROR("vertex_buffer::update_at_offset called with invalid state");
            return;
        }

        if (offset_bytes + size > m_size)
        {
            LUMINA_LOG_WARN("Update at offset {} with size {} exceeds buffer size {}", offset_bytes, size, m_size);
            if (offset_bytes >= m_size)
                return;
            size = m_size - offset_bytes;
        }

        if (is_dynamic())
        {
            // Dynamic buffers use CPU mapping
            auto* nvrhi_device = m_device.get_nvrhi_device();
            void* mapped = nvrhi_device->mapBuffer(m_handle, nvrhi::CpuAccessMode::Write);
            if (mapped)
            {
                std::memcpy(static_cast<uint8_t*>(mapped) + offset_bytes, data, size);
                nvrhi_device->unmapBuffer(m_handle);
            }
            else
            {
                LUMINA_LOG_ERROR("Failed to map vertex buffer for update_at_offset");
            }
        }
        else
        {
            // Immutable buffers use writeBuffer through command list
            if (!cmd)
            {
                LUMINA_LOG_ERROR("Command list required for update_at_offset on non-dynamic buffer");
                return;
            }
            cmd->writeBuffer(m_handle, data, size, offset_bytes);
        }
    }
}
