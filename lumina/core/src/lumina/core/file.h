#pragma once

#include <string>
#include <vector>
#include <expected>

namespace lumina::core
{
    class file
    {
    public:
        [[nodiscard]] static std::expected<std::string, std::string> read_text(const std::string& path);
        [[nodiscard]] static std::expected<std::vector<uint8_t>, std::string> read_binary(const std::string& path);

        [[nodiscard]] static bool write_text(const std::string& path, const std::string& text);
        [[nodiscard]] static bool write_binary(const std::string& path, const void* data, size_t size);
        [[nodiscard]] static bool write_binary(const std::string& path, const std::vector<uint8_t>& data);

        [[nodiscard]] static bool exists(const std::string& path);
    };
}
