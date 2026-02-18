#include "file.h"
#include "log.h"

#include <fstream>
#include <sstream>
#include <filesystem>

namespace lumina::core
{
    std::expected<std::string, std::string> file::read_text(const std::string& path)
    {
        std::ifstream stream(path);
        if (!stream.is_open())
        {
            std::string error = "Failed to open file for reading: " + path;
            LUMINA_LOG_ERROR("{}", error);
            return std::unexpected(error);
        }

        std::ostringstream ss;
        ss << stream.rdbuf();
        return ss.str();
    }

    std::expected<std::vector<uint8_t>, std::string> file::read_binary(const std::string& path)
    {
        std::ifstream stream(path, std::ios::binary | std::ios::ate);
        if (!stream.is_open())
        {
            std::string error = "Failed to open file for binary reading: " + path;
            LUMINA_LOG_ERROR("{}", error);
            return std::unexpected(error);
        }

        auto size = stream.tellg();
        stream.seekg(0, std::ios::beg);

        std::vector<uint8_t> buffer(static_cast<size_t>(size));
        stream.read(reinterpret_cast<char*>(buffer.data()), size);
        return buffer;
    }

    bool file::write_text(const std::string& path, const std::string& text)
    {
        std::ofstream stream(path);
        if (!stream.is_open())
        {
            LUMINA_LOG_ERROR("Failed to open file for writing: {}", path);
            return false;
        }
        stream << text;
        return true;
    }

    bool file::write_binary(const std::string& path, const void* data, size_t size)
    {
        std::ofstream stream(path, std::ios::binary);
        if (!stream.is_open())
        {
            LUMINA_LOG_ERROR("Failed to open file for binary writing: {}", path);
            return false;
        }
        stream.write(static_cast<const char*>(data), static_cast<std::streamsize>(size));
        return true;
    }

    bool file::write_binary(const std::string& path, const std::vector<uint8_t>& data)
    {
        return write_binary(path, data.data(), data.size());
    }

    bool file::exists(const std::string& path)
    {
        return std::filesystem::exists(path);
    }
}
