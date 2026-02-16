#pragma once

#include "types.h"

#include <lumina/core/base.h>

#include <cstddef>
#include <string_view>

namespace nvrhi
{
    class IBuffer;
    class ICommandList;
}

namespace lumina::core { class device; }

namespace lumina::graphics
{
    /// GPU constant/uniform buffer for shader parameters.
    /// Buffer size is automatically aligned to 256 bytes for GPU requirements.
    /// Use command list-based update() for proper synchronization.
    class uniform_buffer
    {
    public:
        ~uniform_buffer();

        uniform_buffer(const uniform_buffer&) = delete;
        uniform_buffer& operator=(const uniform_buffer&) = delete;
        uniform_buffer(uniform_buffer&& other) noexcept;
        uniform_buffer& operator=(uniform_buffer&&) = delete;

        /// Creates a uniform buffer with the specified size. Returns nullptr on failure.
        [[nodiscard]] static ref<uniform_buffer> create(
            core::device& dev,
            size_t size,
            std::string_view debug_name = "Lumina Uniform Buffer");

        /// Creates a uniform buffer sized to hold type T.
        template<typename T>
        [[nodiscard]] static ref<uniform_buffer> create(
            core::device& dev,
            std::string_view debug_name = "Lumina Uniform Buffer")
        {
            return create(dev, sizeof(T), debug_name);
        }

        /// Updates the buffer contents using a command list.
        void update(const void* data, size_t size, nvrhi::ICommandList* cmd_list);

        /// Updates the buffer with a typed value.
        template<typename T>
        void update(const T& data, nvrhi::ICommandList* cmd_list)
        {
            update(&data, sizeof(T), cmd_list);
        }

        [[nodiscard]] size_t get_size() const noexcept { return m_size; }
        [[nodiscard]] size_t get_aligned_size() const noexcept { return m_aligned_size; }
        [[nodiscard]] nvrhi::IBuffer* get_buffer() const noexcept { return m_handle; }

    private:
        uniform_buffer(core::device& dev, nvrhi::IBuffer* handle, size_t size, size_t aligned_size)
            : m_device(dev)
            , m_handle(handle)
            , m_size(size)
            , m_aligned_size(aligned_size)
        {}

        core::device& m_device;
        nvrhi::IBuffer* m_handle;
        size_t m_size;
        size_t m_aligned_size;
    };
}
