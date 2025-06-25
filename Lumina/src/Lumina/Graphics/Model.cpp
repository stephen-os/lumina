#include "Model.h"
#include "../Core/Log.h"
#include "../Core/Assert.h"
#include "BufferLayout.h"
#include "RenderCommands.h"

#include <tiny_gltf.h>
#include <iostream>
#include <filesystem>

namespace Lumina
{
	Ref<Model> Model::Create(const std::string& path)
	{
		return Ref<Model>::Create(path);
	}

    void Mesh::SetupMesh()
    {
        // Create vertex array and buffers
        VAO = VertexArray::Create();
        VBO = VertexBuffer::Create(Vertices.size() * sizeof(Vertex3D));

        // Set buffer layout
        VBO->SetLayout({
            { BufferDataType::Float3, "a_Position" },
            { BufferDataType::Float3, "a_Normal" },
            { BufferDataType::Float2, "a_TexCoord" },
            { BufferDataType::Float3, "a_Tangent" },
            { BufferDataType::Float3, "a_Bitangent" }
            });

        VAO->SetVertexBuffer(VBO);

        // Upload vertex data
        VBO->SetData(Vertices.data(), Vertices.size() * sizeof(Vertex3D));

        // Create and upload index buffer
        if (!Indices.empty())
        {
            EBO = IndexBuffer::Create(Indices.data(), Indices.size());
            VAO->SetIndexBuffer(EBO);
        }
    }

    Model::Model(const std::string& path)
    {
        LoadModel(path);
    }

    bool Model::LoadModel(const std::string& path)
    {
        tinygltf::Model gltfModel;
        tinygltf::TinyGLTF loader;
        std::string err;
        std::string warn;

        m_Directory = std::filesystem::path(path).parent_path().string();

        bool ret = false;
        if (path.find(".gltf") != std::string::npos)
        {
            ret = loader.LoadASCIIFromFile(&gltfModel, &err, &warn, path);
        }
        else if (path.find(".glb") != std::string::npos)
        {
            ret = loader.LoadBinaryFromFile(&gltfModel, &err, &warn, path);
        }

        if (!warn.empty())
        {
            LUMINA_LOG_WARN("glTF Warning: {0}", warn);
        }

        if (!err.empty())
        {
            LUMINA_LOG_ERROR("glTF Error: {0}", err);
        }

        if (!ret)
        {
            LUMINA_LOG_ERROR("Failed to parse glTF file: {0}", path);
            return false;
        }

        // Process each scene (typically there's only one)
        const tinygltf::Scene& scene = gltfModel.scenes[gltfModel.defaultScene >= 0 ? gltfModel.defaultScene : 0];

        for (size_t i = 0; i < scene.nodes.size(); ++i)
        {
            ProcessNode(&gltfModel.nodes[scene.nodes[i]], &gltfModel);
        }

        LUMINA_LOG_INFO("Successfully loaded model: {0} with {1} meshes", path, m_Meshes.size());
        return true;
    }

    void Model::ProcessNode(void* nodePtr, void* scenePtr)
    {
        tinygltf::Node* node = static_cast<tinygltf::Node*>(nodePtr);
        tinygltf::Model* model = static_cast<tinygltf::Model*>(scenePtr);

        // Process mesh if this node has one
        if (node->mesh >= 0)
        {
            tinygltf::Mesh& mesh = model->meshes[node->mesh];

            // Process each primitive in the mesh
            for (size_t i = 0; i < mesh.primitives.size(); ++i)
            {
                Mesh processedMesh = ProcessMesh(&mesh.primitives[i], model);
                if (!processedMesh.Vertices.empty())
                {
                    m_Meshes.push_back(std::move(processedMesh));
                }
            }
        }

        // Process child nodes
        for (size_t i = 0; i < node->children.size(); ++i)
        {
            ProcessNode(&model->nodes[node->children[i]], model);
        }
    }

    Mesh Model::ProcessMesh(void* primitivePtr, void* modelPtr)
    {
        tinygltf::Primitive* primitive = static_cast<tinygltf::Primitive*>(primitivePtr);
        tinygltf::Model* model = static_cast<tinygltf::Model*>(modelPtr);

        Mesh mesh;

        // Get vertex positions
        if (primitive->attributes.find("POSITION") != primitive->attributes.end())
        {
            const tinygltf::Accessor& accessor = model->accessors[primitive->attributes["POSITION"]];
            const tinygltf::BufferView& bufferView = model->bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer = model->buffers[bufferView.buffer];

            const float* positions = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

            mesh.Vertices.resize(accessor.count);
            for (size_t i = 0; i < accessor.count; ++i)
            {
                mesh.Vertices[i].Position = glm::vec3(positions[i * 3], positions[i * 3 + 1], positions[i * 3 + 2]);
            }
        }

        // Get vertex normals
        if (primitive->attributes.find("NORMAL") != primitive->attributes.end())
        {
            const tinygltf::Accessor& accessor = model->accessors[primitive->attributes["NORMAL"]];
            const tinygltf::BufferView& bufferView = model->bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer = model->buffers[bufferView.buffer];

            const float* normals = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

            for (size_t i = 0; i < mesh.Vertices.size() && i < accessor.count; ++i)
            {
                mesh.Vertices[i].Normal = glm::vec3(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]);
            }
        }

