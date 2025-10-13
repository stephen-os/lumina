#pragma once

#include <vector>
#include <memory>

#include "Lumina/Core/Base.h"

#include "Buffer.h"

namespace Lumina
{
    class VertexArray
    {
    public:
        static Ref<VertexArray> Create();

        VertexArray();
        ~VertexArray();

        void Bind() const;
        void Unbind() const;

        void SetVertexBuffer(Ref<VertexBuffer> vertexBuffer);
        void SetInstanceBuffer(Ref<VertexBuffer> instanceBuffer);
        void SetIndexBuffer(Ref<IndexBuffer> indexBuffer);

        const Ref<VertexBuffer>& GetVertexBuffer() const { return m_VertexBuffer; }
        const Ref<VertexBuffer>& GetInstanceBuffer() const { return m_InstanceBuffer; }
        const Ref<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }

        bool HasInstanceBuffer() const { return m_InstanceBuffer != nullptr; }

    private:
        uint32_t m_BufferID = 0;
        uint32_t m_AttributeLocationPtr = 0;

        Ref<VertexBuffer> m_VertexBuffer = nullptr;
        Ref<VertexBuffer> m_InstanceBuffer = nullptr;
        Ref<IndexBuffer> m_IndexBuffer = nullptr;

        void SetVertexAttributes(Ref<VertexBuffer> buffer, uint32_t divisor);
    };
}