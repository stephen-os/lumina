#include "RenderCommands.h"

#include <glad/glad.h>

#include "RendererDebug.h"

namespace Lumina
{
    RenderState RenderCommands::s_CurrentState;

    // Viewport and Clearing
    void RenderCommands::SetViewport(int x, int y, int width, int height)
    {
        GLCALL(glViewport(x, y, width, height));
    }

    void RenderCommands::SetClearColor(float r, float g, float b, float a)
    {
        GLCALL(glClearColor(r, g, b, a));
    }

    void RenderCommands::Clear()
    {
        GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
    }

    void RenderCommands::ClearColor()
    {
        GLCALL(glClear(GL_COLOR_BUFFER_BIT));
    }

    void RenderCommands::ClearDepth()
    {
        GLCALL(glClear(GL_DEPTH_BUFFER_BIT));
    }

    void RenderCommands::ClearStencil()
    {
        GLCALL(glClear(GL_STENCIL_BUFFER_BIT));
    }

    // Depth Testing
    void RenderCommands::EnableDepthTest()
    {
        GLCALL(glEnable(GL_DEPTH_TEST));
        s_CurrentState.depthTest = true;
    }

    void RenderCommands::DisableDepthTest()
    {
        GLCALL(glDisable(GL_DEPTH_TEST));
        s_CurrentState.depthTest = false;
    }

    void RenderCommands::SetDepthFunc(DepthFunc func)
    {
        GLCALL(glDepthFunc(ToGLEnum(func)));
        s_CurrentState.depthFunc = func;
    }

    void RenderCommands::SetDepthMask(bool writeEnabled)
    {
        GLCALL(glDepthMask(writeEnabled ? GL_TRUE : GL_FALSE));
        s_CurrentState.depthWrite = writeEnabled;
    }

    // Blending
    void RenderCommands::EnableBlending()
    {
        GLCALL(glEnable(GL_BLEND));
        s_CurrentState.blending = true;
    }

    void RenderCommands::DisableBlending()
    {
        GLCALL(glDisable(GL_BLEND));
        s_CurrentState.blending = false;
    }

    void RenderCommands::SetBlendFunc(BlendFactor sfactor, BlendFactor dfactor)
    {
        GLCALL(glBlendFunc(ToGLEnum(sfactor), ToGLEnum(dfactor)));
        s_CurrentState.srcBlend = sfactor;
        s_CurrentState.dstBlend = dfactor;
    }

    void RenderCommands::SetBlendEquation(BlendEquation mode)
    {
        GLCALL(glBlendEquation(ToGLEnum(mode)));
        s_CurrentState.blendEquation = mode;
    }

    // Face Culling
    void RenderCommands::EnableFaceCulling()
    {
        GLCALL(glEnable(GL_CULL_FACE));
        s_CurrentState.faceCulling = true;
    }

    void RenderCommands::DisableFaceCulling()
    {
        GLCALL(glDisable(GL_CULL_FACE));
        s_CurrentState.faceCulling = false;
    }

    void RenderCommands::SetCullFace(CullFace mode)
    {
        GLCALL(glCullFace(ToGLEnum(mode)));
        s_CurrentState.cullFace = mode;
    }

    void RenderCommands::SetFrontFace(FrontFace mode)
    {
        GLCALL(glFrontFace(ToGLEnum(mode)));
        s_CurrentState.frontFace = mode;
    }

    // Scissor Testing
    void RenderCommands::EnableScissorTest()
    {
        GLCALL(glEnable(GL_SCISSOR_TEST));
        s_CurrentState.scissorTest = true;
    }

    void RenderCommands::DisableScissorTest()
    {
        GLCALL(glDisable(GL_SCISSOR_TEST));
        s_CurrentState.scissorTest = false;
    }

    void RenderCommands::SetScissorBox(int x, int y, int width, int height)
    {
        LUMINA_ASSERT(width >= 0 && height >= 0, "Scissor box dimensions must be non-negative!");
        GLCALL(glScissor(x, y, width, height));
    }

    // Polygon and Line/Point Settings
    void RenderCommands::SetPolygonMode(PolygonMode mode)
    {
        GLCALL(glPolygonMode(GL_FRONT_AND_BACK, ToGLEnum(mode)));
        s_CurrentState.polygonMode = mode;
    }

