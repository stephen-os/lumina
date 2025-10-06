#include "ShaderProgram.h"

#include "../Utils/FileReader.h"

#include "RendererDebug.h"

#include <glad/glad.h>

#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace Lumina
{
    Ref<ShaderProgram> ShaderProgram::Create(std::string_view vertexSource, std::string_view fragmentSource)
	{
		return CreateRef<ShaderProgram>(vertexSource, fragmentSource);
	}

    ShaderProgram::ShaderProgram(std::string_view vertexSource, std::string_view fragmentSource)
    {
        m_VertexShaderID = CompileSource(GL_VERTEX_SHADER, vertexSource.data());
        LUMINA_ASSERT(m_VertexShaderID != 0, "Vertex shader compilation failed!");

        m_FragmentShaderID = CompileSource(GL_FRAGMENT_SHADER, fragmentSource.data());
        LUMINA_ASSERT(m_FragmentShaderID != 0, "Fragment shader compilation failed!");

        m_ShaderProgramID = glCreateProgram();
        LUMINA_ASSERT(m_ShaderProgramID != 0, "Failed to create shader program!");

        GLCALL(glAttachShader(m_ShaderProgramID, m_VertexShaderID));
        GLCALL(glAttachShader(m_ShaderProgramID, m_FragmentShaderID));
        GLCALL(glLinkProgram(m_ShaderProgramID));

        int isOkay;
        GLCALL(glGetProgramiv(m_ShaderProgramID, GL_LINK_STATUS, &isOkay));
        if (!isOkay)
        {
            char message[512];
            GLCALL(glGetProgramInfoLog(m_ShaderProgramID, 512, nullptr, message));
            GLCALL(glDeleteProgram(m_ShaderProgramID));
            m_ShaderProgramID = 0;

            LUMINA_LOG_ERROR("Shader program linking error:\n{}", message);
        }

        // Query uniforms
        m_Uniforms.clear();
        int nuniforms;
        GLCALL(glGetProgramiv(m_ShaderProgramID, GL_ACTIVE_UNIFORMS, &nuniforms));
        for (GLint i = 0; i < nuniforms; ++i)
        {
            char name[256];
            int length;
            int size;
            unsigned int type;
            GLCALL(glGetActiveUniform(m_ShaderProgramID, i, 256, &length, &size, &type, name));
            int location;
            location = glGetUniformLocation(m_ShaderProgramID, name);
            m_Uniforms[name] = location;

            for (int elementIndex = 1; elementIndex < size; ++elementIndex)
            {
                std::string elementName = std::string(name).substr(0, length - 3) + "[" + std::to_string(elementIndex) + "]";
                location = glGetUniformLocation(m_ShaderProgramID, elementName.c_str());
                if (location != -1)
                {
                    m_Uniforms[elementName] = location;
                }
            }
        }

        Unbind();
    }

    ShaderProgram::~ShaderProgram()
    {
        GLCALL(glDeleteShader(m_VertexShaderID));
        GLCALL(glDeleteShader(m_FragmentShaderID));
        GLCALL(glDeleteProgram(m_ShaderProgramID));
    }

    unsigned int ShaderProgram::CompileSource(unsigned int type, const std::string& source)
    {
        unsigned int shader;
        shader = glCreateShader(type);
        LUMINA_ASSERT(shader != 0, "Failed to create shader!");

        const char* src = source.c_str();
        GLCALL(glShaderSource(shader, 1, &src, nullptr));
        GLCALL(glCompileShader(shader));

        int success;
        char message[512];
        GLCALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
        if (!success)
        {
            GLCALL(glGetShaderInfoLog(shader, 512, nullptr, message));
            GLCALL(glDeleteShader(shader));


            LUMINA_LOG_ERROR("Shader compilation error: {}\n{}", (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment"), message);
        }

        return shader;
    }

    int ShaderProgram::GetAttributeLocation(const std::string& name)
    {
        int location;
        location = glGetAttribLocation(m_ShaderProgramID, name.c_str());
        if (location == -1)
        {
            LUMINA_LOG_ERROR("Attribute location for {} is not found", name);
            return -1;
        }
        return location;
    }

    void ShaderProgram::Bind() const
    {
        LUMINA_ASSERT(m_ShaderProgramID != 0, "Cannot bind an invalid shader program!");
        GLCALL(glUseProgram(m_ShaderProgramID));
    }

    void ShaderProgram::Unbind() const
    {
        GLCALL(glUseProgram(0));
    }

    void ShaderProgram::AssertUniform(const std::string& name)
    {
        if (m_Uniforms.find(name) == m_Uniforms.end())
        {
            LUMINA_LOG_ERROR("{} isn't a valid uniform.", name);
        }
    }

    void ShaderProgram::SetUniformInt(const std::string& name, int value)
    {
        AssertUniform(name);
        GLCALL(glUniform1i(m_Uniforms[name], value));
    }

    void ShaderProgram::SetUniformFloat(const std::string& name, float value)
    {
        AssertUniform(name);
        GLCALL(glUniform1f(m_Uniforms[name], value));
    }

    void ShaderProgram::SetUniformVec2(const std::string& name, float a, float b)
    {
        AssertUniform(name);
        GLCALL(glUniform2f(m_Uniforms[name], a, b));
    }

    void ShaderProgram::SetUniformVec2(const std::string& name, const glm::vec2& value)
    {
        AssertUniform(name);
        GLCALL(glUniform2fv(m_Uniforms[name], 1, glm::value_ptr(value)));
    }

    void ShaderProgram::SetUniformVec3(const std::string& name, float a, float b, float c)
    {
        AssertUniform(name);
        GLCALL(glUniform3f(m_Uniforms[name], a, b, c));
    }

    void ShaderProgram::SetUniformVec3(const std::string& name, const glm::vec3& value)
    {
        AssertUniform(name);
        GLCALL(glUniform3fv(m_Uniforms[name], 1, glm::value_ptr(value)));
    }

    void ShaderProgram::SetUniformVec4(const std::string& name, float a, float b, float c, float d)
    {
        AssertUniform(name);
        GLCALL(glUniform4f(m_Uniforms[name], a, b, c, d));
    }

    void ShaderProgram::SetUniformVec4(const std::string& name, const glm::vec4& value)
    {
        AssertUniform(name);
        GLCALL(glUniform4fv(m_Uniforms[name], 1, glm::value_ptr(value)));
    }

    void ShaderProgram::SetUniformMat4(const std::string& name, const glm::mat4& matrix)
    {
        AssertUniform(name);
        GLCALL(glUniformMatrix4fv(m_Uniforms[name], 1, GL_FALSE, glm::value_ptr(matrix)));
    }
}