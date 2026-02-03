#pragma once

#include <nvrhi/nvrhi.h>

struct ImDrawData;

namespace lumina::core
{
    class device;
}

namespace lumina::core::imgui
{
    struct imgui_nvrhi_config
    {
        nvrhi::IDevice* device = nullptr;
        nvrhi::Format render_target_format = nvrhi::Format::RGBA8_UNORM;
        uint32_t initial_vertex_buffer_size = 5000;
        uint32_t initial_index_buffer_size = 10000;
    };

    bool init(const imgui_nvrhi_config& config);
    void shutdown();

    void new_frame();
    void render_draw_data(nvrhi::ICommandList* command_list, nvrhi::IFramebuffer* framebuffer, ImDrawData* draw_data);

    void init_platform_viewports(device& dev);
    void shutdown_platform_viewports();
}
