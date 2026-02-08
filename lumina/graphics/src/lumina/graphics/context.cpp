#include "context.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "uniform_buffer.h"
#include "texture.h"
#include "render_target.h"
#include "shader.h"
#include "binding_layout.h"
#include "pipeline.h"
#include "format_utils.h"

#include <lumina/core/device.h>
#include <lumina/core/log.h>

#include <nvrhi/nvrhi.h>
#include <nvrhi/utils.h>

namespace lumina::graphics
{
    context::context(core::device& dev)
        : m_device(dev)
    {
    }

    context::~context() = default;

    void context::begin_frame()
    {
        // Reset per-frame state
        m_state_dirty = true;
    }

    void context::end_frame()
    {
        // Clear references
        m_current_render_target = nullptr;
        m_current_pipeline = nullptr;
        m_current_binding_set = nullptr;
        m_current_vertex_buffer = nullptr;
        m_current_index_buffer = nullptr;
        m_state_dirty = true;
    }

    void context::set_command_list(nvrhi::ICommandList* cmd_list)
    {
        m_command_list = cmd_list;
    }

    void context::set_render_target(ref<render_target> target)
    {
        m_current_render_target = target;
        m_state_dirty = true;
    }

    void context::set_default_render_target()
    {
        m_current_render_target = nullptr;
        m_state_dirty = true;
    }

    void context::set_swapchain_framebuffer(nvrhi::IFramebuffer* framebuffer)
    {
        m_swapchain_framebuffer = framebuffer;
        m_current_render_target = nullptr;
        m_state_dirty = true;
    }

    void context::clear(const glm::vec4& color)
    {
        clear(clear_color(color.r, color.g, color.b, color.a));
    }

    void context::clear(const clear_color& color)
    {
        if (!m_command_list)
            return;

        nvrhi::IFramebuffer* fb = nullptr;
        if (m_current_render_target)
        {
            fb = m_current_render_target->get_framebuffer();
        }
        else if (m_swapchain_framebuffer)
        {
            fb = m_swapchain_framebuffer;
        }

        if (fb)
        {
            nvrhi::utils::ClearColorAttachment(m_command_list, fb, 0, nvrhi::Color(color.r, color.g, color.b, color.a));
        }
    }

    void context::clear_depth(float depth, uint8_t stencil)
    {
        if (!m_command_list)
            return;

        nvrhi::IFramebuffer* fb = nullptr;
        if (m_current_render_target)
        {
            fb = m_current_render_target->get_framebuffer();
        }
        else if (m_swapchain_framebuffer)
        {
            fb = m_swapchain_framebuffer;
        }

        if (fb && fb->getDesc().depthAttachment.texture)
        {
            m_command_list->clearDepthStencilTexture(fb->getDesc().depthAttachment.texture, nvrhi::AllSubresources, true, depth, true, stencil);
        }
    }

    void context::set_viewport(float x, float y, float width, float height)
    {
        viewport vp;
        vp.x = x;
        vp.y = y;
        vp.width = width;
        vp.height = height;
        set_viewport(vp);
    }

    void context::set_viewport(const viewport& vp)
    {
        m_current_viewport = vp;
        m_state_dirty = true;
    }

    void context::set_scissor(int32_t x, int32_t y, int32_t width, int32_t height)
    {
        scissor_rect rect;
        rect.x = x;
        rect.y = y;
        rect.width = width;
        rect.height = height;
        set_scissor(rect);
    }

    void context::set_scissor(const scissor_rect& rect)
    {
        m_current_scissor = rect;
        m_state_dirty = true;
    }

    void context::set_pipeline(ref<pipeline> pso)
    {
        m_current_pipeline = pso;
        m_state_dirty = true;
    }

    void context::set_binding_set(ref<binding_set> bindings)
    {
        m_current_binding_set = bindings;
        m_state_dirty = true;
    }

    void context::set_vertex_buffer(ref<vertex_buffer> buffer)
    {
        m_current_vertex_buffer = buffer;
        m_state_dirty = true;
    }

    void context::set_index_buffer(ref<index_buffer> buffer)
    {
        m_current_index_buffer = buffer;
        m_state_dirty = true;
    }

