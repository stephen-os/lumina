#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>

#include <glm/glm.hpp>

#include "Core/Base.h"

namespace Lumina
{
    class ShaderProgram
    {
    public:
        static Ref<ShaderProgram> Create(std::string_view vertexSource, std::string_view fragmentSource);

        ShaderProgram(std::string_view vertexSource, std::string_view fragmentSource);
        ~ShaderProgram();

        void Bind() const ;
        void Unbind() const ;

        int GetAttributeLocation(const std::string& name);

        void SetUniformInt(const std::string& name, int value);
        void SetUniformFloat(const std::string& name, float value);

        void SetUniformVec2(const std::string& name, float a, float b);
        void SetUniformVec2(const std::string& name, const glm::vec2& value);

        void SetUniformVec3(const std::string& name, float a, float b, float c);
        void SetUniformVec3(const std::string& name, const glm::vec3& value);

        void SetUniformVec4(const std::string& name, float a, float b, float c, float d);
        void SetUniformVec4(const std::string& name, const glm::vec4& value);

        void SetUniformMat4(const std::string& name, const glm::mat4& value);
    
        unsigned int CompileSource(unsigned int type, const std::string& source);
        void AssertUniform(const std::string& name);

		uint32_t GetID() const { return m_ShaderProgramID; }
    private:
        unsigned int m_VertexShaderID = 0;
        unsigned int m_FragmentShaderID = 0;
        unsigned int m_ShaderProgramID = 0;

        std::unordered_map<std::string, int> m_Uniforms;
    };
}