#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>

#include "../Core/Ref.h"

#include "Texture.h"
#include "ShaderProgram.h"
#include "VertexArray.h"
#include "Buffer.h"

namespace Lumina
{
    struct SkyboxAttributes
    {
        float Intensity = 1.0f;                                 // Brightness multiplier
        glm::vec3 Tint = glm::vec3(1.0f);                       // Color tint (RGB)
        float Rotation = 0.0f;                                  // Rotation around Y-axis (radians)
        glm::vec3 RotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);   // Custom rotation axis
    }; 

    class Skybox : public Referencable
    {
    public:
        static void BindAttributes(Ref<ShaderProgram> shader, const SkyboxAttributes& attributes);
        
        static Ref<Skybox> Create(const std::string& name = "Unnamed Skybox");
        static Ref<Skybox> Create(const std::vector<std::string>& faces, const std::string& name = "Unnamed Skybox");
        static Ref<Skybox> Create(const Ref<Texture>& cubemapTexture, const std::string& name = "Unnamed Skybox");

        Skybox();
        Skybox(const std::vector<std::string>& faces);
        Skybox(const Ref<Texture>& cubemapTexture);
        ~Skybox() = default;

        void SetTexture(const Ref<Texture>& cubemapTexture);
        void SetTexture(const std::vector<std::string>& faces);
        Ref<Texture> GetTexture() const { return m_CubemapTexture; }

        Ref<VertexArray> GetVAO() const { return m_VAO; }

        bool IsValid() const { return m_CubemapTexture != nullptr && m_VAO != nullptr; }

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

        static bool s_GeometryInitialized;
        static Ref<VertexArray> s_SharedVAO;
        static Ref<VertexBuffer> s_SharedVBO;

        static void InitializeSharedGeometry();
    };
}