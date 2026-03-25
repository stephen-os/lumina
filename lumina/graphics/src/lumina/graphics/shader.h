#pragma once

#include "types.h"

#include <lumina/core/base.h>

#include <nvrhi/nvrhi.h>

#include <cstddef>
#include <string>

namespace Lumina { class Device; }

namespace Lumina
{
    /// Configuration for shader creation from compiled bytecode.
    struct ShaderDesc
    {
        const void* VertexBlob = nullptr;
        size_t VertexSize = 0;
        const void* PixelBlob = nullptr;
        size_t PixelSize = 0;
        std::string VertexEntry = "main";
        std::string PixelEntry = "main";
        std::string DebugName = "Lumina Shader";
    };

    /// GPU shader program containing vertex and pixel shaders.
    /// Shaders are created from pre-compiled bytecode (DXBC/DXIL/SPIR-V).
    class Shader
    {
    public:
        ~Shader();

        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

        /// Creates a shader from vertex and pixel shader bytecode. Returns nullptr on failure.
        [[nodiscard]] static Ref<Shader> Create(
            Device& dev,
            const void* vertexBlob,
            size_t vertexSize,
            const void* pixelBlob,
            size_t pixelSize);

        /// Creates a shader from a descriptor. Returns nullptr on failure.
        [[nodiscard]] static Ref<Shader> Create(Device& dev, const ShaderDesc& desc);

        [[nodiscard]] bool IsValid() const noexcept { return m_VertexShader && m_PixelShader; }
        [[nodiscard]] nvrhi::IShader* GetVertexShader() const noexcept { return m_VertexShader.Get(); }
        [[nodiscard]] nvrhi::IShader* GetPixelShader() const noexcept { return m_PixelShader.Get(); }

    private:
        Shader(Device& dev, nvrhi::ShaderHandle vertexShader, nvrhi::ShaderHandle pixelShader)
            : m_Device(dev)
            , m_VertexShader(std::move(vertexShader))
            , m_PixelShader(std::move(pixelShader))
        {}

        Device& m_Device;
        nvrhi::ShaderHandle m_VertexShader;
        nvrhi::ShaderHandle m_PixelShader;
    };
}
