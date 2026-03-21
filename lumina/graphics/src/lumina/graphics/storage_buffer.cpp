#include "storage_buffer.h"

#include <lumina/core/device.h>
#include <lumina/core/assert.h>
#include <lumina/core/log.h>

#include <string>
#include <utility>

namespace lumina::graphics
{
    storage_buffer::~storage_buffer() = default;

    storage_buffer::storage_buffer(storage_buffer&& other) noexcept
        : m_device(other.m_device)
        , m_handle(std::move(other.m_handle))
        , m_size(other.m_size)
        , m_stride(other.m_stride)
        , m_pending_data(std::move(other.m_pending_data))
    {
        other.m_size = 0;
        other.m_stride = 0;
    }

    ref<storage_buffer> storage_buffer::create(
        core::device& dev,
        size_t size,
        size_t stride,
        std::string_view debug_name)
    {
        auto* nvrhi_device = dev.get_nvrhi_device();
        if (!nvrhi_device)
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
        desc.debugName = std::string(debug_name);
        desc.initialState = nvrhi::ResourceStates::Common;
        desc.keepInitialState = true;
        desc.canHaveUAVs = true;

        nvrhi::BufferHandle buffer = nvrhi_device->createBuffer(desc);
        if (!buffer)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI storage buffer");
            return nullptr;
        }

        return ref<storage_buffer>(new storage_buffer(dev, std::move(buffer), size, stride));
    }

    void storage_buffer::update(const void* data, size_t size, nvrhi::ICommandList* cmd_list)
    {
        LUMINA_ASSERT(data != nullptr, "Cannot update storage buffer with null data");
        LUMINA_ASSERT(cmd_list != nullptr, "Command list is required for storage buffer update");
        LUMINA_ASSERT(m_handle != nullptr, "Storage buffer handle is null");

        if (!data || !cmd_list || !m_handle)
        {
            LUMINA_LOG_ERROR("storage_buffer::update called with invalid state");
            return;
        }

        if (size > m_size)
        {
            LUMINA_LOG_WARN("Update size {} exceeds buffer size {}", size, m_size);
            size = m_size;
        }

        cmd_list->writeBuffer(m_handle.Get(), data, size);
        // Let NVRHI handle state transitions based on binding usage (SRV or UAV)
    }

    void storage_buffer::upload_pending(nvrhi::ICommandList* cmd_list)
    {
        if (m_pending_data.empty())
        {
            return;
        }

        update(m_pending_data.data(), m_pending_data.size(), cmd_list);
        m_pending_data.clear();
        m_pending_data.shrink_to_fit();
    }
}
