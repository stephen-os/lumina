#include "GLTFLoader.h"
#include "../Material.h"
#include "../Mesh.h"
#include "../Model.h"
#include "../../Core/Log.h"

#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_EXTERNAL_IMAGE

#include <tiny_gltf.h>

#include <filesystem>

namespace Lumina
{
    Ref<Model> GLTFLoader::LoadModel(const std::string& path)
    {
        tinygltf::Model gltfModel;
        tinygltf::TinyGLTF loader;
        std::string err, warn;

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
            return nullptr;
        }

        // Create the model
        std::string modelName = std::filesystem::path(path).stem().string();
        auto model = Model::Create(modelName);
        std::string directory = std::filesystem::path(path).parent_path().string();

        // Pre-load all materials
        std::vector<Ref<Material>> materials;
        materials.reserve(gltfModel.materials.size());
        for (size_t i = 0; i < gltfModel.materials.size(); ++i)
        {
            materials.push_back(ProcessMaterial(&gltfModel.materials[i], &gltfModel, directory));
        }

        // Process each scene (typically there's only one)
        const tinygltf::Scene& scene = gltfModel.scenes[gltfModel.defaultScene >= 0 ? gltfModel.defaultScene : 0];

        for (size_t i = 0; i < scene.nodes.size(); ++i)
        {
            ProcessNode(&gltfModel.nodes[scene.nodes[i]], &gltfModel, model, materials);
        }

