#include "Material.h"

#include "TextureSlot.h"

namespace Lumina
{
    void Material::BindMaterial(Ref<ShaderProgram> shader, Ref<Material> material, Ref<Texture> whiteTexture, Ref<Texture> normalMap)
    {
        LUMINA_ASSERT(shader, "Material: Shader cannot be null");
        LUMINA_ASSERT(whiteTexture, "Material: White texture cannot be null");
        LUMINA_ASSERT(normalMap, "Material: Normal map cannot be null");

        if (!material)
            return;

        if (material->GetAlbedoTexture())
        {
            material->GetAlbedoTexture()->Bind(TextureSlots::MATERIAL_ALBEDO);
            shader->SetUniformInt("u_AlbedoTexture", TextureSlots::MATERIAL_ALBEDO);
            shader->SetUniformInt("u_HasAlbedoTexture", true);
        }
        else
        {
            whiteTexture->Bind(TextureSlots::MATERIAL_ALBEDO);
            shader->SetUniformInt("u_AlbedoTexture", TextureSlots::MATERIAL_ALBEDO);
            shader->SetUniformInt("u_HasAlbedoTexture", false);
        }

        if (material->GetNormalTexture())
        {
            material->GetNormalTexture()->Bind(TextureSlots::MATERIAL_NORMAL);
            shader->SetUniformInt("u_NormalTexture", TextureSlots::MATERIAL_NORMAL);
            shader->SetUniformInt("u_HasNormalTexture", true);
        }
        else
        {
            normalMap->Bind(TextureSlots::MATERIAL_NORMAL);
            shader->SetUniformInt("u_NormalTexture", TextureSlots::MATERIAL_NORMAL);
            shader->SetUniformInt("u_HasNormalTexture", false);
        }

        // Bind Metallic Texture
        if (material->GetMetallicTexture())
        {
            material->GetMetallicTexture()->Bind(TextureSlots::MATERIAL_METALLIC);
            shader->SetUniformInt("u_MetallicTexture", TextureSlots::MATERIAL_METALLIC);
            shader->SetUniformInt("u_HasMetallicTexture", true);
        }
        else
        {
            whiteTexture->Bind(TextureSlots::MATERIAL_METALLIC);
            shader->SetUniformInt("u_MetallicTexture", TextureSlots::MATERIAL_METALLIC);
            shader->SetUniformInt("u_HasMetallicTexture", false);
        }

        // Bind Roughness Texture
        if (material->GetRoughnessTexture())
        {
            material->GetRoughnessTexture()->Bind(TextureSlots::MATERIAL_ROUGHNESS);
            shader->SetUniformInt("u_RoughnessTexture", TextureSlots::MATERIAL_ROUGHNESS);
            shader->SetUniformInt("u_HasRoughnessTexture", true);
        }
        else
        {
            whiteTexture->Bind(TextureSlots::MATERIAL_ROUGHNESS);
            shader->SetUniformInt("u_RoughnessTexture", TextureSlots::MATERIAL_ROUGHNESS);
            shader->SetUniformInt("u_HasRoughnessTexture", false);
        }

        // Bind AO Texture
        if (material->GetAOTexture())
        {
            material->GetAOTexture()->Bind(TextureSlots::MATERIAL_AO);
            shader->SetUniformInt("u_AOTexture", TextureSlots::MATERIAL_AO);
            shader->SetUniformInt("u_HasAOTexture", true);
        }
        else
        {
            whiteTexture->Bind(TextureSlots::MATERIAL_AO);
            shader->SetUniformInt("u_AOTexture", TextureSlots::MATERIAL_AO);
            shader->SetUniformInt("u_HasAOTexture", false);
        }

        // Bind Emission Texture
        if (material->GetEmissionTexture())
        {
            material->GetEmissionTexture()->Bind(TextureSlots::MATERIAL_EMISSION);
            shader->SetUniformInt("u_EmissionTexture", TextureSlots::MATERIAL_EMISSION);
            shader->SetUniformInt("u_HasEmissionTexture", true);
        }
        else
        {
            whiteTexture->Bind(TextureSlots::MATERIAL_EMISSION);
            shader->SetUniformInt("u_EmissionTexture", TextureSlots::MATERIAL_EMISSION);
            shader->SetUniformInt("u_HasEmissionTexture", false);
        }

        // Bind Height Texture
        if (material->GetHeightTexture())
        {
            material->GetHeightTexture()->Bind(TextureSlots::MATERIAL_HEIGHT);
            shader->SetUniformInt("u_HeightTexture", TextureSlots::MATERIAL_HEIGHT);
            shader->SetUniformInt("u_HasHeightTexture", true);
        }
        else
        {
            whiteTexture->Bind(TextureSlots::MATERIAL_HEIGHT);
            shader->SetUniformInt("u_HeightTexture", TextureSlots::MATERIAL_HEIGHT);
            shader->SetUniformInt("u_HasHeightTexture", false);
        }

        // Bind Alpha Texture
        if (material->GetAlphaTexture())
        {
            material->GetAlphaTexture()->Bind(TextureSlots::MATERIAL_ALPHA);
            shader->SetUniformInt("u_AlphaTexture", TextureSlots::MATERIAL_ALPHA);
            shader->SetUniformInt("u_HasAlphaTexture", true);
        }
        else
        {
            whiteTexture->Bind(TextureSlots::MATERIAL_ALPHA);
            shader->SetUniformInt("u_AlphaTexture", TextureSlots::MATERIAL_ALPHA);
            shader->SetUniformInt("u_HasAlphaTexture", false);
        }

        // Set Material Properties
        if (material)
        {
            // Base PBR Properties
            shader->SetUniformVec3("u_Material.Albedo", material->GetAlbedo());
            shader->SetUniformFloat("u_Material.Metallic", material->GetMetallic());
            shader->SetUniformFloat("u_Material.Roughness", material->GetRoughness());
            shader->SetUniformFloat("u_Material.AO", material->GetAO());

            // New Properties
            shader->SetUniformVec3("u_Material.Emission", material->GetEmission());
            shader->SetUniformFloat("u_Material.EmissionIntensity", material->GetEmissionIntensity());
            shader->SetUniformFloat("u_Material.HeightScale", material->GetHeightScale());
            shader->SetUniformFloat("u_Material.Alpha", material->GetAlpha());
            shader->SetUniformFloat("u_Material.NormalIntensity", material->GetNormalIntensity());
        }
        else
        {
            // Default Material Properties
            shader->SetUniformVec3("u_Material.Albedo", glm::vec3(1.0f));
            shader->SetUniformFloat("u_Material.Metallic", 0.0f);
            shader->SetUniformFloat("u_Material.Roughness", 0.5f);
            shader->SetUniformFloat("u_Material.AO", 1.0f);

            // Default New Properties
            shader->SetUniformVec3("u_Material.Emission", glm::vec3(0.0f));
            shader->SetUniformFloat("u_Material.EmissionIntensity", 1.0f);
            shader->SetUniformFloat("u_Material.HeightScale", 0.05f);
            shader->SetUniformFloat("u_Material.Alpha", 1.0f);
            shader->SetUniformFloat("u_Material.NormalIntensity", 1.0f);
        }
    }

    Ref<Material> Material::Create()
    {
        return CreateRef<Material>();
    }

    Ref<Material> Material::Create(const std::string& name)
    {
        return CreateRef<Material>(name);
    }

    Material::Material(const std::string& name) : m_Name(name) {}

    bool Material::IsValid() const
    {
        return !m_Name.empty() && (m_AlbedoTexture != nullptr || m_Albedo.x >= 0.0f && m_Albedo.y >= 0.0f && m_Albedo.z >= 0.0f);
    }
}