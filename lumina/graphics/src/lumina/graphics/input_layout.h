#pragma once

#include "types.h"

#include <lumina/core/base.h>

#include <string>
#include <vector>

namespace nvrhi { class IInputLayout; }
namespace lumina::core { class device; }

namespace lumina::graphics
{
    class shader;

    enum class vertex_semantic
    {
		position,   // Vertex position
		color,      // Vertex color
		texcoord,   // Texture coordinates
		normal,     // Vertex normal
		tangent,    // Vertex tangent
		custom      // Custom attribute 
    };

    struct vertex_attribute
    {
        std::string name;                                   // Shader input name (e.g., "POSITION", "COLOR")
        format attr_format;                                 // Data format (e.g., format::rgba32_float)
        uint32_t offset = 0;                                // Byte offset within vertex
		vertex_semantic semantic = vertex_semantic::custom; // Semantic hint

        vertex_attribute() = default;
        vertex_attribute(const std::string& name, format fmt, uint32_t offset = 0, vertex_semantic sem = vertex_semantic::custom)
            : name(name), attr_format(fmt), offset(offset), semantic(sem) {}
    };

    struct input_layout_desc
    {
        std::vector<vertex_attribute> attributes;
        uint32_t stride = 0;

        input_layout_desc& add(const std::string& name, format fmt, vertex_semantic sem = vertex_semantic::custom)
        {
            uint32_t offset = stride;
            attributes.push_back({ name, fmt, offset, sem });
            stride += static_cast<uint32_t>(format_bytes_per_pixel(fmt));
            return *this;
        }

        input_layout_desc& add_position(format fmt = format::rgba32_float)
        {
            return add("POSITION", fmt, vertex_semantic::position);
        }

        input_layout_desc& add_color(format fmt = format::rgba32_float)
        {
            return add("COLOR", fmt, vertex_semantic::color);
        }

        input_layout_desc& add_texcoord(format fmt = format::rg32_float)
        {
            return add("TEXCOORD", fmt, vertex_semantic::texcoord);
        }

        input_layout_desc& add_normal(format fmt = format::rgba32_float)
        {
            return add("NORMAL", fmt, vertex_semantic::normal);
        }
    };

    class input_layout
    {
    public:
        ~input_layout();

        input_layout(const input_layout&) = delete;
        input_layout& operator=(const input_layout&) = delete;

        static ref<input_layout> create(core::device& dev, const input_layout_desc& desc, ref<shader> vertex_shader);

        const input_layout_desc& get_desc() const { return m_desc; }
        uint32_t get_stride() const { return m_desc.stride; }
        size_t get_attribute_count() const { return m_desc.attributes.size(); }

        nvrhi::IInputLayout* get_layout() const { return m_handle; }

    private:
        input_layout(core::device& dev, nvrhi::IInputLayout* handle, const input_layout_desc& desc)
            : m_device(dev)
            , m_handle(handle)
            , m_desc(desc)
        {}

        core::device& m_device;
        nvrhi::IInputLayout* m_handle;
        input_layout_desc m_desc;
    };

    namespace vertex_layouts
    {
        // Position only (3 floats)
        inline input_layout_desc position()
        {
            input_layout_desc desc;
            desc.add("POSITION", format::rgba32_float, vertex_semantic::position);
            return desc;
        }

        // Position + Color (3 + 4 floats)
        inline input_layout_desc position_color()
        {
            input_layout_desc desc;
            desc.add("POSITION", format::rgba32_float, vertex_semantic::position);
            desc.add("COLOR", format::rgba32_float, vertex_semantic::color);
            return desc;
        }

        // Position + TexCoord (3 + 2 floats)
        inline input_layout_desc position_texcoord()
        {
            input_layout_desc desc;
            desc.add("POSITION", format::rgba32_float, vertex_semantic::position);
            desc.add("TEXCOORD", format::rg32_float, vertex_semantic::texcoord);
            return desc;
        }

        // Position + Color + TexCoord (3 + 4 + 2 floats) - common for 2D
        inline input_layout_desc position_color_texcoord()
        {
            input_layout_desc desc;
            desc.add("POSITION", format::rgba32_float, vertex_semantic::position);
            desc.add("COLOR", format::rgba32_float, vertex_semantic::color);
            desc.add("TEXCOORD", format::rg32_float, vertex_semantic::texcoord);
            return desc;
        }

        // Full 3D vertex (position + normal + texcoord)
        inline input_layout_desc position_normal_texcoord()
        {
            input_layout_desc desc;
            desc.add("POSITION", format::rgba32_float, vertex_semantic::position);
            desc.add("NORMAL", format::rgba32_float, vertex_semantic::normal);
            desc.add("TEXCOORD", format::rg32_float, vertex_semantic::texcoord);
            return desc;
        }
    }
}
