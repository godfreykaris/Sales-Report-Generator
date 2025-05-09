#include "admin_ui.h"
#include "insert_components.h"
#include "retrieve_data.h"
#include "delete_components.h"
#include "imgui_internal.h"
#include "agents.h"
#include "finance_ui/finance_ui.h"
#include <regex>

AdminUi::AddAgentWindow::AddAgentWindow(mongocxx::database db)
{
    this->agents = db["Agents"];
    this->agent_sales = db["AgentSales"];
}

char AdminUi::AddAgentWindow::agent_name[128] = "";
char AdminUi::AddAgentWindow::agent_phone[13] = "";
char AdminUi::AddAgentWindow::agent_location[128] = "";

int AdminUi::AddAgentWindow::create_add_agent_window()
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(60, 60, 60, 255));

    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(300, 350), ImGuiCond_Once);

    ImGui::Begin("Agent Details", &this->show_window, this->window_flags | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1.0f), "Add New Agent");
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    ImGui::Text("Agent Name:");
    ImGui::SameLine(115.0f, 0.0f);
    ImGui::SetNextItemWidth(160);
    ImGui::PushID("Agent Name");
    ImGui::InputTextWithHint("###", "name", this->agent_name, IM_ARRAYSIZE(this->agent_name));
    ImGui::PopID();

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::Text("Phone:");
    ImGui::SameLine(115.0f, 0.0f);
    ImGui::SetNextItemWidth(160);
    ImGui::PushID("Phone");
    ImGui::InputTextWithHint("###", "phone", this->agent_phone, IM_ARRAYSIZE(this->agent_phone));
    ImGui::PopID();

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::Text("Agent Location:");
    ImGui::SameLine(115.0f, 0.0f);
    ImGui::SetNextItemWidth(160);
    ImGui::PushID("Agent Location");
    ImGui::InputTextWithHint("###", "location", this->agent_location, IM_ARRAYSIZE(this->agent_location));
    ImGui::PopID();

    if (this->confirm == this->ACCEPTED)
    {
        AGENT agent;
        agent.agent_name = this->s_agent_name;
        agent.agent_location = this->s_agent_location;
        agent.phone = this->s_agent_phone;

        // Use RUserUi::AgentsWindow::add_new_agent
        this->s_agent_id = add_new_agent(this->agents, this->agent_sales, agent);

        this->success = true;
        this->show_id = true;
        this->show_confirm_window = false;
        this->confirm = this->NONE;
    }
    else if (this->confirm == this->REJECTED)
    {
        this->show_confirm_window = false;
        this->confirm = this->NONE;
    }

    if (this->show_confirm_window)
        // Use RUserUi::AddQuantityWindow::admin_confirm_message
        RUserUi::AddQuantityWindow::admin_confirm_message(this->confirm);
    else if (this->input_error)
        // Use RUserUi::AddQuantityWindow::admin_error_message
        RUserUi::AddQuantityWindow::admin_error_message(this->input_error);

    if (this->show_id)
        ImGui::TextColored(ImVec4(0.543f, 0.931f, 0.886f, 0.784f), std::string("Agent ID: " + this->s_agent_id).c_str());

    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(50, 100, 255, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(70, 120, 255, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(90, 140, 255, 255));

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 120.0f) * 0.5f);
    if (ImGui::Button("Add", ImVec2(120, 0)))
    {
        this->s_agent_name = this->agent_name;
        this->s_agent_location = this->agent_location;
        this->s_agent_phone = this->agent_phone;

        size_t phone_digits = this->s_agent_phone.size();
        bool is_number = std::regex_match(this->s_agent_phone, std::regex("[0-9]+"));
        // Use RUserUi::AddQuantityWindow::check_unwanted_characters
        bool found_unwanted_char = this->check_unwanted_characters(this->s_agent_name) ||
                                  this->check_unwanted_characters(this->s_agent_location);

        if (this->s_agent_name.empty() || this->s_agent_location.empty() || phone_digits < 10 || phone_digits > 13 || !is_number || found_unwanted_char)
        {
            this->input_error = true;
        }
        else
        {
            this->show_confirm_window = true;
        }
    }

    ImGui::PopStyleColor(3);
    ImGui::PopStyleColor();
    ImGui::End();
    return 1;
}

AdminUi::AddorRemoveAgentsWindow::AddorRemoveAgentsWindow(AddAgentWindow& add_agent, RemoveAgentWindow& remove_agent)
{
    this->add_agent = &add_agent;
    this->remove_agent = &remove_agent;
}

int AdminUi::AddorRemoveAgentsWindow::create_add_or_remove_agents_window()
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(60, 60, 60, 255));

    ImGui::SetNextWindowPos(ImVec2(350, 150), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(220, 250), ImGuiCond_Once);

    ImGui::Begin("Add Or Remove Agent", &this->show_window, this->window_flags);

    if (this->add_agent->show_window)
        this->add_agent->create_add_agent_window();

  /*  if (this->add_agent->success == true)
        FUserUi::Window::notify_sale_status(this->add_agent->success);*/

    ImGui::Dummy(ImVec2(0.0, 10.0));
    ImGui::NewLine();
    ImGui::SameLine(50.0, 0.0);
    if (ImGui::Button("Add Agent", ImVec2(145.0, 20.0)))
        this->add_agent->show_window = true;

    if (this->remove_agent->show_window)
        this->remove_agent->create_remove_agent_window();

    //if (this->remove_agent->success == true)
    //    // Use RUserUi::SellWindow::notify_sale_status
    //    notify_sale_status(this->remove_agent->success);

    ImGui::Dummy(ImVec2(0.0, 10.0));
    ImGui::NewLine();
    ImGui::SameLine(50.0, 0.0);
    if (ImGui::Button("Remove Agent", ImVec2(145.0, 20.0)))
        this->remove_agent->show_window = true;

    ImGui::PopStyleColor();
    ImGui::End();

    return 1;
}