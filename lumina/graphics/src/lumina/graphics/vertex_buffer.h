#pragma once

#include "types.h"

#include <lumina/core/base.h>

#include <nvrhi/nvrhi.h>

#include <cstddef>
#include <span>
#include <string_view>

namespace lumina::core { class device; }

namespace lumina::graphics
{
    /// GPU vertex buffer for storing vertex data.
    /// Supports immutable (upload once) and dynamic (frequently updated) usage patterns.
    class vertex_buffer
    {
    public:
        ~vertex_buffer();

        vertex_buffer(const vertex_buffer&) = delete;
        vertex_buffer& operator=(const vertex_buffer&) = delete;

        vertex_buffer(vertex_buffer&& other) noexcept;
        vertex_buffer& operator=(vertex_buffer&&) = delete;

        /// Creates a vertex buffer. Returns nullptr on failure.
        /// For dynamic buffers, data can be nullptr to create an uninitialized buffer.
        [[nodiscard]] static ref<vertex_buffer> create(
            core::device& dev,
            const void* data,
            size_t size,
            size_t stride,
            buffer_usage usage,
            std::string_view debug_name = "Lumina Vertex Buffer");

        /// Creates a vertex buffer from a span. Stride is inferred from sizeof(T).
        template<typename T>
        [[nodiscard]] static ref<vertex_buffer> create(
            core::device& dev,
            std::span<const T> vertices,
            buffer_usage usage,
            std::string_view debug_name = "Lumina Vertex Buffer")
        {
            return create(dev, vertices.data(), vertices.size_bytes(), sizeof(T), usage, debug_name);
        }

        /// Updates buffer using direct CPU mapping.
        /// Warning: has race conditions if buffer is in use by GPU. Prefer the command list version.
        void update(const void* data, size_t size);

        /// Updates buffer using command list (preferred for proper synchronization).
        void update(const void* data, size_t size, nvrhi::ICommandList* cmd);

        /// Updates a portion of the buffer at a byte offset.
        void update_at_offset(const void* data, size_t size, size_t offset_bytes, nvrhi::ICommandList* cmd);

        template<typename T>
        void update(std::span<const T> vertices)
        {
            update(vertices.data(), vertices.size_bytes());
        }

        template<typename T>
        void update(std::span<const T> vertices, nvrhi::ICommandList* cmd)
        {
            update(vertices.data(), vertices.size_bytes(), cmd);
        }

        template<typename T>
        void update_at_offset(std::span<const T> vertices, size_t offset_bytes, nvrhi::ICommandList* cmd)
        {
            update_at_offset(vertices.data(), vertices.size_bytes(), offset_bytes, cmd);
        }

        [[nodiscard]] size_t get_size() const noexcept { return m_size; }
        [[nodiscard]] size_t get_stride() const noexcept { return m_stride; }
        [[nodiscard]] size_t get_vertex_count() const noexcept { return m_stride > 0 ? m_size / m_stride : 0; }
        [[nodiscard]] buffer_usage get_usage() const noexcept { return m_usage; }
        [[nodiscard]] bool is_dynamic() const noexcept { return m_usage == buffer_usage::dynamic; }
        [[nodiscard]] nvrhi::IBuffer* get_buffer() const noexcept { return m_handle.Get(); }

    private:
        vertex_buffer(core::device& dev, nvrhi::BufferHandle handle, size_t size, size_t stride, buffer_usage usage)
            : m_device(dev)
            , m_handle(std::move(handle))
            , m_size(size)
            , m_stride(stride)
            , m_usage(usage)
        {}

        void* map_for_write();
        void unmap();

        core::device& m_device;
        nvrhi::BufferHandle m_handle;
        size_t m_size;
        size_t m_stride;
        buffer_usage m_usage;
    };
}