    void context::apply_state()
    {
        if (!m_state_dirty)
            return;

        if (!m_command_list)
            return;

        // Build graphics state
        nvrhi::GraphicsState state;

        // Pipeline
        if (m_current_pipeline)
        {
            state.pipeline = m_current_pipeline->get_pipeline();
        }

        // Framebuffer
        if (m_current_render_target)
        {
            state.framebuffer = m_current_render_target->get_framebuffer();
        }
        else if (m_swapchain_framebuffer)
        {
            state.framebuffer = m_swapchain_framebuffer;
        }

        // Viewport
        state.viewport.addViewport(nvrhi::Viewport(
            m_current_viewport.x,
            m_current_viewport.x + m_current_viewport.width,
            m_current_viewport.y,
            m_current_viewport.y + m_current_viewport.height,
            m_current_viewport.min_depth,
            m_current_viewport.max_depth
        ));

        // Scissor (use viewport as default if not set)
        if (m_current_scissor.width > 0 && m_current_scissor.height > 0)
        {
            state.viewport.addScissorRect(nvrhi::Rect(
                m_current_scissor.x,
                m_current_scissor.x + m_current_scissor.width,
                m_current_scissor.y,
                m_current_scissor.y + m_current_scissor.height
            ));
        }
        else
        {
            state.viewport.addScissorRect(nvrhi::Rect(
                static_cast<int>(m_current_viewport.x),
                static_cast<int>(m_current_viewport.x + m_current_viewport.width),
                static_cast<int>(m_current_viewport.y),
                static_cast<int>(m_current_viewport.y + m_current_viewport.height)
            ));
        }

        // Binding set
        if (m_current_binding_set)
        {
            state.bindings.push_back(m_current_binding_set->get_binding_set());
        }

        // Vertex buffer
        if (m_current_vertex_buffer)
        {
            nvrhi::VertexBufferBinding vb;
            vb.buffer = m_current_vertex_buffer->get_buffer();
            vb.slot = 0;
            vb.offset = 0;
            state.vertexBuffers.push_back(vb);
        }

        // Index buffer
        if (m_current_index_buffer)
        {
            state.indexBuffer.buffer = m_current_index_buffer->get_buffer();
            state.indexBuffer.format = m_current_index_buffer->is_32bit() ? nvrhi::Format::R32_UINT : nvrhi::Format::R16_UINT;
            state.indexBuffer.offset = 0;
        }

        m_command_list->setGraphicsState(state);
        m_state_dirty = false;
    }

    void context::draw(uint32_t vertex_count, uint32_t start_vertex)
    {
        if (!m_command_list || !m_current_pipeline)
            return;

        apply_state();

        nvrhi::DrawArguments args;
        args.vertexCount = vertex_count;
        args.startVertexLocation = start_vertex;
        args.instanceCount = 1;
        args.startInstanceLocation = 0;

        m_command_list->draw(args);
    }

    void context::draw_indexed(uint32_t index_count, uint32_t start_index, int32_t base_vertex)
    {
        if (!m_command_list || !m_current_pipeline || !m_current_index_buffer)
            return;

        apply_state();

        nvrhi::DrawArguments args;
        args.vertexCount = index_count;
        args.startVertexLocation = start_index;
        args.instanceCount = 1;
        args.startInstanceLocation = 0;

        m_command_list->drawIndexed(args);
    }

    void context::draw_instanced(uint32_t vertex_count, uint32_t instance_count, uint32_t start_vertex, uint32_t start_instance)
    {
        if (!m_command_list || !m_current_pipeline)
            return;

        apply_state();

        nvrhi::DrawArguments args;
        args.vertexCount = vertex_count;
        args.startVertexLocation = start_vertex;
        args.instanceCount = instance_count;
        args.startInstanceLocation = start_instance;

        m_command_list->draw(args);
    }

    void context::draw_indexed_instanced(uint32_t index_count, uint32_t instance_count, uint32_t start_index, int32_t base_vertex, uint32_t start_instance)
    {
        if (!m_command_list || !m_current_pipeline || !m_current_index_buffer)
            return;

        apply_state();

        nvrhi::DrawArguments args;
        args.vertexCount = index_count;
        args.startVertexLocation = start_index;
        args.instanceCount = instance_count;
        args.startInstanceLocation = start_instance;

        m_command_list->drawIndexed(args);
    }
}
