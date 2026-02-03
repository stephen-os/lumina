#pragma once

#include "types.h"

#include <lumina/core/base.h>

#include <cstddef>

namespace nvrhi
{
    class IBuffer;
    class ICommandList;
}

namespace lumina::core { class device; }

namespace lumina::graphics
{

    class uniform_buffer
    {
    public:
        ~uniform_buffer();

        uniform_buffer(const uniform_buffer&) = delete;
        uniform_buffer& operator=(const uniform_buffer&) = delete;
        uniform_buffer(uniform_buffer&& other) noexcept;
        uniform_buffer& operator=(uniform_buffer&&) = delete;

        static ref<uniform_buffer> create(core::device& dev, size_t size);

        template<typename T>
        static ref<uniform_buffer> create(core::device& dev)
        {
            return create(dev, sizeof(T));
        }

        void update(const void* data, size_t size, nvrhi::ICommandList* cmd_list);

        template<typename T>
        void update(const T& data, nvrhi::ICommandList* cmd_list)
        {
            update(&data, sizeof(T), cmd_list);
        }

        size_t get_size() const { return m_size; }
        size_t get_aligned_size() const { return m_aligned_size; }

        nvrhi::IBuffer* get_buffer() const { return m_handle; }

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
