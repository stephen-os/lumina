#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>

#include "Core/Base.h"

#include "Texture.h"
#include "ShaderProgram.h"
#include "VertexArray.h"
#include "Buffer.h"

namespace Lumina
{
    struct SkyboxAttributes
    {
        // Basic properties
        float Intensity = 1.0f;                                 // Brightness multiplier
        glm::vec3 Tint = glm::vec3(1.0f);                       // Color tint (RGB)
        float Rotation = 0.0f;                                  // Rotation around Y-axis (radians)
        glm::vec3 RotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);   // Custom rotation axis
        float Exposure = 1.0f;                                  // HDR exposure adjustment
        float Saturation = 1.0f;                                // Color saturation (0.0 = grayscale, 1.0 = normal)
        float Contrast = 1.0f;                                  // Contrast adjustment
        bool EnableRotation = false;                            // Auto-rotate the skybox
        float RotationSpeed = 0.1f;                             // Rotation speed (radians per second)
        float Alpha = 1.0f;                                     // Transparency (for blending with previous skybox)
        float TimeOfDay = 0.5f;                                 // 0.0 = midnight, 0.5 = noon, 1.0 = midnight
        bool UseTimeOfDay = false;                              // Whether to apply time-based modifications

        // Default constructor
        SkyboxAttributes() = default;

        // Convenience constructors
        SkyboxAttributes(float intensity, const glm::vec3& tint = glm::vec3(1.0f))
            : Intensity(intensity), Tint(tint) {
        }

        SkyboxAttributes(const glm::vec3& tint, float intensity = 1.0f)
            : Intensity(intensity), Tint(tint) {
        }
    };

    class Skybox
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