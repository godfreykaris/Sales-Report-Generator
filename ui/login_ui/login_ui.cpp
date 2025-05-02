#include "login_ui.h"
#include "regular_ui/regular_user_ui.h"
#include "finance_ui/finance_ui.h"
#include "marketer_ui/marketer_ui.h"
#include "admin_ui/admin_ui.h"

bool login_window = true;

char LoginUi::LoginWindow::c_password[128] = "";

LoginUi::LoginWindow::LoginWindow(mongocxx::database db, ImFont* default_font, ImFont* large_font)
    : login(db["LogInData"]), default_font(default_font), large_font(large_font)
{
}

int LoginUi::LoginWindow::create_login_window()
{
    this->passwords = get_users_data(this->login);
    this->s_password = "";

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
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f); // Rounded input/button corners
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f)); // Window padding
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3.0f, 3.0f)); // Reduced button/input padding

    // Center and size the window
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f)); // Center
    ImGui::SetNextWindowSize(ImVec2(450.0f, 200.0f), ImGuiCond_Always); // Wider and shorter window

    // Begin window
    ImGui::Begin("Login", NULL, this->window_flags | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    // Title with larger font
    ImGui::PushFont(this->large_font); // Use 24.0f font
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("POS System Login").x) * 0.5f); // Center title
    ImGui::Text("POS System Login");
    ImGui::PopFont();
    ImGui::Separator(); // Visual separator

    // Password label and input (centered)
    ImGui::Dummy(ImVec2(0.0f, 10.0f)); // Reduced vertical spacing
    float input_width = 200.0f;
    float button_width = 60.0f;
    float label_width = ImGui::CalcTextSize("Password: ").x;
    float total_width = label_width + input_width + button_width + ImGui::GetStyle().ItemSpacing.x * 2;
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - total_width) * 0.5f); // Center input group
    ImGui::Text("Password: ");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(input_width);
    static bool show_password = false;
    if (ImGui::InputTextWithHint("##password", "Enter password", this->c_password, IM_ARRAYSIZE(this->c_password), show_password ? 0 : ImGuiInputTextFlags_Password)) {
        this->login_error = false; // Clear error when user types
    }
    ImGui::SameLine();
    if (ImGui::Button(show_password ? "Hide" : "Show", ImVec2(button_width, 0.0f))) {
        show_password = !show_password; // Toggle password visibility
    }

    // Log In button (centered)
    ImGui::Dummy(ImVec2(0.0f, 15.0f)); // Reduced vertical spacing
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 150.0f) * 0.5f); // Center button
    if (ImGui::Button("Log In", ImVec2(150.0f, 30.0f))) {
        this->s_password = this->c_password;
        memset(this->c_password, 0, 128);

        if (this->s_password.compare(this->passwords[0].password) == 0) {
            admin_window = true;
            login_window = false;
        }
        else if (this->s_password.compare(this->passwords[1].password) == 0) {
            finance_window = true;
            login_window = false;
        }
        else if (this->s_password.compare(this->passwords[2].password) == 0) {
            marketer_window = true;
            login_window = false;
        }
        else if (this->s_password.compare(this->passwords[3].password) == 0) {
            regular_user_window = true;
            login_window = false;
        }
        else {
            this->login_error = true;
        }
    }

    ImGui::End();

    // Render error popup after main window to ensure it's on top
    if (this->login_error) {
        ImGui::OpenPopup("Login Error");
        this->login_error_message(this->login_error);
    }

    // Pop styles
    ImGui::PopStyleColor(8); // Pop all colors
    ImGui::PopStyleVar(4); // Pop all style vars

    return 1;
}

int LoginUi::LoginWindow::login_error_message(bool& error_flag)
{
    // Center the popup
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    // Apply error popup styling
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(50, 50, 50, 255)); // Dark background
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(255, 80, 80, 255)); // Red border
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 200, 200, 255)); // Light red text
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 150, 80, 255)); // Green button
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 180, 100, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0, 120, 60, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f); // Rounded corners
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f)); // Padding
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f); // Thicker border
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3.0f, 3.0f)); // Reduced button padding

    bool popup_open = true;
    if (ImGui::BeginPopupModal("Login Error", &popup_open, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::PushFont(this->large_font); // Use 24.0f font
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Invalid Password!").x) * 0.5f); // Center title
        ImGui::Text("Invalid Password!");
        ImGui::PopFont();
        ImGui::Separator();
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Please check your password and try again.").x) * 0.5f); // Center text
        ImGui::Text("Please check your password and try again.");

        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // Spacing
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 100.0f) * 0.5f); // Center button
        if (ImGui::Button("OK", ImVec2(100.0f, 30.0f))) {
            error_flag = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    else {
        // Popup was closed (e.g., via "X" button)
        error_flag = false; // Reset error flag to prevent reopening
    }

    // Pop styles
    ImGui::PopStyleColor(6);
    ImGui::PopStyleVar(4);

    return 1;
}