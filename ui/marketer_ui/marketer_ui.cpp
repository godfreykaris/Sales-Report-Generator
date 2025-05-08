#include "marketer_ui/marketer_ui.h"
#include "admin_ui/admin_ui.h"
#include "login_ui/login_ui.h"
#include <imgui_internal.h>
#include <thread>
#include <shellapi.h>

bool marketer_window = false;

namespace MUserUi {
    // Apply a professional ImGui theme
    void ApplyProfessionalTheme() {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 8.0f;
        style.FrameRounding = 4.0f;
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.18f, 1.0f);
        style.Colors[ImGuiCol_Text] = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.5f, 0.8f, 1.0f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.6f, 0.9f, 1.0f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.4f, 0.7f, 1.0f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.2f, 0.25f, 1.0f);
        style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.25f, 0.25f, 0.3f, 1.0f);
        style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.18f, 0.18f, 0.22f, 1.0f);
        style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.22f, 0.22f, 0.26f, 1.0f);
        style.Colors[ImGuiCol_ChildBg] = ImVec4(0.18f, 0.18f, 0.22f, 1.0f);
        style.Colors[ImGuiCol_Separator] = ImVec4(0.3f, 0.3f, 0.35f, 1.0f);
    }
}

MUserUi::ShowMarketerWindow::ShowMarketerWindow(GeneralReportWindow& gr_window, CustomizedReportWindow& cr_window, PassWord& change_password)
{
    this->gr_window = &gr_window;
    this->cr_window = &cr_window;
    this->change_password = &change_password;
    ApplyProfessionalTheme(); // Apply theme on construction
}

int MUserUi::ShowMarketerWindow::create_show_marketer_window()
{
    // Apply modern styling
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(40, 40, 40, 255)); // Dark gray background
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(60, 60, 60, 255)); // Input field background
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(80, 80, 80, 255)); // Input hover
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(100, 100, 100, 255)); // Input active
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 150, 80, 255)); // Green button
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 180, 100, 255)); // Lighter green on hover
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0, 120, 60, 255)); // Darker green when clicked
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255)); // Light gray text
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f); // Rounded window corners
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f); // Rounded button corners
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f)); // Consistent padding
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 5.0f)); // Button/input padding

    // Center and size the window
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(350.0f, 400.0f), ImGuiCond_Always);

    // Begin window
    ImGui::Begin("Marketer Dashboard", &this->show_window, this->window_flags);

    // Title
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]); // Assuming large font at index 1
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Marketer Dashboard").x) * 0.5f);
    ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.4f, 1.0f), "Marketer Dashboard");
    ImGui::PopFont();
    ImGui::Separator();

    // Button layout (centered, evenly spaced)
    const float button_width = 200.0f;
    const float button_height = 40.0f;
    const float spacing = 15.0f;

    ImGui::Dummy(ImVec2(0.0f, spacing));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
    if (ImGui::Button("View Report", ImVec2(button_width, button_height))) {
        this->gr_window->show_window = true;
        this->cr_window->populate_products(); // Pre-populate products for general report
    }
    if (this->gr_window->show_window) {
        this->gr_window->create_general_report_window(this->cr_window->products_array);
    }

    // ImGui::Dummy(ImVec2(0.0f, spacing));
    // ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
    // if (ImGui::Button("Custom Report", ImVec2(button_width, button_height))) {
    //     this->cr_window->show_window = true;
    // }
    // if (this->cr_window->show_window) {
    //     this->cr_window->create_customized_report_window();
    // }

    // Change Password with distinct styling
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(220, 75, 150, 255)); // Pink
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 105, 180, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(200, 130, 0, 255));
    ImGui::Dummy(ImVec2(0.0f, spacing));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
    if (ImGui::Button("Change Password", ImVec2(button_width, button_height))) {
        this->change_password->show_window = true;
    }
    if (this->change_password->show_window) {
        this->change_password->create_password_window();
    }
    ImGui::PopStyleColor(3);

    // Log Out with distinct styling
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(220, 50, 50, 255)); // Red
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 80, 80, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(180, 30, 30, 255));
    ImGui::Dummy(ImVec2(0.0f, spacing));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
    if (ImGui::Button("Log Out", ImVec2(button_width, button_height))) {
        login_window = true;
        marketer_window = false;
        admin_window = false;
        regular_user_window = false;
        finance_window = false;
        showing_admin_extensions = false;
    }
    ImGui::PopStyleColor(3);

    ImGui::End();

    // Pop styles
    ImGui::PopStyleColor(8);
    ImGui::PopStyleVar(4);

    return 1;
}

MUserUi::GeneralReportWindow::GeneralReportWindow(mongocxx::database db, GLFWwindow* window)
{
    this->sales_collection = db["Products"];
    this->v_seasons = get_seasons_data(this->sales_collection);
    this->v_age_groups = get_age_groups_data(this->sales_collection);
    this->v_genders = get_genders_data(this->sales_collection);
    this->v_locations = get_locations_data(this->sales_collection);
    this->window = window;
    this->main_window = glfwGetWin32Window(this->window);
    ApplyProfessionalTheme();
}

int MUserUi::GeneralReportWindow::create_general_report_window()
{
    std::vector<std::string> empty_products;
    return create_general_report_window(empty_products);
}

