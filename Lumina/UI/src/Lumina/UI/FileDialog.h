#pragma once

#include <ImGuiFileDialog.h>

#include <string>
#include <map>
#include <functional>

namespace Lumina::UI
{
    // File dialog flags - re-export for convenience
    using FileDialogFlags = ImGuiFileDialogFlags;

    namespace FileDialogFlag
    {
        constexpr FileDialogFlags None = ImGuiFileDialogFlags_None;
        constexpr FileDialogFlags ConfirmOverwrite = ImGuiFileDialogFlags_ConfirmOverwrite;
        constexpr FileDialogFlags HideHiddenFiles = ImGuiFileDialogFlags_DontShowHiddenFiles;
        constexpr FileDialogFlags DisableCreateDir = ImGuiFileDialogFlags_DisableCreateDirectoryButton;
        constexpr FileDialogFlags HideTypeColumn = ImGuiFileDialogFlags_HideColumnType;
        constexpr FileDialogFlags HideSizeColumn = ImGuiFileDialogFlags_HideColumnSize;
        constexpr FileDialogFlags HideDateColumn = ImGuiFileDialogFlags_HideColumnDate;
        constexpr FileDialogFlags ReadOnlyFilename = ImGuiFileDialogFlags_ReadOnlyFileNameField;
        constexpr FileDialogFlags Modal = ImGuiFileDialogFlags_Modal;
        constexpr FileDialogFlags DefaultFlags = ImGuiFileDialogFlags_Default;
    }

    // Open a file dialog for opening files
    inline void OpenFileDialog(
        const char* key,
        const char* title,
        const char* filters,
        const char* path = ".",
        const char* defaultFilename = "",
        int maxSelection = 1,
        FileDialogFlags flags = FileDialogFlag::DefaultFlags)
    {
        IGFD::FileDialogConfig config;
        config.path = path;
        config.fileName = defaultFilename;
        config.countSelectionMax = maxSelection;
        config.flags = flags;
        ImGuiFileDialog::Instance()->OpenDialog(key, title, filters, config);
    }

    // Open a file dialog for saving files
    inline void SaveFileDialog(
        const char* key,
        const char* title,
        const char* filters,
        const char* path = ".",
        const char* defaultFilename = "",
        FileDialogFlags flags = FileDialogFlag::DefaultFlags | FileDialogFlag::ConfirmOverwrite)
    {
        IGFD::FileDialogConfig config;
        config.path = path;
        config.fileName = defaultFilename;
        config.countSelectionMax = 1;
        config.flags = flags;
        ImGuiFileDialog::Instance()->OpenDialog(key, title, filters, config);
    }

    // Open a folder picker dialog
    inline void OpenFolderDialog(
        const char* key,
        const char* title,
        const char* path = ".",
        FileDialogFlags flags = FileDialogFlag::DefaultFlags)
    {
        IGFD::FileDialogConfig config;
        config.path = path;
        config.countSelectionMax = 1;
        config.flags = flags;
        ImGuiFileDialog::Instance()->OpenDialog(key, title, nullptr, config);
    }

    // Display the file dialog - returns true if dialog was closed (ok or cancel)
    inline bool DisplayFileDialog(const char* key, const ImVec2& minSize = ImVec2(400, 300),
                                    const ImVec2& maxSize = ImVec2(FLT_MAX, FLT_MAX))
    {
        return ImGuiFileDialog::Instance()->Display(key, ImGuiWindowFlags_None, minSize, maxSize);
    }

    // Check if dialog was closed with OK
    inline bool FileDialogOk()
    {
        return ImGuiFileDialog::Instance()->IsOk();
    }

    // Get selected file path (for single selection / save dialogs)
    inline std::string GetFileDialogPath()
    {
        return ImGuiFileDialog::Instance()->GetFilePathName();
    }

    // Get selected directory path
    inline std::string GetFileDialogDirectory()
    {
        return ImGuiFileDialog::Instance()->GetCurrentPath();
    }

    // Get filename only (no path)
    inline std::string GetFileDialogFilename()
    {
        return ImGuiFileDialog::Instance()->GetCurrentFileName();
    }

    // Get multiple selections as map<filename, filepath>
    inline std::map<std::string, std::string> GetFileDialogSelection()
    {
        return ImGuiFileDialog::Instance()->GetSelection();
    }

    // Close the dialog
    inline void CloseFileDialog()
    {
        ImGuiFileDialog::Instance()->Close();
    }

    // Check if a dialog with key is open
    inline bool IsFileDialogOpen(const char* key)
    {
        return ImGuiFileDialog::Instance()->IsOpened(key);
    }

    // Helper: Run file dialog and call callback on OK
    // Usage:
    //   if (UI::Button("Open File")) UI::OpenFileDialog("open", "Open", ".cpp,.h");
    //   UI::FileDialog("open", [](const std::string& path) { LoadFile(path); });
    inline void FileDialog(const char* key, std::function<void(const std::string&)> onOk,
                           const ImVec2& minSize = ImVec2(400, 300))
    {
        if (DisplayFileDialog(key, minSize))
        {
            if (FileDialogOk())
            {
                onOk(GetFileDialogPath());
            }
            CloseFileDialog();
        }
    }

    // Helper: Run file dialog for multiple selection
    inline void FileDialogMulti(const char* key,
                                  std::function<void(const std::map<std::string, std::string>&)> onOk,
                                  const ImVec2& minSize = ImVec2(400, 300))
    {
        if (DisplayFileDialog(key, minSize))
        {
            if (FileDialogOk())
            {
                onOk(GetFileDialogSelection());
            }
            CloseFileDialog();
        }
    }
}
