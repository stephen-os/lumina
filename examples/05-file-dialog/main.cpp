// 05-file-dialog: File picker dialogs
// Demonstrates: Open file, save file, folder picker, multi-select

#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/ui/ui.h>

#include <string>
#include <vector>
#include <map>

namespace ui = lumina::ui;

class file_dialog_layer : public lumina::core::layer
{
public:
    file_dialog_layer() : layer("file_dialog") {}

    void on_render() override
    {
        ui::begin_window("05-file-dialog");
        ui::text("File Dialog Demo");
        ui::separator();

        // Open single file
        if (ui::begin_section("Open Single File"))
        {
            if (ui::button("Open File..."))
            {
                ui::open_file_dialog(
                    "OpenFileDlg",
                    "Open File",
                    ".cpp,.h,.txt,.md",
                    ".",
                    "",
                    1,
                    ui::file_dialog_flag::modal
                );
            }

            if (!m_selected_file.empty())
            {
                ui::text("Selected:");
                ui::text_colored(ImVec4(0.4f, 0.8f, 0.4f, 1.0f), m_selected_file.c_str());
            }
            ui::end_section();
        }

        // Open multiple files
        if (ui::begin_section("Open Multiple Files"))
        {
            if (ui::button("Open Files..."))
            {
                ui::open_file_dialog(
                    "OpenMultiFileDlg",
                    "Open Files",
                    ".cpp,.h,.txt,.md,.*",
                    ".",
                    "",
                    0,  // 0 = unlimited selection
                    ui::file_dialog_flag::modal
                );
            }

            if (!m_selected_files.empty())
            {
                ui::text_fmt("Selected {} files:", m_selected_files.size());
                for (const auto& [name, path] : m_selected_files)
                {
                    ui::text_colored(ImVec4(0.4f, 0.8f, 0.4f, 1.0f), name.c_str());
                }
            }
            ui::end_section();
        }

        // Save file
        if (ui::begin_section("Save File"))
        {
            if (ui::button("Save As..."))
            {
                ui::save_file_dialog(
                    "SaveFileDlg",
                    "Save File",
                    ".txt,.md,.json",
                    ".",
                    "untitled.txt"
                );
            }

            if (!m_save_path.empty())
            {
                ui::text("Save path:");
                ui::text_colored(ImVec4(0.4f, 0.6f, 1.0f, 1.0f), m_save_path.c_str());
            }
            ui::end_section();
        }

        // Folder picker
        if (ui::begin_section("Choose Folder"))
        {
            if (ui::button("Choose Folder..."))
            {
                ui::open_folder_dialog(
                    "FolderDlg",
                    "Select Folder",
                    "."
                );
            }

            if (!m_selected_folder.empty())
            {
                ui::text("Selected folder:");
                ui::text_colored(ImVec4(1.0f, 0.8f, 0.4f, 1.0f), m_selected_folder.c_str());
            }
            ui::end_section();
        }

        // Filter examples
        if (ui::begin_section("Filter Examples"))
        {
            if (ui::button("Images Only"))
            {
                ui::open_file_dialog("ImagesDlg", "Open Image", ".png,.jpg,.jpeg,.gif,.bmp", ".");
            }
            ui::same_line();

            if (ui::button("Source Files"))
            {
                ui::open_file_dialog("SourceDlg", "Open Source", ".cpp,.c,.h,.hpp", ".");
            }
            ui::same_line();

            if (ui::button("All Files"))
            {
                ui::open_file_dialog("AllDlg", "Open Any File", ".*", ".");
            }
            ui::end_section();
        }

        ui::separator();
        ui::text("Click buttons above to open file dialogs");

        ui::end_window();

        // Handle all dialogs
        handle_dialogs();
    }

private:
    void handle_dialogs()
    {
        // Open single file dialog
        if (ui::display_file_dialog("OpenFileDlg"))
        {
            if (ui::file_dialog_ok())
            {
                m_selected_file = ui::get_file_dialog_path();
                LUMINA_LOG_INFO("Selected file: {}", m_selected_file);
            }
            ui::close_file_dialog();
        }

        // Open multiple files dialog
        if (ui::display_file_dialog("OpenMultiFileDlg"))
        {
            if (ui::file_dialog_ok())
            {
                m_selected_files = ui::get_file_dialog_selection();
                LUMINA_LOG_INFO("Selected {} files", m_selected_files.size());
            }
            ui::close_file_dialog();
        }

        // Save file dialog
        if (ui::display_file_dialog("SaveFileDlg"))
        {
            if (ui::file_dialog_ok())
            {
                m_save_path = ui::get_file_dialog_path();
                LUMINA_LOG_INFO("Save to: {}", m_save_path);
            }
            ui::close_file_dialog();
        }

        // Folder dialog
        if (ui::display_file_dialog("FolderDlg"))
        {
            if (ui::file_dialog_ok())
            {
                m_selected_folder = ui::get_file_dialog_path();
                LUMINA_LOG_INFO("Selected folder: {}", m_selected_folder);
            }
            ui::close_file_dialog();
        }

        // Filter example dialogs (just close them on any result)
        const char* filter_dialogs[] = {"ImagesDlg", "SourceDlg", "AllDlg"};
        for (const char* key : filter_dialogs)
        {
            if (ui::display_file_dialog(key))
            {
                if (ui::file_dialog_ok())
                {
                    m_selected_file = ui::get_file_dialog_path();
                    LUMINA_LOG_INFO("Selected: {}", m_selected_file);
                }
                ui::close_file_dialog();
            }
        }
    }

    std::string m_selected_file;
    std::map<std::string, std::string> m_selected_files;
    std::string m_save_path;
    std::string m_selected_folder;
};

lumina::core::application* lumina::core::create_application(int argc, char** argv)
{
    lumina::core::application_spec spec;
    spec.name = "05-file-dialog";
    spec.width = 1024;
    spec.height = 768;

    auto* app = new lumina::core::application(spec);
    app->push_layer<file_dialog_layer>();
    return app;
}
