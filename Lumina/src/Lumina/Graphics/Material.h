#pragma once

#include <string>

#include <glm/glm.hpp>

#include "../Core/Ref.h"

#include "Texture.h"
#include "ShaderProgram.h"

namespace Lumina
{
    class Material : public Referencable
    {
    public:
        static void BindMaterial(Ref<ShaderProgram> shader, Ref<Material> material, Ref<Texture> whiteTexture, Ref<Texture> normalMap);

        static Ref<Material> Create();
        static Ref<Material> Create(const std::string& name);

        Material() = default;
        Material(const std::string& name);
        ~Material() = default;

        void SetAlbedo(const glm::vec3& albedo) { m_Albedo = albedo; }
        const glm::vec3& GetAlbedo() const { return m_Albedo; }

        void SetMetallic(float metallic) { m_Metallic = metallic; }
        float GetMetallic() const { return m_Metallic; }

        void SetRoughness(float roughness) { m_Roughness = roughness; }
        float GetRoughness() const { return m_Roughness; }

        void SetAO(float ao) { m_AO = ao; }
        float GetAO() const { return m_AO; }

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

        void SetName(const std::string& name) { m_Name = name; }
        const std::string& GetName() const { return m_Name; }

        bool IsValid() const;

    private:
        std::string m_Name = "Unnamed Material";

        glm::vec3 m_Albedo = glm::vec3(1.0f, 1.0f, 1.0f);
        float m_Metallic = 0.0f;
        float m_Roughness = 0.5f;
        float m_AO = 1.0f;

        Ref<Texture> m_AlbedoTexture = nullptr;
        Ref<Texture> m_NormalTexture = nullptr;
        Ref<Texture> m_MetallicTexture = nullptr;
        Ref<Texture> m_RoughnessTexture = nullptr;
        Ref<Texture> m_AOTexture = nullptr;
    };
}