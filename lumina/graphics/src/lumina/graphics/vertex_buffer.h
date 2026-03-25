#pragma once

#include "types.h"

#include <lumina/core/base.h>

#include <nvrhi/nvrhi.h>

#include <cstddef>
#include <span>
#include <string_view>

namespace Lumina { class Device; }

namespace Lumina
{
    /// GPU vertex buffer for storing vertex data.
    /// Supports immutable (upload once) and dynamic (frequently updated) usage patterns.
    class VertexBuffer
    {
    public:
        ~VertexBuffer();

        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer& operator=(const VertexBuffer&) = delete;

        VertexBuffer(VertexBuffer&& other) noexcept;
        VertexBuffer& operator=(VertexBuffer&&) = delete;

        /// Creates a vertex buffer. Returns nullptr on failure.
        /// For dynamic buffers, data can be nullptr to create an uninitialized buffer.
        [[nodiscard]] static Ref<VertexBuffer> Create(
            Device& dev,
            const void* data,
            size_t size,
            size_t stride,
            BufferUsage usage,
            std::string_view debugName = "Lumina Vertex Buffer");

        /// Creates a vertex buffer from a span. Stride is inferred from sizeof(T).
        template<typename T>
        [[nodiscard]] static Ref<VertexBuffer> Create(
            Device& dev,
            std::span<const T> vertices,
            BufferUsage usage,
            std::string_view debugName = "Lumina Vertex Buffer")
        {
            return Create(dev, vertices.data(), vertices.size_bytes(), sizeof(T), usage, debugName);
        }

        /// Updates buffer using direct CPU mapping.
        /// Warning: has race conditions if buffer is in use by GPU. Prefer the command list version.
        void Update(const void* data, size_t size);

        /// Updates buffer using command list (preferred for proper synchronization).
        void Update(const void* data, size_t size, nvrhi::ICommandList* cmd);

        /// Updates a portion of the buffer at a byte offset.
        void UpdateAtOffset(const void* data, size_t size, size_t offsetBytes, nvrhi::ICommandList* cmd);

        template<typename T>
        void Update(std::span<const T> vertices)
        {
            Update(vertices.data(), vertices.size_bytes());
        }

        template<typename T>
        void Update(std::span<const T> vertices, nvrhi::ICommandList* cmd)
        {
            Update(vertices.data(), vertices.size_bytes(), cmd);
        }

        template<typename T>
        void UpdateAtOffset(std::span<const T> vertices, size_t offsetBytes, nvrhi::ICommandList* cmd)
        {
            UpdateAtOffset(vertices.data(), vertices.size_bytes(), offsetBytes, cmd);
        }

        [[nodiscard]] size_t GetSize() const noexcept { return m_Size; }
        [[nodiscard]] size_t GetStride() const noexcept { return m_Stride; }
        [[nodiscard]] size_t GetVertexCount() const noexcept { return m_Stride > 0 ? m_Size / m_Stride : 0; }
        [[nodiscard]] BufferUsage GetUsage() const noexcept { return m_Usage; }
        [[nodiscard]] bool IsDynamic() const noexcept { return m_Usage == BufferUsage::Dynamic; }
        [[nodiscard]] nvrhi::IBuffer* GetBuffer() const noexcept { return m_Handle.Get(); }

    private:
        VertexBuffer(Device& dev, nvrhi::BufferHandle handle, size_t size, size_t stride, BufferUsage usage)
            : m_Device(dev)
            , m_Handle(std::move(handle))
            , m_Size(size)
            , m_Stride(stride)
            , m_Usage(usage)
        {}

        void* MapForWrite();
        void Unmap();

        Device& m_Device;
        nvrhi::BufferHandle m_Handle;
        size_t m_Size;
        size_t m_Stride;
        BufferUsage m_Usage;
    };
}
