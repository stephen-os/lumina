#pragma once

#include "types.h"

#include <lumina/core/base.h>

#include <cstddef>
#include <span>

namespace nvrhi { class IBuffer; }

namespace lumina::graphics
{
    class device;

    class vertex_buffer
    {
    public:
        ~vertex_buffer();

        vertex_buffer(const vertex_buffer&) = delete;
        vertex_buffer& operator=(const vertex_buffer&) = delete;

        vertex_buffer(vertex_buffer&& other) noexcept;
        vertex_buffer& operator=(vertex_buffer&&) = delete;

        static ref<vertex_buffer> create(device& dev, const void* data, size_t size, size_t stride, buffer_usage usage);

        template<typename T>
        static ref<vertex_buffer> create(device& dev, std::span<const T> vertices, buffer_usage usage)
        {
            return create(dev, vertices.data(), vertices.size_bytes(), sizeof(T), usage);
        }

        void update(const void* data, size_t size);

        template<typename T>
        void update(std::span<const T> vertices)
        {
            update(vertices.data(), vertices.size_bytes());
        }

        size_t get_size() const { return m_size; }
        size_t get_stride() const { return m_stride; }
        size_t get_vertex_count() const { return m_stride > 0 ? m_size / m_stride : 0; }
        buffer_usage get_usage() const { return m_usage; }
        bool is_dynamic() const { return m_usage == buffer_usage::dynamic; }

        nvrhi::IBuffer* get_buffer() const { return m_handle; }

    private:
        vertex_buffer(device& dev, nvrhi::IBuffer* handle, size_t size, size_t stride, buffer_usage usage)
            : m_device(dev)
            , m_handle(handle)
            , m_size(size)
            , m_stride(stride)
            , m_usage(usage)
        {}

        device& m_device;
        nvrhi::IBuffer* m_handle;
        size_t m_size;
        size_t m_stride;
        buffer_usage m_usage;
    };
}