    void RenderCommands::SetLineWidth(float width)
    {
        LUMINA_ASSERT(width > 0.0f, "Line width must be greater than zero!");
        GLCALL(glLineWidth(width));
        s_CurrentState.lineWidth = width;
    }

    void RenderCommands::SetPointSize(float size)
    {
        LUMINA_ASSERT(size > 0.0f, "Point size must be greater than zero!");
        GLCALL(glPointSize(size));
        s_CurrentState.pointSize = size;
    }

    // Point Size Program Control
    void RenderCommands::EnableProgramPointSize()
    {
        GLCALL(glEnable(GL_PROGRAM_POINT_SIZE));
    }

    void RenderCommands::DisableProgramPointSize()
    {
        GLCALL(glDisable(GL_PROGRAM_POINT_SIZE));
    }

    // State Management
    void RenderCommands::SetRenderState(const RenderState& state)
    {
        // Only change state if it's different to avoid unnecessary GL calls
        if (s_CurrentState.depthTest != state.depthTest)
        {
            if (state.depthTest) EnableDepthTest();
            else DisableDepthTest();
        }

        if (s_CurrentState.depthFunc != state.depthFunc)
            SetDepthFunc(state.depthFunc);

        if (s_CurrentState.depthWrite != state.depthWrite)
            SetDepthMask(state.depthWrite);

        if (s_CurrentState.blending != state.blending)
        {
            if (state.blending) EnableBlending();
            else DisableBlending();
        }

        if (s_CurrentState.srcBlend != state.srcBlend || s_CurrentState.dstBlend != state.dstBlend)
            SetBlendFunc(state.srcBlend, state.dstBlend);

        if (s_CurrentState.blendEquation != state.blendEquation)
            SetBlendEquation(state.blendEquation);

        if (s_CurrentState.faceCulling != state.faceCulling)
        {
            if (state.faceCulling) EnableFaceCulling();
            else DisableFaceCulling();
        }

        if (s_CurrentState.cullFace != state.cullFace)
            SetCullFace(state.cullFace);

        if (s_CurrentState.frontFace != state.frontFace)
            SetFrontFace(state.frontFace);

        if (s_CurrentState.scissorTest != state.scissorTest)
        {
            if (state.scissorTest) EnableScissorTest();
            else DisableScissorTest();
        }

        if (s_CurrentState.polygonMode != state.polygonMode)
            SetPolygonMode(state.polygonMode);

        if (s_CurrentState.lineWidth != state.lineWidth)
            SetLineWidth(state.lineWidth);

        if (s_CurrentState.pointSize != state.pointSize)
            SetPointSize(state.pointSize);
    }

    RenderState RenderCommands::GetRenderState()
    {
        return s_CurrentState;
    }

    // Basic Drawing Commands
    void RenderCommands::DrawArrays(const Ref<VertexArray>& vao, PrimitiveType primitive, uint32_t count, uint32_t offset)
    {
        LUMINA_ASSERT(vao, "VertexArray cannot be null!");
        LUMINA_ASSERT(count > 0, "Vertex count must be greater than zero!");

        vao->Bind();
        GLCALL(glDrawArrays(ToGLEnum(primitive), offset, count));
        vao->Unbind();
    }

    void RenderCommands::DrawElements(const Ref<VertexArray>& vao, PrimitiveType primitive)
    {
        LUMINA_ASSERT(vao, "VertexArray cannot be null!");

        vao->Bind();
        const auto ib = vao->GetIndexBuffer();
        LUMINA_ASSERT(ib, "IndexBuffer cannot be null for indexed drawing!");
        GLCALL(glDrawElements(ToGLEnum(primitive), ib->GetCount(), GL_UNSIGNED_INT, nullptr));
        vao->Unbind();
    }

    // Instanced Drawing
    void RenderCommands::DrawArraysInstanced(const Ref<VertexArray>& vao, PrimitiveType primitive, uint32_t count, uint32_t instanceCount, uint32_t offset)
    {
        LUMINA_ASSERT(vao, "VertexArray cannot be null!");
        LUMINA_ASSERT(count > 0, "Vertex count must be greater than zero!");
        LUMINA_ASSERT(instanceCount > 0, "Instance count must be greater than zero!");

        vao->Bind();
        GLCALL(glDrawArraysInstanced(ToGLEnum(primitive), offset, count, instanceCount));
        vao->Unbind();
    }

