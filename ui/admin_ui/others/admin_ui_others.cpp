#include "admin_ui.h"
#include "insert_components.h"
#include "delete_components.h"
#include "retrieve_data.h"
#include "imgui_internal.h"

AdminUi::AddOthersWindow::AddOthersWindow(mongocxx::database db)
{
    this->products_collection = db["Products"];
    // Initialize p_others
    this->p_others = std::make_unique<char[]>(this->others.size() + 1);
    std::fill(this->p_others.get(), this->p_others.get() + this->others.size(), 0);
    std::strncpy(this->p_others.get(), this->others.c_str(), this->others.size() + 1);
    // Prepare items for combo box
    this->prepare_items_pointer(this->others, '_', this->p_others);
}

int AdminUi::AddOthersWindow::others_current_item = 0;
char AdminUi::AddOthersWindow::other_name[128] = "";

int AdminUi::AddOthersWindow::create_add_others_window()
{
    // Professional styling
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.92f, 0.93f, 0.94f, 1.0f)); // Soft off-white text
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.15f, 0.16f, 0.18f, 1.0f)); // Deep slate gray background
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.65f, 0.60f, 1.0f)); // Vibrant teal buttons
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.30f, 0.75f, 0.70f, 1.0f)); // Lighter teal on hover
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.55f, 0.50f, 1.0f)); // Darker teal when clicked
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(50, 50, 55, 255)); // Dark gray input field background
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(80, 180, 180, 150)); // Soft cyan border
    ImGui::PushStyleColor(ImGuiCol_Separator, IM_COL32(80, 80, 90, 255)); // Dim gray separator
    ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.18f, 0.19f, 0.21f, 1.0f)); // Slightly lighter slate for popups
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f); // Softer window corners
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f); // Smoother input/button corners
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 8.0f)); // Spacious padding
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 10.0f)); // Balanced spacing

    // Center the window with +20.0f vertical offset
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(ImVec2(center.x, center.y + 20.0f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(320, 360), ImGuiCond_Always);

    // Begin window (movable, non-collapsible, non-resizable)
    ImGui::Begin("Add Others", &this->show_window, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    // Title (centered)
    const char* title = "Add Others";
    float title_width = ImGui::CalcTextSize(title).x;
    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - title_width) * 0.5f);
    ImGui::Text("%s", title);
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 15.0f));

    // Fixed label width for alignment
    const float label_width = 100.0f;
    const float input_width = 160.0f;

    // Entity type dropdown
    ImGui::Text("Entity Type:");
    ImGui::SameLine(label_width);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3.0f);
    ImGui::SetNextItemWidth(input_width);
    create_combo(this->p_others, "Others", this->others_current_item, ImVec2(input_width, 0));

    ImGui::Dummy(ImVec2(0.0f, 15.0f));

    // Name input
    ImGui::Text("Name:");
    ImGui::SameLine(label_width);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3.0f);
    ImGui::SetNextItemWidth(input_width);
    ImGui::InputTextWithHint("##OtherName", "name", this->other_name, IM_ARRAYSIZE(this->other_name));

    // Confirmation handling
    if (this->confirm == confirm_state::ACCEPTED)
    {
        this->s_other_name = this->other_name;
        if (this->v_others[others_current_item - 1] == "Season")
        {
            insert_season(this->products_collection, this->s_other_name);
        }
        else if (this->v_others[others_current_item - 1] == "Age Group")
        {
            insert_age_group(this->products_collection, this->s_other_name);
        }
        else if (this->v_others[others_current_item - 1] == "Location")
        {
            insert_location(this->products_collection, this->s_other_name);
        }

        this->success = true;
        this->show_confirm_window = false;
        this->confirm = confirm_state::NONE;
        this->others_current_item = 0;
        std::fill_n(this->other_name, IM_ARRAYSIZE(this->other_name), 0);
    }
    else if (this->confirm == confirm_state::REJECTED)
    {
        this->show_confirm_window = false;
        this->confirm = confirm_state::NONE;
    }

    // Error or confirmation popup
    if (this->show_confirm_window)
        this->admin_confirm_message(this->confirm);
    else if (this->input_error)
        this->admin_error_message(this->input_error);

    // Add button (centered)
    ImGui::Dummy(ImVec2(0.0f, 20.0f));
    float button_width = 120.0f;
    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - button_width) * 0.5f);
    if (ImGui::Button("Add", ImVec2(button_width, 0)))
    {
        this->s_other_name = this->other_name;
        bool found_unwanted_char = check_unwanted_characters(this->s_other_name);

        if (others_current_item == 0 || this->s_other_name.empty() || found_unwanted_char)
        {
            this->input_error = true;
        }
        else
        {
            std::vector<std::string> current_others;
            bool exists = false;
            if (this->v_others[others_current_item - 1] == "Season")
            {
                current_others = get_seasons_array(this->products_collection);
                exists = check_for_duplicate(current_others, this->s_other_name);
            }
            else if (this->v_others[others_current_item - 1] == "Age Group")
            {
                current_others = get_age_groups_array(this->products_collection);
                exists = check_for_duplicate(current_others, this->s_other_name);
            }
            else if (this->v_others[others_current_item - 1] == "Location")
            {
                current_others = get_locations_array(this->products_collection);
                exists = check_for_duplicate(current_others, this->s_other_name);
            }

            if (!exists)
                this->show_confirm_window = true;
            else
                this->input_error = true;
        }
    }

    ImGui::PopStyleColor(9); // Text, WindowBg, Button, ButtonHovered, ButtonActive, FrameBg, Border, Separator,  PopupBg
    ImGui::PopStyleVar(5); // WindowRounding, FrameRounding, FrameBorderSize, FramePadding, ItemSpacing
    ImGui::End();
    return 1;
}

