#include "Mesh.h"
#include "BufferLayout.h"
#include "../Core/Log.h"

namespace Lumina
{
    Ref<Mesh> Mesh::Create()
    {
        return Ref<Mesh>::Create();
    }

    Ref<Mesh> Mesh::Create(const std::vector<Vertex3D>& vertices,
        const std::vector<uint32_t>& indices,
        const Ref<Material>& material)
    {
        auto mesh = Ref<Mesh>::Create();
        mesh->SetVertices(vertices);
        if (!indices.empty())
            mesh->SetIndices(indices);
        if (material)
            mesh->SetMaterial(material);
        return mesh;
    }

    void Mesh::SetVertices(const std::vector<Vertex3D>& vertices)
    {
        m_Vertices = vertices;
    }

    void Mesh::SetVertices(std::vector<Vertex3D>&& vertices)
    {
        m_Vertices = std::move(vertices);
    }

    void Mesh::SetIndices(const std::vector<uint32_t>& indices)
    {
        m_Indices = indices;
    }

    void Mesh::SetIndices(std::vector<uint32_t>&& indices)
    {
        m_Indices = std::move(indices);
    }

    void Mesh::SetupMesh()
    {
        if (m_Vertices.empty())
        {
            LUMINA_LOG_WARN("Cannot setup mesh '{0}': no vertices", m_Name);
            return;
        }

        // Create vertex array and buffers
        m_VAO = VertexArray::Create();
        m_VBO = VertexBuffer::Create(m_Vertices.size() * sizeof(Vertex3D));

        // Set buffer layout
        BufferLayout layout = {
            { BufferDataType::Float3, "a_Position" },
            { BufferDataType::Float3, "a_Normal" },
            { BufferDataType::Float2, "a_TexCoord" },
            { BufferDataType::Float3, "a_Tangent" },
            { BufferDataType::Float3, "a_Bitangent" }
        };
        m_VBO->SetLayout(layout);
        m_VAO->SetVertexBuffer(m_VBO);

        // Upload vertex data
        m_VBO->SetData(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex3D));

        // Create and upload index buffer if we have indices
        if (!m_Indices.empty())
        {
            m_EBO = IndexBuffer::Create(m_Indices.data(), m_Indices.size());
            m_VAO->SetIndexBuffer(m_EBO);
        }
    }

    void Mesh::UpdateGeometry()
    {
        if (!IsSetup())
        {
            SetupMesh();
            return;
        }

        // Update existing buffers
        if (m_VBO && !m_Vertices.empty())
        {
            m_VBO->SetData(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex3D));
        }

        if (m_EBO && !m_Indices.empty())
        {
            m_EBO->SetData(m_Indices.data(), m_Indices.size());
        }
    }

    size_t Mesh::GetTriangleCount() const
    {
        if (!m_Indices.empty())
        {
            return m_Indices.size() / 3;
        }
        return m_Vertices.size() / 3;
    }

    void Mesh::Clear()
    {
        m_Vertices.clear();
        m_Indices.clear();
        m_Material = nullptr;
        m_VAO = nullptr;
        m_VBO = nullptr;
        m_EBO = nullptr;
    }

    void Mesh::Reserve(size_t vertexCount, size_t indexCount)
    {
        m_Vertices.reserve(vertexCount);
        if (indexCount > 0)
            m_Indices.reserve(indexCount);
    }
}