#pragma once
#include <memory>
#include "VertexArray.h"
#include "../Core/Ref.h"

namespace Lumina
{
    enum class PolygonMode
    {
        Fill = 0,
        Line,
        Point
    };

    enum class BlendFactor
    {
        Zero = 0,
        One,
        SrcColor,
        OneMinusSrcColor,
        DstColor,
        OneMinusDstColor,
        SrcAlpha,
        OneMinusSrcAlpha,
        DstAlpha,
        OneMinusDstAlpha,
        ConstantColor,
        OneMinusConstantColor,
        ConstantAlpha,
        OneMinusConstantAlpha
    };

    enum class BlendEquation
    {
        Add = 0,
        Subtract,
        ReverseSubtract,
        Min,
        Max
    };

    enum class DepthFunc
    {
        Never = 0,
        Less,
        Equal,
        LessEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always
    };

    enum class CullFace
    {
        Front = 0,
        Back,
        FrontAndBack
    };

    enum class FrontFace
    {
        Clockwise = 0,
        CounterClockwise
    };

    enum class PrimitiveType
    {
        Points = 0,
        Lines,
        LineStrip,
        LineLoop,
        Triangles,
        TriangleStrip,
        TriangleFan
    };

    struct RenderState
    {
        bool depthTest = true;
        bool depthWrite = true;
        bool blending = false;
        bool faceCulling = false;
        bool scissorTest = false;
        PolygonMode polygonMode = PolygonMode::Fill;
        DepthFunc depthFunc = DepthFunc::Less;
        CullFace cullFace = CullFace::Back;
        FrontFace frontFace = FrontFace::CounterClockwise;
        BlendFactor srcBlend = BlendFactor::SrcAlpha;
        BlendFactor dstBlend = BlendFactor::OneMinusSrcAlpha;
        BlendEquation blendEquation = BlendEquation::Add;
        float lineWidth = 1.0f;
        float pointSize = 1.0f;
    };

    class RenderCommands
    {
    public:
        // Viewport and Clearing
        static void SetViewport(int x, int y, int width, int height);
        static void SetClearColor(float r, float g, float b, float a = 1.0f);
        static void Clear();
        static void ClearColor();
        static void ClearDepth();
        static void ClearStencil();

        // Depth Testing
        static void EnableDepthTest();
        static void DisableDepthTest();
        static void SetDepthFunc(DepthFunc func);
        static void SetDepthMask(bool writeEnabled);

        // Blending
        static void EnableBlending();
        static void DisableBlending();
        static void SetBlendFunc(BlendFactor sfactor, BlendFactor dfactor);
        static void SetBlendEquation(BlendEquation mode);

        // Face Culling
        static void EnableFaceCulling();
        static void DisableFaceCulling();
        static void SetCullFace(CullFace mode);
        static void SetFrontFace(FrontFace mode);

        // Scissor Testing
        static void EnableScissorTest();
        static void DisableScissorTest();
        static void SetScissorBox(int x, int y, int width, int height);

        // Polygon and Line/Point Settings
        static void SetPolygonMode(PolygonMode mode);
        static void SetLineWidth(float width);
        static void SetPointSize(float size);

        // State Management
        static void SetRenderState(const RenderState& state);
        static RenderState GetRenderState();

        // Basic Drawing Commands
        static void DrawArrays(const Ref<VertexArray>& vao, PrimitiveType primitive, uint32_t count, uint32_t offset = 0);
        static void DrawElements(const Ref<VertexArray>& vao, PrimitiveType primitive);

        // Instanced Drawing
        static void DrawArraysInstanced(const Ref<VertexArray>& vao, PrimitiveType primitive, uint32_t count, uint32_t instanceCount, uint32_t offset = 0);
        static void DrawElementsInstanced(const Ref<VertexArray>& vao, PrimitiveType primitive, uint32_t instanceCount);

        // Multi-draw Commands
        static void MultiDrawArrays(const Ref<VertexArray>& vao, PrimitiveType primitive, const int* first, const int* count, int drawCount);

        // Convenience Drawing Methods (backwards compatibility and ease of use)
        static void DrawPoints(const Ref<VertexArray>& vao, uint32_t count, uint32_t offset = 0);
        static void DrawLines(const Ref<VertexArray>& vao, uint32_t count, uint32_t offset = 0);
        static void DrawLineStrips(const Ref<VertexArray>& vao, uint32_t count, uint32_t offset = 0);
        static void DrawTriangles(const Ref<VertexArray>& vao, uint32_t count = 0, uint32_t offset = 0); // count = 0 means use index buffer
        static void DrawTriangleStrip(const Ref<VertexArray>& vao, uint32_t count, uint32_t offset = 0);
        static void DrawTriangleFan(const Ref<VertexArray>& vao, uint32_t count, uint32_t offset = 0);

        // Indexed versions
        static void DrawPointsIndexed(const Ref<VertexArray>& vao);
        static void DrawLinesIndexed(const Ref<VertexArray>& vao);
        static void DrawTrianglesIndexed(const Ref<VertexArray>& vao);

    private:
        static RenderState s_CurrentState;

        // Internal helper functions for enum conversion
        static unsigned int ToGLEnum(PolygonMode mode);
        static unsigned int ToGLEnum(BlendFactor factor);
        static unsigned int ToGLEnum(BlendEquation equation);
        static unsigned int ToGLEnum(DepthFunc func);
        static unsigned int ToGLEnum(CullFace face);
        static unsigned int ToGLEnum(FrontFace face);
        static unsigned int ToGLEnum(PrimitiveType primitive);
    };
}