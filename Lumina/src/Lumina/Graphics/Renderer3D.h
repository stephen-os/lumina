#pragma once
#include "VertexArray.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "RenderCommands.h"
#include "FrameBuffer.h"
#include "Model.h"
#include "Cameras/Camera.h"
#include <glm/glm.hpp>
#include <string>
#include <memory>

namespace Lumina
{
    enum class RenderMode
    {
        Normal = 0,
        Wireframe,
        Points
    };

    struct ModelAttributes
    {
        glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };
        glm::vec4 TintColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        float PointSize = 1.0f; // For point rendering
    };

    struct DirectionalLight
    {
        glm::vec3 Direction = { -0.2f, -1.0f, -0.3f };
        glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
        float Intensity = 1.0f;
    };

    struct PointLight
    {
        glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Color = { 1.0f, 1.0f, 1.0f };
        float Intensity = 1.0f;
        float Constant = 1.0f;
        float Linear = 0.09f;
        float Quadratic = 0.032f;
    };

    class Renderer3D
    {
    public:
        // Core Renderer Functions
        static void Init();
        static void Shutdown();

        // Begin and End Rendering
        static void Begin(Camera& camera);
        static void Begin(const glm::mat4& viewProjection);
        static void End();

        // Rendering Functions
        static void DrawModel(const Ref<Model>& model, const ModelAttributes& attributes = {});
        static void DrawMesh(const Mesh& mesh, const ModelAttributes& attributes = {});

        // Resolution Management
        static void SetResolution(uint32_t width, uint32_t height);
        static glm::vec2 GetResolution();

        // Render Mode
        static void SetRenderMode(RenderMode mode);
        static RenderMode GetRenderMode();

        // Point rendering settings
        static void SetGlobalPointSize(float size);
        static float GetGlobalPointSize();

        // Framebuffer Access
        static uint32_t GetImage();

        // Lighting
        static void SetDirectionalLight(const DirectionalLight& light);
        static void AddPointLight(const PointLight& light);
        static void ClearPointLights();

        // Environment
        static void SetEnvironmentMap(const Ref<Texture>& envMap);
        static void SetSkybox(const Ref<Texture>& skybox);

        // Statistics
        struct Statistics
        {
            uint32_t DrawCalls = 0;
            uint32_t ModelCount = 0;
            uint32_t MeshCount = 0;
            uint32_t TriangleCount = 0;
            uint32_t VertexCount = 0;
            uint32_t TexturesUsed = 0;
            uint32_t DataSize = 0;
        };

        static Statistics GetStats();
        static void ResetStats();

    private:
        static void SetupLighting();
        static void SetupRenderMode(const ModelAttributes& attributes);
        static glm::mat4 CalculateModelMatrix(const ModelAttributes& attributes);
    };
}