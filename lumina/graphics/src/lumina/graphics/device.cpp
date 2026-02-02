#include "device.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "uniform_buffer.h"
#include "texture.h"
#include "render_target.h"
#include "shader.h"
#include "context.h"

#include <lumina/core/assert.h>
#include <lumina/core/graphics_device.h>
#include <lumina/core/log.h>

#include <nvrhi/nvrhi.h>

namespace lumina::graphics
{
    device::~device() = default;

    ref<device> device::create(const device_desc& desc)
    {
        auto dev = ref<device>(new device());
        if (!dev->init(desc))
        {
            LUMINA_LOG_ERROR("Failed to create graphics device");
            return nullptr;
        }
        return dev;
    }

    bool device::init(const device_desc& desc)
    {
        if (!desc.core_device)
        {
            LUMINA_LOG_ERROR("Core device is required");
            return false;
        }

        m_core_device = desc.core_device;

        auto core_api = m_core_device->get_api();
        m_backend = (core_api == lumina::core::graphics_api::d3d12) ? backend::d3d12 : backend::vulkan;

        m_context = make_scope<context>(*this);

        LUMINA_LOG_INFO("Graphics device initialized with {} backend", m_backend == backend::d3d12 ? "D3D12" : "Vulkan");

        return true;
    }

    nvrhi::IDevice* device::get_nvrhi_device() const
    {
        return m_core_device ? static_cast<nvrhi::IDevice*>(m_core_device->get_device()) : nullptr;
    }

    context& device::get_context()
    {
        LUMINA_ASSERT(m_context != nullptr, "Context not initialized - was device::create() successful?");
        return *m_context;
    }

    const context& device::get_context() const
    {
        LUMINA_ASSERT(m_context != nullptr, "Context not initialized - was device::create() successful?");
        return *m_context;
    }

    void device::begin_frame()
    {
        if (m_core_device)
        {
            m_context->begin_frame();
            m_context->set_command_list(m_core_device->get_command_list());
            m_context->set_swapchain_framebuffer(m_core_device->get_current_framebuffer());
        }
    }

    void device::end_frame()
    {
        if (m_context)
        {
            m_context->end_frame();
        }
    }

    format device::get_swapchain_format() const
    {
        if (!m_core_device)
            return format::rgba8_unorm;

        auto nvrhi_fmt = m_core_device->get_swapchain_format();
        switch (nvrhi_fmt)
        {
            case nvrhi::Format::RGBA8_UNORM:  return format::rgba8_unorm;
            case nvrhi::Format::BGRA8_UNORM:  return format::bgra8_unorm;
            case nvrhi::Format::RGBA16_FLOAT: return format::rgba16_float;
            default: return format::bgra8_unorm; // Most swapchains use BGRA8
        }
    }

    uint32_t device::get_width() const
    {
        return m_core_device ? m_core_device->get_width() : 0;
    }

    uint32_t device::get_height() const
    {
        return m_core_device ? m_core_device->get_height() : 0;
    }

    ref<vertex_buffer> device::create_vertex_buffer(const void* data, size_t size, size_t stride)
    {
        return vertex_buffer::create(*this, data, size, stride, buffer_usage::immutable);
    }

    ref<vertex_buffer> device::create_dynamic_vertex_buffer(size_t max_size, size_t stride)
    {
        return vertex_buffer::create(*this, nullptr, max_size, stride, buffer_usage::dynamic);
    }

    ref<index_buffer> device::create_index_buffer(const uint32_t* data, size_t count)
    {
        return index_buffer::create(*this, data, count, sizeof(uint32_t));
    }

    ref<index_buffer> device::create_index_buffer(const uint16_t* data, size_t count)
    {
        return index_buffer::create(*this, data, count, sizeof(uint16_t));
    }

    ref<uniform_buffer> device::create_uniform_buffer(size_t size)
    {
        return uniform_buffer::create(*this, size);
    }

    ref<texture> device::create_texture(uint32_t width, uint32_t height, format fmt, const void* data)
    {
        return texture::create(*this, width, height, fmt, data);
    }

    ref<render_target> device::create_render_target(uint32_t width, uint32_t height, format color_format, format depth_format)
    {
        return render_target::create(*this, width, height, color_format, depth_format);
    }

    ref<shader> device::create_shader(const void* vertex_blob, size_t vertex_size, const void* pixel_blob, size_t pixel_size)
    {
        return shader::create(*this, vertex_blob, vertex_size, pixel_blob, pixel_size);
    }
}
