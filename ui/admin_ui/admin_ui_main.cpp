#include "admin_ui.h"
#include "imgui_internal.h"
#include <algorithm>
#include <agents.h>
#include <marketer_ui/marketer_ui.h>
#include "regular_ui/regular_user_ui.h"
#include "login_ui/login_ui.h"
#include "change_password.h"
#include "change_values.h" 

bool admin_window = false;
bool showing_admin_extensions = false;
bool log_out = false;


AdminUi::ShowAdminWindow::ShowAdminWindow(AddorRemoveAgentsWindow& add_or_remove_agents, 
                                         PassWord& change_password, 
                                         DBManagementWindow& db_management,
                                         mongocxx::database db)
{
    this->add_or_remove_agents = &add_or_remove_agents;
    this->change_password = &change_password;
    this->db_management = &db_management;
    this->products = db["Products"];
}

int AdminUi::ShowAdminWindow::create_show_admin_window()
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
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(350.0f, 450.0f), ImGuiCond_Always); // Adjusted size for all buttons

    // Begin window
    ImGui::Begin("Admin Dashboard", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    // Title
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]); // Assuming large font at index 1
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Admin Dashboard").x) * 0.5f);
    ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.4f, 1.0f), "Admin Dashboard");
    ImGui::PopFont();
    ImGui::Separator();

    // Button layout (centered, evenly spaced)
    const float button_width = 150.0f;
    const float button_height = 30.0f;
    const float spacing = 10.0f;

    if (!showing_admin_extensions)
    {
        // // Agents button (commented out as in original)
        // ImGui::Dummy(ImVec2(0.0f, spacing));
        // ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
        // if (ImGui::Button("Agents", ImVec2(button_width, button_height)))
        //     this->add_or_remove_agents->show_window = true;

        // if (this->add_or_remove_agents->show_window)
        //     this->add_or_remove_agents->create_add_or_remove_agents_window();
    }

    if (finance_window || marketer_window || regular_user_window)
    {
        showing_admin_extensions = true;
    }

    if (!finance_window && !marketer_window && !regular_user_window)
    {
        // Finance button
        ImGui::Dummy(ImVec2(0.0f, spacing));
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
        if (ImGui::Button("Finance", ImVec2(button_width, button_height)))
            finance_window = true;

        // Reports button
        ImGui::Dummy(ImVec2(0.0f, spacing));
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
        if (ImGui::Button("Reports", ImVec2(button_width, button_height)))
            marketer_window = true;

        // Regular button
        ImGui::Dummy(ImVec2(0.0f, spacing));
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
        if (ImGui::Button("Regular", ImVec2(button_width, button_height)))
            regular_user_window = true;

        ImGui::Separator();
        
        // Database Management button
        ImGui::Dummy(ImVec2(0.0f, spacing));
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width - 5.0f) * 0.5f);
        if (ImGui::Button("Backup The Database", ImVec2(button_width + 10.0f, button_height)))
            this->db_management->show_window = true;

        if (this->db_management->show_window)
            this->db_management->create_db_management_window();

        // Reset Sales Data button
        ImGui::Dummy(ImVec2(0.0f, spacing));
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
        if (ImGui::Button("Reset Sales Data", ImVec2(button_width, button_height)))
        {
            ImGui::OpenPopup("Confirm Reset Sales Data");
        }

        // Confirmation popup for resetting sales data
        static char reset_input[32] = "";
        bool reset_confirmed = false;
        if (ImGui::BeginPopupModal("Confirm Reset Sales Data", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Are you sure you want to reset all sales data?");
            ImGui::Text("This action cannot be undone.");
            ImGui::Dummy(ImVec2(0.0f, spacing));
            ImGui::Text("Type 'reset' in lowercase to confirm:");

            ImGui::PushItemWidth(150.0f);
            ImGui::InputText("##ResetInput", reset_input, IM_ARRAYSIZE(reset_input));
            ImGui::PopItemWidth();

            ImGui::Dummy(ImVec2(0.0f, spacing));

            // Confirm button
            ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x - 220.0f) * 0.5f);
            if (ImGui::Button("Confirm", ImVec2(100.0f, button_height)))
            {
                if (strcmp(reset_input, "reset") == 0)
                {
                    reset_confirmed = true;
                    ImGui::CloseCurrentPopup();
                }
                else
                {
                    ImGui::OpenPopup("Invalid Input Error");
                }
            }

            ImGui::SameLine();

            // Cancel button
            if (ImGui::Button("Cancel", ImVec2(100.0f, button_height)))
            {
                reset_input[0] = '\0'; // Clear input
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        // Error popup for invalid input
        if (ImGui::BeginPopupModal("Invalid Input Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Invalid input!");
            ImGui::Text("Please type exactly 'reset' in lowercase.");
            if (strlen(reset_input) > 5)
            {
                ImGui::Text("Input is too long.");
            }

            ImGui::Dummy(ImVec2(0.0f, spacing));

            ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x - 100.0f) * 0.5f);
            if (ImGui::Button("OK", ImVec2(100.0f, button_height)))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        // Execute reset if confirmed
        if (reset_confirmed)
        {
            if (reset_sales_data(products) == 1)
            {
                ImGui::OpenPopup("Reset Success");
            }
            reset_confirmed = false;
            reset_input[0] = '\0'; // Clear input after reset
        }

        // Success popup
        if (ImGui::BeginPopupModal("Reset Success", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Sales data reset successfully!");

            ImGui::Dummy(ImVec2(0.0f, spacing));

            ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x - 100.0f) * 0.5f);
            if (ImGui::Button("OK", ImVec2(100.0f, button_height)))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::Separator();

        // Change Password button (Pink)
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(220, 75, 150, 255)); // Pink
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 105, 180, 255));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(200, 130, 0, 255));
        ImGui::Dummy(ImVec2(0.0f, spacing));
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
        if (ImGui::Button("Change Password", ImVec2(button_width, button_height)))
            this->change_password->show_window = true;

        if (this->change_password->show_window)
            this->change_password->create_password_window();
        
        ImGui::PopStyleColor(3);

        // Log Out button (Red)
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(220, 50, 50, 255)); // Red
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 80, 80, 255));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(180, 30, 30, 255));
        ImGui::Dummy(ImVec2(0.0f, spacing));
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
        if (ImGui::Button("Log Out", ImVec2(button_width, button_height)))
        {
            login_window = true;
            admin_window = false;
            showing_admin_extensions = false;
        }
        ImGui::PopStyleColor(3);
    }

    if (showing_admin_extensions)
    {
        // Back button
        ImGui::Dummy(ImVec2(0.0f, spacing));
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
        if (ImGui::Button("Back To Admin", ImVec2(button_width, button_height)))
        {
            showing_admin_extensions = false;
            finance_window = false;
            marketer_window = false;
            regular_user_window = false;
        }
    }

    ImGui::End();

    // Pop styles
    ImGui::PopStyleColor(8);
    ImGui::PopStyleVar(4);

    return 1;
}