AdminUi::RemoveOthersWindow::RemoveOthersWindow(mongocxx::database db)
{
    this->products_collection = db["Products"];
    // Initialize p_others
    this->p_others = std::make_unique<char[]>(this->others.size() + 1);
    std::fill(this->p_others.get(), this->p_others.get() + this->others.size(), 0);
    std::strncpy(this->p_others.get(), this->others.c_str(), this->others.size() + 1);
    this->prepare_items_pointer(this->others, '_', this->p_others);
    // Initialize p_selected_others
    this->p_selected_others = std::make_unique<char[]>(this->selected_others.size() + 1);
    std::fill(this->p_selected_others.get(), this->p_selected_others.get() + this->selected_others.size(), 0);
    std::strncpy(this->p_selected_others.get(), this->selected_others.c_str(), this->selected_others.size() + 1);
    this->prepare_items_pointer(this->selected_others, '_', this->p_selected_others);
}

int AdminUi::RemoveOthersWindow::r_others_current_item = 0;
int AdminUi::RemoveOthersWindow::r_other_selected_current_item = 0;

int AdminUi::RemoveOthersWindow::create_remove_others_window()
{
    // Professional styling
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.92f, 0.93f, 0.94f, 1.0f)); // Soft off-white text
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.15f, 0.16f, 0.18f, 1.0f)); // Deep slate gray background
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.65f, 0.60f, 1.0f)); // Vibrant teal buttons
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.30f, 0.75f, 0.70f, 1.0f)); // Lighter teal on hover
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.55f, 0.50f, 1.0f)); // Darker teal when clicked
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(50, 50, 55, 255)); // Dark gray input field background
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(80, 180, 180, 150)); // Soft cyan border
    ImGui::PushStyleColor(ImGuiCol_Separator, IM_COL32(80, 80, 90, 255)); // Dim gray separator
    ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.18f, 0.19f, 0.21f, 1.0f)); // Slightly lighter slate for popups
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f); // Softer window corners
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f); // Smoother input/button corners
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 8.0f)); // Spacious padding
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 10.0f)); // Balanced spacing

    // Center the window with +20.0f vertical offset
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(ImVec2(center.x, center.y + 20.0f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(320, 360), ImGuiCond_Always);

    // Begin window (movable, non-collapsible, non-resizable)
    ImGui::Begin("Remove Others", &this->show_window, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    // Title (centered)
    const char* title = "Remove Others";
    float title_width = ImGui::CalcTextSize(title).x;
    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - title_width) * 0.5f);
    ImGui::Text("%s", title);
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 15.0f));

    // Fixed label width for alignment
    const float label_width = 100.0f;
    const float input_width = 160.0f;

    // Entity type dropdown
    ImGui::Text("Entity Type:");
    ImGui::SameLine(label_width);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3.0f);
    ImGui::SetNextItemWidth(input_width);
    create_combo(this->p_others, "Others", this->r_others_current_item, ImVec2(input_width, 0));

    // Populate selected others based on entity type
    if (r_others_current_item != 0)
    {
        if (r_others_current_item == 1)
            this->v_selected_others = get_seasons_array(this->products_collection);
        else if (r_others_current_item == 2)
            this->v_selected_others = get_age_groups_array(this->products_collection);
        else if (r_others_current_item == 3)
            this->v_selected_others = get_locations_array(this->products_collection);

        mset_others(this->v_selected_others);

        this->p_selected_others = std::make_unique<char[]>(this->selected_others.size() + 1);
        std::fill(this->p_selected_others.get(), this->p_selected_others.get() + this->selected_others.size(), 0);
        std::strncpy(this->p_selected_others.get(), this->selected_others.c_str(), this->selected_others.size() + 1);
        this->prepare_items_pointer(this->selected_others, '_', this->p_selected_others);
    }

    ImGui::Dummy(ImVec2(0.0f, 15.0f));

    // Selected other dropdown
    ImGui::Text("Entity Name:");
    ImGui::SameLine(label_width);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3.0f);
    ImGui::SetNextItemWidth(input_width);
    create_combo(this->p_selected_others, "Other", this->r_other_selected_current_item, ImVec2(input_width, 0));

    // Confirmation handling
    if (this->confirm == confirm_state::ACCEPTED)
    {
        if (r_others_current_item == 1)
            delete_season(this->products_collection, this->v_selected_others[this->r_other_selected_current_item - 1]);
        else if (r_others_current_item == 2)
            delete_age_group(this->products_collection, this->v_selected_others[this->r_other_selected_current_item - 1]);
        else if (r_others_current_item == 3)
            delete_location(this->products_collection, this->v_selected_others[this->r_other_selected_current_item - 1]);

        this->success = true;
        this->show_confirm_window = false;
        this->confirm = confirm_state::NONE;
        this->r_others_current_item = 0;
        this->r_other_selected_current_item = 0;
    }
    else if (this->confirm == confirm_state::REJECTED)
    {
        this->show_confirm_window = false;
        this->confirm = confirm_state::NONE;
    }

    // Error or confirmation popup
    if (this->show_confirm_window)
        this->admin_confirm_message(this->confirm);
    else if (this->input_error)
        this->admin_error_message(this->input_error);

    // Remove button (centered)
    ImGui::Dummy(ImVec2(0.0f, 20.0f));
    float button_width = 120.0f;
    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - button_width) * 0.5f);
    if (ImGui::Button("Remove", ImVec2(button_width, 0)))
    {
        if (r_others_current_item == 0 || r_other_selected_current_item == 0)
            this->input_error = true;
        else
            this->show_confirm_window = true;
    }

    ImGui::PopStyleColor(9); // Text, WindowBg, Button, ButtonHovered, ButtonActive, FrameBg, Border, Separator,  PopupBg
    ImGui::PopStyleVar(5); // WindowRounding, FrameRounding, FrameBorderSize, FramePadding, ItemSpacing
    ImGui::End();
    return 1;
}

