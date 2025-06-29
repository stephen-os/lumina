#include "Model.h"

#include "ModelRegistry.h"

#include "../Core/Log.h"

namespace Lumina
{
    Ref<Model> Model::Load(const std::string& path, ModelFormat format)
    {
        return ModelRegistry::LoadModel(path, format);
    }

    Ref<Model> Model::Create()
    {
        return Ref<Model>::Create();
    }

    Ref<Model> Model::Create(const std::string& name)
    {
        return Ref<Model>::Create(name);
    }

    Model::Model(const std::string& name)
        : m_Name(name)
    {
    }

    void Model::AddMesh(const Ref<Mesh>& mesh)
    {
        if (mesh)
        {
            m_Meshes.push_back(mesh);
        }
    }

    void Model::AddMesh(Ref<Mesh>&& mesh)
    {
        if (mesh)
        {
            m_Meshes.push_back(std::move(mesh));
        }
    }

    void Model::RemoveMesh(size_t index)
    {
        if (index < m_Meshes.size())
        {
            m_Meshes.erase(m_Meshes.begin() + index);
        }
    }

    void Model::ClearMeshes()
    {
        m_Meshes.clear();
    }

    Ref<Mesh> Model::GetMesh(size_t index) const
    {
        if (index < m_Meshes.size())
        {
            return m_Meshes[index];
        }
        return nullptr;
    }

    void Model::Reserve(size_t meshCount)
    {
        m_Meshes.reserve(meshCount);
    }

    size_t Model::GetTotalVertexCount() const
    {
        size_t totalVertices = 0;
        for (const auto& mesh : m_Meshes)
        {
            if (mesh)
                totalVertices += mesh->GetVertexCount();
        }
        return totalVertices;
    }

    size_t Model::GetTotalTriangleCount() const
    {
        size_t totalTriangles = 0;
        for (const auto& mesh : m_Meshes)
        {
            if (mesh)
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

    void Model::Clear()
    {
        m_Meshes.clear();
    }
}