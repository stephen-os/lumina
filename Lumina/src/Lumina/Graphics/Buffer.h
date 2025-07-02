#pragma once
#include <cstdint>
#include <memory>
#include "BufferLayout.h"
#include "../Core/Ref.h"

namespace Lumina
{
    enum class BufferUsage
    {
        Static = 0,     // Data uploaded once, rarely modified (GL_STATIC_DRAW)
        Dynamic,        // Data modified frequently (GL_DYNAMIC_DRAW)
        Stream          // Data modified every frame (GL_STREAM_DRAW)
    };

    class VertexBuffer : public Referencable
    {
    public:
        static Ref<VertexBuffer> Create(uint32_t size, BufferUsage usage = BufferUsage::Static);
        static Ref<VertexBuffer> Create(const void* data, uint32_t size, BufferUsage usage = BufferUsage::Static);

        VertexBuffer(uint32_t size, BufferUsage usage = BufferUsage::Static);
        VertexBuffer(const void* data, uint32_t size, BufferUsage usage = BufferUsage::Static);
        ~VertexBuffer();

        void Bind() const;
        void Unbind() const;
        void SetData(const void* data, uint32_t size);

        const BufferLayout& GetLayout() const { return m_Layout; }
        void SetLayout(const BufferLayout& layout) { m_Layout = layout; }

        BufferUsage GetUsage() const { return m_Usage; }
        uint32_t GetSize() const { return m_Size; }

    private:
        uint32_t m_BufferID;
        uint32_t m_Size;
        BufferUsage m_Usage;
        BufferLayout m_Layout;
    };

    class IndexBuffer : public Referencable
    {
    public:
        static Ref<IndexBuffer> Create(uint32_t* data, uint32_t count, BufferUsage usage = BufferUsage::Static);

        IndexBuffer(uint32_t* data, uint32_t count, BufferUsage usage = BufferUsage::Static);
        virtual ~IndexBuffer();

        void Bind() const;
        void Unbind() const;
        void SetData(uint32_t* data, uint32_t count);

        uint32_t GetCount() const { return m_Count; }
        BufferUsage GetUsage() const { return m_Usage; }

    private:
        uint32_t m_BufferID;
        uint32_t m_Count;
        BufferUsage m_Usage;
    };
}