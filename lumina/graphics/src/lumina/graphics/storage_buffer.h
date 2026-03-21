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
    /// GPU storage buffer (structured buffer) for compute shader read/write access.
    /// Can be used as UAV in compute shaders for flexible data storage.
    class storage_buffer
    {
    public:
        ~storage_buffer();

        storage_buffer(const storage_buffer&) = delete;
        storage_buffer& operator=(const storage_buffer&) = delete;
        storage_buffer(storage_buffer&& other) noexcept;
        storage_buffer& operator=(storage_buffer&&) = delete;

        /// Creates a storage buffer with the specified size and stride.
        /// @param dev The graphics device.
        /// @param size Total size in bytes.
        /// @param stride Size of each element in bytes (for structured buffers).
        /// @param debug_name Debug name for graphics debugging tools.
        [[nodiscard]] static ref<storage_buffer> create(
            core::device& dev,
            size_t size,
            size_t stride,
            std::string_view debug_name = "Lumina Storage Buffer");

        /// Creates a storage buffer sized for a span of elements.
        template<typename T>
        [[nodiscard]] static ref<storage_buffer> create(
            core::device& dev,
            std::span<const T> data,
            std::string_view debug_name = "Lumina Storage Buffer")
        {
            auto buffer = create(dev, data.size_bytes(), sizeof(T), debug_name);
            if (buffer && !data.empty())
            {
                buffer->m_pending_data.assign(
                    reinterpret_cast<const uint8_t*>(data.data()),
                    reinterpret_cast<const uint8_t*>(data.data()) + data.size_bytes());
            }
            return buffer;
        }

        /// Creates a storage buffer for a fixed number of elements.
        template<typename T>
        [[nodiscard]] static ref<storage_buffer> create(
            core::device& dev,
            size_t element_count,
            std::string_view debug_name = "Lumina Storage Buffer")
        {
            return create(dev, element_count * sizeof(T), sizeof(T), debug_name);
        }

        /// Updates the buffer contents using a command list.
        void update(const void* data, size_t size, nvrhi::ICommandList* cmd_list);

        /// Updates the buffer with typed data.
        template<typename T>
        void update(std::span<const T> data, nvrhi::ICommandList* cmd_list)
        {
            update(data.data(), data.size_bytes(), cmd_list);
        }

        /// Uploads any pending data that was set during creation.
        /// Call this once with a command list before first use.
        void upload_pending(nvrhi::ICommandList* cmd_list);

        /// Checks if there is pending data to upload.
        [[nodiscard]] bool has_pending_data() const noexcept { return !m_pending_data.empty(); }

        [[nodiscard]] size_t get_size() const noexcept { return m_size; }
        [[nodiscard]] size_t get_stride() const noexcept { return m_stride; }
        [[nodiscard]] size_t get_element_count() const noexcept { return m_stride > 0 ? m_size / m_stride : 0; }
        [[nodiscard]] nvrhi::IBuffer* get_buffer() const noexcept { return m_handle.Get(); }

    private:
        storage_buffer(core::device& dev, nvrhi::BufferHandle handle, size_t size, size_t stride)
            : m_device(dev)
            , m_handle(std::move(handle))
            , m_size(size)
            , m_stride(stride)
        {}

        core::device& m_device;
        nvrhi::BufferHandle m_handle;
        size_t m_size;
        size_t m_stride;
        std::vector<uint8_t> m_pending_data;
    };
}
