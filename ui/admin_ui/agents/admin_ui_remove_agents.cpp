#include "admin_ui.h"
#include "insert_components.h"
#include "retrieve_data.h"
#include "delete_components.h"
#include "imgui_internal.h"
#include "agents.h"
#include <regex>

int AdminUi::RemoveAgentWindow::remove_current_filter_type_index = 0;

AdminUi::RemoveAgentWindow::RemoveAgentWindow(mongocxx::database db)
{
    this->agents = db["Agents"];
    this->agent_sales = db["AgentSales"];

    table_flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Reorderable;
    TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

    // Use RUserUi::AgentsWindow::get_agent_details
    this->agent_details = get_agent_details(this->agents);

    this->ids = this->agent_details[0];
    this->names = this->agent_details[1];
    this->phones = this->agent_details[2];
    this->locations = this->agent_details[3];

    this->filtered_ids = this->ids;
    this->filtered_names = this->names;
    this->filtered_phones = this->phones;
    this->filtered_locations = this->locations;

    this->p_admin_filter_types = std::make_unique<char[]>(this->admin_filter_types.size() + 1);
    std::fill(this->p_admin_filter_types.get(), this->p_admin_filter_types.get() + this->admin_filter_types.size(), 0);
    std::strncpy(this->p_admin_filter_types.get(), this->admin_filter_types.c_str(), this->admin_filter_types.size() + 1);

    this->prepare_filter_types_pointer(this->admin_filter_types, '_', this->p_admin_filter_types);
}

int AdminUi::RemoveAgentWindow::create_remove_agent_window()
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(60, 60, 60, 255));

    ImGui::SetNextWindowPos(ImVec2(100, 200), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(520, 460), ImGuiCond_Once);

    ImGui::Begin("Remove Agents", &this->show_window, this->window_flags | ImGuiWindowFlags_AlwaysAutoResize);

    // Use RUserUi::AgentsWindow::get_agent_details
    this->agent_details = get_agent_details(this->agents);

    this->ids = this->agent_details[0];
    this->names = this->agent_details[1];
    this->phones = this->agent_details[2];
    this->locations = this->agent_details[3];

    if (this->names.size() != 0)
    {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* parent_window = g.CurrentWindow;
        ImRect mrect = parent_window->Rect();

        float new_xcenter = mrect.GetCenter().x;

        ImGui::Dummy(ImVec2(0.0, 10.0));
        ImGui::NewLine();
        ImGui::SameLine((new_xcenter - (350 / 2)) - mrect.Min.x, 0.0);

        create_search_filter_combo(this->p_admin_filter_types, "Filter Criteria", this->remove_current_filter_type_index, ImVec2(350, 30));

        if (this->remove_current_filter_type_index != 0)
        {
            this->create_admin_agents_filter();
        }
        else
        {
            this->filtered_ids = this->ids;
            this->filtered_names = this->names;
            this->filtered_phones = this->phones;
            this->filtered_locations = this->locations;
        }

        ImGui::Dummy(ImVec2(0.0, 10.0));
        ImGui::NewLine();
        ImGui::SameLine(200.0, 0.0);
        ImGui::TextColored(ImVec4(0.543f, 0.931f, 0.886f, 0.784f), "Agents Details");

        this->draw_admin_agents_table(this->TEXT_BASE_HEIGHT);

        if (this->confirm == this->ACCEPTED)
        {
            for (int i = 0; i < this->agents_count; i++)
            {
                this->agent_name = this->names[this->checked_agents[i]];
                this->agent_id = this->ids[this->checked_agents[i]];

                // Use RUserUi::AgentsWindow::remove_agent
                remove_agent(this->agents, this->agent_sales, this->agent_name, std::stoi(this->agent_id));
            }

            this->filtered_ids = this->ids;
            this->filtered_names = this->names;
            this->filtered_phones = this->phones;
            this->filtered_locations = this->locations;

            this->success = true;

            this->show_confirm_window = false;
            this->confirm = this->NONE;

            this->checked_agents.clear();
            remove_current_filter_type_index = 0;
        }
        else if (this->confirm == this->REJECTED)
        {
            this->show_confirm_window = false;
            this->confirm = this->NONE;
        }

        if (this->show_confirm_window == true)
            // Use RUserUi::AddQuantityWindow::admin_confirm_message
            RUserUi::AddQuantityWindow::admin_confirm_message(this->confirm);
        else if (this->input_error)
            // Use RUserUi::AddQuantityWindow::admin_error_message
            RUserUi::AddQuantityWindow::admin_error_message(this->input_error);

        this->agents_count = (int)this->checked_agents.size();
        ImGui::Dummy(ImVec2(0.0, 10.0));
        ImGui::NewLine();
        ImGui::SameLine((new_xcenter - (115 / 2)) - mrect.Min.x, 0.0);
        if (ImGui::Button("Remove", ImVec2(115.0, 20.0)))
        {
            if (this->agents_count == 0)
            {
                this->input_error = true;
            }
            else
            {
                this->show_confirm_window = true;
            }
        }
    }
    else
    {
        ImGui::Dummy(ImVec2(0.0, 10.0));
        ImGui::NewLine();
        ImGui::Text(std::string("No Agents Found").c_str());
        ImGui::Dummy(ImVec2(0.0, 10.0));
    }

    ImGui::PopStyleColor();
    ImGui::End();

    return 1;
}

