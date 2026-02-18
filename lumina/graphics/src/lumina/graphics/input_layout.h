#pragma once

#include "types.h"

#include <lumina/core/base.h>

#include <nvrhi/nvrhi.h>

#include <string>
#include <utility>
#include <vector>

namespace lumina::core { class device; }

namespace lumina::graphics
{
    class shader;

    /// Semantic hint for vertex attributes.
    enum class vertex_semantic
    {
        position,   // Vertex position
        color,      // Vertex color
        texcoord,   // Texture coordinates
        normal,     // Vertex normal
        tangent,    // Vertex tangent
        custom      // Custom attribute
    };

    /// Describes a single vertex attribute within a vertex buffer.
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

    /// Configuration for vertex input layout.
    /// Use the builder methods to construct layouts incrementally.
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

    /// GPU input layout describing vertex data format for the vertex shader.
    /// Defines how vertex buffer data maps to shader input attributes.
    class input_layout
    {
    public:
        ~input_layout();

        input_layout(const input_layout&) = delete;
        input_layout& operator=(const input_layout&) = delete;

        /// Creates an input layout. Returns nullptr on failure.
        [[nodiscard]] static ref<input_layout> create(
            core::device& dev,
            const input_layout_desc& desc,
            ref<shader> vertex_shader);

        [[nodiscard]] const input_layout_desc& get_desc() const noexcept { return m_desc; }
        [[nodiscard]] uint32_t get_stride() const noexcept { return m_desc.stride; }
        [[nodiscard]] size_t get_attribute_count() const noexcept { return m_desc.attributes.size(); }
        [[nodiscard]] nvrhi::IInputLayout* get_layout() const noexcept { return m_handle.Get(); }

    private:
        input_layout(core::device& dev, nvrhi::InputLayoutHandle handle, const input_layout_desc& desc)
            : m_device(dev)
            , m_handle(std::move(handle))
            , m_desc(desc)
        {}

        core::device& m_device;
        nvrhi::InputLayoutHandle m_handle;
        input_layout_desc m_desc;
    };

    /// Common predefined vertex layouts.
    namespace vertex_layouts
    {
        /// Position only (4 floats for xyzw)
        [[nodiscard]] inline input_layout_desc position() noexcept
        {
            input_layout_desc desc;
            desc.add("POSITION", format::rgba32_float, vertex_semantic::position);
            return desc;
        }

        /// Position + Color (4 + 4 floats)
        [[nodiscard]] inline input_layout_desc position_color() noexcept
        {
            input_layout_desc desc;
            desc.add("POSITION", format::rgba32_float, vertex_semantic::position);
            desc.add("COLOR", format::rgba32_float, vertex_semantic::color);
            return desc;
        }

        /// Position + TexCoord (4 + 2 floats)
        [[nodiscard]] inline input_layout_desc position_texcoord() noexcept
        {
            input_layout_desc desc;
            desc.add("POSITION", format::rgba32_float, vertex_semantic::position);
            desc.add("TEXCOORD", format::rg32_float, vertex_semantic::texcoord);
            return desc;
        }

        /// Position + Color + TexCoord (4 + 4 + 2 floats) - common for 2D
        [[nodiscard]] inline input_layout_desc position_color_texcoord() noexcept
        {
            input_layout_desc desc;
            desc.add("POSITION", format::rgba32_float, vertex_semantic::position);
            desc.add("COLOR", format::rgba32_float, vertex_semantic::color);
            desc.add("TEXCOORD", format::rg32_float, vertex_semantic::texcoord);
            return desc;
        }

        /// Full 3D vertex (position + normal + texcoord)
        [[nodiscard]] inline input_layout_desc position_normal_texcoord() noexcept
        {
            input_layout_desc desc;
            desc.add("POSITION", format::rgba32_float, vertex_semantic::position);
            desc.add("NORMAL", format::rgba32_float, vertex_semantic::normal);
            desc.add("TEXCOORD", format::rg32_float, vertex_semantic::texcoord);
            return desc;
        }
    }
}
