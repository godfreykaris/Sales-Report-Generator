#pragma once

#include "app_common.h"
#include "retrieve_data.h"
#include "regular_ui/regular_user_ui.h"
#include "change_password.h"

#include <imgui.h>
#include <glfw/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>

#include <thread>
#include <Windows.h>
#include <hpdf.h>
#include <commdlg.h>
#include <shellapi.h>
#include <string>
#include <vector>
#include <tuple>
#include <memory>

extern bool marketer_window;

namespace MUserUi
{
    // Apply a professional ImGui theme
    void ApplyProfessionalTheme();

    class GeneralReportWindow
    {
    public:
        GeneralReportWindow(mongocxx::database db, GLFWwindow* window);
        GeneralReportWindow() = default;
        ~GeneralReportWindow() = default;

    public:
        mongocxx::collection sales_collection;
        std::vector<CategoryData> v_seasons;
        std::vector<CategoryData> v_age_groups;
        std::vector<CategoryData> v_genders;
        std::vector<CategoryData> v_locations;
        std::string last_saved_file;
        bool saving = false;
		bool show_print_success = false; 
		bool show_print_error = false;  
        bool show_window = false;
        ImGuiWindowFlags window_flags;
        GLFWwindow* window;
        HWND main_window;

    public:
        int create_general_report_window();
        int create_general_report_window(const std::vector<std::string>& products);
        int display_report(std::vector<std::tuple<std::string, std::string, int>>* report_data, const std::vector<std::string>& products);
        int handle_general_report_print(const std::vector<std::tuple<std::string, std::string, int>>& print_data);
        int LoadingIndicatorCircle(const char* label, const float indicator_radius, const ImVec4& main_color,
            const ImVec4& backdrop_color, const int circle_count, const float speed);
    };

    class CustomizedReportWindow : public GeneralReportWindow, public RUserUi::Window
    {
    public:
        CustomizedReportWindow(mongocxx::database db, GLFWwindow* window);
        CustomizedReportWindow() = default;
        ~CustomizedReportWindow() = default;

    public:
        static int current_selected_brand;
        int product_index = 0;
        std::vector<ProductData> v_products;
        std::vector<std::string> products_array;
        std::vector<std::string> brands_array;
        std::string products;
        std::string brands;
        std::unique_ptr<char[]> p_brands;
        std::string last_saved_file;
        bool saving = false;
        bool show_window = false;
        ImGuiWindowFlags window_flags;

    public:
        int create_customized_report_window();
        int populate_products();
        int populate_brands(std::vector<BrandData> v_brands);
        int display_products_sales(std::vector<std::tuple<std::string, std::string, std::string, int>>* report_data);
        int display_brands_sales(std::vector<std::tuple<std::string, std::string, std::string, int>>* report_data);
        int display_brand_quantities_sales(std::vector<std::tuple<std::string, std::string, std::string, int>>* report_data);
        int handle_customized_report_print(const std::vector<std::tuple<std::string, std::string, std::string, int>>& print_data);

    private:
        int prepare_items_pointer(const std::string& src, char delimiter, std::unique_ptr<char[]>& dest);
    };

    class ShowMarketerWindow : public CustomizedReportWindow
    {
    public:
        ShowMarketerWindow(GeneralReportWindow& gr_window, CustomizedReportWindow& cr_window, PassWord& change_password);
        ~ShowMarketerWindow() = default;

    public:
        GeneralReportWindow* gr_window;
        CustomizedReportWindow* cr_window;
        PassWord* change_password;
        bool show_window = false;
        ImGuiWindowFlags window_flags;

    public:
        int create_show_marketer_window();
    };
}