#include "VertexArray.h"
#include "RendererDebug.h"

#include <glad/glad.h>

#include "../Core/Assert.h"
#include "../Core/Log.h"

#include "BufferLayout.h"

namespace Lumina
{
    Ref<VertexArray> VertexArray::Create()
    {
        return Ref<VertexArray>::Create();
    }

    VertexArray::VertexArray()
    {
        GLCALL(glCreateVertexArrays(1, &m_BufferID));
        LUMINA_ASSERT(m_BufferID != 0, "Failed to create OpenGL Vertex Array Object.");
    }

    VertexArray::~VertexArray()
    {
        LUMINA_ASSERT(m_BufferID != 0, "Trying to delete an invalid Vertex Array Object.");
        GLCALL(glDeleteVertexArrays(1, &m_BufferID));
    }

    void VertexArray::Bind() const
    {
        LUMINA_ASSERT(m_BufferID != 0, "Cannot bind VAO with invalid ID.");
        GLCALL(glBindVertexArray(m_BufferID));

        if (m_VertexBuffer)
            m_VertexBuffer->Bind();
        if (m_InstanceBuffer)
            m_InstanceBuffer->Bind();
        if (m_IndexBuffer)
            m_IndexBuffer->Bind();
    }

    void VertexArray::Unbind() const
    {
        GLCALL(glBindVertexArray(0));

        if (m_VertexBuffer)
            m_VertexBuffer->Unbind();
        if (m_InstanceBuffer)
            m_InstanceBuffer->Unbind();
        if (m_IndexBuffer)
            m_IndexBuffer->Unbind();
    }

    void VertexArray::SetVertexBuffer(Ref<VertexBuffer> vertexBuffer)
    {
        LUMINA_ASSERT(vertexBuffer != nullptr, "VertexBuffer passed to SetVertexBuffer is null.");

        // Set up attributes with divisor = 0 (per-vertex)
        SetVertexAttributes(vertexBuffer, 0);

        m_VertexBuffer.Reset();
        m_VertexBuffer = vertexBuffer;
    }

    void VertexArray::SetInstanceBuffer(Ref<VertexBuffer> instanceBuffer)
    {
        LUMINA_ASSERT(instanceBuffer != nullptr, "InstanceBuffer passed to SetInstanceBuffer is null.");

        // Set up attributes with divisor = 1 (per-instance)
        SetVertexAttributes(instanceBuffer, 1);

        m_InstanceBuffer.Reset();
        m_InstanceBuffer = instanceBuffer;
    }

    void VertexArray::SetIndexBuffer(Ref<IndexBuffer> indexBuffer)
    {
        LUMINA_ASSERT(indexBuffer != nullptr, "IndexBuffer passed to SetIndexBuffer is null.");

        Bind();
        indexBuffer->Bind();
        m_IndexBuffer = indexBuffer;
    }

    void VertexArray::SetVertexAttributes(Ref<VertexBuffer> buffer, uint32_t divisor)
    {
        LUMINA_ASSERT(buffer != nullptr, "Buffer passed to SetVertexAttributes is null.");

        Bind();
        buffer->Bind();

        const auto& layout = buffer->GetLayout();
        LUMINA_ASSERT(layout.GetElements().size() > 0, "Buffer has no layout elements defined.");

        for (const auto& element : layout)
        {
            switch (element.Type)
            {
            case BufferDataType::Float:
            {
                GLCALL(glEnableVertexAttribArray(m_VertexBufferIndex));
                GLCALL(glVertexAttribPointer(
                    m_VertexBufferIndex,
                    element.GetComponentCount(),
                    GL_FLOAT,
                    element.Normalized ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    (const void*)element.Offset));
                GLCALL(glVertexAttribDivisor(m_VertexBufferIndex, divisor));
                m_VertexBufferIndex++;
                break;
            }
            case BufferDataType::Float2:
            {
                GLCALL(glEnableVertexAttribArray(m_VertexBufferIndex));
                GLCALL(glVertexAttribPointer(
                    m_VertexBufferIndex,
                    element.GetComponentCount(),
                    GL_FLOAT,
                    element.Normalized ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    (const void*)element.Offset));
                GLCALL(glVertexAttribDivisor(m_VertexBufferIndex, divisor));
                m_VertexBufferIndex++;
                break;
            }
            case BufferDataType::Float3:
            {
                GLCALL(glEnableVertexAttribArray(m_VertexBufferIndex));
                GLCALL(glVertexAttribPointer(
                    m_VertexBufferIndex,
                    element.GetComponentCount(),
                    GL_FLOAT,
                    element.Normalized ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    (const void*)element.Offset));
                GLCALL(glVertexAttribDivisor(m_VertexBufferIndex, divisor));
                m_VertexBufferIndex++;
                break;
            }
            case BufferDataType::Float4:
            {
                GLCALL(glEnableVertexAttribArray(m_VertexBufferIndex));
                GLCALL(glVertexAttribPointer(m_VertexBufferIndex,
                    element.GetComponentCount(),
                    GL_FLOAT,
                    element.Normalized ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    (const void*)element.Offset));
                GLCALL(glVertexAttribDivisor(m_VertexBufferIndex, divisor));
                m_VertexBufferIndex++;
                break;
            }
            case BufferDataType::Int:
            case BufferDataType::Int2:
            case BufferDataType::Int3:
            case BufferDataType::Int4:
            case BufferDataType::Bool:
            {
                GLCALL(glEnableVertexAttribArray(m_VertexBufferIndex));
                GLCALL(glVertexAttribIPointer(m_VertexBufferIndex,
                    element.GetComponentCount(),
                    CalculateDataTypeSize(element.Type),
                    layout.GetStride(),
                    (const void*)element.Offset));
                GLCALL(glVertexAttribDivisor(m_VertexBufferIndex, divisor));
                m_VertexBufferIndex++;
                break;
            }
            case BufferDataType::Mat3:
            case BufferDataType::Mat4:
            {
                uint8_t count = element.GetComponentCount();
                for (uint8_t i = 0; i < count; i++)
                {
                    GLCALL(glEnableVertexAttribArray(m_VertexBufferIndex));
                    GLCALL(glVertexAttribPointer(m_VertexBufferIndex,
                        count,
                        GL_FLOAT, // Matrices are always float
                        element.Normalized ? GL_TRUE : GL_FALSE,
                        layout.GetStride(),
                        (const void*)(element.Offset + sizeof(float) * count * i)));
                    GLCALL(glVertexAttribDivisor(m_VertexBufferIndex, divisor));
                    m_VertexBufferIndex++;
                }
                break;
            }
            default:
                LUMINA_LOG_ERROR("Unknown BufferDataType: {0}", element.Name);
            }
        }
    }
}