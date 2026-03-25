#pragma once

#include "Types.h"

#include <lumina/core/Base.h>

#include <nvrhi/nvrhi.h>

#include <cstddef>
#include <string>

namespace Lumina { class Device; }

namespace Lumina
{
    /// Configuration for compute shader creation from compiled bytecode.
    struct ComputeShaderDesc
    {
        const void* Bytecode = nullptr;
        size_t BytecodeSize = 0;
        std::string EntryPoint = "main";
        std::string DebugName = "Lumina Compute Shader";
    };

    /// GPU compute shader for general-purpose GPU computation.
    /// Shaders are created from pre-compiled bytecode (DXBC/DXIL/SPIR-V).
    class ComputeShader
    {
    public:
        ~ComputeShader();

        ComputeShader(const ComputeShader&) = delete;
        ComputeShader& operator=(const ComputeShader&) = delete;

        /// Creates a compute shader from bytecode. Returns nullptr on failure.
        [[nodiscard]] static Ref<ComputeShader> Create(
            Device& dev,
            const void* bytecode,
            size_t bytecodeSize,
            std::string_view debugName = "Lumina Compute Shader");

        /// Creates a compute shader from a descriptor. Returns nullptr on failure.
        [[nodiscard]] static Ref<ComputeShader> Create(Device& dev, const ComputeShaderDesc& desc);

        [[nodiscard]] bool IsValid() const noexcept { return m_Shader != nullptr; }
        [[nodiscard]] nvrhi::IShader* GetShader() const noexcept { return m_Shader.Get(); }

    private:
        ComputeShader(Device& dev, nvrhi::ShaderHandle shader)
            : m_Device(dev)
            , m_Shader(std::move(shader))
        {}

        Device& m_Device;
        nvrhi::ShaderHandle m_Shader;
    };
}