int AdminUi::RemoveAgentWindow::create_admin_agents_filter()
{
    ImGui::Dummy(ImVec2(0.0, 10.0));
    ImGui::NewLine();
    ImGui::SameLine(150.0, 0.0);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(50, 50, 50, 255));
    ImGui::BeginChild("##", ImVec2(200, 35), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    static ImGuiTextFilter filter;
    filter.Draw("Search");

    size_t count = this->names.size();

    this->filtered_ids.clear();
    this->filtered_names.clear();
    this->filtered_phones.clear();
    this->filtered_locations.clear();

    for (int item_num = 0; item_num < count; item_num++)
    {
        if (this->remove_current_filter_type_index == 1)
        {
            if (filter.PassFilter(this->ids[item_num].c_str()))
            {
                this->filtered_ids.push_back(this->ids[item_num]);
                this->filtered_names.push_back(this->names[item_num]);
                this->filtered_phones.push_back(this->phones[item_num]);
                this->filtered_locations.push_back(this->locations[item_num]);
            }
        }
        else if (this->remove_current_filter_type_index == 2)
        {
            if (filter.PassFilter(this->names[item_num].c_str()))
            {
                this->filtered_ids.push_back(this->ids[item_num]);
                this->filtered_names.push_back(this->names[item_num]);
                this->filtered_phones.push_back(this->phones[item_num]);
                this->filtered_locations.push_back(this->locations[item_num]);
            }
        }
        else if (this->remove_current_filter_type_index == 3)
        {
            if (filter.PassFilter(this->phones[item_num].c_str()))
            {
                this->filtered_ids.push_back(this->ids[item_num]);
                this->filtered_names.push_back(this->names[item_num]);
                this->filtered_phones.push_back(this->phones[item_num]);
                this->filtered_locations.push_back(this->locations[item_num]);
            }
        }
        else if (this->remove_current_filter_type_index == 4)
        {
            if (filter.PassFilter(this->locations[item_num].c_str()))
            {
                this->filtered_ids.push_back(this->ids[item_num]);
                this->filtered_names.push_back(this->names[item_num]);
                this->filtered_phones.push_back(this->phones[item_num]);
                this->filtered_locations.push_back(this->locations[item_num]);
            }
        }
    }

    ImGui::PopStyleColor();
    ImGui::EndChild();

    return 1;
}

int AdminUi::RemoveAgentWindow::draw_admin_agents_table(const float& m_TEXT_BASE_HEIGHT)
{
    bool* checked_statuses = new bool[this->filtered_names.size()];
    memset(checked_statuses, 0, this->filtered_names.size());
    for (int index : this->checked_agents)
        checked_statuses[index] = true;

    ImVec2 outer_size = ImVec2(0.0f, m_TEXT_BASE_HEIGHT * this->filtered_names.size() * 10);
    if (ImGui::BeginTable("table_scrolly", 5, this->table_flags, outer_size))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Phone", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Location", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Check", ImGuiTableColumnFlags_None);

        ImGui::TableHeadersRow();

        std::vector<int>::iterator position;
        bool clicked;
        bool present;

        ImGuiListClipper clipper;
        clipper.Begin((int)this->filtered_names.size());
        while (clipper.Step())
        {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 5; column++)
                {
                    ImU32 cell_bg_color = ImGui::GetColorU32(this->colum_colors[column]);
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cell_bg_color);

                    ImGui::TableSetColumnIndex(column);
                    if (column == 0)
                    {
                        ImGui::Text(this->filtered_ids[row].c_str());
                    }
                    else if (column == 1)
                    {
                        ImGui::Text(this->filtered_names[row].c_str());
                    }
                    else if (column == 2)
                    {
                        ImGui::Text(this->filtered_phones[row].c_str());
                    }
                    else if (column == 3)
                    {
                        ImGui::Text(this->filtered_locations[row].c_str());
                    }
                    else
                    {
                        position = std::find(this->checked_agents.begin(), this->checked_agents.end(), row);
                        present = (position != this->checked_agents.end()) ? true : false;

                        ImGui::PushID(row);
                        clicked = ImGui::Checkbox("##", &checked_statuses[row]);
                        ImGui::PopID();

                        if (clicked && present == true && !ImGui::IsItemActive())
                        {
                            this->checked_agents.erase(position);
                        }
                        else if (clicked && present == false && !ImGui::IsItemActive())
                        {
                            this->checked_agents.push_back(row);
                        }
                    }
                }
            }
        }
        ImGui::EndTable();
    }

    delete[] checked_statuses;
    checked_statuses = NULL;

    return 1;
}