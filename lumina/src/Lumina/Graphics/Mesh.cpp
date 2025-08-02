#include "Mesh.h"

#include "BufferLayout.h"

namespace Lumina
{
    Ref<Mesh> Mesh::Create()
    {
        return CreateRef<Mesh>();
    }

    Ref<Mesh> Mesh::Create(const std::vector<Vertex3D>& vertices,
        const std::vector<uint32_t>& indices,
        const Ref<Material>& material)
    {
        LUMINA_ASSERT(!vertices.empty(), "Mesh: Cannot create mesh with empty vertices");

        auto mesh = Mesh::Create();
        mesh->SetVertices(vertices);
        if (!indices.empty())
            mesh->SetIndices(indices);
        if (material)
            mesh->SetMaterial(material);
        return mesh;
    }

    void Mesh::SetVertices(const std::vector<Vertex3D>& vertices)
    {
        LUMINA_ASSERT(!vertices.empty(), "Mesh: Vertices cannot be empty");
        LUMINA_ASSERT(vertices.size() <= MAX_VERTEX_COUNT, "Mesh: Too many vertices");

        m_Vertices = vertices;
    }

    void Mesh::SetVertices(std::vector<Vertex3D>&& vertices)
    {
        LUMINA_ASSERT(!vertices.empty(), "Mesh: Vertices cannot be empty");
        LUMINA_ASSERT(vertices.size() <= MAX_VERTEX_COUNT, "Mesh: Too many vertices");

        m_Vertices = std::move(vertices);
    }

    void Mesh::SetIndices(const std::vector<uint32_t>& indices)
    {
        LUMINA_ASSERT(!indices.empty(), "Mesh: Indices cannot be empty");
        LUMINA_ASSERT(indices.size() % 3 == 0, "Mesh: Index count must be multiple of 3");
        LUMINA_ASSERT(indices.size() <= MAX_INDEX_COUNT, "Mesh: Too many indices");

        m_Indices = indices;
    }

    void Mesh::SetIndices(std::vector<uint32_t>&& indices)
    {
        LUMINA_ASSERT(!indices.empty(), "Mesh: Indices cannot be empty");
        LUMINA_ASSERT(indices.size() % 3 == 0, "Mesh: Index count must be multiple of 3");
        LUMINA_ASSERT(indices.size() <= MAX_INDEX_COUNT, "Mesh: Too many indices");

        m_Indices = std::move(indices);
    }

    void Mesh::SetupMesh()
    {
        LUMINA_ASSERT(!m_Vertices.empty(), "Mesh: Cannot setup mesh with no vertices");

        if (IsSetup())
        {
            LUMINA_LOG_WARN("Mesh: '{}' already setup, skipping", m_Name);
            return;
        }

        m_VAO = VertexArray::Create();
        LUMINA_ASSERT(m_VAO, "Mesh: Failed to create VAO");

        m_VBO = VertexBuffer::Create((uint32_t)m_Vertices.size() * sizeof(Vertex3D));
        LUMINA_ASSERT(m_VBO, "Mesh: Failed to create VBO");

        BufferLayout layout =
        {
            { BufferDataType::Float3, "a_Position" },
            { BufferDataType::Float3, "a_Normal" },
            { BufferDataType::Float2, "a_TexCoord" },
            { BufferDataType::Float3, "a_Tangent" },
            { BufferDataType::Float3, "a_Bitangent" }
        };

        m_VBO->SetLayout(layout);
        m_VAO->SetVertexBuffer(m_VBO);
        m_VBO->SetData(m_Vertices.data(), (uint32_t)m_Vertices.size() * sizeof(Vertex3D));

        if (!m_Indices.empty())
        {
            m_EBO = IndexBuffer::Create(m_Indices.data(), (uint32_t)m_Indices.size());
            LUMINA_ASSERT(m_EBO, "Mesh: Failed to create EBO");
            m_VAO->SetIndexBuffer(m_EBO);
        }
    }

    void Mesh::UpdateGeometry()
    {
        LUMINA_ASSERT(!m_Vertices.empty(), "Mesh: Cannot update geometry with no vertices");

        if (!IsSetup())
        {
            SetupMesh();
            return;
        }

        LUMINA_ASSERT(m_VBO, "Mesh: VBO is null during update");
        m_VBO->SetData(m_Vertices.data(), (uint32_t)m_Vertices.size() * sizeof(Vertex3D));

        if (!m_Indices.empty())
        {
            LUMINA_ASSERT(m_EBO, "Mesh: EBO is null but indices exist");
            m_EBO->SetData(m_Indices.data(), (uint32_t)m_Indices.size());
        }
    }

    size_t Mesh::GetTriangleCount() const
    {
        if (!m_Indices.empty())
        {
            LUMINA_ASSERT(m_Indices.size() % 3 == 0, "Mesh: Invalid index count for triangles");
            return m_Indices.size() / 3;
        }

        LUMINA_ASSERT(m_Vertices.size() % 3 == 0, "Mesh: Invalid vertex count for triangles");
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
        LUMINA_ASSERT(vertexCount > 0, "Mesh: Vertex count must be greater than 0");
        LUMINA_ASSERT(vertexCount <= MAX_VERTEX_COUNT, "Mesh: Vertex count exceeds maximum");

        if (indexCount > 0)
        {
            LUMINA_ASSERT(indexCount <= MAX_INDEX_COUNT, "Mesh: Index count exceeds maximum");
            LUMINA_ASSERT(indexCount % 3 == 0, "Mesh: Index count must be multiple of 3");
        }

        m_Vertices.reserve(vertexCount);
        if (indexCount > 0)
            m_Indices.reserve(indexCount);
    }

    void Mesh::SetName(const std::string& name)
    {
        LUMINA_ASSERT(!name.empty(), "Mesh: Name cannot be empty");
        m_Name = name;
    }
}