#pragma once

#include "types.h"

#include <lumina/core/base.h>

#include <cstddef>
#include <span>
#include <string_view>

namespace nvrhi { class IBuffer; class ICommandList; }
namespace lumina::core { class device; }

namespace lumina::graphics
{
    /**
     * @brief GPU vertex buffer for storing vertex data.
     *
     * Wraps an NVRHI buffer configured for vertex data. Supports both immutable
     * (upload once) and dynamic (frequently updated) usage patterns.
     *
     * @note Immutable buffers are more efficient for static geometry.
     * @note Dynamic buffers support CPU mapping for per-frame updates.
     */
    class vertex_buffer
    {
    public:
        ~vertex_buffer();

        vertex_buffer(const vertex_buffer&) = delete;
        vertex_buffer& operator=(const vertex_buffer&) = delete;

        vertex_buffer(vertex_buffer&& other) noexcept;
        vertex_buffer& operator=(vertex_buffer&&) = delete;

        /**
         * @brief Creates a vertex buffer.
         *
         * @param dev Graphics device
         * @param data Initial vertex data (can be nullptr for dynamic buffers)
         * @param size Total buffer size in bytes
         * @param stride Bytes per vertex (must be > 0)
         * @param usage Buffer usage pattern
         * @param debug_name Optional name for graphics debuggers
         * @return Created buffer, or nullptr on failure
         */
        [[nodiscard]] static ref<vertex_buffer> create(
            core::device& dev,
            const void* data,
            size_t size,
            size_t stride,
            buffer_usage usage,
            std::string_view debug_name = "Lumina Vertex Buffer");

        /**
         * @brief Creates a vertex buffer from a span of vertices.
         *
         * @tparam T Vertex type (stride is inferred from sizeof(T))
         * @param dev Graphics device
         * @param vertices Span of vertex data
         * @param usage Buffer usage pattern
         * @param debug_name Optional name for graphics debuggers
         * @return Created buffer, or nullptr on failure
         */
        template<typename T>
        [[nodiscard]] static ref<vertex_buffer> create(
            core::device& dev,
            std::span<const T> vertices,
            buffer_usage usage,
            std::string_view debug_name = "Lumina Vertex Buffer")
        {
            return create(dev, vertices.data(), vertices.size_bytes(), sizeof(T), usage, debug_name);
        }

        /**
         * @brief Updates buffer using direct CPU mapping.
         *
         * @warning Has race conditions if buffer is in use by GPU. Prefer the
         *          command list version for properly synchronized updates.
         *
         * @param data Source data
         * @param size Bytes to copy (clamped to buffer size)
         */
        void update(const void* data, size_t size);

        /**
         * @brief Updates buffer using command list (preferred).
         *
         * For dynamic buffers, uses CPU mapping. For immutable buffers, uses
         * writeBuffer for GPU-side copy.
         *
         * @param data Source data
         * @param size Bytes to copy (clamped to buffer size)
         * @param cmd Command list for synchronization
         */
        void update(const void* data, size_t size, nvrhi::ICommandList* cmd);

        /**
         * @brief Updates a portion of the buffer at a byte offset.
         *
         * @param data Source data
         * @param size Bytes to copy
         * @param offset_bytes Starting offset in buffer
         * @param cmd Command list for synchronization
         */
        void update_at_offset(const void* data, size_t size, size_t offset_bytes, nvrhi::ICommandList* cmd);

        /// @brief Updates buffer from a span (CPU mapping version)
        template<typename T>
        void update(std::span<const T> vertices)
        {
            update(vertices.data(), vertices.size_bytes());
        }

        /// @brief Updates buffer from a span (command list version)
        template<typename T>
        void update(std::span<const T> vertices, nvrhi::ICommandList* cmd)
        {
            update(vertices.data(), vertices.size_bytes(), cmd);
        }

        /// @brief Updates buffer from a span at offset
        template<typename T>
        void update_at_offset(std::span<const T> vertices, size_t offset_bytes, nvrhi::ICommandList* cmd)
        {
            update_at_offset(vertices.data(), vertices.size_bytes(), offset_bytes, cmd);
        }

        /// @brief Returns total buffer size in bytes
        [[nodiscard]] size_t get_size() const noexcept { return m_size; }

        /// @brief Returns bytes per vertex
        [[nodiscard]] size_t get_stride() const noexcept { return m_stride; }

        /// @brief Returns number of vertices (size / stride)
        [[nodiscard]] size_t get_vertex_count() const noexcept { return m_stride > 0 ? m_size / m_stride : 0; }

        /// @brief Returns buffer usage pattern
        [[nodiscard]] buffer_usage get_usage() const noexcept { return m_usage; }

        /// @brief Returns true if buffer supports CPU updates
        [[nodiscard]] bool is_dynamic() const noexcept { return m_usage == buffer_usage::dynamic; }

        /// @brief Returns underlying NVRHI buffer handle
        [[nodiscard]] nvrhi::IBuffer* get_buffer() const noexcept { return m_handle; }

    private:
        vertex_buffer(core::device& dev, nvrhi::IBuffer* handle, size_t size, size_t stride, buffer_usage usage)
            : m_device(dev)
            , m_handle(handle)
            , m_size(size)
            , m_stride(stride)
            , m_usage(usage)
        {}

        /// Maps buffer for CPU write access. Returns nullptr on failure.
        void* map_for_write();

        /// Unmaps previously mapped buffer.
        void unmap();

        core::device& m_device;
        nvrhi::IBuffer* m_handle;
        size_t m_size;
        size_t m_stride;
        buffer_usage m_usage;
    };
}
