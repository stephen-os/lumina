#pragma once

#include "Core/Base.h"

#include "FrameBuffer.h"

#include <glm/glm.hpp>

namespace Lumina
{
    class RenderTarget
    {
    public:
        static Ref<RenderTarget> Create(uint32_t width, uint32_t height);

        RenderTarget(uint32_t width, uint32_t height);

        void Bind();
        void Unbind();

        void Resize(uint32_t width, uint32_t height);
		void Resize(float width, float height);
        void Resize(const glm::vec2& size);

        bool SaveToFile(const std::string& path);

        uint32_t GetTexture() const { return m_FrameBuffer->GetColorAttachment(); }
        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }

    private:
        Ref<FrameBuffer> m_FrameBuffer;
        uint32_t m_Width;
        uint32_t m_Height;
    };
}