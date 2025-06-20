#pragma once

#include "VertexArray.h"
#include "Texture.h"
#include "ShaderProgram.h"

#include <glm/glm.hpp>

#include <string>
#include <memory>

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

    struct CubeAttributes
    {
        glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Size = { 1.0f, 1.0f, 1.0f };
        Ref<Texture> Texture = nullptr;
        glm::vec4 TextureCoords = { 0, 0, 1, 1 };
        glm::vec4 TintColor = glm::vec4(1.0f);
    };

    class Renderer
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
        
        // Get the image stored in the Framebuffer
        static uint32_t GetImage();

        // Helper Functions
		static float ComputeTextureIndex(const Ref<Texture>& texture);

        // Basic Drawing Functions
        static void DrawQuad(const QuadAttributes& attributes);
		static void DrawCube(const CubeAttributes& attributes);
       
        // Statistics Tracking
        struct Statistics
        {
            uint32_t DrawCalls = 0;
            uint32_t QuadCount = 0;
			uint32_t CubeCount = 0;
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