    void RenderCommands::DrawElementsInstanced(const Ref<VertexArray>& vao, PrimitiveType primitive, uint32_t instanceCount)
    {
        LUMINA_ASSERT(vao, "VertexArray cannot be null!");
        LUMINA_ASSERT(instanceCount > 0, "Instance count must be greater than zero!");

        vao->Bind();
        const auto ib = vao->GetIndexBuffer();
        LUMINA_ASSERT(ib, "IndexBuffer cannot be null for indexed drawing!");
        GLCALL(glDrawElementsInstanced(ToGLEnum(primitive), ib->GetCount(), GL_UNSIGNED_INT, nullptr, instanceCount));
        vao->Unbind();
    }

    void RenderCommands::DrawElementsWithCount(const Ref<VertexArray>& vao, PrimitiveType primitive, uint32_t indexCount)
    {
        LUMINA_ASSERT(vao, "VertexArray cannot be null!");
        LUMINA_ASSERT(indexCount > 0, "Index count must be greater than zero!");

        vao->Bind();
        GLCALL(glDrawElements(ToGLEnum(primitive), indexCount, GL_UNSIGNED_INT, nullptr));
        vao->Unbind();
    }

    // Multi-draw Commands
    void RenderCommands::MultiDrawArrays(const Ref<VertexArray>& vao, PrimitiveType primitive, const int* first, const int* count, int drawCount)
    {
        LUMINA_ASSERT(vao, "VertexArray cannot be null!");
        LUMINA_ASSERT(first, "First array cannot be null!");
        LUMINA_ASSERT(count, "Count array cannot be null!");
        LUMINA_ASSERT(drawCount > 0, "Draw count must be greater than zero!");

        vao->Bind();
        GLCALL(glMultiDrawArrays(ToGLEnum(primitive), first, count, drawCount));
        vao->Unbind();
    }

    // Convenience Drawing Methods
    void RenderCommands::DrawPoints(const Ref<VertexArray>& vao, uint32_t count, uint32_t offset)
    {
        DrawArrays(vao, PrimitiveType::Points, count, offset);
    }

    void RenderCommands::DrawLines(const Ref<VertexArray>& vao, uint32_t count, uint32_t offset)
    {
        DrawArrays(vao, PrimitiveType::Lines, count, offset);
    }

    void RenderCommands::DrawLineStrips(const Ref<VertexArray>& vao, uint32_t count, uint32_t offset)
    {
        DrawArrays(vao, PrimitiveType::LineStrip, count, offset);
    }

    void RenderCommands::DrawTriangles(const Ref<VertexArray>& vao, uint32_t count, uint32_t offset)
    {
        if (count == 0)
        {
            // Use indexed drawing
            DrawElements(vao, PrimitiveType::Triangles);
        }
        else
        {
            // Use array drawing
            DrawArrays(vao, PrimitiveType::Triangles, count, offset);
        }
    }

    void RenderCommands::DrawTriangleStrip(const Ref<VertexArray>& vao, uint32_t count, uint32_t offset)
    {
        DrawArrays(vao, PrimitiveType::TriangleStrip, count, offset);
    }

    void RenderCommands::DrawTriangleFan(const Ref<VertexArray>& vao, uint32_t count, uint32_t offset)
    {
        DrawArrays(vao, PrimitiveType::TriangleFan, count, offset);
    }

    // Indexed versions
    void RenderCommands::DrawPointsIndexed(const Ref<VertexArray>& vao)
    {
        DrawElements(vao, PrimitiveType::Points);
    }

    void RenderCommands::DrawLinesIndexed(const Ref<VertexArray>& vao)
    {
        DrawElements(vao, PrimitiveType::Lines);
    }

    void RenderCommands::DrawTrianglesIndexed(const Ref<VertexArray>& vao)
    {
        DrawElements(vao, PrimitiveType::Triangles);
    }

    // Internal helper functions for enum conversion
    unsigned int RenderCommands::ToGLEnum(PolygonMode mode)
    {
        switch (mode)
        {
        case PolygonMode::Fill:  return GL_FILL;
        case PolygonMode::Line:  return GL_LINE;
        case PolygonMode::Point: return GL_POINT;
        default: LUMINA_ASSERT(false, "Unknown polygon mode!"); return GL_FILL;
        }
    }

