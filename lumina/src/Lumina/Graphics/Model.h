#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Core/Ref.h"

#include "Formats/ModelFormat.h"

#include "Mesh.h"

namespace Lumina
{
    struct ModelAttributes
    {
        glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };
        glm::vec4 TintColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        float PointSize = 1.0f;

		glm::mat4 GetModelMatrix() const
        {
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), Position);

            glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            glm::mat4 rotation = rotationZ * rotationY * rotationX;

            glm::mat4 scale = glm::scale(glm::mat4(1.0f), Scale);

            return translation * rotation * scale;
        }
    };

    class Model : public Referencable
    {
    public:
        static Ref<Model> Load(const std::string& path, ModelFormat format = ModelFormat::AUTO_DETECT);
        static Ref<Model> Load(const std::string& path, const std::string& name, ModelFormat format = ModelFormat::AUTO_DETECT);

        static Ref<Model> Create(const std::string& name = "Unnamed Model");

        Model() = default;
        Model(const std::string& name);
        ~Model() = default;

        void AddMesh(const Ref<Mesh>& mesh);
        void AddMesh(Ref<Mesh>&& mesh);
        void RemoveMesh(size_t index);
        void ClearMeshes();

        const std::vector<Ref<Mesh>>& GetMeshes() const { return m_Meshes; }
        std::vector<Ref<Mesh>>& GetMeshes() { return m_Meshes; }

        Ref<Mesh> GetMesh(size_t index) const;

        void SetName(const std::string& name) { m_Name = name; }
        const std::string& GetName() const { return m_Name; }

        void SetDirectory(const std::string& directory) { m_Directory = directory; }
        const std::string& GetDirectory() const { return m_Directory; }

        size_t GetMeshCount() const { return m_Meshes.size(); }
        bool IsEmpty() const { return m_Meshes.empty(); }
        
        void Reserve(size_t meshCount);
        size_t GetTotalVertexCount() const;
        size_t GetTotalTriangleCount() const;
        bool IsValid() const;

        uint64_t GetUUID() const { return m_UUID; }

    private:
        uint64_t m_UUID = 0;
        std::string m_Name = "Unnamed Model";
        std::string m_Directory = "";
        std::vector<Ref<Mesh>> m_Meshes;

        static constexpr size_t MAX_MESH_COUNT = 100000;
        static constexpr size_t MIN_MESH_COUNT = 0; 
    };
}