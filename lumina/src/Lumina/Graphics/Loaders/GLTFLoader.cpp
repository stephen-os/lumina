#include "GLTFLoader.h"

#include <tiny_gltf.h>
#include <filesystem>

#include "Lumina/Core/Log.h"
#include "Lumina/Core/Assert.h"

#include "Lumina/Graphics/Material.h"
#include "Lumina/Graphics/Mesh.h"
#include "Lumina/Graphics/Model.h"

namespace Lumina
{
    Ref<Model> GLTFLoader::LoadModel(const std::string& path)
    {
        LUMINA_ASSERT(!path.empty(), "GLTFLoader: Path cannot be empty");
        LUMINA_ASSERT(std::filesystem::exists(path), "GLTFLoader: File does not exist");

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
        else
        {
            LUMINA_ASSERT(false, "GLTFLoader: Unsupported file extension");
        }

        if (!warn.empty())
        {
            LUMINA_LOG_WARN("GLTFLoader: {}", warn);
        }

        if (!err.empty())
        {
            LUMINA_LOG_ERROR("GLTFLoader: {}", err);
            return nullptr;
        }

        LUMINA_ASSERT(ret, "GLTFLoader: Failed to parse file");

        std::string modelName = std::filesystem::path(path).stem().string();
        auto model = Model::Create(modelName);
        std::string directory = std::filesystem::path(path).parent_path().string();

        std::vector<Ref<Material>> materials;
        materials.reserve(gltfModel.materials.size());
        for (size_t i = 0; i < gltfModel.materials.size(); ++i)
        {
            materials.push_back(ProcessMaterial(&gltfModel.materials[i], &gltfModel, directory));
        }

        int sceneIndex = gltfModel.defaultScene >= 0 ? gltfModel.defaultScene : 0;
        LUMINA_ASSERT(sceneIndex < static_cast<int>(gltfModel.scenes.size()), "GLTFLoader: Invalid scene index");

        const tinygltf::Scene& scene = gltfModel.scenes[sceneIndex];
        for (size_t nodeIndex : scene.nodes)
        {
            LUMINA_ASSERT(nodeIndex < gltfModel.nodes.size(), "GLTFLoader: Invalid node index");
            ProcessNode(&gltfModel.nodes[nodeIndex], &gltfModel, model, materials);
        }

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
        LUMINA_ASSERT(node, "GLTFLoader: Node cannot be null");
        LUMINA_ASSERT(gltfModel, "GLTFLoader: GLTF model cannot be null");
        LUMINA_ASSERT(model, "GLTFLoader: Model cannot be null");

        if (node->mesh >= 0)
        {
            LUMINA_ASSERT(node->mesh < static_cast<int>(gltfModel->meshes.size()), "GLTFLoader: Invalid mesh index");

            tinygltf::Mesh& gltfMesh = gltfModel->meshes[node->mesh];

            for (size_t i = 0; i < gltfMesh.primitives.size(); ++i)
            {
                auto mesh = ProcessMesh(&gltfMesh.primitives[i], gltfModel, materials);
                if (mesh && !mesh->IsEmpty())
                {
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

        for (size_t childIndex : node->children)
        {
            LUMINA_ASSERT(childIndex < gltfModel->nodes.size(), "GLTFLoader: Invalid child node index");
            ProcessNode(&gltfModel->nodes[childIndex], gltfModel, model, materials);
        }
    }

    Ref<Mesh> GLTFLoader::ProcessMesh(tinygltf::Primitive* primitive, tinygltf::Model* gltfModel,
        const std::vector<Ref<Material>>& materials)
    {
        LUMINA_ASSERT(primitive, "GLTFLoader: Primitive cannot be null");
        LUMINA_ASSERT(gltfModel, "GLTFLoader: GLTF model cannot be null");

        auto mesh = Mesh::Create();
        std::vector<Vertex3D> vertices;
        std::vector<uint32_t> indices;

        auto positionIt = primitive->attributes.find("POSITION");
        if (positionIt == primitive->attributes.end())
        {
            LUMINA_LOG_WARN("GLTFLoader: Mesh primitive missing POSITION attribute");
            return nullptr;
        }

        const tinygltf::Accessor& posAccessor = gltfModel->accessors[positionIt->second];
        LUMINA_ASSERT(posAccessor.bufferView >= 0, "GLTFLoader: Invalid position buffer view");
        LUMINA_ASSERT(posAccessor.bufferView < static_cast<int>(gltfModel->bufferViews.size()), "GLTFLoader: Position buffer view out of range");

        const tinygltf::BufferView& posBufferView = gltfModel->bufferViews[posAccessor.bufferView];
        LUMINA_ASSERT(posBufferView.buffer < gltfModel->buffers.size(), "GLTFLoader: Position buffer out of range");

        const tinygltf::Buffer& posBuffer = gltfModel->buffers[posBufferView.buffer];
        const float* positions = reinterpret_cast<const float*>(&posBuffer.data[posBufferView.byteOffset + posAccessor.byteOffset]);

        vertices.resize(posAccessor.count);
        for (size_t i = 0; i < posAccessor.count; ++i)
        {
            vertices[i].Position = glm::vec3(positions[i * 3], positions[i * 3 + 1], positions[i * 3 + 2]);
        }

        auto normalIt = primitive->attributes.find("NORMAL");
        if (normalIt != primitive->attributes.end())
        {
            const tinygltf::Accessor& normalAccessor = gltfModel->accessors[normalIt->second];
            const tinygltf::BufferView& normalBufferView = gltfModel->bufferViews[normalAccessor.bufferView];
            const tinygltf::Buffer& normalBuffer = gltfModel->buffers[normalBufferView.buffer];
            const float* normals = reinterpret_cast<const float*>(&normalBuffer.data[normalBufferView.byteOffset + normalAccessor.byteOffset]);

            for (size_t i = 0; i < vertices.size() && i < normalAccessor.count; ++i)
            {
                vertices[i].Normal = glm::vec3(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]);
            }
        }

        auto texCoordIt = primitive->attributes.find("TEXCOORD_0");
        if (texCoordIt != primitive->attributes.end())
        {
            const tinygltf::Accessor& texAccessor = gltfModel->accessors[texCoordIt->second];
            const tinygltf::BufferView& texBufferView = gltfModel->bufferViews[texAccessor.bufferView];
            const tinygltf::Buffer& texBuffer = gltfModel->buffers[texBufferView.buffer];
            const float* texCoords = reinterpret_cast<const float*>(&texBuffer.data[texBufferView.byteOffset + texAccessor.byteOffset]);

            for (size_t i = 0; i < vertices.size() && i < texAccessor.count; ++i)
            {
                vertices[i].TexCoord = glm::vec2(texCoords[i * 2], texCoords[i * 2 + 1]);
            }
        }

        auto tangentIt = primitive->attributes.find("TANGENT");
        if (tangentIt != primitive->attributes.end())
        {
            const tinygltf::Accessor& tangentAccessor = gltfModel->accessors[tangentIt->second];
            const tinygltf::BufferView& tangentBufferView = gltfModel->bufferViews[tangentAccessor.bufferView];
            const tinygltf::Buffer& tangentBuffer = gltfModel->buffers[tangentBufferView.buffer];
            const float* tangents = reinterpret_cast<const float*>(&tangentBuffer.data[tangentBufferView.byteOffset + tangentAccessor.byteOffset]);

            for (size_t i = 0; i < vertices.size() && i < tangentAccessor.count; ++i)
            {
                vertices[i].Tangent = glm::vec3(tangents[i * 4], tangents[i * 4 + 1], tangents[i * 4 + 2]);
            }
        }

        if (primitive->indices >= 0)
        {
            LUMINA_ASSERT(primitive->indices < static_cast<int>(gltfModel->accessors.size()), "GLTFLoader: Invalid indices accessor");

            const tinygltf::Accessor& indexAccessor = gltfModel->accessors[primitive->indices];
            const tinygltf::BufferView& indexBufferView = gltfModel->bufferViews[indexAccessor.bufferView];
            const tinygltf::Buffer& indexBuffer = gltfModel->buffers[indexBufferView.buffer];

            indices.resize(indexAccessor.count);

            if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
            {
                const uint16_t* gltfIndices = reinterpret_cast<const uint16_t*>(&indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset]);
                for (size_t i = 0; i < indexAccessor.count; ++i)
                {
                    indices[i] = static_cast<uint32_t>(gltfIndices[i]);
                }
            }
            else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
            {
                const uint32_t* gltfIndices = reinterpret_cast<const uint32_t*>(&indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset]);
                for (size_t i = 0; i < indexAccessor.count; ++i)
                {
                    indices[i] = gltfIndices[i];
                }
            }
            else
            {
                LUMINA_LOG_WARN("GLTFLoader: Unsupported index component type: {}", indexAccessor.componentType);
            }
        }

        for (auto& vertex : vertices)
        {
            if (glm::length(vertex.Normal) > 0.0f && glm::length(vertex.Tangent) > 0.0f)
            {
                vertex.Bitangent = glm::cross(vertex.Normal, vertex.Tangent);
            }
        }

        mesh->SetVertices(std::move(vertices));
        if (!indices.empty())
        {
            mesh->SetIndices(std::move(indices));
        }

        if (primitive->material >= 0 && primitive->material < static_cast<int>(materials.size()))
        {
            mesh->SetMaterial(materials[primitive->material]);
        }

        return mesh;
    }

