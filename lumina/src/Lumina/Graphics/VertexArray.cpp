#include "VertexArray.h"

#include <glad/glad.h>

#include "BufferLayout.h"
#include "RendererDebug.h"

namespace Lumina
{
    Ref<VertexArray> VertexArray::Create()
    {
        return CreateRef<VertexArray>();
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

        SetVertexAttributes(vertexBuffer, 0);

        m_VertexBuffer.reset();
        m_VertexBuffer = vertexBuffer;
    }

    void VertexArray::SetInstanceBuffer(Ref<VertexBuffer> instanceBuffer)
    {
        LUMINA_ASSERT(instanceBuffer != nullptr, "InstanceBuffer passed to SetInstanceBuffer is null.");

        SetVertexAttributes(instanceBuffer, 1);

        m_InstanceBuffer.reset();
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
            if (element.IsPadding())
            {
                continue;
            }

            switch (element.Type)
            {
            case BufferDataType::Float:
            {
                GLCALL(glEnableVertexAttribArray(m_AttributeLocationPtr));
                GLCALL(glVertexAttribPointer(
                    m_AttributeLocationPtr,
                    element.GetComponentCount(),
                    GL_FLOAT,
                    element.Normalized ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    (const void*)element.Offset)
                );
                GLCALL(glVertexAttribDivisor(m_AttributeLocationPtr, divisor));
                m_AttributeLocationPtr++;
                break;
            }
            case BufferDataType::Float2:
            {
                GLCALL(glEnableVertexAttribArray(m_AttributeLocationPtr));
                GLCALL(glVertexAttribPointer(
                    m_AttributeLocationPtr,
                    element.GetComponentCount(),
                    GL_FLOAT,
                    element.Normalized ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    (const void*)element.Offset)
                );
                GLCALL(glVertexAttribDivisor(m_AttributeLocationPtr, divisor));
                m_AttributeLocationPtr++;
                break;
            }
            case BufferDataType::Float3:
            {
                GLCALL(glEnableVertexAttribArray(m_AttributeLocationPtr));
                GLCALL(glVertexAttribPointer(
                    m_AttributeLocationPtr,
                    element.GetComponentCount(),
                    GL_FLOAT,
                    element.Normalized ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    (const void*)element.Offset)
                );
                GLCALL(glVertexAttribDivisor(m_AttributeLocationPtr, divisor));
                m_AttributeLocationPtr++;
                break;
            }
            case BufferDataType::Float4:
            {
                GLCALL(glEnableVertexAttribArray(m_AttributeLocationPtr));
                GLCALL(glVertexAttribPointer(
                    m_AttributeLocationPtr,
                    element.GetComponentCount(),
                    GL_FLOAT,
                    element.Normalized ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    (const void*)element.Offset)
                );
                GLCALL(glVertexAttribDivisor(m_AttributeLocationPtr, divisor));
                m_AttributeLocationPtr++;
                break;
            }
            case BufferDataType::Int:
            {
                GLCALL(glEnableVertexAttribArray(m_AttributeLocationPtr));
                GLCALL(glVertexAttribIPointer(
                    m_AttributeLocationPtr,
                    element.GetComponentCount(),
                    GL_INT,
                    layout.GetStride(),
                    (const void*)element.Offset)
                );
                GLCALL(glVertexAttribDivisor(m_AttributeLocationPtr, divisor));
                m_AttributeLocationPtr++;
                break;
            }
            case BufferDataType::Int2:
            {
                GLCALL(glEnableVertexAttribArray(m_AttributeLocationPtr));
                GLCALL(glVertexAttribIPointer(
                    m_AttributeLocationPtr,
                    element.GetComponentCount(),
                    GL_INT,
                    layout.GetStride(),
                    (const void*)element.Offset)
                );
                GLCALL(glVertexAttribDivisor(m_AttributeLocationPtr, divisor));
                m_AttributeLocationPtr++;
                break;
            }
            case BufferDataType::Int3:
            {
                GLCALL(glEnableVertexAttribArray(m_AttributeLocationPtr));
                GLCALL(glVertexAttribIPointer(
                    m_AttributeLocationPtr,
                    element.GetComponentCount(),
                    GL_INT,
                    layout.GetStride(),
                    (const void*)element.Offset)
                );
                GLCALL(glVertexAttribDivisor(m_AttributeLocationPtr, divisor));
                m_AttributeLocationPtr++;
                break;
            }
            case BufferDataType::Int4:
            {
                GLCALL(glEnableVertexAttribArray(m_AttributeLocationPtr));
                GLCALL(glVertexAttribIPointer(
                    m_AttributeLocationPtr,
                    element.GetComponentCount(),
                    GL_INT,
                    layout.GetStride(),
                    (const void*)element.Offset)
                );
                GLCALL(glVertexAttribDivisor(m_AttributeLocationPtr, divisor));
                m_AttributeLocationPtr++;
                break;
            }
            case BufferDataType::Bool:
            {
                GLCALL(glEnableVertexAttribArray(m_AttributeLocationPtr));
                GLCALL(glVertexAttribIPointer(
                    m_AttributeLocationPtr,
                    element.GetComponentCount(),
                    GL_UNSIGNED_BYTE,
                    layout.GetStride(),
                    (const void*)element.Offset)
                );
                GLCALL(glVertexAttribDivisor(m_AttributeLocationPtr, divisor));
                m_AttributeLocationPtr++;
                break;
            }
            case BufferDataType::Mat3:
            {
                uint8_t count = element.GetComponentCount();
                for (uint8_t i = 0; i < count; i++)
                {
                    GLCALL(glEnableVertexAttribArray(m_AttributeLocationPtr));
                    GLCALL(glVertexAttribPointer(
                        m_AttributeLocationPtr,
                        count, 
                        GL_FLOAT,
                        element.Normalized ? GL_TRUE : GL_FALSE,
                        layout.GetStride(),
                        (const void*)(element.Offset + sizeof(float) * 3 * i))
                    );
                    GLCALL(glVertexAttribDivisor(m_AttributeLocationPtr, divisor));
                    m_AttributeLocationPtr++;
                }
                break;
            }
            case BufferDataType::Mat4:
            {
                uint8_t count = element.GetComponentCount();
                for (uint8_t i = 0; i < count; i++)
                {
                    GLCALL(glEnableVertexAttribArray(m_AttributeLocationPtr));
                    GLCALL(glVertexAttribPointer(
                        m_AttributeLocationPtr,
                        count,
                        GL_FLOAT,
                        element.Normalized ? GL_TRUE : GL_FALSE,
                        layout.GetStride(),
                        (const void*)(element.Offset + sizeof(float) * count * i))
                    );
                    GLCALL(glVertexAttribDivisor(m_AttributeLocationPtr, divisor));
                    m_AttributeLocationPtr++;
                }
                break;
            }
            default:
                LUMINA_LOG_ERROR("Unknown BufferDataType: {0}", element.Name);
            }
        }
    }
}