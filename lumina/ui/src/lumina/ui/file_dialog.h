#pragma once

#include <ImGuiFileDialog.h>

#include <string>
#include <map>
#include <functional>

namespace lumina::ui
{
    // File dialog flags - re-export for convenience
    using file_dialog_flags = ImGuiFileDialogFlags;

    namespace file_dialog_flag
    {
        constexpr file_dialog_flags none = ImGuiFileDialogFlags_None;
        constexpr file_dialog_flags confirm_overwrite = ImGuiFileDialogFlags_ConfirmOverwrite;
        constexpr file_dialog_flags hide_hidden_files = ImGuiFileDialogFlags_DontShowHiddenFiles;
        constexpr file_dialog_flags disable_create_dir = ImGuiFileDialogFlags_DisableCreateDirectoryButton;
        constexpr file_dialog_flags hide_type_column = ImGuiFileDialogFlags_HideColumnType;
        constexpr file_dialog_flags hide_size_column = ImGuiFileDialogFlags_HideColumnSize;
        constexpr file_dialog_flags hide_date_column = ImGuiFileDialogFlags_HideColumnDate;
        constexpr file_dialog_flags read_only_filename = ImGuiFileDialogFlags_ReadOnlyFileNameField;
        constexpr file_dialog_flags modal = ImGuiFileDialogFlags_Modal;
        constexpr file_dialog_flags default_flags = ImGuiFileDialogFlags_Default;
    }

    // Open a file dialog for opening files
    inline void open_file_dialog(
        const char* key,
        const char* title,
        const char* filters,
        const char* path = ".",
        const char* default_filename = "",
        int max_selection = 1,
        file_dialog_flags flags = file_dialog_flag::default_flags)
    {
        IGFD::FileDialogConfig config;
        config.path = path;
        config.fileName = default_filename;
        config.countSelectionMax = max_selection;
        config.flags = flags;
        ImGuiFileDialog::Instance()->OpenDialog(key, title, filters, config);
    }

    // Open a file dialog for saving files
    inline void save_file_dialog(
        const char* key,
        const char* title,
        const char* filters,
        const char* path = ".",
        const char* default_filename = "",
        file_dialog_flags flags = file_dialog_flag::default_flags | file_dialog_flag::confirm_overwrite)
    {
        IGFD::FileDialogConfig config;
        config.path = path;
        config.fileName = default_filename;
        config.countSelectionMax = 1;
        config.flags = flags;
        ImGuiFileDialog::Instance()->OpenDialog(key, title, filters, config);
    }

    // Open a folder picker dialog
    inline void open_folder_dialog(
        const char* key,
        const char* title,
        const char* path = ".",
        file_dialog_flags flags = file_dialog_flag::default_flags)
    {
        IGFD::FileDialogConfig config;
        config.path = path;
        config.countSelectionMax = 1;
        config.flags = flags;
        ImGuiFileDialog::Instance()->OpenDialog(key, title, nullptr, config);
    }

    // Display the file dialog - returns true if dialog was closed (ok or cancel)
    inline bool display_file_dialog(const char* key, const ImVec2& min_size = ImVec2(400, 300),
                                    const ImVec2& max_size = ImVec2(FLT_MAX, FLT_MAX))
    {
        return ImGuiFileDialog::Instance()->Display(key, ImGuiWindowFlags_None, min_size, max_size);
    }

    // Check if dialog was closed with OK
    inline bool file_dialog_ok()
    {
        return ImGuiFileDialog::Instance()->IsOk();
    }

    // Get selected file path (for single selection / save dialogs)
    inline std::string get_file_dialog_path()
    {
        return ImGuiFileDialog::Instance()->GetFilePathName();
    }

    // Get selected directory path
    inline std::string get_file_dialog_directory()
    {
        return ImGuiFileDialog::Instance()->GetCurrentPath();
    }

    // Get filename only (no path)
    inline std::string get_file_dialog_filename()
    {
        return ImGuiFileDialog::Instance()->GetCurrentFileName();
    }

    // Get multiple selections as map<filename, filepath>
    inline std::map<std::string, std::string> get_file_dialog_selection()
    {
        return ImGuiFileDialog::Instance()->GetSelection();
    }

    // Close the dialog
    inline void close_file_dialog()
    {
        ImGuiFileDialog::Instance()->Close();
    }

    // Check if a dialog with key is open
    inline bool is_file_dialog_open(const char* key)
    {
        return ImGuiFileDialog::Instance()->IsOpened(key);
    }

    // Helper: Run file dialog and call callback on OK
    // Usage:
    //   if (ui::button("Open File")) ui::open_file_dialog("open", "Open", ".cpp,.h");
    //   ui::file_dialog("open", [](const std::string& path) { load_file(path); });
    inline void file_dialog(const char* key, std::function<void(const std::string&)> on_ok,
                           const ImVec2& min_size = ImVec2(400, 300))
    {
        if (display_file_dialog(key, min_size))
        {
            if (file_dialog_ok())
            {
                on_ok(get_file_dialog_path());
            }
            close_file_dialog();
        }
    }

    // Helper: Run file dialog for multiple selection
    inline void file_dialog_multi(const char* key,
                                  std::function<void(const std::map<std::string, std::string>&)> on_ok,
                                  const ImVec2& min_size = ImVec2(400, 300))
    {
        if (display_file_dialog(key, min_size))
        {
            if (file_dialog_ok())
            {
                on_ok(get_file_dialog_selection());
            }
            close_file_dialog();
        }
    }
}
