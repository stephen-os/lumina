#pragma once

#include "VertexArray.h"
#include "Texture.h" 
#include "ShaderProgram.h"
#include "RenderCommands.h"
#include "RenderTarget.h"

#include <glm/glm.hpp>
#include <unordered_map>
#include <string>

#include "Core/Base.h"
#include "Cameras/Camera.h"

namespace Lumina
{
    class Renderer2D
    {
    public:
        static void Init();
        static void Shutdown();

        static void Begin(Camera& camera);
        static void Begin(glm::mat4& viewProjection);
        static void End();

        static void StartBatch();
        static void EndBatch();
        static void Flush();

        static void SetResolution(uint32_t width, uint32_t height);
        static glm::vec2 GetResolution();
        static void SetRenderMode(PolygonMode mode);
        static uint32_t GetImage();

        static void SetQuadPosition(const glm::vec3& position);
        static void SetQuadRotation(const glm::vec3& rotation);
        static void SetQuadSize(const glm::vec2& size);
        static void SetQuadTexture(const Ref<Texture>& texture);
        static void SetQuadTextureCoords(const glm::vec4& textureCoords);
        static void SetQuadTintColor(const glm::vec4& tintColor);
        static void ResetQuadState();

        static void SetCirclePosition(const glm::vec3& position);
        static void SetCircleRotation(const glm::vec3& rotation);
        static void SetCircleRadius(const glm::vec2& radius);
        static void SetCircleTexture(const Ref<Texture>& texture);
        static void SetCircleTextureCoords(const glm::vec4& textureCoords);
        static void SetCircleColor(const glm::vec4& color);
        static void SetCircleThickness(float thickness);
        static void SetCircleFade(float fade);
        static void ResetCircleState();

        static void SetLineStart(const glm::vec3& start);
        static void SetLineEnd(const glm::vec3& end);
        static void SetLineThickness(float thickness);
        static void SetLineColor(const glm::vec4& color);
        static void ResetLineState();

        static void SetTextContent(const std::string& text);
        static void SetTextPosition(const glm::vec3& position);
        static void SetTextColor(const glm::vec4& color);
        static void SetTextSize(float size);
        static void SetTextFont(const Ref<Texture>& fontTexture);
        static void ResetTextState();

        static void SetPixelPosition(const glm::vec3& position);
        static void SetPixelColor(const glm::vec4& color);
		static void SetPixelSize(float size = 1.0f);
        static void ResetPixelState();

        static void SetTrianglePoint1(const glm::vec3& point1);
        static void SetTrianglePoint2(const glm::vec3& point2);
        static void SetTrianglePoint3(const glm::vec3& point3);
        static void SetTriangleTexture(const Ref<Texture>& texture);
        static void SetTriangleColor(const glm::vec4& color);
        static void ResetTriangleState();

        static void SetGridPosition(const glm::vec3& position);
        static void SetGridRotation(const glm::vec3& rotation);
        static void SetGridSize(const glm::vec2& size);
        static void SetGridCellSize(float gridSize);
        static void SetGridColor(const glm::vec4& color);
        static void SetGridLineWidth(float lineWidth);
        static void SetGridShowCheckerboard(bool showCheckerboard);
        static void SetGridCheckerColor1(const glm::vec4& checkerColor1);
        static void SetGridCheckerColor2(const glm::vec4& checkerColor2);
        static void ResetGridState();

        static void DrawQuad();
        static void DrawCircle();
        static void DrawLine();
        static void DrawText();
        static void DrawPixel();
        static void DrawTriangle();
        static void DrawGrid(); 

        static void SetRenderTarget(Ref<RenderTarget> target);
        static void SetRenderTarget(std::nullptr_t);
        static Ref<RenderTarget> GetCurrentRenderTarget();
        static Ref<RenderTarget> CreateRenderTarget(uint32_t width, uint32_t height);

        struct Statistics
        {
            uint32_t DrawCalls = 0;
            uint32_t QuadCount = 0;
            uint32_t CircleCount = 0;
            uint32_t LineCount = 0;
            uint32_t TextCount = 0;
            uint32_t PixelCount = 0;
            uint32_t TriangleCount = 0;
            uint32_t GridCount = 0;
            uint32_t TexturesUsed = 0;
            uint32_t ShadersUsed = 0;
            uint32_t DataSize = 0;

            uint32_t GetTotalVertexCount() const { return QuadCount * 4 + CircleCount * 4 + LineCount * 2 + TextCount * 4 + PixelCount + TriangleCount * 3 + GridCount * 4; }
            uint32_t GetTotalIndexCount() const { return QuadCount * 6 + CircleCount * 6 + TextCount * 6 + TriangleCount * 3 + GridCount * 6; }
        };

        static Statistics GetStats();
        static void ResetStats();

    private:
        static float ComputeTextureIndex(const Ref<Texture>& texture);
        
    };
}