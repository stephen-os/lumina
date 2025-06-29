#include "ModelRegistry.h"

#include "Loaders/GLTFLoader.h"

#include "../Core/Log.h"
#include "../Core/Assert.h"

#include <filesystem>
#include <algorithm>

namespace Lumina
{
    ModelRegistry::ModelRegistryData ModelRegistry::s_Data;

    const std::unordered_map<std::string, ModelFormat> ModelRegistry::s_ExtensionMap =
    {
        {".gltf", ModelFormat::GLTF},
        {".glb", ModelFormat::GLB}
    };

    void ModelRegistry::Init()
    {
        if (s_Data.Initialized)
        {
            LUMINA_LOG_WARN("ModelRegistry already initialized");
            return;
        }

        LUMINA_LOG_INFO("Initializing ModelRegistry...");

        // Clear any existing data
        s_Data.FormatLoaders.clear();
        s_Data.ExtensionLoaders.clear();

        // Initialize default loaders
        InitializeDefaultLoaders();

        s_Data.Initialized = true;

        // Log what we support
        LogSupportedFormats();

        LUMINA_LOG_INFO("ModelRegistry initialized successfully");
    }

    void ModelRegistry::Shutdown()
    {
        if (!s_Data.Initialized)
        {
            LUMINA_LOG_WARN("ModelRegistry not initialized, nothing to shutdown");
            return;
        }

        LUMINA_LOG_INFO("Shutting down ModelRegistry...");

        // Clear all loaders (unique_ptr will clean up automatically)
        s_Data.FormatLoaders.clear();
        s_Data.ExtensionLoaders.clear();

        s_Data.Initialized = false;

        LUMINA_LOG_INFO("ModelRegistry shutdown complete");
    }

    bool ModelRegistry::IsInitialized()
    {
        return s_Data.Initialized;
    }

    Ref<Model> ModelRegistry::LoadModel(const std::string& path, ModelFormat format)
    {
        LUMINA_ASSERT(s_Data.Initialized, "ModelRegistry not initialized! Call ModelRegistry::Init() first.");

        if (!std::filesystem::exists(path))
        {
            LUMINA_LOG_ERROR("Model file not found: {0}", path);
            return nullptr;
        }

        if (format == ModelFormat::AUTO_DETECT)
        {
            format = DetectFormat(path);
        }

        ModelLoader* loader = GetLoaderForFormat(format);
        if (!loader)
        {
            LUMINA_LOG_ERROR("No loader available for format: {0}", static_cast<int>(format));
            return nullptr;
        }

        try
        {
            LUMINA_LOG_INFO("Loading model: {0}", path);
            auto model = loader->LoadModel(path);
            if (model)
            {
                model->SetDirectory(std::filesystem::path(path).parent_path().string());
                LUMINA_LOG_INFO("Successfully loaded model: {0} with {1} meshes", path, model->GetMeshCount());
            }
            else
            {
                LUMINA_LOG_ERROR("Failed to load model: {0}", path);
            }
            return model;
        }
        catch (const std::exception& e)
        {
            LUMINA_LOG_ERROR("Exception while loading model {0}: {1}", path, e.what());
            return nullptr;
        }
    }

    void ModelRegistry::RegisterLoader(ModelFormat format, std::unique_ptr<ModelLoader> loader)
    {
        LUMINA_ASSERT(s_Data.Initialized, "ModelRegistry not initialized! Call ModelRegistry::Init() first.");
        LUMINA_ASSERT(loader != nullptr, "Cannot register null loader");

        s_Data.FormatLoaders[format] = std::move(loader);
        LUMINA_LOG_INFO("Registered loader for format: {0}", static_cast<int>(format));
    }

    void ModelRegistry::RegisterLoader(const std::string& extension, std::unique_ptr<ModelLoader> loader)
    {
        LUMINA_ASSERT(s_Data.Initialized, "ModelRegistry not initialized! Call ModelRegistry::Init() first.");
        LUMINA_ASSERT(loader != nullptr, "Cannot register null loader");

        std::string ext = NormalizeExtension(extension);
        s_Data.ExtensionLoaders[ext] = std::move(loader);
        LUMINA_LOG_INFO("Registered loader for extension: {0}", ext);
    }

    ModelFormat ModelRegistry::DetectFormat(const std::string& path)
    {
        std::string extension = std::filesystem::path(path).extension().string();
        extension = NormalizeExtension(extension);

        auto it = s_ExtensionMap.find(extension);
        if (it != s_ExtensionMap.end())
        {
            return it->second;
        }

        LUMINA_LOG_WARN("Unknown file extension: {0}, defaulting to GLTF loader", extension);
        return ModelFormat::GLTF;
    }

    std::vector<std::string> ModelRegistry::GetSupportedExtensions()
    {
        LUMINA_ASSERT(s_Data.Initialized, "ModelRegistry not initialized! Call ModelRegistry::Init() first.");

        std::vector<std::string> extensions;
        for (const auto& pair : s_ExtensionMap)
        {
            extensions.push_back(pair.first);
        }

        // Also add any custom extensions
        for (const auto& pair : s_Data.ExtensionLoaders)
        {
            if (std::find(extensions.begin(), extensions.end(), pair.first) == extensions.end())
            {
                extensions.push_back(pair.first);
            }
        }

        return extensions;
    }

    std::vector<ModelFormat> ModelRegistry::GetSupportedFormats()
    {
        LUMINA_ASSERT(s_Data.Initialized, "ModelRegistry not initialized! Call ModelRegistry::Init() first.");

        std::vector<ModelFormat> formats;
        for (const auto& pair : s_Data.FormatLoaders)
        {
            formats.push_back(pair.first);
        }
        return formats;
    }

    void ModelRegistry::LogSupportedFormats()
    {
        if (!s_Data.Initialized)
            return;

        auto extensions = GetSupportedExtensions();
        std::string extensionList;
        for (size_t i = 0; i < extensions.size(); ++i)
        {
            extensionList += extensions[i];
            if (i < extensions.size() - 1)
                extensionList += ", ";
        }

        LUMINA_LOG_INFO("ModelRegistry supports: {0}", extensionList);
    }

    void ModelRegistry::InitializeDefaultLoaders()
    {
        // Register glTF/GLB loader (optimized for these formats)
        s_Data.FormatLoaders[ModelFormat::GLTF] = std::make_unique<GLTFLoader>();
        s_Data.FormatLoaders[ModelFormat::GLB] = std::make_unique<GLTFLoader>();

        LUMINA_LOG_INFO("Default model loaders initialized");
    }

    ModelLoader* ModelRegistry::GetLoaderForFormat(ModelFormat format)
    {
        auto it = s_Data.FormatLoaders.find(format);
        return (it != s_Data.FormatLoaders.end()) ? it->second.get() : nullptr;
    }

    ModelLoader* ModelRegistry::GetLoaderForExtension(const std::string& extension)
    {
        std::string ext = NormalizeExtension(extension);
        auto it = s_Data.ExtensionLoaders.find(ext);
        return (it != s_Data.ExtensionLoaders.end()) ? it->second.get() : nullptr;
    }

    std::string ModelRegistry::NormalizeExtension(const std::string& extension)
    {
        std::string ext = extension;
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (!ext.empty() && ext[0] != '.')
            ext = "." + ext;
        return ext;
    }
}