#pragma once

#include <cstdint>

#include <memory>

#include "BufferLayout.h"

#include "../Core/Ref.h"

namespace Lumina
{
    class VertexBuffer : public Referencable 
    {
    public:
        static Ref<VertexBuffer> Create(uint32_t size); 
        static Ref<VertexBuffer> Create(const void* data, uint32_t size); 

        ~VertexBuffer() = default;

        void Bind() const;
        void Unbind() const;
        void SetData(const void* data, uint32_t size);

        const BufferLayout& GetLayout() const { return m_Layout; };
        void SetLayout(const BufferLayout& layout) { m_Layout = layout; };

    private:
        VertexBuffer(uint32_t size); 
        VertexBuffer(const void* data, uint32_t size); 
    private: 
        uint32_t m_BufferID;
        BufferLayout m_Layout;
    };

    class IndexBuffer
    {
    public:
        static Ref<IndexBuffer> Create(uint32_t* data, uint32_t count); 

        virtual ~IndexBuffer() = default;

        void Bind() const; 
        void Unbind() const;

         uint32_t GetCount() const;

    private:
        IndexBuffer(uint32_t* data, uint32_t count); 
    private: 
        uint32_t m_BufferID; 
        uint32_t m_Count; 
    };
}
