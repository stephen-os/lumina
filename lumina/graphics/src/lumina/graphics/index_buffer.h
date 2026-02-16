#pragma once

#include "types.h"

#include <lumina/core/base.h>

#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>

namespace nvrhi { class IBuffer; }
namespace lumina::core { class device; }

namespace lumina::graphics
{
    /// GPU index buffer for indexed drawing.
    class index_buffer
    {
    public:
        ~index_buffer();

        index_buffer(const index_buffer&) = delete;
        index_buffer& operator=(const index_buffer&) = delete;

        index_buffer(index_buffer&& other) noexcept;
        index_buffer& operator=(index_buffer&&) = delete;

        /// Creates an index buffer. Returns nullptr on failure.
        /// index_size must be 2 (uint16) or 4 (uint32).
        [[nodiscard]] static ref<index_buffer> create(
            core::device& dev,
            const void* data,
            size_t count,
            size_t index_size,
            std::string_view debug_name = "Lumina Index Buffer");

        /// Creates an index buffer from 16-bit indices.
        [[nodiscard]] static ref<index_buffer> create(
            core::device& dev,
            std::span<const uint16_t> indices,
            std::string_view debug_name = "Lumina Index Buffer")
        {
            return create(dev, indices.data(), indices.size(), sizeof(uint16_t), debug_name);
        }

        /// Creates an index buffer from 32-bit indices.
        [[nodiscard]] static ref<index_buffer> create(
            core::device& dev,
            std::span<const uint32_t> indices,
            std::string_view debug_name = "Lumina Index Buffer")
        {
            return create(dev, indices.data(), indices.size(), sizeof(uint32_t), debug_name);
        }

        [[nodiscard]] size_t get_count() const noexcept { return m_count; }
        [[nodiscard]] size_t get_index_size() const noexcept { return m_index_size; }
        [[nodiscard]] size_t get_byte_size() const noexcept { return m_count * m_index_size; }
        [[nodiscard]] bool is_32bit() const noexcept { return m_index_size == 4; }
        [[nodiscard]] bool is_16bit() const noexcept { return m_index_size == 2; }
        [[nodiscard]] nvrhi::IBuffer* get_buffer() const noexcept { return m_handle; }

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