        // Get texture coordinates
        if (primitive->attributes.find("TEXCOORD_0") != primitive->attributes.end())
        {
            const tinygltf::Accessor& accessor = model->accessors[primitive->attributes["TEXCOORD_0"]];
            const tinygltf::BufferView& bufferView = model->bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer = model->buffers[bufferView.buffer];

            const float* texCoords = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

            for (size_t i = 0; i < mesh.Vertices.size() && i < accessor.count; ++i)
            {
                mesh.Vertices[i].TexCoord = glm::vec2(texCoords[i * 2], texCoords[i * 2 + 1]);
            }
        }

        // Get tangents
        if (primitive->attributes.find("TANGENT") != primitive->attributes.end())
        {
            const tinygltf::Accessor& accessor = model->accessors[primitive->attributes["TANGENT"]];
            const tinygltf::BufferView& bufferView = model->bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer = model->buffers[bufferView.buffer];

            const float* tangents = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

            for (size_t i = 0; i < mesh.Vertices.size() && i < accessor.count; ++i)
            {
                // glTF tangents are vec4, we only need xyz
                mesh.Vertices[i].Tangent = glm::vec3(tangents[i * 4], tangents[i * 4 + 1], tangents[i * 4 + 2]);
            }
        }

        // Get indices
        if (primitive->indices >= 0)
        {
            const tinygltf::Accessor& accessor = model->accessors[primitive->indices];
            const tinygltf::BufferView& bufferView = model->bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer = model->buffers[bufferView.buffer];

            mesh.Indices.resize(accessor.count);

            if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
            {
                const uint16_t* indices = reinterpret_cast<const uint16_t*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
                for (size_t i = 0; i < accessor.count; ++i)
                {
                    mesh.Indices[i] = static_cast<uint32_t>(indices[i]);
                }
            }
            else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
            {
                const uint32_t* indices = reinterpret_cast<const uint32_t*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
                for (size_t i = 0; i < accessor.count; ++i)
                {
                    mesh.Indices[i] = indices[i];
                }
            }
        }

        // Calculate bitangents if we have normals and tangents
        for (auto& vertex : mesh.Vertices)
        {
            if (glm::length(vertex.Normal) > 0.0f && glm::length(vertex.Tangent) > 0.0f)
            {
                vertex.Bitangent = glm::cross(vertex.Normal, vertex.Tangent);
            }
        }

        // Process material
        if (primitive->material >= 0)
        {
            const tinygltf::Material& gltfMaterial = model->materials[primitive->material];

            // Get base color
            if (gltfMaterial.pbrMetallicRoughness.baseColorFactor.size() >= 3)
            {
                mesh.Mat.Albedo = glm::vec3(
                    gltfMaterial.pbrMetallicRoughness.baseColorFactor[0],
                    gltfMaterial.pbrMetallicRoughness.baseColorFactor[1],
                    gltfMaterial.pbrMetallicRoughness.baseColorFactor[2]
                );
            }

            mesh.Mat.Metallic = static_cast<float>(gltfMaterial.pbrMetallicRoughness.metallicFactor);
            mesh.Mat.Roughness = static_cast<float>(gltfMaterial.pbrMetallicRoughness.roughnessFactor);

            // Load textures (simplified - you'd want to implement LoadMaterialTextures properly)
            if (gltfMaterial.pbrMetallicRoughness.baseColorTexture.index >= 0)
            {
                const tinygltf::Texture& gltfTexture = model->textures[gltfMaterial.pbrMetallicRoughness.baseColorTexture.index];
                const tinygltf::Image& gltfImage = model->images[gltfTexture.source];

                // Create texture from image data (you'll need to implement this based on your Texture class)
                // mesh.Mat.AlbedoTexture = Texture::CreateFromData(gltfImage.image.data(), gltfImage.width, gltfImage.height, gltfImage.component);
            }
        }

        mesh.SetupMesh();
        return mesh;
    }

    void Model::Draw() const
    {
        for (const auto& mesh : m_Meshes)
        {
            mesh.VAO->Bind();
            if (!mesh.Indices.empty())
            {
                RenderCommands::DrawTriangles(mesh.VAO);
            }
            else
            {
                RenderCommands::DrawArrays(mesh.VAO, PrimitiveType::Triangles, mesh.Vertices.size());
            }
            mesh.VAO->Unbind();
        }
    }

    std::vector<Ref<Texture>> Model::LoadMaterialTextures(void* mat, int type, const std::string& typeName)
    {
        // Placeholder - implement based on your texture loading system
        std::vector<Ref<Texture>> textures;
        return textures;
    }
}