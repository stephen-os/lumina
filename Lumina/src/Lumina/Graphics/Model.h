#pragma once

#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_EXTERNAL_IMAGE

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <memory>
#include "VertexArray.h"
#include "Buffer.h"
#include "Texture.h"

#include "../Core/Ref.h"

namespace Lumina
{
    struct Vertex3D
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoord;
        glm::vec3 Tangent;
        glm::vec3 Bitangent;
    };

    struct Material
    {
        glm::vec3 Albedo = { 1.0f, 1.0f, 1.0f };
        float Metallic = 0.0f;
        float Roughness = 0.5f;
        float AO = 1.0f;

        Ref<Texture> AlbedoTexture = nullptr;
        Ref<Texture> NormalTexture = nullptr;
        Ref<Texture> MetallicTexture = nullptr;
        Ref<Texture> RoughnessTexture = nullptr;
        Ref<Texture> AOTexture = nullptr;
    };

    struct Mesh
    {
        std::vector<Vertex3D> Vertices;
        std::vector<uint32_t> Indices;
        Material Mat;

        Ref<VertexArray> VAO;
        Ref<VertexBuffer> VBO;
        Ref<IndexBuffer> EBO;

        void SetupMesh();
    };

	class Model : public Referencable
    {
    public:
        static Ref<Model> Create(const std::string& path);

        Model() = default;
        Model(const std::string& path);
        ~Model() = default;

        bool LoadModel(const std::string& path);
        void Draw() const;

        const std::vector<Mesh>& GetMeshes() const { return m_Meshes; }
        const std::string& GetDirectory() const { return m_Directory; }

    private:
        std::vector<Mesh> m_Meshes;
        std::string m_Directory;

        void ProcessNode(void* node, void* scene);
        Mesh ProcessMesh(void* mesh, void* scene);
        std::vector<Ref<Texture>> LoadMaterialTextures(void* mat, int type, const std::string& typeName);
    };
}