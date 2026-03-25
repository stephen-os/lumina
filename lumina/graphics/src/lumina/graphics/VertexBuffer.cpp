#include "VertexBuffer.h"

#include <lumina/core/Device.h>
#include <lumina/core/Assert.h>
#include <lumina/core/Log.h>

#include <cstring>
#include <string>
#include <utility>

namespace Lumina
{
    VertexBuffer::~VertexBuffer() = default;

    VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
        : m_Device(other.m_Device)
        , m_Handle(std::move(other.m_Handle))
        , m_Size(other.m_Size)
        , m_Stride(other.m_Stride)
        , m_Usage(other.m_Usage)
    {
        other.m_Size = 0;
        other.m_Stride = 0;
    }

    Ref<VertexBuffer> VertexBuffer::Create(
        Core::Device& dev,
        const void* data,
        size_t size,
        size_t stride,
        BufferUsage usage,
        std::string_view debugName)
    {
        auto* nvrhiDevice = dev.GetNvrhiDevice();
        if (!nvrhiDevice)
        {
            LUMINA_LOG_ERROR("Failed to create vertex buffer: no device");
            return nullptr;
        }

        if (stride == 0)
        {
            LUMINA_LOG_ERROR("Failed to create vertex buffer: stride cannot be zero");
            return nullptr;
        }

        if (size == 0)
        {
            LUMINA_LOG_ERROR("Failed to create vertex buffer: size cannot be zero");
            return nullptr;
        }

        nvrhi::BufferDesc desc;
        desc.byteSize = size;
        desc.isVertexBuffer = true;
        desc.debugName = std::string(debugName);

        if (usage == BufferUsage::Dynamic)
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

        nvrhi::BufferHandle buffer = nvrhiDevice->createBuffer(desc);
        if (!buffer)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI vertex buffer");
            return nullptr;
        }

        // Upload initial data if provided
        if (data)
        {
            if (usage == BufferUsage::Immutable)
            {
                // Immutable buffers require command list upload
                nvrhi::CommandListHandle cmd = nvrhiDevice->createCommandList();
                cmd->open();
                cmd->writeBuffer(buffer, data, size);
                cmd->close();
                nvrhiDevice->executeCommandList(cmd);
                nvrhiDevice->waitForIdle();
            }
            else
            {
                // Dynamic buffers use direct CPU mapping
                void* mapped = nvrhiDevice->mapBuffer(buffer, nvrhi::CpuAccessMode::Write);
                if (mapped)
                {
                    std::memcpy(mapped, data, size);
                    nvrhiDevice->unmapBuffer(buffer);
                }
                else
                {
                    LUMINA_LOG_ERROR("Failed to map dynamic vertex buffer for initial data upload");
                }
            }
        }

        return Ref<VertexBuffer>(new VertexBuffer(dev, std::move(buffer), size, stride, usage));
    }

    void* VertexBuffer::MapForWrite()
    {
        auto* nvrhiDevice = m_Device.GetNvrhiDevice();
        return nvrhiDevice->mapBuffer(m_Handle.Get(), nvrhi::CpuAccessMode::Write);
    }

    void VertexBuffer::Unmap()
    {
        auto* nvrhiDevice = m_Device.GetNvrhiDevice();
        nvrhiDevice->unmapBuffer(m_Handle.Get());
    }

    void VertexBuffer::Update(const void* data, size_t size)
    {
        LUMINA_ASSERT(data != nullptr, "Cannot update vertex buffer with null data");
        LUMINA_ASSERT(m_Handle != nullptr, "Vertex buffer handle is null");

        if (!data || !m_Handle)
        {
            LUMINA_LOG_ERROR("VertexBuffer::Update called with invalid state");
            return;
        }

        if (!IsDynamic())
        {
            LUMINA_LOG_WARN("Cannot update immutable vertex buffer via CPU mapping");
            return;
        }

        if (size > m_Size)
        {
            LUMINA_LOG_WARN("Update size {} exceeds buffer size {}, clamping", size, m_Size);
            size = m_Size;
        }

        void* mapped = MapForWrite();
        if (!mapped)
        {
            LUMINA_LOG_ERROR("Failed to map vertex buffer for update");
            return;
        }

        std::memcpy(mapped, data, size);
        Unmap();
    }

    void VertexBuffer::Update(const void* data, size_t size, nvrhi::ICommandList* cmd)
    {
        LUMINA_ASSERT(data != nullptr, "Cannot update vertex buffer with null data");
        LUMINA_ASSERT(m_Handle != nullptr, "Vertex buffer handle is null");

        if (!data || !m_Handle)
        {
            LUMINA_LOG_ERROR("VertexBuffer::Update called with invalid state");
            return;
        }

        if (size > m_Size)
        {
            LUMINA_LOG_WARN("Update size {} exceeds buffer size {}, clamping", size, m_Size);
            size = m_Size;
        }

        if (IsDynamic())
        {
            // Dynamic buffers use CPU mapping (cmd is not used but accepted for API consistency)
            void* mapped = MapForWrite();
            if (mapped)
            {
                std::memcpy(mapped, data, size);
                Unmap();
            }
            else
            {
                LUMINA_LOG_ERROR("Failed to map vertex buffer for update");
            }
        }
        else
        {
            // Immutable buffers require command list
            if (!cmd)
            {
                LUMINA_LOG_ERROR("Command list required for update on immutable buffer");
                return;
            }
            cmd->writeBuffer(m_Handle.Get(), data, size);
        }
    }

    void VertexBuffer::UpdateAtOffset(const void* data, size_t size, size_t offsetBytes, nvrhi::ICommandList* cmd)
    {
        LUMINA_ASSERT(data != nullptr, "Cannot update vertex buffer with null data");
        LUMINA_ASSERT(m_Handle != nullptr, "Vertex buffer handle is null");

        if (!data || !m_Handle)
        {
            LUMINA_LOG_ERROR("VertexBuffer::UpdateAtOffset called with invalid state");
            return;
        }

        if (offsetBytes >= m_Size)
        {
            LUMINA_LOG_WARN("Update offset {} is beyond buffer size {}", offsetBytes, m_Size);
            return;
        }

        if (offsetBytes + size > m_Size)
        {
            LUMINA_LOG_WARN("Update at offset {} with size {} exceeds buffer size {}, clamping",
                offsetBytes, size, m_Size);
            size = m_Size - offsetBytes;
        }

        if (IsDynamic())
        {
            // Dynamic buffers use CPU mapping
            void* mapped = MapForWrite();
            if (mapped)
            {
                std::memcpy(static_cast<uint8_t*>(mapped) + offsetBytes, data, size);
                Unmap();
            }
            else
            {
                LUMINA_LOG_ERROR("Failed to map vertex buffer for UpdateAtOffset");
            }
        }
        else
        {
            // Immutable buffers require command list
            if (!cmd)
            {
                LUMINA_LOG_ERROR("Command list required for UpdateAtOffset on immutable buffer");
                return;
            }
            cmd->writeBuffer(m_Handle.Get(), data, size, offsetBytes);
        }
    }
}
