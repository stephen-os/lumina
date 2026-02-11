#pragma once

#include "types.h"

#include <lumina/core/base.h>

#include <cstddef>
#include <span>

namespace nvrhi { class IBuffer; class ICommandList; }
namespace lumina::core { class device; }

namespace lumina::graphics
{

    class vertex_buffer
    {
    public:
        ~vertex_buffer();

        vertex_buffer(const vertex_buffer&) = delete;
        vertex_buffer& operator=(const vertex_buffer&) = delete;

        vertex_buffer(vertex_buffer&& other) noexcept;
        vertex_buffer& operator=(vertex_buffer&&) = delete;

        static ref<vertex_buffer> create(core::device& dev, const void* data, size_t size, size_t stride, buffer_usage usage);

        template<typename T>
        static ref<vertex_buffer> create(core::device& dev, std::span<const T> vertices, buffer_usage usage)
        {
            return create(dev, vertices.data(), vertices.size_bytes(), sizeof(T), usage);
        }

        // Update using direct CPU mapping (legacy - has race conditions with in-flight draws)
        void update(const void* data, size_t size);

        // Update using command list writeBuffer (preferred - properly synchronized)
        void update(const void* data, size_t size, nvrhi::ICommandList* cmd);

        // Update at specific byte offset using command list writeBuffer
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

        size_t get_size() const { return m_size; }
        size_t get_stride() const { return m_stride; }
        size_t get_vertex_count() const { return m_stride > 0 ? m_size / m_stride : 0; }
        buffer_usage get_usage() const { return m_usage; }
        bool is_dynamic() const { return m_usage == buffer_usage::dynamic; }

        nvrhi::IBuffer* get_buffer() const { return m_handle; }

    private:
        vertex_buffer(core::device& dev, nvrhi::IBuffer* handle, size_t size, size_t stride, buffer_usage usage)
            : m_device(dev)
            , m_handle(handle)
            , m_size(size)
            , m_stride(stride)
            , m_usage(usage)
        {}

        core::device& m_device;
        nvrhi::IBuffer* m_handle;
        size_t m_size;
        size_t m_stride;
        buffer_usage m_usage;
    };
}
