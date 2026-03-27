#pragma once

#include <string>
#include <vector>
#include <expected>

namespace Lumina
{
	class File
	{
	public:
		[[nodiscard]] static std::expected<std::string, std::string> ReadText(const std::string& path);
		[[nodiscard]] static std::expected<std::vector<uint8_t>, std::string> ReadBinary(const std::string& path);

		[[nodiscard]] static bool WriteText(const std::string& path, const std::string& text);
		[[nodiscard]] static bool WriteBinary(const std::string& path, const void* data, size_t size);
		[[nodiscard]] static bool WriteBinary(const std::string& path, const std::vector<uint8_t>& data);

		[[nodiscard]] static bool Exists(const std::string& path);
	};
}
