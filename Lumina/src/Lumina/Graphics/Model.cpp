#include "Model.h"

#include "ModelRegistry.h"

#include "../Core/Assert.h"

#include "../Utils/UUID.h"

namespace Lumina
{
    Ref<Model> Model::Load(const std::string& path, ModelFormat format)
    {
        LUMINA_ASSERT(!path.empty(), "Model: Model path cannot be empty"); 
       
        auto model = ModelRegistry::LoadModel(path, format); 
        LUMINA_ASSERT(model, "Model: Failed to load model - ModelRegistry returned nullptr");

        LUMINA_LOG_INFO("Model: Successfully loaded '{}'", model->GetName());
        LUMINA_LOG_INFO("- Path: {}", path);
        LUMINA_LOG_INFO("- Directory: {}", model->GetDirectory());
        LUMINA_LOG_INFO("- UUID: {}", model->GetUUID());
        LUMINA_LOG_INFO("- Meshes: {}", model->GetMeshCount());
        LUMINA_LOG_INFO("- Total Vertices: {}", model->GetTotalVertexCount());
        LUMINA_LOG_INFO("- Total Triangles: {}", model->GetTotalTriangleCount());
        LUMINA_LOG_INFO("- Format: {}", ModelRegistry::ModelFormatToString(format));
        LUMINA_LOG_INFO("- Valid: {}", model->IsValid() ? "Yes" : "No");

        return model; 
    }

    Ref<Model> Model::Create(const std::string& name)
    {
        LUMINA_ASSERT(!name.empty(), "Model: Model name cannot be empty");
        return Ref<Model>::Create(name);
    }

    Model::Model(const std::string& name) : m_UUID(UUID::Generate()), m_Name(name)
    {
        LUMINA_ASSERT(!name.empty(), "Model: Model name cannot be empty in constructor");
    }

    void Model::AddMesh(const Ref<Mesh>& mesh)
    {
        LUMINA_ASSERT(mesh, "Model: Cannot add NULL mesh to model");
        
        m_Meshes.push_back(mesh);
    }

    void Model::AddMesh(Ref<Mesh>&& mesh)
    {
        LUMINA_ASSERT(mesh, "Model: Cannot add NULL mesh to model");

        m_Meshes.push_back(std::move(mesh));
    }

    void Model::RemoveMesh(size_t index)
    {
        LUMINA_ASSERT(index < m_Meshes.size(), "Model: Mesh index out of bounds");
        
        m_Meshes.erase(m_Meshes.begin() + index);
    }

    void Model::ClearMeshes()
    {
        m_Meshes.clear();
    }

    Ref<Mesh> Model::GetMesh(size_t index) const
    {
        LUMINA_ASSERT(index < m_Meshes.size(), "Model: Mesh index out of bounds");
         
        return m_Meshes[index];
    }

    void Model::Reserve(size_t meshCount)
    {
        LUMINA_ASSERT(meshCount > MIN_MESH_COUNT, "Model: Reserve count must be greater than 0");
        LUMINA_ASSERT(meshCount <= MAX_MESH_COUNT, "Model: Unreasonably large mesh count - possible memory issue");

        m_Meshes.reserve(meshCount);
    }

    size_t Model::GetTotalVertexCount() const
    {
        size_t totalVertices = 0;
        for (const auto& mesh : m_Meshes)
        {
            LUMINA_ASSERT(mesh, "Model: Found NULL mesh in model - model is corrupted");
            totalVertices += mesh->GetVertexCount();
        }
        return totalVertices;
    }

    size_t Model::GetTotalTriangleCount() const
    {
        size_t totalTriangles = 0;
        for (const auto& mesh : m_Meshes)
        {
            LUMINA_ASSERT(mesh, "Model: Found NULL mesh in model - model is corrupted");
            totalTriangles += mesh->GetTriangleCount();
        }
        return totalTriangles;
    }

    bool Model::IsValid() const
    {
        if (m_Meshes.empty())
            return false;

        for (const auto& mesh : m_Meshes)
        {
            if (!mesh || mesh->IsEmpty())
                return false;
        }
        return true;
    }
}