#pragma once

#include "../Core/Ref.h"

#include "Model.h"
#include "ModelFormat.h"

#include "Loaders/ModelLoader.h"

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace Lumina
{
    class ModelRegistry
    {
    public:
        static void Init();
        static void Shutdown();
        static bool IsInitialized();

        // Main loading functions
        static Ref<Model> LoadModel(const std::string& path, ModelFormat format = ModelFormat::AUTO_DETECT);

        // Loader registration (must be called after Init())
        static void RegisterLoader(ModelFormat format, std::unique_ptr<ModelLoader> loader);
        static void RegisterLoader(const std::string& extension, std::unique_ptr<ModelLoader> loader);

        // Query capabilities
        static std::vector<std::string> GetSupportedExtensions();
        static std::vector<ModelFormat> GetSupportedFormats();
        static ModelFormat DetectFormat(const std::string& path);

        // Debug/info
        static void LogSupportedFormats();

    private:
        // No construction - static class only
        ModelRegistry() = delete;
        ~ModelRegistry() = delete;
        ModelRegistry(const ModelRegistry&) = delete;
        ModelRegistry& operator=(const ModelRegistry&) = delete;

        // Internal data structure
        struct ModelRegistryData
        {
            std::unordered_map<ModelFormat, std::unique_ptr<ModelLoader>> FormatLoaders;
            std::unordered_map<std::string, std::unique_ptr<ModelLoader>> ExtensionLoaders;
            bool Initialized = false;
        };

        static ModelRegistryData s_Data;

        // Internal helper methods
        static void InitializeDefaultLoaders();
        static ModelLoader* GetLoaderForFormat(ModelFormat format);
        static ModelLoader* GetLoaderForExtension(const std::string& extension);
        static std::string NormalizeExtension(const std::string& extension);

        // Format detection map
        static const std::unordered_map<std::string, ModelFormat> s_ExtensionMap;
    };
}