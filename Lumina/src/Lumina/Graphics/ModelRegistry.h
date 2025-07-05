#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

#include "../Core/Ref.h"

#include "Model.h"

#include "Formats/ModelFormat.h"

#include "Loaders/ModelLoader.h"

namespace Lumina
{
    class ModelRegistry
    {
    public:
        static void Init();
        static void Shutdown();
        static bool IsInitialized();

        static Ref<Model> LoadModel(const std::string& path, ModelFormat format = ModelFormat::AUTO_DETECT);

        static void RegisterLoader(ModelFormat format, std::unique_ptr<ModelLoader> loader);
        static void RegisterLoader(const std::string& extension, std::unique_ptr<ModelLoader> loader);

        static std::vector<std::string> GetSupportedExtensions();
        static std::vector<ModelFormat> GetSupportedFormats();
        static ModelFormat DetectFormat(const std::string& path);

        static void LogSupportedFormats();

        static std::string ModelFormatToString(ModelFormat format); 

    private:
        ModelRegistry() = delete;
        ~ModelRegistry() = delete;
        ModelRegistry(const ModelRegistry&) = delete;
        ModelRegistry& operator=(const ModelRegistry&) = delete;

        struct ModelRegistryData
        {
            std::unordered_map<ModelFormat, std::unique_ptr<ModelLoader>> FormatLoaders;
            std::unordered_map<std::string, std::unique_ptr<ModelLoader>> ExtensionLoaders;
            bool Initialized = false;
        };

        static ModelRegistryData s_Data;

        static ModelLoader* GetLoaderForFormat(ModelFormat format);
        static ModelLoader* GetLoaderForExtension(const std::string& extension);
        static std::string NormalizeExtension(const std::string& extension);

        // Format detection map
        static const std::unordered_map<std::string, ModelFormat> s_ExtensionMap;
    };
}