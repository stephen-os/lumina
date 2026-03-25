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
    /// GPU storage buffer (structured buffer) for compute shader read/write access.
    /// Can be used as UAV in compute shaders for flexible data storage.
    class StorageBuffer
    {
    public:
        ~StorageBuffer();

        StorageBuffer(const StorageBuffer&) = delete;
        StorageBuffer& operator=(const StorageBuffer&) = delete;
        StorageBuffer(StorageBuffer&& other) noexcept;
        StorageBuffer& operator=(StorageBuffer&&) = delete;

        /// Creates a storage buffer with the specified size and stride.
        /// @param dev The graphics device.
        /// @param size Total size in bytes.
        /// @param stride Size of each element in bytes (for structured buffers).
        /// @param debugName Debug name for graphics debugging tools.
        [[nodiscard]] static Ref<StorageBuffer> Create(
            Device& dev,
            size_t size,
            size_t stride,
            std::string_view debugName = "Lumina Storage Buffer");

        /// Creates a storage buffer sized for a span of elements.
        template<typename T>
        [[nodiscard]] static Ref<StorageBuffer> Create(
            Device& dev,
            std::span<const T> data,
            std::string_view debugName = "Lumina Storage Buffer")
        {
            auto buffer = Create(dev, data.size_bytes(), sizeof(T), debugName);
            if (buffer && !data.empty())
            {
                buffer->m_PendingData.assign(
                    reinterpret_cast<const uint8_t*>(data.data()),
                    reinterpret_cast<const uint8_t*>(data.data()) + data.size_bytes());
            }
            return buffer;
        }

        /// Creates a storage buffer for a fixed number of elements.
        template<typename T>
        [[nodiscard]] static Ref<StorageBuffer> Create(
            Device& dev,
            size_t elementCount,
            std::string_view debugName = "Lumina Storage Buffer")
        {
            return Create(dev, elementCount * sizeof(T), sizeof(T), debugName);
        }

        /// Updates the buffer contents using a command list.
        void Update(const void* data, size_t size, nvrhi::ICommandList* cmdList);

        /// Updates the buffer with typed data.
        template<typename T>
        void Update(std::span<const T> data, nvrhi::ICommandList* cmdList)
        {
            Update(data.data(), data.size_bytes(), cmdList);
        }

        /// Uploads any pending data that was set during creation.
        /// Call this once with a command list before first use.
        void UploadPending(nvrhi::ICommandList* cmdList);

        /// Checks if there is pending data to upload.
        [[nodiscard]] bool HasPendingData() const noexcept { return !m_PendingData.empty(); }

        [[nodiscard]] size_t GetSize() const noexcept { return m_Size; }
        [[nodiscard]] size_t GetStride() const noexcept { return m_Stride; }
        [[nodiscard]] size_t GetElementCount() const noexcept { return m_Stride > 0 ? m_Size / m_Stride : 0; }
        [[nodiscard]] nvrhi::IBuffer* GetBuffer() const noexcept { return m_Handle.Get(); }

    private:
        StorageBuffer(Device& dev, nvrhi::BufferHandle handle, size_t size, size_t stride)
            : m_Device(dev)
            , m_Handle(std::move(handle))
            , m_Size(size)
            , m_Stride(stride)
        {}

        Device& m_Device;
        nvrhi::BufferHandle m_Handle;
        size_t m_Size;
        size_t m_Stride;
        std::vector<uint8_t> m_PendingData;
    };
}
