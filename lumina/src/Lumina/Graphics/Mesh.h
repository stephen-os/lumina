#pragma once

#include <vector>
#include <string>

#include "Core/Base.h"

#include "Vertex3D.h"
#include "Material.h"
#include "VertexArray.h"
#include "Buffer.h"

namespace Lumina
{
    class Mesh
    {
    public:
        static Ref<Mesh> Create();
        static Ref<Mesh> Create(const std::vector<Vertex3D>& vertices,
            const std::vector<uint32_t>& indices = {},
            const Ref<Material>& material = nullptr);

        Mesh() = default;
        ~Mesh() = default;

        void SetVertices(const std::vector<Vertex3D>& vertices);
        void SetVertices(std::vector<Vertex3D>&& vertices);
        const std::vector<Vertex3D>& GetVertices() const { return m_Vertices; }
        std::vector<Vertex3D>& GetVertices() { return m_Vertices; }

        void SetIndices(const std::vector<uint32_t>& indices);
        void SetIndices(std::vector<uint32_t>&& indices);
        const std::vector<uint32_t>& GetIndices() const { return m_Indices; }
        std::vector<uint32_t>& GetIndices() { return m_Indices; }

        void SetMaterial(const Ref<Material>& material) { m_Material = material; }
        Ref<Material> GetMaterial() const { return m_Material; }

        Ref<VertexArray> GetVAO() const { return m_VAO; }
        Ref<VertexBuffer> GetVBO() const { return m_VBO; }
        Ref<IndexBuffer> GetEBO() const { return m_EBO; }

        void SetupMesh();
        void UpdateGeometry();
        bool IsSetup() const { return m_VAO != nullptr; }

        void SetName(const std::string& name);
        const std::string& GetName() const { return m_Name; }

        size_t GetVertexCount() const { return m_Vertices.size(); }
        size_t GetIndexCount() const { return m_Indices.size(); }
        size_t GetTriangleCount() const;

        bool IsEmpty() const { return m_Vertices.empty(); }
        bool HasIndices() const { return !m_Indices.empty(); }

        void Clear();
        void Reserve(size_t vertexCount, size_t indexCount = 0);

    private:
        std::string m_Name = "Unnamed Mesh";
        std::vector<Vertex3D> m_Vertices;
        std::vector<uint32_t> m_Indices;
        
        Ref<Material> m_Material = nullptr;

        Ref<VertexArray> m_VAO = nullptr;
        Ref<VertexBuffer> m_VBO = nullptr;
        Ref<IndexBuffer> m_EBO = nullptr;

        static constexpr size_t MAX_VERTEX_COUNT = 10000000;  // 10M vertices
        static constexpr size_t MAX_INDEX_COUNT = 30000000;   // 30M indices
    };
}