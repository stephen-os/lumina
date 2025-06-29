#pragma once

#include <glm/glm.hpp>
#include <string>

#include "../Core/Ref.h"

#include "Texture.h"

namespace Lumina
{
    class Material : public Referencable
    {
    public:
        static Ref<Material> Create();
        static Ref<Material> Create(const std::string& name);

        Material() = default;
        Material(const std::string& name);
        ~Material() = default;

        // PBR Properties
        void SetAlbedo(const glm::vec3& albedo) { m_Albedo = albedo; }
        const glm::vec3& GetAlbedo() const { return m_Albedo; }

        void SetMetallic(float metallic) { m_Metallic = metallic; }
        float GetMetallic() const { return m_Metallic; }

        void SetRoughness(float roughness) { m_Roughness = roughness; }
        float GetRoughness() const { return m_Roughness; }

        void SetAO(float ao) { m_AO = ao; }
        float GetAO() const { return m_AO; }

        // Texture Properties
        void SetAlbedoTexture(const Ref<Texture>& texture) { m_AlbedoTexture = texture; }
        Ref<Texture> GetAlbedoTexture() const { return m_AlbedoTexture; }

        void SetNormalTexture(const Ref<Texture>& texture) { m_NormalTexture = texture; }
        Ref<Texture> GetNormalTexture() const { return m_NormalTexture; }

        void SetMetallicTexture(const Ref<Texture>& texture) { m_MetallicTexture = texture; }
        Ref<Texture> GetMetallicTexture() const { return m_MetallicTexture; }

        void SetRoughnessTexture(const Ref<Texture>& texture) { m_RoughnessTexture = texture; }
        Ref<Texture> GetRoughnessTexture() const { return m_RoughnessTexture; }

        void SetAOTexture(const Ref<Texture>& texture) { m_AOTexture = texture; }
        Ref<Texture> GetAOTexture() const { return m_AOTexture; }

        // Material Properties
        void SetName(const std::string& name) { m_Name = name; }
        const std::string& GetName() const { return m_Name; }

        // Validation
        bool IsValid() const;

    private:
        std::string m_Name = "Unnamed Material";

        // PBR Properties
        glm::vec3 m_Albedo = glm::vec3(1.0f, 1.0f, 1.0f);
        float m_Metallic = 0.0f;
        float m_Roughness = 0.5f;
        float m_AO = 1.0f;

        // Texture Maps
        Ref<Texture> m_AlbedoTexture = nullptr;
        Ref<Texture> m_NormalTexture = nullptr;
        Ref<Texture> m_MetallicTexture = nullptr;
        Ref<Texture> m_RoughnessTexture = nullptr;
        Ref<Texture> m_AOTexture = nullptr;
    };
}