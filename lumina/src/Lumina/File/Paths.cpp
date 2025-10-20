#include "Paths.h"
 
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

namespace Lumina
{
    namespace Path
    {
        std::filesystem::path AppData()
        {
#ifdef _WIN32
            const char* appData = std::getenv("APPDATA");
            if (appData)
                return std::filesystem::path(appData);
            return std::filesystem::path(std::getenv("USERPROFILE")) / "AppData" / "Roaming";
#elif __APPLE__
            return std::filesystem::path(std::getenv("HOME")) / "Library" / "Application Support";
#else
            const char* xdgConfig = std::getenv("XDG_CONFIG_HOME");
            if (xdgConfig)
                return std::filesystem::path(xdgConfig);
            return std::filesystem::path(std::getenv("HOME")) / ".config";
#endif
        }

        std::filesystem::path Documents()
        {
#ifdef _WIN32
            wchar_t* path = nullptr;
            if (SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &path) == S_OK)
            {
                std::filesystem::path result(path);
                CoTaskMemFree(path);
                return result;
            }
            return std::filesystem::path(std::getenv("USERPROFILE")) / "Documents";
#elif __APPLE__
            return std::filesystem::path(std::getenv("HOME")) / "Documents";
#else
            const char* xdgDocs = std::getenv("XDG_DOCUMENTS_DIR");
            if (xdgDocs)
                return std::filesystem::path(xdgDocs);
            return std::filesystem::path(std::getenv("HOME")) / "Documents";
#endif
        }

        std::filesystem::path Desktop()
        {
#ifdef _WIN32
            wchar_t* path = nullptr;
            if (SHGetKnownFolderPath(FOLDERID_Desktop, 0, nullptr, &path) == S_OK)
            {
                std::filesystem::path result(path);
                CoTaskMemFree(path);
                return result;
            }
            return std::filesystem::path(std::getenv("USERPROFILE")) / "Desktop";
#elif __APPLE__
            return std::filesystem::path(std::getenv("HOME")) / "Desktop";
#else
            const char* xdgDesktop = std::getenv("XDG_DESKTOP_DIR");
            if (xdgDesktop)
                return std::filesystem::path(xdgDesktop);
            return std::filesystem::path(std::getenv("HOME")) / "Desktop";
#endif
        }

        std::filesystem::path Downloads()
        {
#ifdef _WIN32
            wchar_t* path = nullptr;
            if (SHGetKnownFolderPath(FOLDERID_Downloads, 0, nullptr, &path) == S_OK)
            {
                std::filesystem::path result(path);
                CoTaskMemFree(path);
                return result;
            }
            return std::filesystem::path(std::getenv("USERPROFILE")) / "Downloads";
#elif __APPLE__
            return std::filesystem::path(std::getenv("HOME")) / "Downloads";
#else
            const char* xdgDownload = std::getenv("XDG_DOWNLOAD_DIR");
            if (xdgDownload)
                return std::filesystem::path(xdgDownload);
            return std::filesystem::path(std::getenv("HOME")) / "Downloads";
#endif
        }

        std::filesystem::path Home()
        {
#ifdef _WIN32
            return std::filesystem::path(std::getenv("USERPROFILE"));
#else
            return std::filesystem::path(std::getenv("HOME"));
#endif
        }

        std::filesystem::path Temp()
        {
#ifdef _WIN32
            return std::filesystem::path(std::getenv("TEMP"));
#else
            const char* tmpDir = std::getenv("TMPDIR");
            if (tmpDir)
                return std::filesystem::path(tmpDir);
            return std::filesystem::path("/tmp");
#endif
        }
    }
}