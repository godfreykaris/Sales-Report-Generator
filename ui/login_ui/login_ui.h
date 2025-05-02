#pragma once

#include <imgui.h>
#include <mongocxx/database.hpp>
#include <string>
#include <vector>
#include "retrieve_data.h"

extern bool login_window;

namespace LoginUi
{
    class LoginWindow
    {
        /* Constructor and Destructor */
    public:
        LoginWindow(mongocxx::database db, ImFont* default_font, ImFont* large_font);
        LoginWindow() = default;
        ~LoginWindow() = default;

        /* Properties */
    public:
        // Login details in the order: 0 admin, 1 finance, 2 marketer, 3 regular
        std::vector<UserData> passwords;

        mongocxx::collection login;
        std::string s_password = "";
        static char c_password[128];

        bool login_error = false;

        bool show_window = false;
        ImGuiWindowFlags window_flags;

    private:
        ImFont* default_font; // 16.0f font
        ImFont* large_font;   // 24.0f font

        /* Methods */
    public:
        int create_login_window();
        int login_error_message(bool& error_flag);
    };
}