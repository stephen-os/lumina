#pragma once

#include "types.h"
#include "pipeline.h"

#include <lumina/core/base.h>

#include <glm/glm.hpp>

namespace nvrhi
{
    class ICommandList;
    class IFramebuffer;
}
namespace lumina::core { class device; }

namespace lumina::graphics
{
    class vertex_buffer;
    class index_buffer;
    class uniform_buffer;
    class texture;
    class render_target;
    class shader;
    class input_layout;
    class binding_layout;
    class binding_set;
    class sampler;
    class pipeline;

    class context
    {
    public:
        explicit context(core::device& dev);
        ~context();

        context(const context&) = delete;
        context& operator=(const context&) = delete;

        // Frame management
        void begin_frame();
        void end_frame();

        // Render target
        void set_render_target(ref<render_target> target);
        void set_default_render_target();
        void set_swapchain_framebuffer(nvrhi::IFramebuffer* framebuffer);
        void clear(const glm::vec4& color);
        void clear(const clear_color& color);
        void clear_depth(float depth = 1.0f, uint8_t stencil = 0);

        // Viewport and scissor
        void set_viewport(float x, float y, float width, float height);
        void set_viewport(const viewport& vp);
        void set_scissor(int32_t x, int32_t y, int32_t width, int32_t height);
        void set_scissor(const scissor_rect& rect);

        // Pipeline binding
        void set_pipeline(ref<pipeline> pso);

        // Resource binding
        void set_binding_set(ref<binding_set> bindings);
        void set_vertex_buffer(ref<vertex_buffer> buffer);
        void set_index_buffer(ref<index_buffer> buffer);

        // Drawing
        void draw(uint32_t vertex_count, uint32_t start_vertex = 0);
        void draw_indexed(uint32_t index_count, uint32_t start_index = 0, int32_t base_vertex = 0);
        void draw_instanced(uint32_t vertex_count, uint32_t instance_count, uint32_t start_vertex = 0, uint32_t start_instance = 0);
        void draw_indexed_instanced(uint32_t index_count, uint32_t instance_count, uint32_t start_index = 0, int32_t base_vertex = 0, uint32_t start_instance = 0);

        // Access
        core::device& get_device() { return m_device; }
        nvrhi::ICommandList* get_command_list() const { return m_command_list; }
        bool has_framebuffer() const { return m_swapchain_framebuffer != nullptr || m_current_render_target != nullptr; }

        // Set command list from core device (called by application)
        void set_command_list(nvrhi::ICommandList* cmd_list);

    private:
        void apply_state();

        core::device& m_device;

        // Current state
        ref<render_target> m_current_render_target;
        ref<pipeline> m_current_pipeline;
        ref<binding_set> m_current_binding_set;
        ref<vertex_buffer> m_current_vertex_buffer;
        ref<index_buffer> m_current_index_buffer;

        viewport m_current_viewport;
        scissor_rect m_current_scissor;

        // State tracking
        bool m_state_dirty = true;

        // NVRHI command list and swapchain
        nvrhi::ICommandList* m_command_list = nullptr;
        nvrhi::IFramebuffer* m_swapchain_framebuffer = nullptr;
    };
}
