#pragma once

#include "types.h"

#include <lumina/core/base.h>

#include <nvrhi/nvrhi.h>

#include <cstddef>
#include <string_view>

namespace Lumina { class Device; }

namespace Lumina
{
    /// GPU constant/uniform buffer for shader parameters.
    /// Buffer size is automatically aligned to 256 bytes for GPU requirements.
    /// Use command list-based update() for proper synchronization.
    class UniformBuffer
    {
    public:
        ~UniformBuffer();

        UniformBuffer(const UniformBuffer&) = delete;
        UniformBuffer& operator=(const UniformBuffer&) = delete;
        UniformBuffer(UniformBuffer&& other) noexcept;
        UniformBuffer& operator=(UniformBuffer&&) = delete;

        /// Creates a uniform buffer with the specified size. Returns nullptr on failure.
        [[nodiscard]] static Ref<UniformBuffer> Create(
            Device& dev,
            size_t size,
            std::string_view debugName = "Lumina Uniform Buffer");

        /// Creates a uniform buffer sized to hold type T.
        template<typename T>
        [[nodiscard]] static Ref<UniformBuffer> Create(
            Device& dev,
            std::string_view debugName = "Lumina Uniform Buffer")
        {
            return Create(dev, sizeof(T), debugName);
        }

        /// Updates the buffer contents using a command list.
        void Update(const void* data, size_t size, nvrhi::ICommandList* cmdList);

        /// Updates the buffer with a typed value.
        template<typename T>
        void Update(const T& data, nvrhi::ICommandList* cmdList)
        {
            Update(&data, sizeof(T), cmdList);
        }

        [[nodiscard]] size_t GetSize() const noexcept { return m_Size; }
        [[nodiscard]] size_t GetAlignedSize() const noexcept { return m_AlignedSize; }
        [[nodiscard]] nvrhi::IBuffer* GetBuffer() const noexcept { return m_Handle.Get(); }

    private:
        UniformBuffer(Device& dev, nvrhi::BufferHandle handle, size_t size, size_t alignedSize)
            : m_Device(dev)
            , m_Handle(std::move(handle))
            , m_Size(size)
            , m_AlignedSize(alignedSize)
        {}

        Device& m_Device;
        nvrhi::BufferHandle m_Handle;
        size_t m_Size;
        size_t m_AlignedSize;
    };
}
