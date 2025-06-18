#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>

#include <glm/glm.hpp>

#include "../Core/Ref.h"

namespace Lumina
{
    class ShaderProgram : public Referencable
    {
    public:
        static Ref<ShaderProgram> Create(const std::string& vertexSource, const std::string& fragmentSource);

        ShaderProgram(const std::string& vertexSource, const std::string& fragmentSource);
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

        void SetUniformMat4(const std::string& name, float a, float b, float c, float d);
        void SetUniformMat4(const std::string& name, const glm::mat4& value);
    
        unsigned int CompileSource(unsigned int type, const std::string& source);
        void AssertUniform(const std::string& name);
    private:
        unsigned int m_VertexShaderID = 0;
        unsigned int m_FragmentShaderID = 0;
        unsigned int m_ShaderProgramID = 0;

        std::unordered_map<std::string, int> m_Uniforms;
    };
}