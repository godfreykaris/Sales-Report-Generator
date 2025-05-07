#pragma once

#include <imgui.h>
#include <mongocxx/database.hpp>
#include <string>
#include <vector>
#include <filesystem>
#include <cctype>
#include <windows.h>
#include <mongocxx/v_noabi/mongocxx/exception/exception.hpp>

#include "retrieve_data.h"
#include "../background/background_renderer.h"

extern bool login_window;

namespace LoginUi
{
    class LoginWindow
    {
    public:
        LoginWindow(mongocxx::database db, ImFont* default_font, ImFont* large_font, ImFont* emoji_font, BackgroundRenderer& bgRenderer);
        LoginWindow() = default;
        ~LoginWindow() = default;

    public:
        std::vector<UserData> passwords;
        mongocxx::collection login;
        mongocxx::collection background_images_coll;
        std::string s_password = "";
        static char c_password[128];
        bool login_error = false;
        bool show_window = false;
        ImGuiWindowFlags window_flags;
        bool showBackgroundSettings = false;
        std::string active_popup;

        // New properties
        float blurRadius = 0.0f; // Blur radius for slider
        bool useSolidColor = false; // Toggle for solid color mode
        float solidColor[3] = { 0.2f, 0.3f, 0.4f }; // Default solid color (RGB)

    private:
        ImFont* default_font;
        ImFont* large_font;
        ImFont* emoji_font;
        BackgroundRenderer& backgroundRenderer;
        char newImagePath[256] = "";

    public:
        int create_login_window();
        int login_error_message(bool& error_flag);
        int create_background_settings_window();
        int render_login_extras(int display_w, int display_h);
        void apply_window_styles();
        void cleanup_window_styles();
        void setup_window();
        void render_header();
        void render_popups();
        void render_background_effects();
        void render_add_image();
        void render_select_image();
        void render_slideshow_settings();
        void render_close_button();
        void render_popup(const char* title, const char* emoji, const char* message, bool& popup_open);

    private:
        void initialize_background_images();
    };
}