    unsigned int RenderCommands::ToGLEnum(BlendFactor factor)
    {
        switch (factor)
        {
        case BlendFactor::Zero:                     return GL_ZERO;
        case BlendFactor::One:                      return GL_ONE;
        case BlendFactor::SrcColor:                 return GL_SRC_COLOR;
        case BlendFactor::OneMinusSrcColor:         return GL_ONE_MINUS_SRC_COLOR;
        case BlendFactor::DstColor:                 return GL_DST_COLOR;
        case BlendFactor::OneMinusDstColor:         return GL_ONE_MINUS_DST_COLOR;
        case BlendFactor::SrcAlpha:                 return GL_SRC_ALPHA;
        case BlendFactor::OneMinusSrcAlpha:         return GL_ONE_MINUS_SRC_ALPHA;
        case BlendFactor::DstAlpha:                 return GL_DST_ALPHA;
        case BlendFactor::OneMinusDstAlpha:         return GL_ONE_MINUS_DST_ALPHA;
        case BlendFactor::ConstantColor:            return GL_CONSTANT_COLOR;
        case BlendFactor::OneMinusConstantColor:    return GL_ONE_MINUS_CONSTANT_COLOR;
        case BlendFactor::ConstantAlpha:            return GL_CONSTANT_ALPHA;
        case BlendFactor::OneMinusConstantAlpha:    return GL_ONE_MINUS_CONSTANT_ALPHA;
        default: LUMINA_ASSERT(false, "Unknown blend factor!"); return GL_ZERO;
        }
    }

    unsigned int RenderCommands::ToGLEnum(BlendEquation equation)
    {
        switch (equation)
        {
        case BlendEquation::Add:             return GL_FUNC_ADD;
        case BlendEquation::Subtract:        return GL_FUNC_SUBTRACT;
        case BlendEquation::ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
        case BlendEquation::Min:             return GL_MIN;
        case BlendEquation::Max:             return GL_MAX;
        default: LUMINA_ASSERT(false, "Unknown blend equation!"); return GL_FUNC_ADD;
        }
    }

    unsigned int RenderCommands::ToGLEnum(DepthFunc func)
    {
        switch (func)
        {
        case DepthFunc::Never:        return GL_NEVER;
        case DepthFunc::Less:         return GL_LESS;
        case DepthFunc::Equal:        return GL_EQUAL;
        case DepthFunc::LessEqual:    return GL_LEQUAL;
        case DepthFunc::Greater:      return GL_GREATER;
        case DepthFunc::NotEqual:     return GL_NOTEQUAL;
        case DepthFunc::GreaterEqual: return GL_GEQUAL;
        case DepthFunc::Always:       return GL_ALWAYS;
        default: LUMINA_ASSERT(false, "Unknown depth function!"); return GL_LESS;
        }
    }

    unsigned int RenderCommands::ToGLEnum(CullFace face)
    {
        switch (face)
        {
        case CullFace::Front:        return GL_FRONT;
        case CullFace::Back:         return GL_BACK;
        case CullFace::FrontAndBack: return GL_FRONT_AND_BACK;
        default: LUMINA_ASSERT(false, "Unknown cull face!"); return GL_BACK;
        }
    }

    unsigned int RenderCommands::ToGLEnum(FrontFace face)
    {
        switch (face)
        {
        case FrontFace::Clockwise:        return GL_CW;
        case FrontFace::CounterClockwise: return GL_CCW;
        default: LUMINA_ASSERT(false, "Unknown front face!"); return GL_CCW;
        }
    }

    unsigned int RenderCommands::ToGLEnum(PrimitiveType primitive)
    {
        switch (primitive)
        {
        case PrimitiveType::Points:        return GL_POINTS;
        case PrimitiveType::Lines:         return GL_LINES;
        case PrimitiveType::LineStrip:     return GL_LINE_STRIP;
        case PrimitiveType::LineLoop:      return GL_LINE_LOOP;
        case PrimitiveType::Triangles:     return GL_TRIANGLES;
        case PrimitiveType::TriangleStrip: return GL_TRIANGLE_STRIP;
        case PrimitiveType::TriangleFan:   return GL_TRIANGLE_FAN;
        default: LUMINA_ASSERT(false, "Unknown primitive type!"); return GL_TRIANGLES;
        }
    }
}