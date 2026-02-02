#pragma once

#include "types.h"

#include <lumina/core/base.h>

namespace nvrhi { class IDevice; }
namespace lumina::core { class graphics_device; }

namespace lumina::graphics
{
    class vertex_buffer;
    class index_buffer;
    class uniform_buffer;
    class texture;
    class render_target;
    class shader;
    class context;

    struct device_desc
    {
        lumina::core::graphics_device* core_device = nullptr;
    };

    class device
    {
    public:
        ~device();

        device(const device&) = delete;
        device& operator=(const device&) = delete;
        device(device&&) = delete;
        device& operator=(device&&) = delete;

        static ref<device> create(const device_desc& desc);

        ref<vertex_buffer> create_vertex_buffer(const void* data, size_t size, size_t stride);
        ref<vertex_buffer> create_dynamic_vertex_buffer(size_t max_size, size_t stride);
        ref<index_buffer> create_index_buffer(const uint32_t* data, size_t count);
        ref<index_buffer> create_index_buffer(const uint16_t* data, size_t count);
        ref<uniform_buffer> create_uniform_buffer(size_t size);
        ref<texture> create_texture(uint32_t width, uint32_t height, format fmt, const void* data = nullptr);
        ref<render_target> create_render_target(uint32_t width, uint32_t height, format color_format, format depth_format = format::unknown);
        ref<shader> create_shader(const void* vertex_blob, size_t vertex_size, const void* pixel_blob, size_t pixel_size);

        void begin_frame();
        void end_frame();

        context& get_context();
        const context& get_context() const;

        backend get_backend() const { return m_backend; }
        format get_swapchain_format() const;
        uint32_t get_width() const;
        uint32_t get_height() const;

        nvrhi::IDevice* get_nvrhi_device() const;

    private:
        device() = default;
        bool init(const device_desc& desc);

        lumina::core::graphics_device* m_core_device = nullptr;
        backend m_backend = backend::vulkan;
        scope<context> m_context;
    };
}