    Ref<Material> GLTFLoader::ProcessMaterial(tinygltf::Material* gltfMaterial, tinygltf::Model* gltfModel,
        const std::string& directory)
    {
        LUMINA_ASSERT(gltfMaterial, "GLTFLoader: Material cannot be null");
        LUMINA_ASSERT(gltfModel, "GLTFLoader: GLTF model cannot be null");

        std::string materialName = gltfMaterial->name.empty() ? "Unnamed Material" : gltfMaterial->name;
        auto material = Material::Create(materialName);

        if (gltfMaterial->pbrMetallicRoughness.baseColorFactor.size() >= 3)
        {
            material->SetAlbedo(glm::vec3(
                static_cast<float>(gltfMaterial->pbrMetallicRoughness.baseColorFactor[0]),
                static_cast<float>(gltfMaterial->pbrMetallicRoughness.baseColorFactor[1]),
                static_cast<float>(gltfMaterial->pbrMetallicRoughness.baseColorFactor[2])
            ));
        }

        material->SetMetallic(static_cast<float>(gltfMaterial->pbrMetallicRoughness.metallicFactor));
        material->SetRoughness(static_cast<float>(gltfMaterial->pbrMetallicRoughness.roughnessFactor));

        if (gltfMaterial->pbrMetallicRoughness.baseColorTexture.index >= 0)
        {
            auto texture = LoadTexture(gltfModel, gltfMaterial->pbrMetallicRoughness.baseColorTexture.index, directory);
            if (texture)
                material->SetAlbedoTexture(texture);
        }

        if (gltfMaterial->normalTexture.index >= 0)
        {
            auto texture = LoadTexture(gltfModel, gltfMaterial->normalTexture.index, directory);
            if (texture)
                material->SetNormalTexture(texture);
        }

        if (gltfMaterial->pbrMetallicRoughness.metallicRoughnessTexture.index >= 0)
        {
            auto texture = LoadTexture(gltfModel, gltfMaterial->pbrMetallicRoughness.metallicRoughnessTexture.index, directory);
            if (texture)
            {
                material->SetMetallicTexture(texture);
                material->SetRoughnessTexture(texture);
            }
        }

        if (gltfMaterial->occlusionTexture.index >= 0)
        {
            auto texture = LoadTexture(gltfModel, gltfMaterial->occlusionTexture.index, directory);
            if (texture)
                material->SetAOTexture(texture);
        }

        return material;
    }

