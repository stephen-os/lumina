#pragma once

#include <string>
#include <vector>
#include <expected>

namespace lumina::core
{
    class file
    {
    public:
        static std::expected<std::string, std::string> read_text(const std::string& path);
        static std::expected<std::vector<uint8_t>, std::string> read_binary(const std::string& path);

        static bool write_text(const std::string& path, const std::string& text);
        static bool write_binary(const std::string& path, const void* data, size_t size);
        static bool write_binary(const std::string& path, const std::vector<uint8_t>& data);

        static bool exists(const std::string& path);
    };
}
