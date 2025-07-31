#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include <stdexcept>

#include "../Core/Assert.h"

namespace Lumina
{
	ImageData Image::Load(const std::string& path)
	{
		int width, height, channels;
		stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &channels, 0);

		LUMINA_ASSERT(pixels, "Failed to load image: {}", path);
		
		size_t dataSize = static_cast<size_t>(width * height * channels);
		std::vector<unsigned char> data(pixels, pixels + dataSize);

		stbi_image_free(pixels);

		return ImageData{ width, height, channels, std::move(data) };
	}
}
