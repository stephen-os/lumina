#include "Buffer.h"

#include <glad/glad.h>

#include "RendererDebug.h"
#include "../Core/Assert.h"

namespace Lumina
{
    // Vertex Buffer

    VertexBuffer::VertexBuffer(uint32_t size)
    {
        GLCALL(glCreateBuffers(1, &m_BufferID));
        LUMINA_ASSERT(m_BufferID != 0, "Failed to create vertex buffer!");

        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, m_BufferID));
        GLCALL(glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW));
    }

    VertexBuffer::VertexBuffer(const void* vertices, uint32_t size)
    {
        GLCALL(glCreateBuffers(1, &m_BufferID));
        LUMINA_ASSERT(m_BufferID != 0, "Failed to create vertex buffer!");

        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, m_BufferID));
        GLCALL(glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW));
    }

    VertexBuffer::~VertexBuffer()
    {
        GLCALL(glDeleteBuffers(1, &m_BufferID));
    }

    void VertexBuffer::Bind() const
    {
        LUMINA_ASSERT(m_BufferID != 0, "Trying to bind an invalid vertex buffer!");
        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, m_BufferID));
    }

    void VertexBuffer::Unbind() const
    {
        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }

    void VertexBuffer::SetData(const void* data, uint32_t size)
    {
        LUMINA_ASSERT(data != nullptr, "VertexBuffer::SetData called with null data!");
        LUMINA_ASSERT(size > 0, "VertexBuffer::SetData called with zero size!");

        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, m_BufferID));
        GLCALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
    }

    // Index Buffer

    IndexBuffer::IndexBuffer(uint32_t* indices, uint32_t count) : m_Count(count)
    {
        LUMINA_ASSERT(indices != nullptr, "Null index data passed to IndexBuffer!");
        LUMINA_ASSERT(count > 0, "Index buffer count is zero!");

        GLCALL(glCreateBuffers(1, &m_BufferID));
        LUMINA_ASSERT(m_BufferID != 0, "Failed to create index buffer!");

        GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID));
        GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW));
    }

    IndexBuffer::~IndexBuffer()
    {
        GLCALL(glDeleteBuffers(1, &m_BufferID));
    }

    void IndexBuffer::Bind() const
    {
        LUMINA_ASSERT(m_BufferID != 0, "Trying to bind an invalid index buffer!");
        GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID));
    }

    void IndexBuffer::Unbind() const
    {
        GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    }

	void IndexBuffer::SetData(uint32_t* data, uint32_t count)
	{
		LUMINA_ASSERT(data != nullptr, "IndexBuffer::SetData called with null data!");
		LUMINA_ASSERT(count > 0, "IndexBuffer::SetData called with zero count!");
		m_Count = count;
		GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID));
		GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_STATIC_DRAW));
	}
}