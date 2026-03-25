#include "UniformBuffer.h"

#include <lumina/core/Device.h>

#include <lumina/core/Assert.h>
#include <lumina/core/Log.h>

#include <string>
#include <utility>

namespace Lumina
{
    UniformBuffer::~UniformBuffer() = default;

    UniformBuffer::UniformBuffer(UniformBuffer&& other) noexcept
        : m_Device(other.m_Device)
        , m_Handle(std::move(other.m_Handle))
        , m_Size(other.m_Size)
        , m_AlignedSize(other.m_AlignedSize)
    {
        other.m_Size = 0;
        other.m_AlignedSize = 0;
    }

    Ref<UniformBuffer> UniformBuffer::Create(Core::Device& dev, size_t size, std::string_view debugName)
    {
        auto* nvrhiDevice = dev.GetNvrhiDevice();
        if (!nvrhiDevice)
        {
            LUMINA_LOG_ERROR("Failed to create uniform buffer: no device");
            return nullptr;
        }

        if (size == 0)
        {
            LUMINA_LOG_ERROR("Failed to create uniform buffer: size cannot be zero");
            return nullptr;
        }

        size_t alignedSize = (size + 255) & ~255;

        nvrhi::BufferDesc desc;
        desc.byteSize = alignedSize;
        desc.isConstantBuffer = true;
        desc.debugName = std::string(debugName);
        desc.initialState = nvrhi::ResourceStates::CopyDest;
        desc.keepInitialState = true;

        nvrhi::BufferHandle buffer = nvrhiDevice->createBuffer(desc);
        if (!buffer)
        {
            LUMINA_LOG_ERROR("Failed to create NVRHI uniform buffer");
            return nullptr;
        }

        return Ref<UniformBuffer>(new UniformBuffer(dev, std::move(buffer), size, alignedSize));
    }

    void UniformBuffer::Update(const void* data, size_t size, nvrhi::ICommandList* cmdList)
    {
        LUMINA_ASSERT(data != nullptr, "Cannot update uniform buffer with null data");
        LUMINA_ASSERT(cmdList != nullptr, "Command list is required for uniform buffer update");
        LUMINA_ASSERT(m_Handle != nullptr, "Uniform buffer handle is null");

        if (!data || !cmdList || !m_Handle)
        {
            LUMINA_LOG_ERROR("UniformBuffer::Update called with invalid state");
            return;
        }

        if (size > m_Size)
        {
            LUMINA_LOG_WARN("Update size {} exceeds buffer size {}", size, m_Size);
            size = m_Size;
        }

        cmdList->writeBuffer(m_Handle.Get(), data, size);

        cmdList->setBufferState(m_Handle.Get(), nvrhi::ResourceStates::ConstantBuffer);
    }
}
