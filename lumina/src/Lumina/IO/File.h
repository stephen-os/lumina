#pragma once

#include <string>

namespace Lumina
{
	class File
	{
	public:
		static std::string ReadText(const std::string& path);
		static std::string ReadBinary(const std::string& path);

		static void WriteText(const std::string& path, const std::string& text);
		static void WriteBinary(const std::string& path, const std::string& text);

		static bool Exists(const std::string& path);
	};
}