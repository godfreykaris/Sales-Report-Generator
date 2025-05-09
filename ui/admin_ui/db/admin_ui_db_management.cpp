#include "admin_ui_db_management.h"
#include <windows.h>
#include <shlobj.h>
#include <commdlg.h>
#include <filesystem>
#include <sstream>
#include <iostream>
#include <fstream>

namespace AdminUi
{
    DBManagementWindow::DBManagementWindow(mongocxx::database db)
        : db(db)
    {
        window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
    }

    bool DBManagementWindow::select_path(wchar_t* path, const wchar_t* title, bool is_file_selection)
    {
        std::filesystem::path original_cwd = std::filesystem::current_path();

        bool result = false;
        if (is_file_selection)
        {
            OPENFILENAMEW ofn = { 0 };
            path[0] = L'\0';
            ofn.lStructSize = sizeof(OPENFILENAMEW);
            ofn.hwndOwner = GetActiveWindow();
            ofn.lpstrFile = path;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrTitle = title;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
            ofn.lpstrFilter = L"MongoDB Archive Files (*.archive)\0*.archive\0All Files (*.*)\0*.*\0";

            result = GetOpenFileNameW(&ofn);
        }
        else
        {
            BROWSEINFO bi = { 0 };
            bi.lpszTitle = title;
            bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
            bi.hwndOwner = GetActiveWindow();

            LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
            if (pidl != nullptr)
            {
                if (SHGetPathFromIDList(pidl, path))
                {
                    result = true;
                }
                IMalloc* imalloc = nullptr;
                if (SUCCEEDED(SHGetMalloc(&imalloc)))
                {
                    imalloc->Free(pidl);
                    imalloc->Release();
                }
            }
        }

        std::filesystem::current_path(original_cwd);
        return result;
    }

    bool DBManagementWindow::export_database(const std::wstring& folder_path)
    {
        try
        {
            // Get the executable's directory
            wchar_t exe_path[MAX_PATH] = { 0 };
            GetModuleFileNameW(nullptr, exe_path, MAX_PATH);
            std::filesystem::path exe_dir = std::filesystem::path(exe_path).parent_path();
            std::filesystem::path mongo_tools_path = exe_dir / "assets" / "bin" / "mongodump.exe";
            std::string mongo_tools_path_str = mongo_tools_path.string();
            std::string db_name = std::string(db.name());

            // Convert wstring to string using WideCharToMultiByte
            std::string folder_path_mb;
            int size_needed = WideCharToMultiByte(CP_UTF8, 0, folder_path.c_str(), -1, nullptr, 0, nullptr, nullptr);
            if (size_needed > 0)
            {
                std::vector<char> mb_buffer(size_needed);
                WideCharToMultiByte(CP_UTF8, 0, folder_path.c_str(), -1, mb_buffer.data(), size_needed, nullptr, nullptr);
                folder_path_mb = mb_buffer.data();
            }
            else
            {
                error_message = "Failed to convert folder path to multibyte string.";
                show_error_popup = true;
                return false;
            }

            if (!std::filesystem::exists(folder_path_mb) || !std::filesystem::is_directory(folder_path_mb))
            {
                error_message = "Selected folder does not exist or is not a directory.";
                show_error_popup = true;
                return false;
            }

            if (!std::filesystem::exists(mongo_tools_path))
            {
                error_message = "mongodump.exe not found in assets\\bin folder.";
                show_error_popup = true;
                return false;
            }

            std::string folder_path_mb_escaped = folder_path_mb;
            std::replace(folder_path_mb_escaped.begin(), folder_path_mb_escaped.end(), '\\', '/');

            std::string command = "\"" + mongo_tools_path_str + "\"" + " --db " + db_name + " --archive=" + folder_path_mb_escaped + '/' + db_name + ".archive";

            std::ofstream log_file("db_management_log.txt", std::ios::app);
            log_file << "Export Command: " << command << "\n";
            log_file.close();

            int result = system(command.c_str());
            if (result != 0)
            {
                error_message = "Failed to export database. Check log file (db_management_log.txt) for details.";
                show_error_popup = true;
                return false;
            }
            return true;
        }
        catch (const std::exception& e)
        {
            error_message = "Export error: " + std::string(e.what());
            show_error_popup = true;
            return false;
        }
    }

