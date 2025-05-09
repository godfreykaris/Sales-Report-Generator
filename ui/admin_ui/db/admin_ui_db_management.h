#pragma once

#include <imgui.h>
#include <mongocxx/database.hpp>
#include <mongocxx/client.hpp>

namespace AdminUi
{
    class DBManagementWindow
    {
    public:
        DBManagementWindow(mongocxx::database db);
        ~DBManagementWindow() = default;

        bool show_window = false;
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

        int create_db_management_window();

    private:
        mongocxx::database db;
        wchar_t export_path[260] = L"";
        wchar_t import_path[260] = L"";
        char import_db_name[128] = ""; // Buffer for new database name input
        bool show_error_popup = false;
        std::string error_message;

        bool select_path(wchar_t* path, const wchar_t* title, bool is_file_selection);
        bool export_database(const std::wstring& folder_path);
        bool import_database(const std::wstring& file_path, const std::string& new_db_name);
    };
}