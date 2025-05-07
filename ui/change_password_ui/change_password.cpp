

#include "change_password.h"
#include "admin_ui/admin_ui.h"
#include "marketer_ui/marketer_ui.h"

//initialize the static variables
char PassWord::c_old_password[128] = "";
char PassWord::c_new_password[128] = "";
PassWord::PassWord(mongocxx::database db)
{
	this->login = db["LogInData"]; //Access the collection that store the login data
	
}

int PassWord::create_password_window()
{
	this->passwords = get_users_data(this->login);

	this->s_old_password = "";
	this->s_new_password = "";

	ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(60, 60, 60, 255));

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Once, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(400.0, 200.0), ImGuiCond_Once);

	ImGui::Begin("Change Password", &this->show_window, this->window_flags);

	if (this->password_error)
		this->password_error_message(this->password_error);

	if (this->password_success)
		this->password_success_message(this->password_success);	//display the success message

	// Use a table-like layout for alignment
	const float label_width = 130.0f; // Fixed width for labels with extra space
	const float input_width = 200.0f; // Fixed width for input fields

	// Current Password
    ImGui::SetCursorPosY(50.0f); // Set Y position for first label
    ImGui::Text("Current Password:");
    ImGui::SameLine(label_width + 10.0f);
	ImGui::SetCursorPosY(45.0f); 
    ImGui::SetNextItemWidth(input_width);
    ImGui::PushID("Current Password");
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f); // Add border to input field
    ImGui::InputTextWithHint("##Current", "Enter current password", this->c_old_password, IM_ARRAYSIZE(this->c_old_password), ImGuiInputTextFlags_Password);
    ImGui::PopStyleVar(); // Pop input field border
    ImGui::PopID();

    // New Password
    ImGui::SetCursorPosY(100.0f); // Set Y position for second label
    ImGui::Text("New Password:");
    ImGui::SameLine(label_width + 10.0f);
	ImGui::SetCursorPosY(95.0f); 
    ImGui::SetNextItemWidth(input_width);
    ImGui::PushID("New Password");
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f); // Add border to input field
    ImGui::InputTextWithHint("##New", "Enter new password", this->c_new_password, IM_ARRAYSIZE(this->c_new_password), ImGuiInputTextFlags_Password);
    ImGui::PopStyleVar(); // Pop input field border
    ImGui::PopID();

	ImGui::NewLine();
	ImGui::SameLine(130.0, 0.0);
	ImGui::SetCursorPosY(150.0f); 
	if (ImGui::Button("Submit", ImVec2(135.0, 30.0)))
	{
		this->s_old_password = this->c_old_password;
		memset(this->c_old_password, 0, 128);

		this->s_new_password = this->c_new_password;
		memset(this->c_new_password, 0, 128);

		if (this->s_old_password.compare(this->passwords[0].password) == 0 && admin_window)
		{
			set_password(this->login, this->passwords[0].level, this->s_new_password);
			this->password_success = true;
		}
		else if (this->s_old_password.compare(this->passwords[1].password) == 0 && finance_window)
		{
			set_password(this->login, this->passwords[1].level, this->s_new_password);
			this->password_success = true;
		}
		else if (this->s_old_password.compare(this->passwords[2].password) == 0 && marketer_window)
		{
			set_password(this->login, this->passwords[2].level, this->s_new_password);
			this->password_success = true;
		}
		else if (this->s_old_password.compare(this->passwords[3].password) == 0 && regular_user_window)
		{
			set_password(this->login, this->passwords[3].level, this->s_new_password);
			this->password_success = true;
		}
		else
		{
			this->password_error = true;
		}
	}


	ImGui::PopStyleColor();
	ImGui::End();

	return 1;
}

int PassWord::password_success_message(bool& success_flag)
{
	ImGui::OpenPopup("Success");
	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Success", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Password Changed!");
		ImGui::Separator();
		ImGui::SetCursorPosX(38.0f); 
		if (ImGui::Button("OK", ImVec2(10, 0)))
		{
			success_flag = false;
			ImGui::CloseCurrentPopup();

		}

		ImGui::EndPopup();
	}

	return 1;
}

int PassWord::password_error_message(bool& error_flag)
{
	ImGui::OpenPopup("Password Error");
	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Password Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Password Mismatch!");
		ImGui::Separator();
		ImGui::SetCursorPosX(38.0f); 
		if (ImGui::Button("OK", ImVec2(90, 0)))
		{
			error_flag = false;
			ImGui::CloseCurrentPopup();

		}

		ImGui::EndPopup();
	}

	return 1;
}