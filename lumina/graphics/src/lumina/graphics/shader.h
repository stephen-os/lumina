#pragma once

#include "types.h"

#include <lumina/core/base.h>

#include <cstddef>
#include <string>

namespace nvrhi { class IShader; }
namespace lumina::core { class device; }

namespace lumina::graphics
{

    struct shader_desc
    {
        const void* vertex_blob = nullptr;
        size_t vertex_size = 0;
        const void* pixel_blob = nullptr;
        size_t pixel_size = 0;
        std::string vertex_entry = "main";
        std::string pixel_entry = "main";
        std::string debug_name = "Lumina Shader";
    };

    class shader
    {
    public:
        ~shader();

        shader(const shader&) = delete;
        shader& operator=(const shader&) = delete;

        static ref<shader> create(core::device& dev, const void* vertex_blob, size_t vertex_size, const void* pixel_blob, size_t pixel_size);
        static ref<shader> create(core::device& dev, const shader_desc& desc);

        bool is_valid() const { return m_vertex_shader && m_pixel_shader; }

        nvrhi::IShader* get_vertex_shader() const { return m_vertex_shader; }
        nvrhi::IShader* get_pixel_shader() const { return m_pixel_shader; }

    private:
        shader(core::device& dev, nvrhi::IShader* vertex_shader, nvrhi::IShader* pixel_shader)
            : m_device(dev)
            , m_vertex_shader(vertex_shader)
            , m_pixel_shader(pixel_shader)
        {}

        core::device& m_device;
        nvrhi::IShader* m_vertex_shader;
        nvrhi::IShader* m_pixel_shader;
    };
}