int AdminUi::RemoveOthersWindow::mset_others(std::vector<std::string> v_s_others)
{
    this->selected_others = "Select_";
    for (const std::string& selected_other : v_s_others)
        this->selected_others += selected_other + "_";
    return 1;
}

AdminUi::AddorRemoveOthersWindow::AddorRemoveOthersWindow(AddOthersWindow& add_other, RemoveOthersWindow& remove_other)
{
    this->add_other = &add_other;
    this->remove_other = &remove_other;
}

int AdminUi::AddorRemoveOthersWindow::create_add_or_remove_others_window()
{
    // Professional styling
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.92f, 0.93f, 0.94f, 1.0f)); // Soft off-white text
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.15f, 0.16f, 0.18f, 1.0f)); // Deep slate gray background
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.65f, 0.60f, 1.0f)); // Vibrant teal buttons
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.30f, 0.75f, 0.70f, 1.0f)); // Lighter teal on hover
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.55f, 0.50f, 1.0f)); // Darker teal when clicked
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(50, 50, 55, 255)); // Dark gray input field background
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(80, 180, 180, 150)); // Soft cyan border
    ImGui::PushStyleColor(ImGuiCol_Separator, IM_COL32(80, 80, 90, 255)); // Dim gray separator
    ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.18f, 0.19f, 0.21f, 1.0f)); // Slightly lighter slate for popups
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f); // Softer window corners
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f); // Smoother input/button corners
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 8.0f)); // Spacious padding
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 10.0f)); // Balanced spacing

    // Center the window with +20.0f vertical offset
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(ImVec2(center.x, center.y + 20.0f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(320, 250), ImGuiCond_Always);

    // Begin window (movable, non-collapsible, non-resizable)
    ImGui::Begin("Manage Others", &this->show_window, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    // Title (centered)
    const char* title = "Manage Others";
    float title_width = ImGui::CalcTextSize(title).x;
    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - title_width) * 0.5f);
    ImGui::Text("%s", title);
    ImGui::Separator();
   
    // Center buttons
    float button_width = 140.0f;
    float window_width = ImGui::GetContentRegionAvail().x;

    ImGui::SetCursorPosX((window_width - button_width) * 0.5f);
    if (ImGui::Button("Add Other", ImVec2(button_width, 0)))
        this->add_other->show_window = true;

    ImGui::Dummy(ImVec2(0.0f, 15.0f));
    ImGui::SetCursorPosX((window_width - button_width) * 0.5f);
    if (ImGui::Button("Remove Other", ImVec2(button_width, 0)))
        this->remove_other->show_window = true;

    // Nested window calls
    if (this->add_other->show_window)
        this->add_other->create_add_others_window();

    if (this->add_other->success)
    {
        this->add_other->notify_sale_status(this->add_other->success);
        this->add_other->show_window = false;
        this->add_other->success = false;
    }

    if (this->remove_other->show_window)
        this->remove_other->create_remove_others_window();

    if (this->remove_other->success)
    {
        this->remove_other->notify_sale_status(this->remove_other->success);
        this->remove_other->show_window = false;
        this->remove_other->success = false;
    }

    ImGui::PopStyleColor(9); // Text, WindowBg, Button, ButtonHovered, ButtonActive, FrameBg, Border, Separator, PopupBg
    ImGui::PopStyleVar(5); // WindowRounding, FrameRounding, FrameBorderSize, FramePadding, ItemSpacing
    ImGui::End();
    return 1;
}