#pragma once

#include "Cameras/Camera.h"

#include "Core/Base.h"

#include "VertexArray.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "RenderCommands.h"
#include "Model.h"
#include "Skybox.h"
#include "Instance.h"

#include <glm/glm.hpp>

#include <string>

namespace Lumina
{
    enum class RenderMode
    {
        Normal = 0,
        Wireframe,
        Points
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
        static void Init();
        static void Shutdown();

        static void Begin(Camera& camera);
        static void Begin(const glm::mat4& viewProjection);
        static void End();

		static void Draw(const Ref<Model>& model, const ModelAttributes& attributes = {});
        static void Draw(const Ref<Mesh>& mesh, const ModelAttributes& attributes = {});
        static void Draw(const Ref<Skybox>& skybox, const SkyboxAttributes& attributes = {});

		static void Submit(const Ref<Model>& model, const ModelAttributes& attributes = {});
		static void Flush(); 

        static void SetResolution(uint32_t width, uint32_t height);
        static glm::vec2 GetResolution();

         static void SetRenderMode(RenderMode mode);
        static RenderMode GetRenderMode();

        static void SetGlobalPointSize(float size);
        static float GetGlobalPointSize();

        static uint32_t GetImage();

        static void SetDirectionalLight(const DirectionalLight& light);
        static void AddPointLight(const PointLight& light);
        static void ClearPointLights();

        static void SetEnvironmentMap(const Ref<Texture>& envMap);

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

    };
}