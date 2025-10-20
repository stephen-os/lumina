#pragma once

#include <filesystem>
#include <string>

namespace Lumina
{
	namespace Path
	{
        std::filesystem::path AppData();
        std::filesystem::path Documents();
        std::filesystem::path Desktop();
        std::filesystem::path Downloads();
        std::filesystem::path Home();
        std::filesystem::path Temp();
	}
}