// ui/02-file-dialog: File picker dialogs
// Demonstrates: Open file, save file, folder picker, multi-select

#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/ui/ui.h>

#include <glm/glm.hpp>
#include <string>
#include <map>

namespace UI = Lumina::UI;

class FileDialogLayer : public Lumina::Layer
{
public:
    FileDialogLayer() : Layer("FileDialogLayer") {}

    void OnRender() override
    {
        UI::BeginWindow("File Dialog Demo");
        UI::Text("File Dialog Demo");
        UI::Separator();

        // Open single file
        if (UI::BeginSection("Open Single File"))
        {
            if (UI::Button("Open File..."))
                UI::OpenFileDialog("OpenFileDlg", "Open File", ".cpp,.h,.txt,.md", ".", "", 1, UI::FileDialogFlag::Modal);

            if (!m_SelectedFile.empty())
            {
                UI::Text("Selected:");
                UI::TextColored(glm::vec4(0.4f, 0.8f, 0.4f, 1.0f), m_SelectedFile.c_str());
            }
            UI::EndSection();
        }

        // Open multiple files
        if (UI::BeginSection("Open Multiple Files"))
        {
            if (UI::Button("Open Files..."))
                UI::OpenFileDialog("OpenMultiFileDlg", "Open Files", ".cpp,.h,.txt,.md,.*", ".", "", 0, UI::FileDialogFlag::Modal);

            if (!m_SelectedFiles.empty())
            {
                UI::TextFmt("Selected {} files:", m_SelectedFiles.size());
                for (const auto& [name, path] : m_SelectedFiles)
                    UI::TextColored(glm::vec4(0.4f, 0.8f, 0.4f, 1.0f), name.c_str());
            }
            UI::EndSection();
        }

        // Save file
        if (UI::BeginSection("Save File"))
        {
            if (UI::Button("Save As..."))
                UI::SaveFileDialog("SaveFileDlg", "Save File", ".txt,.md,.json", ".", "untitled.txt");

            if (!m_SavePath.empty())
            {
                UI::Text("Save path:");
                UI::TextColored(glm::vec4(0.4f, 0.6f, 1.0f, 1.0f), m_SavePath.c_str());
            }
            UI::EndSection();
        }

        // Folder picker
        if (UI::BeginSection("Choose Folder"))
        {
            if (UI::Button("Choose Folder..."))
                UI::OpenFolderDialog("FolderDlg", "Select Folder", ".");

            if (!m_SelectedFolder.empty())
            {
                UI::Text("Selected folder:");
                UI::TextColored(glm::vec4(1.0f, 0.8f, 0.4f, 1.0f), m_SelectedFolder.c_str());
            }
            UI::EndSection();
        }

        // Filter examples
        if (UI::BeginSection("Filter Examples"))
        {
            if (UI::Button("Images Only"))
                UI::OpenFileDialog("ImagesDlg", "Open Image", ".png,.jpg,.jpeg,.gif,.bmp", ".");
            UI::SameLine();
            if (UI::Button("Source Files"))
                UI::OpenFileDialog("SourceDlg", "Open Source", ".cpp,.c,.h,.hpp", ".");
            UI::SameLine();
            if (UI::Button("All Files"))
                UI::OpenFileDialog("AllDlg", "Open Any File", ".*", ".");
            UI::EndSection();
        }

        UI::EndWindow();
        HandleDialogs();
    }

private:
    void HandleDialogs()
    {
        if (UI::DisplayFileDialog("OpenFileDlg"))
        {
            if (UI::FileDialogOk())
                m_SelectedFile = UI::GetFileDialogPath();
            UI::CloseFileDialog();
        }

        if (UI::DisplayFileDialog("OpenMultiFileDlg"))
        {
            if (UI::FileDialogOk())
                m_SelectedFiles = UI::GetFileDialogSelection();
            UI::CloseFileDialog();
        }

        if (UI::DisplayFileDialog("SaveFileDlg"))
        {
            if (UI::FileDialogOk())
                m_SavePath = UI::GetFileDialogPath();
            UI::CloseFileDialog();
        }

        if (UI::DisplayFileDialog("FolderDlg"))
        {
            if (UI::FileDialogOk())
                m_SelectedFolder = UI::GetFileDialogPath();
            UI::CloseFileDialog();
        }

        const char* filterDialogs[] = {"ImagesDlg", "SourceDlg", "AllDlg"};
        for (const char* key : filterDialogs)
        {
            if (UI::DisplayFileDialog(key))
            {
                if (UI::FileDialogOk())
                    m_SelectedFile = UI::GetFileDialogPath();
                UI::CloseFileDialog();
            }
        }
    }

    std::string m_SelectedFile;
    std::map<std::string, std::string> m_SelectedFiles;
    std::string m_SavePath;
    std::string m_SelectedFolder;
};

Lumina::Application* Lumina::CreateApplication(int argc, char** argv)
{
    ApplicationSpecifications specs;
    specs.Title = "ui/02-file-dialog";
    auto* app = new Application(specs);
    app->PushLayer<FileDialogLayer>();
    return app;
}
