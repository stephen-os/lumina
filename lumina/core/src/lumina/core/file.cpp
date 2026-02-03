#include "file.h"
#include "assert.h"

#include <fstream>
#include <sstream>
#include <filesystem>

namespace lumina::core
{
    std::string file::read_text(const std::string& path)
    {
        std::ifstream stream(path);
        LUMINA_ASSERT(stream.is_open(), "Failed to open file for reading: {}", path);

        std::ostringstream ss;
        ss << stream.rdbuf();
        return ss.str();
    }

    std::vector<uint8_t> file::read_binary(const std::string& path)
    {
        std::ifstream stream(path, std::ios::binary | std::ios::ate);
        LUMINA_ASSERT(stream.is_open(), "Failed to open file for binary reading: {}", path);

        auto size = stream.tellg();
        stream.seekg(0, std::ios::beg);

        std::vector<uint8_t> buffer(static_cast<size_t>(size));
        stream.read(reinterpret_cast<char*>(buffer.data()), size);
        return buffer;
    }

    void file::write_text(const std::string& path, const std::string& text)
    {
        std::ofstream stream(path);
        LUMINA_ASSERT(stream.is_open(), "Failed to open file for writing: {}", path);
        stream << text;
    }

    void file::write_binary(const std::string& path, const void* data, size_t size)
    {
        std::ofstream stream(path, std::ios::binary);
        LUMINA_ASSERT(stream.is_open(), "Failed to open file for binary writing: {}", path);
        stream.write(static_cast<const char*>(data), static_cast<std::streamsize>(size));
    }

    void file::write_binary(const std::string& path, const std::vector<uint8_t>& data)
    {
        write_binary(path, data.data(), data.size());
    }

    bool file::exists(const std::string& path)
    {
        return std::filesystem::exists(path);
    }
}
