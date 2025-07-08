#include "Buffer.h"

#include <glad/glad.h>

#include "RendererDebug.h"
#include "../Core/Assert.h"

namespace Lumina
{
    // Vertex Buffer

    namespace Utils
    {
        static uint32_t UsageToEnum(BufferUsage usage)
        {
            switch (usage)
            {
            case BufferUsage::Static:   return GL_STATIC_DRAW; 
            case BufferUsage::Dynamic:  return GL_DYNAMIC_DRAW; 
            case BufferUsage::Stream:   return GL_STREAM_DRAW; 
            }
        }
    }


     Ref<VertexBuffer> VertexBuffer::Create(uint32_t size, BufferUsage usage)
    { 
        return Ref<VertexBuffer>::Create(size, usage);
    }

    Ref<VertexBuffer> VertexBuffer::Create(const void* data, uint32_t size, BufferUsage usage)
    { 
        return Ref<VertexBuffer>::Create(data, size, usage);
    }

    VertexBuffer::VertexBuffer(uint32_t size, BufferUsage usage) : m_Size(size), m_Usage(usage)
    {
        GLCALL(glCreateBuffers(1, &m_BufferID));
        LUMINA_ASSERT(m_BufferID != 0, "Failed to create vertex buffer!");

        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, m_BufferID));
        GLCALL(glBufferData(GL_ARRAY_BUFFER, size, nullptr, Utils::UsageToEnum(m_Usage)));
    }

    VertexBuffer::VertexBuffer(const void* vertices, uint32_t size, BufferUsage usage) : m_Size(size), m_Usage(usage)
    {
        GLCALL(glCreateBuffers(1, &m_BufferID));
        LUMINA_ASSERT(m_BufferID != 0, "Failed to create vertex buffer!");

        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, m_BufferID));
        GLCALL(glBufferData(GL_ARRAY_BUFFER, size, vertices, Utils::UsageToEnum(m_Usage)));
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

		if (size <= m_Size)
		{
			GLCALL(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data));
            return; 
		}

        m_Size = size;

        GLCALL(glBufferData(GL_ARRAY_BUFFER, size, data, Utils::UsageToEnum(m_Usage)));
    }

    // Index Buffer

    Ref<IndexBuffer> IndexBuffer::Create(uint32_t * data, uint32_t count, BufferUsage usage)
    { 
        return Ref<IndexBuffer>::Create(data, count, usage);
    }

	IndexBuffer::IndexBuffer(uint32_t* data, uint32_t count, BufferUsage usage) : m_Count(count), m_Usage(usage)
    {
        LUMINA_ASSERT(data != nullptr, "Null index data passed to IndexBuffer!");
        LUMINA_ASSERT(count > 0, "Index buffer count is zero!");

        GLCALL(glCreateBuffers(1, &m_BufferID));
        LUMINA_ASSERT(m_BufferID != 0, "Failed to create index buffer!");

        GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID));
        GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, Utils::UsageToEnum(m_Usage)));
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

		uint32_t newSize = count * sizeof(uint32_t);
		uint32_t currentSize = m_Count * sizeof(uint32_t);

        m_Count = count;

        GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID));

        if (newSize <= currentSize)
        {
            GLCALL(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, newSize, data));
            return; 
        }

        GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, newSize, data, Utils::UsageToEnum(m_Usage)));
	}
}