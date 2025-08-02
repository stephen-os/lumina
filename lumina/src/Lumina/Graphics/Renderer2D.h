#pragma once

#include "VertexArray.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "RenderCommands.h"
#include "RenderTarget.h"

#include <glm/glm.hpp>

#include <string>

#include "Core/Base.h"

#include "Cameras/Camera.h"

namespace Lumina
{
    struct QuadAttributes
    {
        glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
        glm::vec2 Size = { 1.0f, 1.0f };
        Ref<Texture> Texture = nullptr;
		glm::vec4 TextureCoords = { 0, 0, 1, 1 };
        glm::vec4 TintColor = glm::vec4(1.0f);
    };

    struct CircleAttributes
    {
        glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
        glm::vec2 Radius = { 1.0f, 1.0f }; 
		Ref<Texture> Texture = nullptr;
		glm::vec4 TextureCoords = { 0, 0, 1, 1 };
        glm::vec4 Color = glm::vec4(1.0f);
		float Thickness = 1.0f;
		float Fade = 0.0f;
    };

    struct LineAttributes
    {
        glm::vec3 Start = { 0.0f, 0.0f, 0.0f };
        glm::vec3 End = { 1.0f, 1.0f, 0.0f };
        float Thickness = 2.0f;
        glm::vec4 Color = glm::vec4(1.0f);
    };

    // Text: to be implemented with bitmap fonts
    struct TextAttributes
    {
        std::string Text;
        glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
        glm::vec4 Color = glm::vec4(1.0f);
        float Size = 1.0f;
        Ref<Texture> FontTexture = nullptr;
    };

    struct GridAttributes
    {
        glm::vec3 Position = { 0.0f, 0.0f, 0.0f };              // Grid center position
        glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };              // Grid rotation (for flexibility)
        glm::vec2 Size = { 1000.0f, 1000.0f };                    // Grid coverage area in world units
        float GridSize = 1.0f;                                  // Size of each grid cell in world units
        glm::vec4 Color = { 0.3f, 0.3f, 0.3f, 0.8f };           // Grid line color with alpha
        float LineWidth = 1.0f;                                 // Grid line thickness
        bool ShowCheckerboard = true;                           // Whether to show alternating squares
        glm::vec4 CheckerColor1 = { 0.9f, 0.9f, 0.9f, 0.2f };   // Light checker squares
        glm::vec4 CheckerColor2 = { 0.8f, 0.8f, 0.8f, 0.2f };   // Dark checker squares
    };

    class Renderer2D
    {
    public:
        // Core Renderer Functions
        static void Init();
        static void Shutdown();

		// Begin and End Batch
        static void Begin(Camera& camera);
        static void Begin(glm::mat4& viewProjection);
        static void End();
        
        static void StartBatch();
		static void EndBatch();

		// Draw Batch
        static void Flush();

        // Resolution Management
        static void SetResolution(uint32_t width, uint32_t height);
        static glm::vec2 GetResolution();
        
        static void SetRenderMode(PolygonMode mode); 

        // Get the image stored in the Framebuffer
        static uint32_t GetImage();

        // Helper Functions
		static float ComputeTextureIndex(const Ref<Texture>& texture);

        // Basic Drawing Functions
        static void DrawQuad(const QuadAttributes& attributes);
        static void DrawCircle(const CircleAttributes& attributes);
        static void DrawLine(const LineAttributes& attributes);
        static void DrawText(const TextAttributes& attributes);
        static void DrawGrid(const GridAttributes& attributes);
       
        // Render Target
        static void SetRenderTarget(Ref<RenderTarget> target);
        static void SetRenderTarget(std::nullptr_t);
        static Ref<RenderTarget> GetCurrentRenderTarget();
        static Ref<RenderTarget> CreateRenderTarget(uint32_t width, uint32_t height);

        // Statistics Tracking
        struct Statistics
        {
            uint32_t DrawCalls = 0;
            uint32_t QuadCount = 0;
			uint32_t CircleCount = 0;
			uint32_t LineCount = 0;
            uint32_t TexturesUsed = 0;
			uint32_t ShadersUsed = 0;
            uint32_t DataSize = 0; 

            uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
            uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
        };

        static Statistics GetStats();
        static void ResetStats();
    };
}