#include "uniform_buffer.h"

#include <lumina/core/device.h>

#include <lumina/core/assert.h>
#include <lumina/core/log.h>

#include <string>
#include <utility>

namespace lumina::graphics
{
    uniform_buffer::~uniform_buffer() = default;

    uniform_buffer::uniform_buffer(uniform_buffer&& other) noexcept
        : m_device(other.m_device)
        , m_handle(std::move(other.m_handle))
        , m_size(other.m_size)
        , m_aligned_size(other.m_aligned_size)
    {
        other.m_size = 0;
        other.m_aligned_size = 0;
    }

    ref<uniform_buffer> uniform_buffer::create(core::device& dev, size_t size, std::string_view debug_name)
    {
        auto* nvrhi_device = dev.get_nvrhi_device();
        if (!nvrhi_device)
        {
            LUMINA_LOG_ERROR("Failed to create uniform buffer: no device");
            return nullptr;
        }

        if (size == 0)
        {
            LUMINA_LOG_ERROR("Failed to create uniform buffer: size cannot be zero");
            return nullptr;
        }

        size_t aligned_size = (size + 255) & ~255;

        nvrhi::BufferDesc desc;
        desc.byteSize = aligned_size;
        desc.isConstantBuffer = true;
        desc.debugName = std::string(debug_name);
        desc.initialState = nvrhi::ResourceStates::CopyDest;
        desc.keepInitialState = true;

        nvrhi::BufferHandle buffer = nvrhi_device->createBuffer(desc);
        if (!buffer)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI uniform buffer");
            return nullptr;
        }

        return ref<uniform_buffer>(new uniform_buffer(dev, std::move(buffer), size, aligned_size));
    }

    void uniform_buffer::update(const void* data, size_t size, nvrhi::ICommandList* cmd_list)
    {
        LUMINA_ASSERT(data != nullptr, "Cannot update uniform buffer with null data");
        LUMINA_ASSERT(cmd_list != nullptr, "Command list is required for uniform buffer update");
        LUMINA_ASSERT(m_handle != nullptr, "Uniform buffer handle is null");

        if (!data || !cmd_list || !m_handle)
        {
            LUMINA_LOG_ERROR("uniform_buffer::update called with invalid state");
            return;
        }

        if (size > m_size)
        {
            LUMINA_LOG_WARN("Update size {} exceeds buffer size {}", size, m_size);
            size = m_size;
        }

        cmd_list->writeBuffer(m_handle.Get(), data, size);

        cmd_list->setBufferState(m_handle.Get(), nvrhi::ResourceStates::ConstantBuffer);
    }
}