        LUMINA_LOG_INFO("Successfully loaded glTF model: {0} with {1} meshes", path, model->GetMeshCount());
        return model;
    }

    bool GLTFLoader::CanLoadFormat(const std::string& extension) const
    {
        return extension == ".gltf" || extension == ".glb";
    }

    std::vector<std::string> GLTFLoader::GetSupportedExtensions() const
    {
        return { ".gltf", ".glb" };
    }

    void GLTFLoader::ProcessNode(tinygltf::Node* node, tinygltf::Model* gltfModel,
        Ref<Model> model, const std::vector<Ref<Material>>& materials)
    {
        // Process mesh if this node has one
        if (node->mesh >= 0)
        {
            tinygltf::Mesh& gltfMesh = gltfModel->meshes[node->mesh];

            // Process each primitive in the mesh
            for (size_t i = 0; i < gltfMesh.primitives.size(); ++i)
            {
                auto mesh = ProcessMesh(&gltfMesh.primitives[i], gltfModel, materials);
                if (mesh && !mesh->IsEmpty())
                {
                    // Set mesh name from glTF mesh name + primitive index
                    std::string meshName = gltfMesh.name.empty() ?
                        "Mesh_" + std::to_string(node->mesh) :
                        gltfMesh.name;
                    if (gltfMesh.primitives.size() > 1)
                        meshName += "_" + std::to_string(i);

                    mesh->SetName(meshName);
                    mesh->SetupMesh();
                    model->AddMesh(mesh);
                }
            }
        }

        // Process child nodes
        for (size_t i = 0; i < node->children.size(); ++i)
        {
            ProcessNode(&gltfModel->nodes[node->children[i]], gltfModel, model, materials);
        }
    }

    Ref<Mesh> GLTFLoader::ProcessMesh(tinygltf::Primitive* primitive, tinygltf::Model* gltfModel,
        const std::vector<Ref<Material>>& materials)
    {
        auto mesh = Mesh::Create();
        std::vector<Vertex3D> vertices;
        std::vector<uint32_t> indices;

        // Get vertex positions
        if (primitive->attributes.find("POSITION") != primitive->attributes.end())
        {
            const tinygltf::Accessor& accessor = gltfModel->accessors[primitive->attributes["POSITION"]];
            const tinygltf::BufferView& bufferView = gltfModel->bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer = gltfModel->buffers[bufferView.buffer];

            const float* positions = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

            vertices.resize(accessor.count);
            for (size_t i = 0; i < accessor.count; ++i)
            {
                vertices[i].Position = glm::vec3(positions[i * 3], positions[i * 3 + 1], positions[i * 3 + 2]);
            }
        }

        // Get vertex normals
        if (primitive->attributes.find("NORMAL") != primitive->attributes.end())
        {
            const tinygltf::Accessor& accessor = gltfModel->accessors[primitive->attributes["NORMAL"]];
            const tinygltf::BufferView& bufferView = gltfModel->bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer = gltfModel->buffers[bufferView.buffer];

            const float* normals = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

            for (size_t i = 0; i < vertices.size() && i < accessor.count; ++i)
            {
                vertices[i].Normal = glm::vec3(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]);
            }
        }

        // Get texture coordinates
        if (primitive->attributes.find("TEXCOORD_0") != primitive->attributes.end())
        {
            const tinygltf::Accessor& accessor = gltfModel->accessors[primitive->attributes["TEXCOORD_0"]];
            const tinygltf::BufferView& bufferView = gltfModel->bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer = gltfModel->buffers[bufferView.buffer];

            const float* texCoords = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

            for (size_t i = 0; i < vertices.size() && i < accessor.count; ++i)
            {
                vertices[i].TexCoord = glm::vec2(texCoords[i * 2], texCoords[i * 2 + 1]);
            }
        }

        // Get tangents
        if (primitive->attributes.find("TANGENT") != primitive->attributes.end())
        {
            const tinygltf::Accessor& accessor = gltfModel->accessors[primitive->attributes["TANGENT"]];
            const tinygltf::BufferView& bufferView = gltfModel->bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer = gltfModel->buffers[bufferView.buffer];

            const float* tangents = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

            for (size_t i = 0; i < vertices.size() && i < accessor.count; ++i)
            {
                // glTF tangents are vec4, we only need xyz
                vertices[i].Tangent = glm::vec3(tangents[i * 4], tangents[i * 4 + 1], tangents[i * 4 + 2]);
            }
        }

        // Get indices
        if (primitive->indices >= 0)
        {
            const tinygltf::Accessor& accessor = gltfModel->accessors[primitive->indices];
            const tinygltf::BufferView& bufferView = gltfModel->bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer = gltfModel->buffers[bufferView.buffer];

            indices.resize(accessor.count);

            if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
            {
                const uint16_t* gltfIndices = reinterpret_cast<const uint16_t*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
                for (size_t i = 0; i < accessor.count; ++i)
                {
                    indices[i] = static_cast<uint32_t>(gltfIndices[i]);
                }
            }
            else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
            {
                const uint32_t* gltfIndices = reinterpret_cast<const uint32_t*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
                for (size_t i = 0; i < accessor.count; ++i)
                {
                    indices[i] = gltfIndices[i];
                }
            }
        }

        // Calculate bitangents if we have normals and tangents
        for (auto& vertex : vertices)
        {
            if (glm::length(vertex.Normal) > 0.0f && glm::length(vertex.Tangent) > 0.0f)
            {
                vertex.Bitangent = glm::cross(vertex.Normal, vertex.Tangent);
            }
        }

        // Set mesh data
        mesh->SetVertices(std::move(vertices));
        if (!indices.empty())
        {
            mesh->SetIndices(std::move(indices));
        }

        // Set material
        if (primitive->material >= 0 && primitive->material < static_cast<int>(materials.size()))
        {
            mesh->SetMaterial(materials[primitive->material]);
        }

        return mesh;
    }

    Ref<Material> GLTFLoader::ProcessMaterial(tinygltf::Material* gltfMaterial, tinygltf::Model* gltfModel,
        const std::string& directory)
    {
        std::string materialName = gltfMaterial->name.empty() ? "Unnamed Material" : gltfMaterial->name;
        auto material = Material::Create(materialName);

        // Get base color
        if (gltfMaterial->pbrMetallicRoughness.baseColorFactor.size() >= 3)
        {
            material->SetAlbedo(glm::vec3(
                static_cast<float>(gltfMaterial->pbrMetallicRoughness.baseColorFactor[0]),
                static_cast<float>(gltfMaterial->pbrMetallicRoughness.baseColorFactor[1]),
                static_cast<float>(gltfMaterial->pbrMetallicRoughness.baseColorFactor[2])
            ));
        }

        // Get metallic and roughness factors
        material->SetMetallic(static_cast<float>(gltfMaterial->pbrMetallicRoughness.metallicFactor));
        material->SetRoughness(static_cast<float>(gltfMaterial->pbrMetallicRoughness.roughnessFactor));

        // Load textures
        if (gltfMaterial->pbrMetallicRoughness.baseColorTexture.index >= 0)
        {
            auto texture = LoadTexture(gltfModel, gltfMaterial->pbrMetallicRoughness.baseColorTexture.index, directory);
            material->SetAlbedoTexture(texture);
        }

        if (gltfMaterial->normalTexture.index >= 0)
        {
            auto texture = LoadTexture(gltfModel, gltfMaterial->normalTexture.index, directory);
            material->SetNormalTexture(texture);
        }

        if (gltfMaterial->pbrMetallicRoughness.metallicRoughnessTexture.index >= 0)
        {
            auto texture = LoadTexture(gltfModel, gltfMaterial->pbrMetallicRoughness.metallicRoughnessTexture.index, directory);
            // glTF stores metallic in B channel and roughness in G channel
            material->SetMetallicTexture(texture);
            material->SetRoughnessTexture(texture);
        }

        if (gltfMaterial->occlusionTexture.index >= 0)
        {
            auto texture = LoadTexture(gltfModel, gltfMaterial->occlusionTexture.index, directory);
            material->SetAOTexture(texture);
        }

        return material;
    }


    Ref<Texture> GLTFLoader::LoadTexture(tinygltf::Model* gltfModel, int textureIndex, const std::string& directory)
    {
        if (textureIndex < 0 || textureIndex >= static_cast<int>(gltfModel->textures.size()))
        {
            return nullptr;
        }

        const tinygltf::Texture& gltfTexture = gltfModel->textures[textureIndex];

        if (gltfTexture.source < 0 || gltfTexture.source >= static_cast<int>(gltfModel->images.size()))
        {
            return nullptr;
        }

        const tinygltf::Image& gltfImage = gltfModel->images[gltfTexture.source];

        // If image has embedded data, create texture from data
        if (!gltfImage.image.empty())
        {
            // Now we properly use the component information!
            return Texture::CreateFromData(
                gltfImage.image.data(),
                gltfImage.width,
                gltfImage.height,
                gltfImage.component
            );
        }

        // If image has URI, load from file
        if (!gltfImage.uri.empty())
        {
            std::string texturePath = directory + "/" + gltfImage.uri;
            return Texture::Create(texturePath);
        }

        LUMINA_LOG_WARN("Failed to load texture at index {0}", textureIndex);
        return nullptr;
    }
}