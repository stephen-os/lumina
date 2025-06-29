#include "Material.h"

namespace Lumina
{
    Ref<Material> Material::Create()
    {
        return Ref<Material>::Create();
    }

    Ref<Material> Material::Create(const std::string& name)
    {
        return Ref<Material>::Create(name);
    }

    Material::Material(const std::string& name) : m_Name(name) {}

    bool Material::IsValid() const
    {
        return !m_Name.empty() && (m_AlbedoTexture != nullptr || m_Albedo.x >= 0.0f && m_Albedo.y >= 0.0f && m_Albedo.z >= 0.0f);
    }
}