#pragma once

#include <string>
#include <vector>

namespace lumina::core
{
    class file
    {
    public:
        static std::string read_text(const std::string& path);
        static std::vector<uint8_t> read_binary(const std::string& path);

        static void write_text(const std::string& path, const std::string& text);
        static void write_binary(const std::string& path, const void* data, size_t size);
        static void write_binary(const std::string& path, const std::vector<uint8_t>& data);

        static bool exists(const std::string& path);
    };
}
