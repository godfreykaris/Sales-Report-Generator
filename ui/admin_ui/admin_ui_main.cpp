#include "admin_ui.h"
#include "imgui_internal.h"

#include <algorithm>
#include <agents.h>
#include <marketer_ui/marketer_ui.h>
#include "regular_ui/regular_user_ui.h"
#include "login_ui/login_ui.h"
#include "change_password.h"

bool admin_window = false;
bool showing_admin_extensions = false;
bool log_out = false;

AdminUi::ShowAdminWindow::ShowAdminWindow(AddorRemoveItemWindow& add_or_remove_item, AddorRemoveOthersWindow& add_or_remove_others, AddorRemoveAgentsWindow& add_or_remove_agents, PassWord& change_password)
{
	this->add_or_remove_item = &add_or_remove_item;
	this->add_or_remove_others = &add_or_remove_others;
	this->add_or_remove_agents = &add_or_remove_agents;
	this->change_password = &change_password;
}

int AdminUi::ShowAdminWindow::create_show_admin_window()
{
	
	if (showing_admin_extensions == false)
	{
		ImGui::Dummy(ImVec2(0.0, 10.0));
		ImGui::NewLine();
		ImGui::SameLine(50.0, 0.0);
		if (ImGui::Button("Products", ImVec2(115.0, 20.0)))
			this->add_or_remove_item->show_window = true;

		if (this->add_or_remove_item->show_window)
			this->add_or_remove_item->create_add_or_remove_items_window();


		ImGui::Dummy(ImVec2(0.0, 10.0));
		ImGui::NewLine();
		ImGui::SameLine(50.0, 0.0);
		if (ImGui::Button("Others", ImVec2(115.0, 20.0)))
			this->add_or_remove_others->show_window = true;

		if (this->add_or_remove_others->show_window)
			this->add_or_remove_others->create_add_or_remove_others_window();


		ImGui::Dummy(ImVec2(0.0, 10.0));
		ImGui::NewLine();
		ImGui::SameLine(50.0, 0.0);
		if (ImGui::Button("Agents", ImVec2(115.0, 20.0)))
			this->add_or_remove_agents->show_window = true;

		if (this->add_or_remove_agents->show_window)
			this->add_or_remove_agents->create_add_or_remove_agents_window();
	}
	

	if (finance_window != true && marketer_window != true && regular_user_window != true)
	{
		ImGui::Dummy(ImVec2(0.0, 10.0));
		ImGui::NewLine();
		ImGui::SameLine(50.0, 0.0);
		if (ImGui::Button("Finance", ImVec2(115.0, 20.0)))
			finance_window = true;

		ImGui::Dummy(ImVec2(0.0, 10.0));
		ImGui::NewLine();
		ImGui::SameLine(50.0, 0.0);
		if (ImGui::Button("Reports", ImVec2(115.0, 20.0)))
			marketer_window = true;

		ImGui::Dummy(ImVec2(0.0, 10.0));
		ImGui::NewLine();
		ImGui::SameLine(50.0, 0.0);
		if (ImGui::Button("Regular", ImVec2(115.0, 20.0)))
			regular_user_window = true;

		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(6 / 7.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(6 / 7.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(6 / 7.0f, 0.8f, 0.8f));	
		
		ImGui::Dummy(ImVec2(0.0, 10.0));
		ImGui::NewLine();
		ImGui::SameLine(50.0, 0.0);
		if (ImGui::Button("Log Out", ImVec2(115.0, 20.0)))
		{
			login_window = true;
			admin_window = false;
			showing_admin_extensions = false;
		}
			
		ImGui::PopStyleColor(3);

		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.8f / 7.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.8f / 7.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.8f / 7.0f, 0.8f, 0.8f));

		ImGui::Dummy(ImVec2(0.0, 10.0));
		ImGui::NewLine();
		ImGui::SameLine(50.0, 0.0);
		if (ImGui::Button("Change Password", ImVec2(115.0, 20.0)))
			this->change_password->show_window = true;
		ImGui::PopStyleColor(3);

		if (this->change_password->show_window)
			this->change_password->create_password_window();

		
	}
	else
		showing_admin_extensions = true;

	if (showing_admin_extensions == true)
	{
		ImGui::Dummy(ImVec2(0.0, 10.0));
		ImGui::NewLine();
		ImGui::SameLine(50.0, 0.0);
		if (ImGui::Button("Back", ImVec2(115.0, 20.0)))
		{
			showing_admin_extensions = false;
			finance_window = false;
			marketer_window = false;
			regular_user_window = false;
		}
	}


	return 1;
}