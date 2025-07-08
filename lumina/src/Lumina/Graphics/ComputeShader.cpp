#include "ComputeShader.h"

#include "../Core/Log.h"
#include "../Core/Assert.h"
#include "../Utils/FileReader.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include "RendererDebug.h"

namespace Lumina
{
    Ref<ComputeShader> Create(const std::string& source) 
    { 
        return Ref<ComputeShader>::Create(source); 
    }

    ComputeShader::ComputeShader(const std::string& source)
    {
        m_ComputeShaderID = CompileShader(source);
        LUMINA_ASSERT(m_ComputeShaderID != 0, "Compute shader compilation failed!");

        m_ShaderProgramID = glCreateProgram();
        LUMINA_ASSERT(m_ShaderProgramID != 0, "Failed to create compute shader program!");

        glAttachShader(m_ShaderProgramID, m_ComputeShaderID);
        glLinkProgram(m_ShaderProgramID);

        int isLinked;
        glGetProgramiv(m_ShaderProgramID, GL_LINK_STATUS, &isLinked);
        if (!isLinked)
        {
            char log[512];
            glGetProgramInfoLog(m_ShaderProgramID, 512, nullptr, log);
            glDeleteProgram(m_ShaderProgramID);
            m_ShaderProgramID = 0;
            LUMINA_LOG_ERROR("Compute shader linking failed:\n{0}", log);
        }

        // Extract name from first line (if "#name MyComputeShader")
        size_t namePos = source.find("#name ");
        if (namePos != std::string::npos)
        {
            size_t end = source.find('\n', namePos);
            m_Name = source.substr(namePos + 6, end - namePos - 6);
        }
        else
        {
            m_Name = "UnnamedComputeShader";
        }

        // Cache uniform locations
        int nuniforms = 0;
        glGetProgramiv(m_ShaderProgramID, GL_ACTIVE_UNIFORMS, &nuniforms);
        for (int i = 0; i < nuniforms; ++i)
        {
            char name[256];
            int size;
            GLenum type;
            glGetActiveUniform(m_ShaderProgramID, i, sizeof(name), nullptr, &size, &type, name);
            int location = glGetUniformLocation(m_ShaderProgramID, name);
            m_Uniforms[name] = location;
        }

        Unbind();
    }

    ComputeShader::~ComputeShader()
    {
        glDeleteShader(m_ComputeShaderID);
        glDeleteProgram(m_ShaderProgramID);
    }

    uint32_t ComputeShader::CompileShader(const std::string& source)
    {
        GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        int success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char log[512];
            glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
            glDeleteShader(shader);
            LUMINA_LOG_ERROR("Compute shader compilation failed:\n{0}", log);
            return 0;
        }

        return shader;
    }

    void ComputeShader::Bind() const
    {
        glUseProgram(m_ShaderProgramID);
    }

    void ComputeShader::Unbind() const
    {
        glUseProgram(0);
    }

    void ComputeShader::Dispatch(uint32_t groupX, uint32_t groupY, uint32_t groupZ) const
    {
        glDispatchCompute(groupX, groupY, groupZ);
    }

    void ComputeShader::InsertMemoryBarrier() const
    {
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }

    void ComputeShader::BindImageTexture(uint32_t unit, uint32_t textureID, bool read, bool write, int mipLevel)
    {
        GLenum access = (read && write) ? GL_READ_WRITE : (read ? GL_READ_ONLY : GL_WRITE_ONLY);
        glBindImageTexture(unit, textureID, mipLevel, GL_FALSE, 0, access, GL_RGBA8);
    }

    void ComputeShader::AssertUniform(const std::string& name)
    {
        if (m_Uniforms.find(name) == m_Uniforms.end())
        {
            LUMINA_LOG_ERROR("Uniform '{0}' not found in compute shader!", name);
        }
    }

    void ComputeShader::SetUniformInt(const std::string& name, int value)
    {
        AssertUniform(name);
        glUniform1i(m_Uniforms[name], value);
    }

    void ComputeShader::SetUniformFloat(const std::string& name, float value)
    {
        AssertUniform(name);
        glUniform1f(m_Uniforms[name], value);
    }

    void ComputeShader::SetUniformVec2(const std::string& name, float a, float b)
    {
        AssertUniform(name);
        glUniform2f(m_Uniforms[name], a, b);
    }

    void ComputeShader::SetUniformVec2(const std::string& name, const glm::vec2& value)
    {
        AssertUniform(name);
        glUniform2fv(m_Uniforms[name], 1, glm::value_ptr(value));
    }

    void ComputeShader::SetUniformVec3(const std::string& name, float a, float b, float c)
    {
        AssertUniform(name);
        glUniform3f(m_Uniforms[name], a, b, c);
    }

    void ComputeShader::SetUniformVec3(const std::string& name, const glm::vec3& value)
    {
        AssertUniform(name);
        glUniform3fv(m_Uniforms[name], 1, glm::value_ptr(value));
    }

    void ComputeShader::SetUniformMat4(const std::string& name, float a, float b, float c, float d)
    {
        AssertUniform(name);
        glUniform4f(m_Uniforms[name], a, b, c, d);
    }

    void ComputeShader::SetUniformMat4(const std::string& name, const glm::mat4& value)
    {
        AssertUniform(name);
        glUniformMatrix4fv(m_Uniforms[name], 1, GL_FALSE, glm::value_ptr(value));
    }

    void ComputeShader::SetStorageBuffer(const std::string& name, const void* data, size_t size)
    {
        GLuint blockIndex = glGetProgramResourceIndex(m_ShaderProgramID, GL_SHADER_STORAGE_BLOCK, name.c_str());
        if (blockIndex == GL_INVALID_INDEX)
        {
            LUMINA_LOG_ERROR("Shader storage block '{0}' not found in compute shader!", name);
            return;
        }

        GLint binding = -1;
        GLenum props[] = { GL_BUFFER_BINDING };
        glGetProgramResourceiv(m_ShaderProgramID, GL_SHADER_STORAGE_BLOCK, blockIndex, 1, props, 1, nullptr, &binding);

        // If the binding wasn't set in GLSL explicitly, assign one
        if (binding == -1)
        {
            binding = static_cast<GLint>(m_SSBOs.size()); // Next available
            glShaderStorageBlockBinding(m_ShaderProgramID, blockIndex, binding);
        }

        // Create and upload buffer
        GLuint ssbo = 0;
        glGenBuffers(1, &ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        m_SSBOs[name] = ssbo;
    }
}
