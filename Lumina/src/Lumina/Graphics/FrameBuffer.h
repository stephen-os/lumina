#pragma once

#include <cstdint>

#include "../Core/Ref.h"

namespace Lumina
{
    class FrameBuffer : public Referencable
    {
    public:
        static Ref<FrameBuffer> Create();

        FrameBuffer();
        ~FrameBuffer();

        void Bind() const;
        void Unbind() const;
        void Resize(uint32_t width, uint32_t height);

        uint32_t GetID() const { return m_BufferID; }
        uint32_t GetColorAttachment() const { return m_ColorAttachment; }

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }

        void ReadPixels(int x, int y, uint32_t width, uint32_t height, void* data) const;

    private:
        uint32_t m_BufferID = 0;
        uint32_t m_ColorAttachment = 0;
        uint32_t m_DepthAttachment = 0;

        uint32_t m_Width = 900;
        uint32_t m_Height = 900;
    };
}