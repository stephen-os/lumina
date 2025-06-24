#pragma once

#include <memory>

#include "VertexArray.h"

#include "../Core/Ref.h"

namespace Lumina
{
    class RenderCommands 
    {
    public:
        static void SetViewport(int x, int y, int width, int height);
        static void Clear();
        static void EnableDepthTest();

        static void SetLineWidth(float width);

        static void DrawLines(const Ref<VertexArray>& vao, uint32_t count);
        static void DrawLineStrips(const Ref<VertexArray>& vao, uint32_t count);
        static void DrawTriangles(const Ref<VertexArray>& vao);
    };
}