    bool DBManagementWindow::import_database(const std::wstring& file_path, const std::string& new_db_name)
{
    try
    {
        // Validate the new database name
        if (new_db_name.empty())
        {
            error_message = "Database name cannot be empty.";
            show_error_popup = true;
            return false;
        }

        // Get the executable's directory
        wchar_t exe_path[MAX_PATH] = { 0 };
        GetModuleFileNameW(nullptr, exe_path, MAX_PATH);
        std::filesystem::path exe_dir = std::filesystem::path(exe_path).parent_path();
        std::filesystem::path mongo_tools_path = exe_dir / "assets" / "bin" / "mongorestore.exe";
        std::string mongo_tools_path_str = mongo_tools_path.string();

        // Convert wstring to string using WideCharToMultiByte
        std::string file_path_mb;
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, file_path.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (size_needed > 0)
        {
            std::vector<char> mb_buffer(size_needed);
            WideCharToMultiByte(CP_UTF8, 0, file_path.c_str(), -1, mb_buffer.data(), size_needed, nullptr, nullptr);
            file_path_mb = mb_buffer.data();
        }
        else
        {
            error_message = "Failed to convert file path to multibyte string.";
            show_error_popup = true;
            return false;
        }

        std::filesystem::path archive_path = file_path_mb;
        if (!std::filesystem::exists(archive_path) || std::filesystem::is_directory(archive_path))
        {
            error_message = "Selected archive file does not exist or is a directory.";
            show_error_popup = true;
            return false;
        }

        if (!std::filesystem::exists(mongo_tools_path))
        {
            error_message = "mongorestore.exe not found in assets\\bin folder.";
            show_error_popup = true;
            return false;
        }

        std::string file_path_mb_escaped = file_path_mb;
        std::replace(file_path_mb_escaped.begin(), file_path_mb_escaped.end(), '\\', '/');

        std::string command = "\"" + mongo_tools_path_str + "\"" + " --db " + new_db_name + " --archive=\"" + file_path_mb + "\"";

        std::ofstream log_file("db_management_log.txt", std::ios::app);
        log_file << "Import Command: " << command << "\n";
        log_file.close();

        int result = system(command.c_str());
        if (result != 0)
        {
            error_message = "Failed to import database from archive. Check log file (db_management_log.txt).";
            show_error_popup = true;
            return false;
        }

        return true;
    }
    catch (const std::exception& e)
    {
        error_message = "Import error: " + std::string(e.what());
        show_error_popup = true;
        return false;
    }
}

    int DBManagementWindow::create_db_management_window()
    {
        if (!show_window)
            return 0;

        ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(40, 40, 40, 255));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(60, 60, 60, 255));
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 150, 80, 255));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 180, 100, 255));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0, 120, 60, 255));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f));

        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(400.0f, 350.0f), ImGuiCond_Always); // Increased height for new input

        ImGui::Begin("Database Management", &show_window, window_flags);

        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Database Management").x) * 0.5f);
        ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.4f, 1.0f), "Database Management");
        ImGui::PopFont();
        ImGui::Separator();

        // Export Database
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 200.0f) * 0.5f);
        if (ImGui::Button("Export Database", ImVec2(200.0f, 40.0f)))
        {
            if (select_path(export_path, L"Select Folder to Export Database", false))
            {
                if (export_database(export_path))
                {
                    error_message = "Database exported successfully!";
                    show_error_popup = true;
                }
            }
        }

        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 350.0f) * 0.5f);
        char export_path_mb[MAX_PATH] = { 0 };
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, export_path, -1, nullptr, 0, nullptr, nullptr);
        if (size_needed > 0)
        {
            WideCharToMultiByte(CP_UTF8, 0, export_path, -1, export_path_mb, MAX_PATH, nullptr, nullptr);
        }
        ImGui::InputText("Export Path", export_path_mb, MAX_PATH, ImGuiInputTextFlags_ReadOnly);

        // // Import Database
        // ImGui::Dummy(ImVec2(0.0f, 20.0f));
        // ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 350.0f) * 0.5f);
        // ImGui::InputText("New Database Name", import_db_name, sizeof(import_db_name));
        // ImGui::Dummy(ImVec2(0.0f, 5.0f));
        // ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 200.0f) * 0.5f);
        // if (ImGui::Button("Import Database", ImVec2(200.0f, 40.0f)))
        // {
        //     if (select_path(import_path, L"Select Database Archive File", true))
        //     {
        //         if (import_database(import_path, import_db_name))
        //         {
        //             error_message = "Database imported successfully!";
        //             show_error_popup = true;
        //         }
        //     }
        // }

        // ImGui::Dummy(ImVec2(0.0f, 5.0f));
        // ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 350.0f) * 0.5f);
        // char import_path_mb[MAX_PATH] = { 0 };
        // size_needed = WideCharToMultiByte(CP_UTF8, 0, import_path, -1, nullptr, 0, nullptr, nullptr);
        // if (size_needed > 0)
        // {
        //     WideCharToMultiByte(CP_UTF8, 0, import_path, -1, import_path_mb, MAX_PATH, nullptr, nullptr);
        // }
        // ImGui::InputText("Import Path", import_path_mb, MAX_PATH, ImGuiInputTextFlags_ReadOnly);

        // Error/Success Popup
        if (show_error_popup)
        {
            ImGui::OpenPopup("Message");
            if (ImGui::BeginPopupModal("Message", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("%s", error_message.c_str());
                ImGui::Separator();
                ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 100.0f) * 0.5f);
                if (ImGui::Button("OK", ImVec2(100.0f, 30.0f)))
                {
                    show_error_popup = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }

        ImGui::End();

        ImGui::PopStyleColor(6);
        ImGui::PopStyleVar(3);

        return 1;
    }
}