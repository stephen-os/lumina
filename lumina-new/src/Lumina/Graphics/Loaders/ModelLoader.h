#pragma once

#include "../../Core/Ref.h"

#include "../Model.h"

namespace Lumina
{
	class ModelLoader
	{
	public:
		virtual ~ModelLoader() = default;
		virtual Ref<Model> LoadModel(const std::string& path) = 0;
		virtual bool CanLoadFormat(const std::string& extension) const = 0;
		virtual std::vector<std::string> GetSupportedExtensions() const = 0;
	};
	
}