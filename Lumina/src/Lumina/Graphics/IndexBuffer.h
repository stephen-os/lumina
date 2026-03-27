#pragma once

#include "Types.h"

#include <Lumina/Core/Base.h>

#include <nvrhi/nvrhi.h>

#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>

namespace Lumina { class Device; }

namespace Lumina
{
    /// GPU index buffer for indexed drawing.
    class IndexBuffer
    {
    public:
        ~IndexBuffer();

        IndexBuffer(const IndexBuffer&) = delete;
        IndexBuffer& operator=(const IndexBuffer&) = delete;

        IndexBuffer(IndexBuffer&& other) noexcept;
        IndexBuffer& operator=(IndexBuffer&&) = delete;

        /// Creates an index buffer. Returns nullptr on failure.
        /// indexSize must be 2 (uint16) or 4 (uint32).
        [[nodiscard]] static Ref<IndexBuffer> Create(
            Device& dev,
            const void* data,
            size_t count,
            size_t indexSize,
            std::string_view debugName = "Lumina Index Buffer");

        /// Creates an index buffer from 16-bit indices.
        [[nodiscard]] static Ref<IndexBuffer> Create(
            Device& dev,
            std::span<const uint16_t> indices,
            std::string_view debugName = "Lumina Index Buffer")
        {
            return Create(dev, indices.data(), indices.size(), sizeof(uint16_t), debugName);
        }

        /// Creates an index buffer from 32-bit indices.
        [[nodiscard]] static Ref<IndexBuffer> Create(
            Device& dev,
            std::span<const uint32_t> indices,
            std::string_view debugName = "Lumina Index Buffer")
        {
            return Create(dev, indices.data(), indices.size(), sizeof(uint32_t), debugName);
        }

        [[nodiscard]] size_t GetCount() const noexcept { return m_Count; }
        [[nodiscard]] size_t GetIndexSize() const noexcept { return m_IndexSize; }
        [[nodiscard]] size_t GetByteSize() const noexcept { return m_Count * m_IndexSize; }
        [[nodiscard]] bool Is32Bit() const noexcept { return m_IndexSize == 4; }
        [[nodiscard]] bool Is16Bit() const noexcept { return m_IndexSize == 2; }
        [[nodiscard]] nvrhi::IBuffer* GetBuffer() const noexcept { return m_Handle.Get(); }

    private:
        IndexBuffer(Device& dev, nvrhi::BufferHandle handle, size_t count, size_t indexSize)
            : m_Device(dev)
            , m_Handle(std::move(handle))
            , m_Count(count)
            , m_IndexSize(indexSize)
        {}

        Device& m_Device;
        nvrhi::BufferHandle m_Handle;
        size_t m_Count;
        size_t m_IndexSize;
    };
}