int MUserUi::GeneralReportWindow::create_general_report_window(const std::vector<std::string>& products)
{
    // Apply modern styling
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(40, 40, 40, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(60, 60, 60, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(80, 80, 80, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(100, 100, 100, 255));
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 150, 80, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 180, 100, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0, 120, 60, 255));
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 5.0f));

    // Center and size the window
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(850, 620), ImGuiCond_Always);

    // Begin window
    if (!ImGui::Begin("General Report", &this->show_window, this->window_flags | ImGuiWindowFlags_NoScrollWithMouse)) {
        ImGui::End();
        ImGui::PopStyleColor(8);
        ImGui::PopStyleVar(4);
        return 1;
    }

    // Title
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("General Sales Report").x) * 0.5f);
    ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.4f, 1.0f), "General Sales Report");
    ImGui::PopFont();
    ImGui::Separator();

    std::vector<std::tuple<std::string, std::string, int>> report_data;
    this->display_report(&report_data, products);

    if (this->saving)
        this->LoadingIndicatorCircle("Loading", 30.0f, ImVec4(0.0f, 0.8f, 0.4f, 1.0f), ImVec4(0.0f, 0.6f, 0.3f, 1.0f), 15, 7.0f);

    ImGui::Dummy(ImVec2(0.0f, 15.0f));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 120.0f) * 0.5f);
    bool isDisabled = this->saving;
    if (isDisabled)
        ImGui::BeginDisabled(true);
    if (ImGui::Button("Save Report", ImVec2(120.0f, 30.0f))) {
        this->saving = true;
        std::thread save_thread(&MUserUi::GeneralReportWindow::handle_general_report_print, this, report_data);
        save_thread.detach();
    }
    if (isDisabled)
        ImGui::EndDisabled();

    // Render popups in main thread
    if (this->show_print_success) {
        ImGui::OpenPopup("Print Success");
        this->show_print_success = false; // Reset flag after opening
    }
    if (this->show_print_error) {
        ImGui::OpenPopup("Print Error");
        this->show_print_error = false; // Reset flag after opening
    }

    if (ImGui::BeginPopupModal("Print Success", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Report saved to %s", this->last_saved_file.c_str());
        ImGui::Text("Open the PDF to print or review.");
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 250.0f) * 0.5f);
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("View", ImVec2(120, 0))) {
            // Convert to wide string for ShellExecute
            wchar_t file_name_w[4096] = {0};
            MultiByteToWideChar(CP_UTF8, 0, this->last_saved_file.c_str(), -1, file_name_w, 4096);
            ShellExecuteW(nullptr, L"open", file_name_w, nullptr, nullptr, SW_SHOWNORMAL);
        }
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopupModal("Print Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Failed to save PDF. Check file permissions or try another location.");
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 120.0f) * 0.5f);
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::End();
    ImGui::PopStyleColor(8);
    ImGui::PopStyleVar(4);
    return 1;
}

int MUserUi::GeneralReportWindow::display_report(std::vector<std::tuple<std::string, std::string, int>>* report_data, const std::vector<std::string>& products)
{
    // Always populate report_data with all categories
    for (const auto& product : products) {
        report_data->emplace_back("Product", product, 0);
        for (const auto& season : v_seasons) {
            report_data->emplace_back("Season", season.category_name, season.category_total);
        }
        for (const auto& age_group : v_age_groups) {
            report_data->emplace_back("Age Group", age_group.category_name, age_group.category_total);
        }
        for (const auto& gender : v_genders) {
            report_data->emplace_back("Gender", gender.category_name, gender.category_total);
        }
        for (const auto& location : v_locations) {
            report_data->emplace_back("Location", location.category_name, location.category_total);
        }
    }

    // Display hierarchical view
    float table_height = ImGui::GetContentRegionAvail().y - 80.0f;
    ImGui::BeginChild("GeneralReportScroll", ImVec2(0, table_height), true);

    for (const auto& product : products) {
        if (ImGui::TreeNode(product.c_str())) {
            if (ImGui::TreeNode("Seasons")) {
                for (const auto& season : v_seasons) {
                    ImGui::Text("  %s: %d", season.category_name.c_str(), season.category_total);
                }
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Age Groups")) {
                for (const auto& age_group : v_age_groups) {
                    ImGui::Text("  %s: %d", age_group.category_name.c_str(), age_group.category_total);
                }
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Genders")) {
                for (const auto& gender : v_genders) {
                    ImGui::Text("  %s: %d", gender.category_name.c_str(), gender.category_total);
                }
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Locations")) {
                for (const auto& location : v_locations) {
                    ImGui::Text("  %s: %d", location.category_name.c_str(), location.category_total);
                }
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
    }

    if (products.empty()) {
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
        const char* message = "No products available";
        float text_width = ImGui::CalcTextSize(message).x;
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - text_width) * 0.5f);
        ImGui::TextColored(ImVec4(0.8f, 0.3f, 0.3f, 1.0f), message);
        ImGui::PopFont();
    }

    ImGui::EndChild();
    return 1;
}

