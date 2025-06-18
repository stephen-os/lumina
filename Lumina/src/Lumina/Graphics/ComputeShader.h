#pragma once

#include <memory>

#include <string>
#include <glm/glm.hpp>

#include "../Core/Ref.h"

namespace Lumina 
{
    class ComputeShader : public Referencable
    {
    public:
        static Ref<ComputeShader> Create(const std::string& source); 

        virtual ~ComputeShader() = default;

        // Bind and dispatch
        void Bind() const;
        void Unbind() const;
        void Dispatch(uint32_t groupX, uint32_t groupY, uint32_t groupZ) const;

        // Uniform setters
        void SetUniformInt(const std::string& name, int value);
        void SetUniformFloat(const std::string& name, float value);

        void SetUniformVec2(const std::string& name, float a, float b);
        void SetUniformVec2(const std::string& name, const glm::vec2& value);

        void SetUniformVec3(const std::string& name, float a, float b, float c);
        void SetUniformVec3(const std::string& name, const glm::vec3& value);

        void SetUniformMat4(const std::string& name, float a, float b, float c, float d);
        void SetUniformMat4(const std::string& name, const glm::mat4& value);

		void SetStorageBuffer(const std::string& name, const void* data, size_t size);

        // Bind texture to image unit
        void BindImageTexture(uint32_t unit, uint32_t textureID, bool read, bool write, int mipLevel = 0);

        // Optional: memory barrier to synchronize
        void InsertMemoryBarrier() const;

        // Debugging
        const std::string& GetName() const;

    private: 
        ComputeShader(const std::string& source);

        uint32_t CompileShader(const std::string& source);
        void AssertUniform(const std::string& name);
    private: 
        uint32_t m_ShaderProgramID = 0;
        uint32_t m_ComputeShaderID = 0;
        std::unordered_map<std::string, int> m_Uniforms;
        std::unordered_map<std::string, int> m_SSBOs;
        std::string m_Name;
    };
}
