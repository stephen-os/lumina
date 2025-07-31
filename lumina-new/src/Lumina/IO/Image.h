#pragma once

#include "../Graphics/Texture.h"

#include <string>
#include <vector>

namespace Lumina
{
	struct ImageData
	{
		int width = 0;
		int height = 0;
		int channels = 0;

		std::vector<unsigned char> data;
	};

	class Image
	{
	public:
		static ImageData Load(const std::string& path);

		static bool SaveImageToPNG(const std::string& path, const ImageData& data);
		static bool SaveImageToJPEG(const std::string& path, const ImageData& data);

		static bool SaveTextureToPNG(const std::string& path, const std::shared_ptr<Texture>& texture);
		static bool SaveTextureToJPEG(const std::string& path, const std::shared_ptr<Texture>& texture);
	};
}