int MUserUi::GeneralReportWindow::handle_general_report_print(const std::vector<std::tuple<std::string, std::string, int>>& print_data)
{
    // Copy data for thread safety
    std::vector<std::tuple<std::string, std::string, int>> local_data = print_data;

    if (local_data.empty()) {
        this->show_print_error = true;
        this->saving = false;
        return 1;
    }

    // Use larger buffer for long paths
    wchar_t file_name[4096] = L"general_report.pdf";
    OPENFILENAMEW ofn = { sizeof(OPENFILENAMEW) };
    ofn.hwndOwner = this->main_window;
    ofn.lpstrFilter = L"PDF Files (*.pdf)\0*.pdf\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = file_name;
    ofn.nMaxFile = 4096;
    ofn.lpstrDefExt = L"pdf";
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

    if (GetSaveFileNameW(&ofn)) {
        // Convert wide string to UTF-8
        char file_name_mb[4096] = {0};
        int len = WideCharToMultiByte(CP_UTF8, 0, file_name, -1, file_name_mb, sizeof(file_name_mb), nullptr, nullptr);
        if (len == 0) {
            this->show_print_error = true;
            this->saving = false;
            return 1;
        }

        HPDF_Doc pdf = HPDF_New(nullptr, nullptr);
        if (!pdf) {
            this->show_print_error = true;
            this->saving = false;
            return 1;
        }

        HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);
        HPDF_Page page = HPDF_AddPage(pdf);
        if (!page) {
            HPDF_Free(pdf);
            this->show_print_error = true;
            this->saving = false;
            return 1;
        }

        HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);

        HPDF_Font title_font = HPDF_GetFont(pdf, "Helvetica-Bold", nullptr);
        if (!title_font) title_font = HPDF_GetFont(pdf, "Times-Roman", nullptr);
        HPDF_Font header_font = HPDF_GetFont(pdf, "Helvetica-Bold", nullptr);
        if (!header_font) header_font = HPDF_GetFont(pdf, "Times-Roman", nullptr);
        HPDF_Font data_font = HPDF_GetFont(pdf, "Helvetica", nullptr);
        if (!data_font) data_font = HPDF_GetFont(pdf, "Times-Roman", nullptr);

        size_t max_category_len = 20, max_value_len = 20;
        for (const auto& [category, value, count] : local_data) {
            max_category_len = (std::max)(max_category_len, category.length());
            max_value_len = (std::max)(max_value_len, value.length());
        }

        const float char_width = 7.0f;
        const float max_page_width = 500.0f;
        const size_t max_chars_per_line = static_cast<size_t>(max_page_width / char_width / 2);
        max_category_len = (std::min)(max_category_len, max_chars_per_line);
        max_value_len = (std::min)(max_value_len, max_chars_per_line);

        const float text_padding = 12.0f;
        const float category_width = (std::max)(50.0f, static_cast<float>(max_category_len) * char_width + 2 * text_padding);
        const float value_width = (std::max)(50.0f, static_cast<float>(max_value_len) * char_width + 2 * text_padding);
        const float count_width = (std::max)(50.0f, 10.0f * char_width + 2 * text_padding);
        const float table_width = category_width + value_width + count_width;
        const float x_offset = (HPDF_Page_GetWidth(page) - table_width) / 2;
        const float base_row_height = 20.0f;
        const float line_height = 15.0f;

        float y = HPDF_Page_GetHeight(page) - 50;
        float table_top_y = y - 40 - 30;

        HPDF_Page_SetFontAndSize(page, title_font, 16);
        HPDF_Page_SetRGBFill(page, 0.0f, 0.2f, 0.4f);
        HPDF_Page_BeginText(page);
        const char* title = "General Sales Report";
        float title_width = HPDF_Page_TextWidth(page, title);
        HPDF_Page_TextOut(page, (HPDF_Page_GetWidth(page) - title_width) / 2, y, title);
        HPDF_Page_EndText(page);
        y -= 40;

        time_t now = time(nullptr);
        char timestamp[32];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
        HPDF_Page_SetFontAndSize(page, data_font, 10);
        HPDF_Page_SetRGBFill(page, 0.5f, 0.5f, 0.5f);
        HPDF_Page_BeginText(page);
        float timestamp_width = HPDF_Page_TextWidth(page, timestamp);
        HPDF_Page_TextOut(page, (HPDF_Page_GetWidth(page) - timestamp_width) / 2, y, timestamp);
        HPDF_Page_EndText(page);
        y -= 30;

        HPDF_Page_SetFontAndSize(page, header_font, 12);
        HPDF_Page_SetRGBFill(page, 0.0f, 0.4f, 0.6f);
        HPDF_Page_BeginText(page);
        HPDF_Page_TextOut(page, x_offset + text_padding, y, "Category");
        HPDF_Page_TextOut(page, x_offset + category_width + text_padding, y, "Value");
        HPDF_Page_TextOut(page, x_offset + category_width + value_width + text_padding, y, "Sales Count");
        HPDF_Page_EndText(page);

        HPDF_Page_SetRGBFill(page, 0.9f, 0.95f, 1.0f);
        HPDF_Page_SetRGBStroke(page, 0.0f, 0.2f, 0.4f);
        HPDF_Page_Rectangle(page, x_offset, y - 5, table_width, base_row_height + 10);
        HPDF_Page_FillStroke(page);

        y -= base_row_height;
        float table_bottom_y = y;
        bool even_row = false;
        int page_count = 1;

        auto split_text = [max_chars_per_line](const std::string& text) -> std::vector<std::string> {
            std::vector<std::string> lines;
            if (text.empty()) {
                lines.push_back("");
                return lines;
            }
            size_t start = 0;
            while (start < text.length()) {
                size_t len = (std::min)(max_chars_per_line, text.length() - start);
                if (start + len < text.length()) {
                    while (len > 0 && text[start + len - 1] != ' ') --len;
                    if (len == 0) len = max_chars_per_line;
                }
                lines.push_back(text.substr(start, len));
                start += len;
            }
            return lines;
        };

        std::string last_category;
        for (const auto& [category, value, count] : local_data) {
            std::vector<std::string> category_lines = split_text(category);
            std::vector<std::string> value_lines = split_text(value);
            size_t max_lines = (std::max)({category_lines.size(), value_lines.size(), size_t(1)});
            float row_height = max_lines * line_height + 10.0f;

            if (y - row_height < 50) {
                HPDF_Page_SetFontAndSize(page, data_font, 10);
                HPDF_Page_SetRGBFill(page, 0.5f, 0.5f, 0.5f);
                HPDF_Page_BeginText(page);
                char page_num[32];
                snprintf(page_num, sizeof(page_num), "Page %d", page_count);
                HPDF_Page_TextOut(page, x_offset, 30, page_num);
                HPDF_Page_EndText(page);

                float x_positions[] = {x_offset, x_offset + category_width, x_offset + category_width + value_width, x_offset + category_width + value_width + count_width};
                HPDF_Page_SetRGBStroke(page, 0.7f, 0.7f, 0.7f);
                HPDF_Page_SetLineWidth(page, 0.5);
                for (float x : x_positions) {
                    HPDF_Page_MoveTo(page, x, table_top_y + 5);
                    HPDF_Page_LineTo(page, x, table_bottom_y - 5);
                    HPDF_Page_Stroke(page);
                }

                page = HPDF_AddPage(pdf);
                if (!page) {
                    HPDF_Free(pdf);
                    this->show_print_error = true;
                    this->saving = false;
                    return 1;
                }
                page_count++;
                HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
                y = HPDF_Page_GetHeight(page) - 50;
                table_top_y = y - 40 - 30;

                HPDF_Page_SetFontAndSize(page, header_font, 12);
                HPDF_Page_SetRGBFill(page, 0.0f, 0.4f, 0.6f);
                HPDF_Page_BeginText(page);
                HPDF_Page_TextOut(page, x_offset + text_padding, y, "Category");
                HPDF_Page_TextOut(page, x_offset + category_width + text_padding, y, "Value");
                HPDF_Page_TextOut(page, x_offset + category_width + value_width + text_padding, y, "Sales Count");
                HPDF_Page_EndText(page);

                HPDF_Page_SetRGBFill(page, 0.9f, 0.95f, 1.0f);
                HPDF_Page_SetRGBStroke(page, 0.0f, 0.2f, 0.4f);
                HPDF_Page_Rectangle(page, x_offset, y - 5, table_width, base_row_height + 10);
                HPDF_Page_FillStroke(page);

                y -= base_row_height;
                table_bottom_y = y;
            }

            // Determine if this is a product row with 0 count
            bool is_product_row = (category == "Product" && count == 0);

            // Set background color
            if (is_product_row) {
                HPDF_Page_SetRGBFill(page, 0.2f, 0.2f, 0.2f); // Darker background for product rows
            } else {
                HPDF_Page_SetRGBFill(page, even_row ? 0.98f : 1.0f, even_row ? 0.98f : 1.0f, even_row ? 0.98f : 1.0f);
            }
            HPDF_Page_Rectangle(page, x_offset, y - 5, table_width, row_height);
            HPDF_Page_Fill(page);
            if (!is_product_row) even_row = !even_row; // Only toggle for non-product rows

            HPDF_Page_SetRGBStroke(page, 0.7f, 0.7f, 0.7f);
            HPDF_Page_SetLineWidth(page, 0.5);
            HPDF_Page_MoveTo(page, x_offset, y - 2);
            HPDF_Page_LineTo(page, x_offset + table_width, y - 2);
            HPDF_Page_MoveTo(page, x_offset, y - row_height - 2);
            HPDF_Page_LineTo(page, x_offset + table_width, y - row_height - 2);
            HPDF_Page_Stroke(page);

            HPDF_Page_SetFontAndSize(page, data_font, 12);
            // Set text color
            if (is_product_row) {
                HPDF_Page_SetRGBFill(page, 1.0f, 1.0f, 1.0f); // White text for product rows
            } else {
                HPDF_Page_SetRGBFill(page, 0.0f, 0.0f, 0.0f); // Black text for others
            }
            HPDF_Page_BeginText(page);
            // Category column
            if (category.empty() || category == last_category) {
                for (size_t i = 0; i < category_lines.size(); ++i)
                    HPDF_Page_TextOut(page, x_offset + text_padding, y - (i * line_height), "");
            } else {
                for (size_t i = 0; i < category_lines.size(); ++i)
                    HPDF_Page_TextOut(page, x_offset + text_padding, y - (i * line_height), category_lines[i].c_str());
                last_category = category;
            }
            // Value column
            for (size_t i = 0; i < value_lines.size(); ++i)
                HPDF_Page_TextOut(page, x_offset + category_width + text_padding, y - (i * line_height), value_lines[i].c_str());
            // Sales Count column (skip for product rows with 0 count)
            if (!is_product_row) {
                std::string count_str = std::to_string(count);
                HPDF_Page_TextOut(page, x_offset + category_width + value_width + text_padding, y, count_str.c_str());
            }
            HPDF_Page_EndText(page);

            y -= row_height;
            table_bottom_y = y;
        }

        float x_positions[] = {x_offset, x_offset + category_width, x_offset + category_width + value_width, x_offset + category_width + value_width + count_width};
        HPDF_Page_SetRGBStroke(page, 0.7f, 0.7f, 0.7f);
        HPDF_Page_SetLineWidth(page, 0.5);
        for (float x : x_positions) {
            HPDF_Page_MoveTo(page, x, table_top_y + 5);
            HPDF_Page_LineTo(page, x, table_bottom_y - 5);
            HPDF_Page_Stroke(page);
        }

        HPDF_Page_SetFontAndSize(page, data_font, 10);
        HPDF_Page_SetRGBFill(page, 0.5f, 0.5f, 0.5f);
        HPDF_Page_BeginText(page);
        char page_num[32];
        snprintf(page_num, sizeof(page_num), "Page %d", page_count);
        HPDF_Page_TextOut(page, x_offset, 30, page_num);
        HPDF_Page_EndText(page);

        HPDF_STATUS error = HPDF_SaveToFile(pdf, file_name_mb);
        HPDF_Free(pdf);
        if (error == HPDF_OK) {
            this->last_saved_file = file_name_mb;
            this->show_print_success = true;
        } else {
            this->show_print_error = true;
        }
    } else {
        this->show_print_error = true;
    }

    this->saving = false;
    return 1;
}

