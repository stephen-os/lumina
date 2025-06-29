#pragma once

#include "ModelLoader.h"
#include <tiny_gltf.h>

namespace Lumina
{
    class GLTFLoader : public ModelLoader
    {
    public:
        Ref<Model> LoadModel(const std::string& path) override;
        bool CanLoadFormat(const std::string& extension) const override;
        std::vector<std::string> GetSupportedExtensions() const override;

    private:
        void ProcessNode(tinygltf::Node* node, tinygltf::Model* gltfModel,
            Ref<Model> model, const std::vector<Ref<Material>>& materials);

        Ref<Mesh> ProcessMesh(tinygltf::Primitive* primitive, tinygltf::Model* gltfModel,
            const std::vector<Ref<Material>>& materials);

        Ref<Material> ProcessMaterial(tinygltf::Material* gltfMaterial, tinygltf::Model* gltfModel,
            const std::string& directory);

        Ref<Texture> LoadTexture(tinygltf::Model* gltfModel, int textureIndex, const std::string& directory);
    };
}