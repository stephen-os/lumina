#include "Buffer.h"

#include <glad/glad.h>

#include "RendererDebug.h"

#include "Lumina/Core/Assert.h"

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
            default: LUMINA_ASSERT(false, "Unknown BufferUsage enum value!");
            }
        }
    }

     Ref<VertexBuffer> VertexBuffer::Create(uint32_t size, BufferUsage usage)
    { 
        return CreateRef<VertexBuffer>(size, usage);
    }

    Ref<VertexBuffer> VertexBuffer::Create(const void* data, uint32_t size, BufferUsage usage)
    { 
        return CreateRef<VertexBuffer>(data, size, usage);
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
		}
        else
        {
            GLCALL(glBufferData(GL_ARRAY_BUFFER, size, data, Utils::UsageToEnum(m_Usage)));
        }

        m_Size = size;
    }

    // Index Buffer
     
    Ref<IndexBuffer> IndexBuffer::Create(uint32_t * data, uint32_t count, BufferUsage usage)
    { 
        return CreateRef<IndexBuffer>(data, count, usage);
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

	// Uniform Buffer
    Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, BufferUsage usage)
    {
        return CreateRef<UniformBuffer>(size, usage);
    }

    Ref<UniformBuffer> UniformBuffer::Create(const void* data, uint32_t size, BufferUsage usage)
    {
        return CreateRef<UniformBuffer>(data, size, usage);
    }

    UniformBuffer::UniformBuffer(uint32_t size, BufferUsage usage) : m_Size(size), m_Usage(usage)
    {
        GLCALL(glCreateBuffers(1, &m_BufferID));
        LUMINA_ASSERT(m_BufferID != 0, "Failed to create uniform buffer!");

        GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, m_BufferID));
        GLCALL(glBufferData(GL_UNIFORM_BUFFER, size, nullptr, Utils::UsageToEnum(m_Usage)));
        GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
    }

    UniformBuffer::UniformBuffer(const void* data, uint32_t size, BufferUsage usage) : m_Size(size), m_Usage(usage)
    {
        LUMINA_ASSERT(data != nullptr, "Null data passed to UniformBuffer constructor!");
        LUMINA_ASSERT(size > 0, "Uniform buffer size is zero!");

        GLCALL(glCreateBuffers(1, &m_BufferID));
        LUMINA_ASSERT(m_BufferID != 0, "Failed to create uniform buffer!");

        GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, m_BufferID));
        GLCALL(glBufferData(GL_UNIFORM_BUFFER, size, data, Utils::UsageToEnum(m_Usage)));
        GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
    }

    UniformBuffer::~UniformBuffer()
    {
        GLCALL(glDeleteBuffers(1, &m_BufferID));
    }

    void UniformBuffer::Bind(uint32_t bindingPoint) const
    {
        LUMINA_ASSERT(m_BufferID != 0, "Trying to bind an invalid uniform buffer!");
        GLCALL(glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_BufferID));
        m_CurrentBindingPoint = bindingPoint;
    }

    void UniformBuffer::Unbind() const
    {
        GLCALL(glBindBufferBase(GL_UNIFORM_BUFFER, m_CurrentBindingPoint, 0));
    }

    void UniformBuffer::SetData(const void* data, uint32_t size)
    {
        LUMINA_ASSERT(data != nullptr, "UniformBuffer::SetData called with null data!");
        LUMINA_ASSERT(size > 0, "UniformBuffer::SetData called with zero size!");

        GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, m_BufferID));

        if (size <= m_Size)
        {
            GLCALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data));
        }
        else
        {
            GLCALL(glBufferData(GL_UNIFORM_BUFFER, size, data, Utils::UsageToEnum(m_Usage)));
            m_Size = size;
        }

        GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
    }

    void UniformBuffer::SetSubData(const void* data, uint32_t size, uint32_t offset)
    {
        LUMINA_ASSERT(data != nullptr, "UniformBuffer::SetSubData called with null data!");
        LUMINA_ASSERT(size > 0, "UniformBuffer::SetSubData called with zero size!");
        LUMINA_ASSERT(offset + size <= m_Size, "UniformBuffer::SetSubData: offset + size exceeds buffer size!");

        GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, m_BufferID));
        GLCALL(glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data));
        GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
    }
}