int MUserUi::GeneralReportWindow::LoadingIndicatorCircle(const char* label, const float indicator_radius, const ImVec4& main_color,
    const ImVec4& backdrop_color, const int circle_count, const float speed)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return 0;

    ImGuiContext& g = *GImGui;
    const ImGuiID id = window->GetID(label);

    ImVec2 window_pos = ImGui::GetWindowPos();
    ImVec2 window_size = ImGui::GetWindowSize();
    ImVec2 window_center = ImVec2(window_pos.x + window_size.x * 0.5f, window_pos.y + window_size.y * 0.5f);
    const ImVec2 pos = window_center;

    const float circle_radius = indicator_radius / 10.0f;
    const ImRect bb(pos, ImVec2(pos.x + indicator_radius * 2.0f, pos.y + indicator_radius * 2.0f));

    ImGui::ItemSize(bb, g.Style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
        return 0;

    const float t = (float)g.Time;
    const auto degree_offset = 2.0f * IM_PI / circle_count;

    for (int i = 0; i < circle_count; ++i) {
        const auto x = indicator_radius * std::sin(degree_offset * i);
        const auto y = indicator_radius * std::cos(degree_offset * i);
        const auto growth = (std::max)(0.0f, std::sin(t * speed - i * degree_offset));

        ImVec4 color;
        color.x = main_color.x * growth + backdrop_color.x * (1.0f - growth);
        color.y = main_color.y * growth + backdrop_color.y * (1.0f - growth);
        color.z = main_color.z * growth + backdrop_color.z * (1.0f - growth);
        color.w = 1.0f;

        window->DrawList->AddCircleFilled(ImVec2(pos.x + indicator_radius + x, pos.y + indicator_radius - y),
            circle_radius + growth * circle_radius, ImGui::GetColorU32(color));
    }

    return 1;
}

int MUserUi::CustomizedReportWindow::current_selected_brand = 0;

