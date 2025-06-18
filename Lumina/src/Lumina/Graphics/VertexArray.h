#pragma once

#include <vector>
#include <memory>

#include "Buffer.h"

#include "../Core/Ref.h"

namespace Lumina
{
    class VertexArray : public Referencable
    {
    public:
        static Ref<VertexArray> Create();

        VertexArray();
        ~VertexArray();

        void Bind() const;
        void Unbind() const;

        void SetVertexBuffer(Ref<VertexBuffer> vertexBuffer);
        void SetIndexBuffer(Ref<IndexBuffer> indexBuffer);

        const Ref<VertexBuffer> GetVertexBuffer() const { return m_VertexBuffer; }
        const Ref<IndexBuffer> GetIndexBuffer() const { return m_IndexBuffer; }

    private:
        uint32_t m_BufferID = 0;
        uint32_t m_VertexBufferIndex = 0;

        Ref<IndexBuffer> m_IndexBuffer = nullptr;
        Ref<VertexBuffer> m_VertexBuffer = nullptr;
    };
}