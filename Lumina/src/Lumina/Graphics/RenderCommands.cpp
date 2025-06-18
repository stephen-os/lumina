#include "RenderCommands.h"

#include <glad/glad.h>
#include "RendererDebug.h"

#include "../Core/Assert.h"

namespace Lumina
{
    void RenderCommands::SetViewport(int x, int y, int width, int height)
    {
        LUMINA_ASSERT(width > 0 && height > 0, "Viewport dimensions must be greater than zero!");

        GLCALL(glViewport(x, y, width, height));
    }

    void RenderCommands::Clear()
    {
        GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
        GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    }

    void RenderCommands::EnableDepthTest()
    {
        GLCALL(glEnable(GL_DEPTH_TEST));
    }

    void RenderCommands::DrawLines(const Ref<VertexArray>& vao, uint32_t count)
    {
        GLCALL(glDrawArrays(GL_LINES, 0, count));
    }

    void RenderCommands::DrawLineStrips(const Ref<VertexArray>& vao, uint32_t count)
    {
        vao->Bind();
        GLCALL(glDrawArrays(GL_LINE_STRIP, 0, count));
        vao->Unbind();
    }

    void RenderCommands::DrawTriangles(const Ref<VertexArray>& vao)
    {
        vao->Bind();
        const auto ib = vao->GetIndexBuffer();

        GLCALL(glDrawElements(GL_TRIANGLES, ib->GetCount(), GL_UNSIGNED_INT, nullptr));

        vao->Unbind();
    }
}