MUserUi::CustomizedReportWindow::CustomizedReportWindow(mongocxx::database db, GLFWwindow* window)
{
    this->sales_collection = db["Products"];
    this->window = window;
    this->main_window = glfwGetWin32Window(this->window);
    this->v_products = get_products_sales_data(this->sales_collection);
    this->populate_products();
    ApplyProfessionalTheme();
}

int MUserUi::CustomizedReportWindow::create_customized_report_window()
{
    // Apply modern styling
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(40, 40, 40, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(60, 60, 60, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(80, 80, 80, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(100, 100, 100, 255));
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 150, 80, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 180, 100, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0, 120, 60, 255));
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 5.0f));

    // Center and size the window
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(600, 650), ImGuiCond_Always);

    // Begin window
    if (!ImGui::Begin("Customized Report", &this->show_window, this->window_flags | ImGuiWindowFlags_NoScrollWithMouse)) {
        ImGui::End();
        ImGui::PopStyleColor(8);
        ImGui::PopStyleVar(4);
        return 1;
    }

    // Title
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Customized Sales Report").x) * 0.5f);
    ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.4f, 1.0f), "Customized Sales Report");
    ImGui::PopFont();
    ImGui::Separator();

    ImGui::SetCursorPosX(20.0f);
    ImGui::Text("Filter by Product and Brand");
    ImGui::SetCursorPosX(20.0f);
    this->populate_products();
    ImGui::PushItemWidth(250.0f);
    ImGui::Combo("##Products", &this->product_index, this->products.c_str(), ImGui::CalcTextSize(this->products.c_str()).y);
    ImGui::PopItemWidth();

    ImGui::SetCursorPosX(20.0f);
    ImGui::PushItemWidth(250.0f);
    this->populate_brands(this->v_products[this->product_index].p_brands);
    ImGui::Combo("##Brands", &this->current_selected_brand, this->brands.c_str(), ImGui::CalcTextSize(this->brands.c_str()).y);
    ImGui::PopItemWidth();

    ImGui::Separator();

    std::vector<std::tuple<std::string, std::string, std::string, int>> report_data;
    ImGui::BeginChild("CustomReportScroll", ImVec2(0, ImGui::GetContentRegionAvail().y - 100.0f), true);

    if (this->product_index == 0) { // "All Products"
        for (const auto& product : v_products) {
            if (ImGui::TreeNode(product.product_name.c_str())) {
                for (const auto& brand : product.p_brands) {
                    if (ImGui::TreeNode(brand.brand_name.c_str())) {
                        for (const auto& quantity : brand.b_quantities) {
                            ImGui::Text("  %s: %d", quantity.quantity_name.c_str(), quantity.quantity_total);
                            report_data.emplace_back(product.product_name, brand.brand_name, quantity.quantity_name, quantity.quantity_total);
                        }
                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }
        }
    }
    else {
        const auto& product = v_products[this->product_index - 1];
        if (this->current_selected_brand == 0) { // All brands
            if (ImGui::TreeNode(product.product_name.c_str())) {
                for (const auto& brand : product.p_brands) {
                    ImGui::Text("  %s: %d", brand.brand_name.c_str(), brand.brand_total);
                    report_data.emplace_back(product.product_name, brand.brand_name, "", brand.brand_total);
                }
                ImGui::TreePop();
            }
        }
        else {
            const auto& brand = product.p_brands[this->current_selected_brand - 1];
            if (ImGui::TreeNode(product.product_name.c_str())) {
                if (ImGui::TreeNode(brand.brand_name.c_str())) {
                    for (const auto& quantity : brand.b_quantities) {
                        ImGui::Text("  %s: %d", quantity.quantity_name.c_str(), quantity.quantity_total);
                        report_data.emplace_back(product.product_name, brand.brand_name, quantity.quantity_name, quantity.quantity_total);
                    }
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }
        }
    }

    if (report_data.empty()) {
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
        const char* message = "No data available";
        float text_width = ImGui::CalcTextSize(message).x;
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - text_width) * 0.5f);
        ImGui::TextColored(ImVec4(0.8f, 0.3f, 0.3f, 1.0f), message);
        ImGui::PopFont();
    }

    ImGui::EndChild();

    if (this->saving)
        this->LoadingIndicatorCircle("Loading", 30.0f, ImVec4(0.0f, 0.8f, 0.4f, 1.0f), ImVec4(0.0f, 0.6f, 0.3f, 1.0f), 15, 7.0f);

    ImGui::Dummy(ImVec2(0.0, 15.0f));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 120.0f) * 0.5f);
    bool isDisabled = this->saving;
    if (isDisabled)
        ImGui::BeginDisabled(true);
    if (ImGui::Button("Save Report", ImVec2(120.0f, 30.0f))) {
        this->saving = true;
        std::thread save_thread(&MUserUi::CustomizedReportWindow::handle_customized_report_print, this, report_data);
        save_thread.detach();
    }
    if (isDisabled)
        ImGui::EndDisabled();

    ImGui::End();
    ImGui::PopStyleColor(8);
    ImGui::PopStyleVar(4);
    return 1;
}

int MUserUi::CustomizedReportWindow::populate_products()
{
    this->products = "All Products_";
    this->products_array.clear();

    for (ProductData p_data : this->v_products) {
        this->products += p_data.product_name + "_";
        this->products_array.push_back(p_data.product_name);
    }

    return 1;
}

int MUserUi::CustomizedReportWindow::populate_brands(std::vector<BrandData> v_brands)
{
    this->brands = "All Brands_";
    this->brands_array.clear();

    for (BrandData b_data : v_brands) {
        this->brands += b_data.brand_name + "_";
        this->brands_array.push_back(b_data.brand_name);
    }

    return 1;
}

