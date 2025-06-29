// Skybox.h - Clean class abstraction
#pragma once

#include "../Core/Ref.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Buffer.h"
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace Lumina
{
    class Skybox : public Referencable
    {
    public:
        static Ref<Skybox> Create();
        static Ref<Skybox> Create(const std::vector<std::string>& faces);
        static Ref<Skybox> Create(const Ref<Texture>& cubemapTexture);

        Skybox();
        Skybox(const std::vector<std::string>& faces);
        Skybox(const Ref<Texture>& cubemapTexture);
        ~Skybox() = default;

        // Texture management
        void SetTexture(const Ref<Texture>& cubemapTexture);
        void SetTexture(const std::vector<std::string>& faces);
        Ref<Texture> GetTexture() const { return m_CubemapTexture; }

        // Visual properties
        void SetIntensity(float intensity) { m_Intensity = intensity; }
        float GetIntensity() const { return m_Intensity; }

        void SetTint(const glm::vec3& tint) { m_Tint = tint; }
        const glm::vec3& GetTint() const { return m_Tint; }

        // Geometry access (for renderer)
        Ref<VertexArray> GetVAO() const { return m_VAO; }

        // Validation
        bool IsValid() const { return m_CubemapTexture != nullptr && m_VAO != nullptr; }

        // Metadata
        void SetName(const std::string& name) { m_Name = name; }
        const std::string& GetName() const { return m_Name; }

    private:
        void CreateGeometry();
        void CreateDefaultTexture();

    private:
        std::string m_Name = "Unnamed Skybox";
        Ref<Texture> m_CubemapTexture = nullptr;
        Ref<VertexArray> m_VAO = nullptr;
        Ref<VertexBuffer> m_VBO = nullptr;
        float m_Intensity = 1.0f;
        glm::vec3 m_Tint = glm::vec3(1.0f);

        // Shared geometry optimization
        static bool s_GeometryInitialized;
        static Ref<VertexArray> s_SharedVAO;
        static Ref<VertexBuffer> s_SharedVBO;

        static void InitializeSharedGeometry();
    };
}