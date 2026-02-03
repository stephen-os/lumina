#pragma once

#include "types.h"

#include <lumina/core/base.h>

#include <cstddef>
#include <cstdint>
#include <span>

namespace nvrhi { class IBuffer; }
namespace lumina::core { class device; }

namespace lumina::graphics
{

    class index_buffer
    {
    public:
        ~index_buffer();

        index_buffer(const index_buffer&) = delete;
        index_buffer& operator=(const index_buffer&) = delete;
        index_buffer(index_buffer&& other) noexcept;
        index_buffer& operator=(index_buffer&&) = delete;

        static ref<index_buffer> create(core::device& dev, const void* data, size_t count, size_t index_size);

        static ref<index_buffer> create(core::device& dev, std::span<const uint16_t> indices)
        {
            return create(dev, indices.data(), indices.size(), sizeof(uint16_t));
        }

        static ref<index_buffer> create(core::device& dev, std::span<const uint32_t> indices)
        {
            return create(dev, indices.data(), indices.size(), sizeof(uint32_t));
        }

        size_t get_count() const { return m_count; }
        size_t get_index_size() const { return m_index_size; }
		size_t get_byte_size() const { return m_count * m_index_size; }
        
        bool is_32bit() const { return m_index_size == 4; }
		bool is_16bit() const { return m_index_size == 2; }

        nvrhi::IBuffer* get_buffer() const { return m_handle; }

    private:
        index_buffer(core::device& dev, nvrhi::IBuffer* handle, size_t count, size_t index_size)
            : m_device(dev)
            , m_handle(handle)
            , m_count(count)
            , m_index_size(index_size)
        {}

        core::device& m_device;
        nvrhi::IBuffer* m_handle;
        size_t m_count;
        size_t m_index_size;
    };
}