    Ref<Texture> GLTFLoader::LoadTexture(tinygltf::Model* gltfModel, int textureIndex, const std::string& directory)
    {
        LUMINA_ASSERT(gltfModel, "GLTFLoader: GLTF model cannot be null");

        if (textureIndex < 0 || textureIndex >= static_cast<int>(gltfModel->textures.size()))
        {
            LUMINA_LOG_WARN("GLTFLoader: Invalid texture index: {}", textureIndex);
            return nullptr;
        }

        const tinygltf::Texture& gltfTexture = gltfModel->textures[textureIndex];

        if (gltfTexture.source < 0 || gltfTexture.source >= static_cast<int>(gltfModel->images.size()))
        {
            LUMINA_LOG_WARN("GLTFLoader: Invalid image source index: {}", gltfTexture.source);
            return nullptr;
        }

        const tinygltf::Image& gltfImage = gltfModel->images[gltfTexture.source];

        if (!gltfImage.image.empty())
        {
            return Texture::CreateFromData(
                gltfImage.image.data(),
                gltfImage.width,
                gltfImage.height,
                gltfImage.component
            );
        }

        if (!gltfImage.uri.empty())
        {
            std::string texturePath = directory + "/" + gltfImage.uri;
            if (std::filesystem::exists(texturePath))
            {
                return Texture::Create(texturePath);
            }
            else
            {
                LUMINA_LOG_WARN("GLTFLoader: Texture file not found: {}", texturePath);
            }
        }

        LUMINA_LOG_WARN("GLTFLoader: Failed to load texture at index {}", textureIndex);
        return nullptr;
    }
}