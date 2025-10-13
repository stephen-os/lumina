#pragma once

#include <string>

#include <glm/glm.hpp>

#include "Lumina/Core/Base.h"

#include "Texture.h"
#include "ShaderProgram.h"

namespace Lumina
{
    class Material
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

        void SetEmission(const glm::vec3& emission) { m_Emission = emission; }
        const glm::vec3& GetEmission() const { return m_Emission; }

        void SetEmissionIntensity(float intensity) { m_EmissionIntensity = intensity; }
        float GetEmissionIntensity() const { return m_EmissionIntensity; }

        void SetHeightScale(float scale) { m_HeightScale = scale; }
        float GetHeightScale() const { return m_HeightScale; }

        void SetAlpha(float alpha) { m_Alpha = alpha; }
        float GetAlpha() const { return m_Alpha; }

        void SetNormalIntensity(float intensity) { m_NormalIntensity = intensity; }
        float GetNormalIntensity() const { return m_NormalIntensity; }\

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

        void SetEmissionTexture(const Ref<Texture>& texture) { m_EmissionTexture = texture; }
        Ref<Texture> GetEmissionTexture() const { return m_EmissionTexture; }

        void SetHeightTexture(const Ref<Texture>& texture) { m_HeightTexture = texture; }
        Ref<Texture> GetHeightTexture() const { return m_HeightTexture; }

        void SetAlphaTexture(const Ref<Texture>& texture) { m_AlphaTexture = texture; }
        Ref<Texture> GetAlphaTexture() const { return m_AlphaTexture; }


        void SetName(const std::string& name) { m_Name = name; }
        const std::string& GetName() const { return m_Name; }

        bool IsValid() const;

    private:
        std::string m_Name = "Unnamed Material";

        glm::vec3 m_Albedo = glm::vec3(1.0f, 1.0f, 1.0f);
        float m_Metallic = 0.0f;
        float m_Roughness = 0.5f;
        float m_AO = 1.0f;
        glm::vec3 m_Emission = glm::vec3(0.0f, 0.0f, 0.0f);
        float m_EmissionIntensity = 1.0f;
        float m_HeightScale = 0.05f;
        float m_Alpha = 1.0f;
        float m_NormalIntensity = 1.0f;

        Ref<Texture> m_AlbedoTexture = nullptr;
        Ref<Texture> m_NormalTexture = nullptr;
        Ref<Texture> m_MetallicTexture = nullptr;
        Ref<Texture> m_RoughnessTexture = nullptr;
        Ref<Texture> m_AOTexture = nullptr;
        Ref<Texture> m_EmissionTexture = nullptr;
        Ref<Texture> m_HeightTexture = nullptr;
        Ref<Texture> m_AlphaTexture = nullptr;
    };
}