#include "File.h"
#include "../Core/Assert.h"

#include <fstream>
#include <sstream>
#include <filesystem>

namespace Lumina
{
    std::string File::ReadText(const std::string& path)
    {
        std::ifstream file(path);
        LUMINA_ASSERT(file.is_open(), "Failed to open file for reading: {}", path);

        std::ostringstream ss;
        ss << file.rdbuf();

        file.close();
        return ss.str();
    }

    std::string File::ReadBinary(const std::string& path)
    {
        std::ifstream file(path, std::ios::binary);
        LUMINA_ASSERT(file.is_open(), "Failed to open file for binary reading: {}", path);

        std::ostringstream ss;
        ss << file.rdbuf();

        file.close();
        return ss.str();
    }

    void File::WriteText(const std::string& path, const std::string& text)
    {
        std::ofstream file(path);
        LUMINA_ASSERT(file.is_open(), "Failed to open file for writing: {}", path);

        file << text;
        file.close();
    }

    void File::WriteBinary(const std::string& path, const std::string& text)
    {
        std::ofstream file(path, std::ios::binary);
        LUMINA_ASSERT(file.is_open(), "Failed to open file for binary writing: {}", path);

        file.write(text.data(), text.size());
        file.close();
    }

    bool File::Exists(const std::string& path)
    {
        return std::filesystem::exists(path);
    }
}
