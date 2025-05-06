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
    // Set dark theme background consistent with other windows
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(60, 60, 60, 255));

    // Center window with responsive size
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(300, 250), ImGuiCond_Once);

    ImGui::Begin("Add Others", &this->show_window, this->window_flags | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    // Entity type dropdown
    ImGui::SetCursorPosX(40.0f);
    ImVec2 combo_size = ImVec2(230, 50);
    create_combo(this->p_others, "Others", this->others_current_item, combo_size);

    // Name input
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::SetCursorPosX(30.0f);
    ImGui::Text("Name:");
    ImGui::SameLine(70.0f);
    ImGui::SetNextItemWidth(150);
    ImGui::InputTextWithHint("###other_name", "name", this->other_name, IM_ARRAYSIZE(this->other_name));

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

    // Add button
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::SetCursorPosX(70.0f);
    if (ImGui::Button("Add", ImVec2(120, 0)))
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

    ImGui::PopStyleColor();
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
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(60, 60, 60, 255));

    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(300, 250), ImGuiCond_Once);

    ImGui::Begin("Remove Others", &this->show_window, this->window_flags | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    // Entity type dropdown
    ImGui::SetCursorPosX(40.0f);
    ImVec2 combo_size = ImVec2(230, 50);
    create_combo(this->p_others, "Others", this->r_others_current_item, combo_size);

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

    // Selected other dropdown
    ImGui::SetCursorPosX(40.0f);
    create_combo(this->p_selected_others, "Other", this->r_other_selected_current_item, combo_size);

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

    // Remove button
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::SetCursorPosX(70.0f);
    if (ImGui::Button("Remove", ImVec2(120, 0)))
    {
        if (r_others_current_item == 0 || r_other_selected_current_item == 0)
            this->input_error = true;
        else
            this->show_confirm_window = true;
    }

    ImGui::PopStyleColor();
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
    // Consistent styling with other windows
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(60, 60, 60, 255));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.95f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(50, 100, 255, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(70, 120, 255, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(90, 140, 255, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 6.0f));

    // Center window
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(220, 250), ImGuiCond_Once);

    ImGui::Begin("Add Or Remove Other", &this->show_window, this->window_flags | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    // Title (centered)
    float title_width = ImGui::CalcTextSize("Manage Others").x;
    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - title_width) * 0.5f);
    ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1.0f), "Manage Others");
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 15.0f));

    // Center buttons
    float button_width = 145.0f;
    float window_width = ImGui::GetContentRegionAvail().x;

    ImGui::SetCursorPosX((window_width - button_width) * 0.5f);
    if (ImGui::Button("Add Other", ImVec2(button_width, 20.0f)))
        this->add_other->show_window = true;

    ImGui::Dummy(ImVec2(0.0f, 15.0f));
    ImGui::SetCursorPosX((window_width - button_width) * 0.5f);
    if (ImGui::Button("Remove Other", ImVec2(button_width, 20.0f)))
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

    // Pop styles
    ImGui::PopStyleColor(5); // WindowBg, Text, Button, ButtonHovered, ButtonActive
    ImGui::PopStyleVar(3);   // WindowRounding, FrameRounding, FramePadding
    ImGui::End();

    return 1;
}

