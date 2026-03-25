#pragma once

#include "types.h"

#include <lumina/core/base.h>

#include <nvrhi/nvrhi.h>

#include <string>
#include <utility>
#include <vector>

namespace Lumina { class Device; }

namespace Lumina
{
    class Shader;

    /// Semantic hint for vertex attributes.
    enum class VertexSemantic
    {
        Position,   // Vertex position
        Color,      // Vertex color
        Texcoord,   // Texture coordinates
        Normal,     // Vertex normal
        Tangent,    // Vertex tangent
        Custom      // Custom attribute
    };

    /// Describes a single vertex attribute within a vertex buffer.
    struct VertexAttribute
    {
        std::string Name;                                   // Shader input name (e.g., "POSITION", "COLOR")
        Format AttrFormat;                                  // Data format (e.g., Format::RGBA32Float)
        uint32_t Offset = 0;                                // Byte offset within vertex
        VertexSemantic Semantic = VertexSemantic::Custom;   // Semantic hint

        VertexAttribute() = default;
        VertexAttribute(const std::string& name, Format fmt, uint32_t offset = 0, VertexSemantic sem = VertexSemantic::Custom)
            : Name(name), AttrFormat(fmt), Offset(offset), Semantic(sem) {}
    };

    /// Configuration for vertex input layout.
    /// Use the builder methods to construct layouts incrementally.
    struct InputLayoutDesc
    {
        std::vector<VertexAttribute> Attributes;
        uint32_t Stride = 0;

        InputLayoutDesc& Add(const std::string& name, Format fmt, VertexSemantic sem = VertexSemantic::Custom)
        {
            uint32_t offset = Stride;
            Attributes.push_back({ name, fmt, offset, sem });
            Stride += static_cast<uint32_t>(FormatBytesPerPixel(fmt));
            return *this;
        }

        InputLayoutDesc& AddPosition(Format fmt = Format::RGBA32Float)
        {
            return Add("POSITION", fmt, VertexSemantic::Position);
        }

        InputLayoutDesc& AddColor(Format fmt = Format::RGBA32Float)
        {
            return Add("COLOR", fmt, VertexSemantic::Color);
        }

        InputLayoutDesc& AddTexcoord(Format fmt = Format::RG32Float)
        {
            return Add("TEXCOORD", fmt, VertexSemantic::Texcoord);
        }

        InputLayoutDesc& AddNormal(Format fmt = Format::RGBA32Float)
        {
            return Add("NORMAL", fmt, VertexSemantic::Normal);
        }
    };

    /// GPU input layout describing vertex data format for the vertex shader.
    /// Defines how vertex buffer data maps to shader input attributes.
    class InputLayout
    {
    public:
        ~InputLayout();

        InputLayout(const InputLayout&) = delete;
        InputLayout& operator=(const InputLayout&) = delete;

        /// Creates an input layout. Returns nullptr on failure.
        [[nodiscard]] static Ref<InputLayout> Create(
            Device& dev,
            const InputLayoutDesc& desc,
            Ref<Shader> vertexShader);

        [[nodiscard]] const InputLayoutDesc& GetDesc() const noexcept { return m_Desc; }
        [[nodiscard]] uint32_t GetStride() const noexcept { return m_Desc.Stride; }
        [[nodiscard]] size_t GetAttributeCount() const noexcept { return m_Desc.Attributes.size(); }
        [[nodiscard]] nvrhi::IInputLayout* GetLayout() const noexcept { return m_Handle.Get(); }

    private:
        InputLayout(Device& dev, nvrhi::InputLayoutHandle handle, const InputLayoutDesc& desc)
            : m_Device(dev)
            , m_Handle(std::move(handle))
            , m_Desc(desc)
        {}

        Device& m_Device;
        nvrhi::InputLayoutHandle m_Handle;
        InputLayoutDesc m_Desc;
    };

    /// Common predefined vertex layouts.
    namespace VertexLayouts
    {
        /// Position only (4 floats for xyzw)
        [[nodiscard]] inline InputLayoutDesc Position() noexcept
        {
            InputLayoutDesc desc;
            desc.Add("POSITION", Format::RGBA32Float, VertexSemantic::Position);
            return desc;
        }

        /// Position + Color (4 + 4 floats)
        [[nodiscard]] inline InputLayoutDesc PositionColor() noexcept
        {
            InputLayoutDesc desc;
            desc.Add("POSITION", Format::RGBA32Float, VertexSemantic::Position);
            desc.Add("COLOR", Format::RGBA32Float, VertexSemantic::Color);
            return desc;
        }

        /// Position + TexCoord (4 + 2 floats)
        [[nodiscard]] inline InputLayoutDesc PositionTexcoord() noexcept
        {
            InputLayoutDesc desc;
            desc.Add("POSITION", Format::RGBA32Float, VertexSemantic::Position);
            desc.Add("TEXCOORD", Format::RG32Float, VertexSemantic::Texcoord);
            return desc;
        }

        /// Position + Color + TexCoord (4 + 4 + 2 floats) - common for 2D
        [[nodiscard]] inline InputLayoutDesc PositionColorTexcoord() noexcept
        {
            InputLayoutDesc desc;
            desc.Add("POSITION", Format::RGBA32Float, VertexSemantic::Position);
            desc.Add("COLOR", Format::RGBA32Float, VertexSemantic::Color);
            desc.Add("TEXCOORD", Format::RG32Float, VertexSemantic::Texcoord);
            return desc;
        }

        /// Full 3D vertex (position + normal + texcoord)
        [[nodiscard]] inline InputLayoutDesc PositionNormalTexcoord() noexcept
        {
            InputLayoutDesc desc;
            desc.Add("POSITION", Format::RGBA32Float, VertexSemantic::Position);
            desc.Add("NORMAL", Format::RGBA32Float, VertexSemantic::Normal);
            desc.Add("TEXCOORD", Format::RG32Float, VertexSemantic::Texcoord);
            return desc;
        }
    }
}