int MUserUi::CustomizedReportWindow::display_products_sales(std::vector<std::tuple<std::string, std::string, std::string, int>>* report_data)
{
    for (const auto& product : this->v_products)
        report_data->emplace_back(product.product_name, "", "", product.product_total);

    float table_height = ImGui::GetContentRegionAvail().y - 80.0f;
    if (report_data->empty()) {
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
        const char* message = "No data found";
        float text_width = ImGui::CalcTextSize(message).x;
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - text_width) * 0.5f);
        ImGui::TextColored(ImVec4(0.3f, 0.8f, 0.5f, 1.0f), message);
        ImGui::PopFont();
    }
    else if (ImGui::BeginTable("CustomizedReportTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY, ImVec2(0, table_height))) {
        ImGui::TableSetupColumn("Product", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableSetupColumn("Brand", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableSetupColumn("Quantity", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Sales Count", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        for (const auto& [product, brand, quantity, count] : *report_data) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", product.c_str());
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", brand.c_str());
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%s", quantity.c_str());
            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%d", count);
        }
        ImGui::EndTable();
    }

    return 1;
}

int MUserUi::CustomizedReportWindow::display_brands_sales(std::vector<std::tuple<std::string, std::string, std::string, int>>* report_data)
{
    auto iterator = std::find(this->products_array.begin(), this->products_array.end(), this->selected_product);
    if (iterator == this->products_array.end())
        return 0;
    this->product_index = (int)(iterator - this->products_array.begin());

    this->populate_brands(this->v_products[this->product_index].p_brands);

    for (const auto& brand : this->v_products[this->product_index].p_brands)
        report_data->emplace_back(this->selected_product, brand.brand_name, "", brand.brand_total);

    float table_height = ImGui::GetContentRegionAvail().y - 80.0f;
    if (report_data->empty()) {
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
        const char* message = "No data found";
        float text_width = ImGui::CalcTextSize(message).x;
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - text_width) * 0.5f);
        ImGui::TextColored(ImVec4(0.3f, 0.8f, 0.5f, 1.0f), message);
        ImGui::PopFont();
    }
    else if (ImGui::BeginTable("CustomizedReportTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY, ImVec2(0, table_height))) {
        ImGui::TableSetupColumn("Product", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableSetupColumn("Brand", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableSetupColumn("Quantity", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Sales Count", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        std::string last_product = "";
        for (const auto& [product, brand, quantity, count] : *report_data) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            if (product.empty() || product == last_product)
                ImGui::Text("");
            else {
                ImGui::Text("%s", product.c_str());
                last_product = product;
            }
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", brand.c_str());
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%s", quantity.c_str());
            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%d", count);
        }
        ImGui::EndTable();
    }

    this->p_brands = std::make_unique<char[]>(this->brands.size() + 1);
    std::fill_n(this->p_brands.get(), this->brands.size() + 1, 0);
    std::strncpy(this->p_brands.get(), this->brands.c_str(), this->brands.size());
    this->prepare_items_pointer(this->brands, '_', this->p_brands);

    return 1;
}

int MUserUi::CustomizedReportWindow::display_brand_quantities_sales(std::vector<std::tuple<std::string, std::string, std::string, int>>* report_data)
{
    for (const auto& quantity : this->v_products[this->product_index].p_brands[this->current_selected_brand - 1].b_quantities)
        report_data->emplace_back(this->v_products[this->product_index].product_name,
            this->v_products[this->product_index].p_brands[this->current_selected_brand - 1].brand_name,
            quantity.quantity_name, quantity.quantity_total);

    float table_height = ImGui::GetContentRegionAvail().y - 80.0f;
    if (report_data->empty()) {
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
        const char* message = "No data found";
        float text_width = ImGui::CalcTextSize(message).x;
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - text_width) * 0.5f);
        ImGui::TextColored(ImVec4(0.3f, 0.8f, 0.5f, 1.0f), message);
        ImGui::PopFont();
    }
    else if (ImGui::BeginTable("CustomizedReportTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY, ImVec2(0, table_height))) {
        ImGui::TableSetupColumn("Product", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableSetupColumn("Brand", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableSetupColumn("Quantity", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Sales Count", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        std::string last_product = "";
        std::string last_brand = "";
        for (const auto& [product, brand, quantity, count] : *report_data) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            if (product.empty() || product == last_product)
                ImGui::Text("");
            else {
                ImGui::Text("%s", product.c_str());
                last_product = product;
            }
            ImGui::TableSetColumnIndex(1);
            if (brand.empty() || brand == last_brand)
                ImGui::Text("");
            else {
                ImGui::Text("%s", brand.c_str());
                last_brand = brand;
            }
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%s", quantity.c_str());
            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%d", count);
        }
        ImGui::EndTable();
    }

    return 1;
}

int MUserUi::CustomizedReportWindow::handle_customized_report_print(const std::vector<std::tuple<std::string, std::string, std::string, int>>& print_data)
{
    this->saving = true;
    if (print_data.empty()) {
        ImGui::OpenPopup("Print Error");
        this->saving = false;
        return 1;
    }

    OPENFILENAME ofn = { 0 };
    wchar_t file_name[MAX_PATH] = L"customized_report.pdf";
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = this->main_window;
    ofn.lpstrFilter = L"PDF Files (*.pdf)\0*.pdf\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = file_name;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = L"pdf";
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

    if (GetSaveFileName(&ofn)) {
        char file_name_mb[MAX_PATH] = { 0 };
        size_t converted_chars = 0;
        wcstombs_s(&converted_chars, file_name_mb, file_name, MAX_PATH);

        HPDF_Doc pdf = HPDF_New(nullptr, nullptr);
        if (!pdf) {
            ImGui::OpenPopup("Print Error");
            this->saving = false;
            return 1;
        }

        HPDF_Page page = HPDF_AddPage(pdf);
        HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);
        HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);

        HPDF_Font title_font = HPDF_GetFont(pdf, "Helvetica-Bold", nullptr);
        if (!title_font) title_font = HPDF_GetFont(pdf, "Times-Roman", nullptr);
        HPDF_Font header_font = HPDF_GetFont(pdf, "Helvetica-Bold", nullptr);
        if (!header_font) header_font = HPDF_GetFont(pdf, "Times-Roman", nullptr);
        HPDF_Font data_font = HPDF_GetFont(pdf, "Helvetica", nullptr);
        if (!data_font) data_font = HPDF_GetFont(pdf, "Times-Roman", nullptr);

        size_t max_product_len = 20, max_brand_len = 20, max_quantity_len = 20;
        for (const auto& [product, brand, quantity, count] : print_data) {
            max_product_len = (std::max)(max_product_len, product.length());
            max_brand_len = (std::max)(max_brand_len, brand.length());
            max_quantity_len = (std::max)(max_quantity_len, quantity.length());
        }

        const float char_width = 7.0f;
        const float max_page_width = 500.0f;
        const size_t max_chars_per_line = static_cast<size_t>(max_page_width / char_width / 2);
        max_product_len = (std::min)(max_product_len, max_chars_per_line);
        max_brand_len = (std::min)(max_brand_len, max_chars_per_line);
        max_quantity_len = (std::min)(max_quantity_len, max_chars_per_line);

        const float text_padding = 12.0f;
        const float product_width = (std::max)(50.0f, static_cast<float>(max_product_len) * char_width + 2 * text_padding);
        const float brand_width = (std::max)(50.0f, static_cast<float>(max_brand_len) * char_width + 2 * text_padding);
        const float quantity_width = (std::max)(50.0f, static_cast<float>(max_quantity_len) * char_width + 2 * text_padding);
        const float count_width = (std::max)(50.0f, 10.0f * char_width + 2 * text_padding);
        const float table_width = product_width + brand_width + quantity_width + count_width;
        const float x_offset = (HPDF_Page_GetWidth(page) - table_width) / 2;
        const float base_row_height = 20.0f;
        const float line_height = 15.0f;

        float y = HPDF_Page_GetHeight(page) - 50;
        float table_top_y = y - 40 - 30;

        HPDF_Page_SetFontAndSize(page, title_font, 16);
        HPDF_Page_SetRGBFill(page, 0.0f, 0.2f, 0.4f);
        HPDF_Page_BeginText(page);
        const char* title = "Customized Sales Report";
        float title_width = HPDF_Page_TextWidth(page, title);
        HPDF_Page_TextOut(page, (HPDF_Page_GetWidth(page) - title_width) / 2, y, title);
        HPDF_Page_EndText(page);
        y -= 40;

        time_t now = time(nullptr);
        char timestamp[32];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
        HPDF_Page_SetFontAndSize(page, data_font, 10);
        HPDF_Page_SetRGBFill(page, 0.5f, 0.5f, 0.5f);
        HPDF_Page_BeginText(page);
        float timestamp_width = HPDF_Page_TextWidth(page, timestamp);
        HPDF_Page_TextOut(page, (HPDF_Page_GetWidth(page) - timestamp_width) / 2, y, timestamp);
        HPDF_Page_EndText(page);
        y -= 30;

        HPDF_Page_SetFontAndSize(page, header_font, 12);
        HPDF_Page_SetRGBFill(page, 0.0f, 0.4f, 0.6f);
        HPDF_Page_BeginText(page);
        HPDF_Page_TextOut(page, x_offset + text_padding, y, "Product");
        HPDF_Page_TextOut(page, x_offset + product_width + text_padding, y, "Brand");
        HPDF_Page_TextOut(page, x_offset + product_width + brand_width + text_padding, y, "Quantity");
        HPDF_Page_TextOut(page, x_offset + product_width + brand_width + quantity_width + text_padding, y, "Sales Count");
        HPDF_Page_EndText(page);

        HPDF_Page_SetRGBFill(page, 0.9f, 0.95f, 1.0f);
        HPDF_Page_SetRGBStroke(page, 0.0f, 0.2f, 0.4f);
        HPDF_Page_Rectangle(page, x_offset, y - 5, table_width, base_row_height + 10);
        HPDF_Page_FillStroke(page);

        y -= base_row_height;
        float table_bottom_y = y;
        bool even_row = false;
        int page_count = 1;

        auto split_text = [max_chars_per_line](const std::string& text) -> std::vector<std::string> {
            std::vector<std::string> lines;
            if (text.empty()) {
                lines.push_back("");
                return lines;
            }
            size_t start = 0;
            while (start < text.length()) {
                size_t len = (std::min)(max_chars_per_line, text.length() - start);
                if (start + len < text.length()) {
                    while (len > 0 && text[start + len - 1] != ' ')
                        --len;
                    if (len == 0)
                        len = max_chars_per_line;
                }
                lines.push_back(text.substr(start, len));
                start += len;
            }
            return lines;
        };

        std::string last_product = "";
        std::string last_brand = "";
        for (const auto& [product, brand, quantity, count] : print_data) {
            std::vector<std::string> product_lines = split_text(product);
            std::vector<std::string> brand_lines = split_text(brand);
            std::vector<std::string> quantity_lines = split_text(quantity);
            size_t max_lines = (std::max)({ product_lines.size(), brand_lines.size(), quantity_lines.size() });
            max_lines = (std::max)(max_lines, size_t(1));
            float row_height = max_lines * line_height + 10.0f;

            if (y - row_height < 50) {
                HPDF_Page_SetFontAndSize(page, data_font, 10);
                HPDF_Page_SetRGBFill(page, 0.5f, 0.5f, 0.5f);
                HPDF_Page_BeginText(page);
                char page_num[32];
                snprintf(page_num, sizeof(page_num), "Page %d", page_count);
                HPDF_Page_TextOut(page, x_offset, 30, page_num);
                HPDF_Page_EndText(page);

                float x_positions[] = { x_offset, x_offset + product_width, x_offset + product_width + brand_width, x_offset + product_width + brand_width + quantity_width, x_offset + product_width + brand_width + quantity_width + count_width };
                HPDF_Page_SetRGBStroke(page, 0.7f, 0.7f, 0.7f);
                HPDF_Page_SetLineWidth(page, 0.5);
                for (float x : x_positions) {
                    HPDF_Page_MoveTo(page, x, table_top_y + 5);
                    HPDF_Page_LineTo(page, x, table_bottom_y - 5);
                    HPDF_Page_Stroke(page);
                }

                page = HPDF_AddPage(pdf);
                page_count++;
                HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
                y = HPDF_Page_GetHeight(page) - 50;
                table_top_y = y - 40 - 30;

                HPDF_Page_SetFontAndSize(page, header_font, 12);
                HPDF_Page_SetRGBFill(page, 0.0f, 0.4f, 0.6f);
                HPDF_Page_BeginText(page);
                HPDF_Page_TextOut(page, x_offset + text_padding, y, "Product");
                HPDF_Page_TextOut(page, x_offset + product_width + text_padding, y, "Brand");
                HPDF_Page_TextOut(page, x_offset + product_width + brand_width + text_padding, y, "Quantity");
                HPDF_Page_TextOut(page, x_offset + product_width + brand_width + quantity_width + text_padding, y, "Sales Count");
                HPDF_Page_EndText(page);

                HPDF_Page_SetRGBFill(page, 0.9f, 0.95f, 1.0f);
                HPDF_Page_SetRGBStroke(page, 0.0f, 0.2f, 0.4f);
                HPDF_Page_Rectangle(page, x_offset, y - 5, table_width, base_row_height + 10);
                HPDF_Page_FillStroke(page);

                y -= base_row_height;
                table_bottom_y = y;
            }

            HPDF_Page_SetRGBFill(page, even_row ? 0.98f : 1.0f, even_row ? 0.98f : 1.0f, even_row ? 0.98f : 1.0f);
            HPDF_Page_Rectangle(page, x_offset, y - 5, table_width, row_height);
            HPDF_Page_Fill(page);
            even_row = !even_row;

            HPDF_Page_SetRGBStroke(page, 0.7f, 0.7f, 0.7f);
            HPDF_Page_SetLineWidth(page, 0.5);
            HPDF_Page_MoveTo(page, x_offset, y - 2);
            HPDF_Page_LineTo(page, x_offset + table_width, y - 2);
            HPDF_Page_MoveTo(page, x_offset, y - row_height - 2);
            HPDF_Page_LineTo(page, x_offset + table_width, y - row_height - 2);
            HPDF_Page_Stroke(page);

            HPDF_Page_SetFontAndSize(page, data_font, 12);
            HPDF_Page_SetRGBFill(page, 0.0f, 0.5f, 0.0f);
            HPDF_Page_BeginText(page);
            if (product.empty() || product == last_product) {
                for (size_t i = 0; i < product_lines.size(); ++i)
                    HPDF_Page_TextOut(page, x_offset + text_padding, y - (i * line_height), "");
            }
            else {
                for (size_t i = 0; i < product_lines.size(); ++i)
                    HPDF_Page_TextOut(page, x_offset + text_padding, y - (i * line_height), product_lines[i].c_str());
                last_product = product;
            }
            HPDF_Page_SetRGBFill(page, 0.0f, 0.0f, 0.0f);
            if (brand.empty() || brand == last_brand) {
                for (size_t i = 0; i < brand_lines.size(); ++i)
                    HPDF_Page_TextOut(page, x_offset + product_width + text_padding, y - (i * line_height), "");
            }
            else {
                for (size_t i = 0; i < brand_lines.size(); ++i)
                    HPDF_Page_TextOut(page, x_offset + product_width + text_padding, y - (i * line_height), brand_lines[i].c_str());
                last_brand = brand;
            }
            for (size_t i = 0; i < quantity_lines.size(); ++i)
                HPDF_Page_TextOut(page, x_offset + product_width + brand_width + text_padding, y - (i * line_height), quantity_lines[i].c_str());
            std::string count_str = std::to_string(count);
            HPDF_Page_TextOut(page, x_offset + product_width + brand_width + quantity_width + text_padding, y, count_str.c_str());
            HPDF_Page_EndText(page);

            y -= row_height;
            table_bottom_y = y;
        }

        float x_positions[] = { x_offset, x_offset + product_width, x_offset + product_width + brand_width, x_offset + product_width + brand_width + quantity_width, x_offset + product_width + brand_width + quantity_width + count_width };
        HPDF_Page_SetRGBStroke(page, 0.7f, 0.7f, 0.7f);
        HPDF_Page_SetLineWidth(page, 0.5);
        for (float x : x_positions) {
            HPDF_Page_MoveTo(page, x, table_top_y + 5);
            HPDF_Page_LineTo(page, x, table_bottom_y - 5);
            HPDF_Page_Stroke(page);
        }

        HPDF_Page_SetFontAndSize(page, data_font, 10);
        HPDF_Page_SetRGBFill(page, 0.5f, 0.5f, 0.5f);
        HPDF_Page_BeginText(page);
        char page_num[32];
        snprintf(page_num, sizeof(page_num), "Page %d", page_count);
        HPDF_Page_TextOut(page, x_offset, 30, page_num);
        HPDF_Page_EndText(page);

        HPDF_STATUS error = HPDF_SaveToFile(pdf, file_name_mb);
        HPDF_Free(pdf);
        if (error == HPDF_OK) {
            this->last_saved_file = file_name_mb;
            ImGui::OpenPopup("Print Success");
        }
        else {
            ImGui::OpenPopup("Print Error");
        }
    }

    if (ImGui::BeginPopupModal("Print Success", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Report saved to %s", this->last_saved_file.c_str());
        ImGui::Text("Open the PDF to print or review.");
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 250.0f) * 0.5f);
        if (ImGui::Button("OK", ImVec2(120, 0)))
            ImGui::CloseCurrentPopup();
        ImGui::SameLine();
        if (ImGui::Button("View", ImVec2(120, 0))) {
            wchar_t file_name_w[MAX_PATH] = { 0 };
            size_t converted_chars = 0;
            mbstowcs_s(&converted_chars, file_name_w, this->last_saved_file.c_str(), MAX_PATH);
            ShellExecute(NULL, L"open", file_name_w, NULL, NULL, SW_SHOWNORMAL);
        }
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopupModal("Print Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Failed to save PDF. Check file permissions or try another location.");
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 120.0f) * 0.5f);
        if (ImGui::Button("OK", ImVec2(120, 0)))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    this->saving = false;
    return 1;
}

int MUserUi::CustomizedReportWindow::prepare_items_pointer(const std::string& src, char delimiter, std::unique_ptr<char[]>& dest)
{
    if (!dest || src.empty()) {
        return 0;
    }

    // Copy source string to destination
    std::strncpy(dest.get(), src.c_str(), src.size());
    dest[src.size()] = '\0';

    // Replace delimiters with null terminators for ImGui combo items
    for (size_t i = 0; i < src.size(); ++i) {
        if (src[i] == delimiter) {
            dest[i] = '\0';
        }
    }

    return 1;
}