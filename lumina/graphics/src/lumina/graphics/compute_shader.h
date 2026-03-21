#pragma once

#include "types.h"

#include <lumina/core/base.h>

#include <nvrhi/nvrhi.h>

#include <cstddef>
#include <string>

namespace lumina::core { class device; }

namespace lumina::graphics
{
    /// Configuration for compute shader creation from compiled bytecode.
    struct compute_shader_desc
    {
        const void* bytecode = nullptr;
        size_t bytecode_size = 0;
        std::string entry_point = "main";
        std::string debug_name = "Lumina Compute Shader";
    };

    /// GPU compute shader for general-purpose GPU computation.
    /// Shaders are created from pre-compiled bytecode (DXBC/DXIL/SPIR-V).
    class compute_shader
    {
    public:
        ~compute_shader();

        compute_shader(const compute_shader&) = delete;
        compute_shader& operator=(const compute_shader&) = delete;

        /// Creates a compute shader from bytecode. Returns nullptr on failure.
        [[nodiscard]] static ref<compute_shader> create(
            core::device& dev,
            const void* bytecode,
            size_t bytecode_size,
            std::string_view debug_name = "Lumina Compute Shader");

        /// Creates a compute shader from a descriptor. Returns nullptr on failure.
        [[nodiscard]] static ref<compute_shader> create(core::device& dev, const compute_shader_desc& desc);

        [[nodiscard]] bool is_valid() const noexcept { return m_shader != nullptr; }
        [[nodiscard]] nvrhi::IShader* get_shader() const noexcept { return m_shader.Get(); }

    private:
        compute_shader(core::device& dev, nvrhi::ShaderHandle shader)
            : m_device(dev)
            , m_shader(std::move(shader))
        {}

        core::device& m_device;
        nvrhi::ShaderHandle m_shader;
    };
}
