#include "ModelRegistry.h"

#include <filesystem>
#include <algorithm>

#include "Lumina/Core/Base.h"

#include "Loaders/GLTFLoader.h"

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
            LUMINA_LOG_WARN("ModelRegistry: Already initialized");
            return;
        }

        LUMINA_LOG_INFO("ModelRegistry: Initializing...");

        s_Data.FormatLoaders.clear();
        s_Data.ExtensionLoaders.clear();
        
        s_Data.FormatLoaders[ModelFormat::GLTF] = std::make_unique<GLTFLoader>();
        s_Data.FormatLoaders[ModelFormat::GLB] = std::make_unique<GLTFLoader>();

        s_Data.Initialized = true;
        LogSupportedFormats();

        LUMINA_LOG_INFO("ModelRegistry: Initialization complete");
    }

    void ModelRegistry::Shutdown()
    {
        LUMINA_ASSERT(s_Data.Initialized, "ModelRegistry: Cannot shutdown - not initialized");

        LUMINA_LOG_INFO("ModelRegistry: Shutting down");
        s_Data.FormatLoaders.clear();
        s_Data.ExtensionLoaders.clear();
        s_Data.Initialized = false;
        LUMINA_LOG_INFO("ModelRegistry: Shutdown complete");
    }

    bool ModelRegistry::IsInitialized()
    {
        return s_Data.Initialized;
    }

    Ref<Model> ModelRegistry::LoadModel(const std::string& path, ModelFormat format)
    {
        LUMINA_ASSERT(s_Data.Initialized, "ModelRegistry: Not initialized");
        LUMINA_ASSERT(!path.empty(), "ModelRegistry: Path cannot be empty");
        LUMINA_ASSERT(std::filesystem::exists(path), "ModelRegistry: File does not exist");

        auto fileSize = std::filesystem::file_size(path);
        ModelFormat originalFormat = format;

        if (format == ModelFormat::AUTO_DETECT)
        {
            format = DetectFormat(path);
        }

        ModelLoader* loader = GetLoaderForFormat(format);
        LUMINA_ASSERT(loader, "ModelRegistry: No loader available for format");

        try
        {
            auto model = loader->LoadModel(path);
            LUMINA_ASSERT(model, "ModelRegistry: Loader returned null");

            model->SetDirectory(std::filesystem::path(path).parent_path().string());
            
            return model;
        }
        catch (const std::exception& e)
        {
            LUMINA_LOG_ERROR("ModelRegistry: Exception loading model '{}': {}", path, e.what());
            return nullptr;
        }
        catch (...)
        {
            LUMINA_LOG_ERROR("ModelRegistry: Unknown exception loading model: {}", path);
            return nullptr;
        }
    }

    void ModelRegistry::RegisterLoader(ModelFormat format, std::unique_ptr<ModelLoader> loader)
    {
        LUMINA_ASSERT(s_Data.Initialized, "ModelRegistry: Not initialized");
        LUMINA_ASSERT(loader != nullptr, "ModelRegistry: Cannot register null loader");
        LUMINA_ASSERT(format != ModelFormat::AUTO_DETECT, "ModelRegistry: Cannot register AUTO_DETECT loader");
        LUMINA_ASSERT(format != ModelFormat::UNKNOWN, "ModelRegistry: Cannot register UNKNOWN loader");

        s_Data.FormatLoaders[format] = std::move(loader);
        LUMINA_LOG_INFO("Registered loader for format: {}", ModelFormatToString(format));
    }

    void ModelRegistry::RegisterLoader(const std::string& extension, std::unique_ptr<ModelLoader> loader)
    {
        LUMINA_ASSERT(s_Data.Initialized, "ModelRegistry: Not initialized");
        LUMINA_ASSERT(loader != nullptr, "ModelRegistry: Cannot register null loader");
        LUMINA_ASSERT(!extension.empty(), "ModelRegistry: Extension cannot be empty");

        std::string ext = NormalizeExtension(extension);
        s_Data.ExtensionLoaders[ext] = std::move(loader);
        LUMINA_LOG_INFO("ModelRegistry: Registered loader for extension: {}", ext);
    }

    ModelFormat ModelRegistry::DetectFormat(const std::string& path)
    {
        LUMINA_ASSERT(!path.empty(), "ModelRegistry: Path cannot be empty");

        std::string extension = std::filesystem::path(path).extension().string();
        extension = NormalizeExtension(extension);

        if (extension.empty())
        {
            LUMINA_LOG_WARN("ModelRegistry: No extension for: {}, defaulting to GLTF", path);
            return ModelFormat::GLTF;
        }

        auto it = s_ExtensionMap.find(extension);
        if (it != s_ExtensionMap.end())
            return it->second;

        auto extIt = s_Data.ExtensionLoaders.find(extension);
        if (extIt != s_Data.ExtensionLoaders.end())
            return ModelFormat::GLTF;

        LUMINA_LOG_WARN("ModelRegistry: Unknown extension '{}' for {}, defaulting to GLTF", extension, path);
        return ModelFormat::GLTF;
    }

    std::vector<std::string> ModelRegistry::GetSupportedExtensions()
    {
        LUMINA_ASSERT(s_Data.Initialized, "ModelRegistry: Not initialized");

        std::vector<std::string> extensions;

        for (const auto& pair : s_ExtensionMap)
            extensions.push_back(pair.first);

        for (const auto& pair : s_Data.ExtensionLoaders)
        {
            if (std::find(extensions.begin(), extensions.end(), pair.first) == extensions.end())
                extensions.push_back(pair.first);
        }

        return extensions;
    }

    std::vector<ModelFormat> ModelRegistry::GetSupportedFormats()
    {
        LUMINA_ASSERT(s_Data.Initialized, "ModelRegistry: Not initialized");

        std::vector<ModelFormat> formats;
        for (const auto& pair : s_Data.FormatLoaders)
            formats.push_back(pair.first);
        return formats;
    }

    std::string ModelRegistry::ModelFormatToString(ModelFormat format)
    {
        switch (format)
        {
        case ModelFormat::AUTO_DETECT: return "AUTO_DETECT";
        case ModelFormat::GLTF:        return "GLTF";
        case ModelFormat::GLB:         return "GLB";
        case ModelFormat::OBJ:         return "OBJ";
        case ModelFormat::FBX:         return "FBX";
        case ModelFormat::DAE:         return "DAE";
        case ModelFormat::BLEND:       return "BLEND";
        case ModelFormat::X3D:         return "X3D";
        case ModelFormat::UNKNOWN:     return "UNKNOWN";
        default:                       return "INVALID";
        }
    }

    void ModelRegistry::LogSupportedFormats()
    {
        LUMINA_ASSERT(s_Data.Initialized, "ModelRegistry: Not initialized");

        auto extensions = GetSupportedExtensions();
        auto formats = GetSupportedFormats();

        std::string extensionList;
        for (size_t i = 0; i < extensions.size(); ++i)
        {
            extensionList += extensions[i];
            if (i < extensions.size() - 1)
                extensionList += ", ";
        }

        std::string formatList;
        for (size_t i = 0; i < formats.size(); ++i)
        {
            formatList += ModelFormatToString(formats[i]);
            if (i < formats.size() - 1)
                formatList += ", ";
        }

        LUMINA_LOG_INFO("ModelRegistry: Supports {} extensions: {}", extensions.size(), extensionList);
        LUMINA_LOG_INFO("ModelRegistry: Supports {} formats: {}", formats.size(), formatList);
    }

    ModelLoader* ModelRegistry::GetLoaderForFormat(ModelFormat format)
    {
        LUMINA_ASSERT(format != ModelFormat::AUTO_DETECT, "ModelRegistry: Cannot get AUTO_DETECT loader");
        LUMINA_ASSERT(format != ModelFormat::UNKNOWN, "ModelRegistry: Cannot get UNKNOWN loader");

        auto it = s_Data.FormatLoaders.find(format);
        return (it != s_Data.FormatLoaders.end()) ? it->second.get() : nullptr;
    }

    ModelLoader* ModelRegistry::GetLoaderForExtension(const std::string& extension)
    {
        LUMINA_ASSERT(!extension.empty(), "ModelRegistry: Extension cannot be empty");

        std::string ext = NormalizeExtension(extension);
        auto it = s_Data.ExtensionLoaders.find(ext);
        return (it != s_Data.ExtensionLoaders.end()) ? it->second.get() : nullptr;
    }

    std::string ModelRegistry::NormalizeExtension(const std::string& extension)
    {
        LUMINA_ASSERT(!extension.empty(), "ModelRegistry: Extension cannot be empty");

        std::string ext = extension;
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (!ext.empty() && ext[0] != '.')
            ext = "." + ext;
        return ext;
    }
}