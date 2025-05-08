#include "finance_ui.h"
#include "retrieve_data.h"
#include "compute_costs.h"
#include "manage_OperationalCosts.h"
#include "agents.h"
#include "agent_sales.h"
#include "change_values.h"
#include "update_stock.h"
#include "login_ui/login_ui.h"
#include "marketer_ui/marketer_ui.h"
#include "admin_ui/admin_ui.h"
#include "imgui_internal.h"

#include <algorithm>

bool finance_window = false;

FUserUi::ShowFinanceWindow::ShowFinanceWindow(Profit_Loss_Window& profit_or_loss, Expenses& expenses, ConfirmSaleWindow& confirm_sale, PassWord& change_password)
{
    this->profit_or_loss = &profit_or_loss;
    this->expenses = &expenses;
    this->confirm_sale = &confirm_sale;
    this->change_password = &change_password;
}

int FUserUi::ShowFinanceWindow::create_show_finance_window()
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
    ImGui::SetNextWindowSize(ImVec2(340.0f, 400.0f), ImGuiCond_Always);

    // Begin window
    ImGui::Begin("Finance Dashboard", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    // Title
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Finance Dashboard").x) * 0.5f);
    ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.4f, 1.0f), "Finance Dashboard");
    ImGui::PopFont();
    ImGui::Separator();

    // Button layout
    const float button_width = 150.0f;
    const float button_height = 30.0f;
    const float spacing = 10.0f;

    // Profit/Loss button
    ImGui::Dummy(ImVec2(0.0f, spacing));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
    if (ImGui::Button("Profit/Loss", ImVec2(button_width, button_height)))
        this->profit_or_loss->show_window = true;

    if (this->profit_or_loss->show_window)
        this->profit_or_loss->create_profit_loss_window();

    // Manage Expenses button
    ImGui::Dummy(ImVec2(0.0f, spacing));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
    if (ImGui::Button("Manage Expenses", ImVec2(button_width, button_height)))
        this->expenses->show_window = true;

    if (this->expenses->show_window)
        this->expenses->create_handle_expenses_window();

    if (this->expenses->add->show_window)
        this->expenses->add->create_expense_window();

    if (this->expenses->remove->show_window)
        this->expenses->remove->remove_expense_window();

    if (this->expenses->set->show_window)
        this->expenses->set->set_expenditure_window();

    // // Confirm Sale button
    // ImGui::Dummy(ImVec2(0.0f, spacing));
    // ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
    // if (ImGui::Button("Confirm Sale", ImVec2(button_width, button_height)))
    //     this->confirm_sale->show_window = true;

    // if (this->confirm_sale->show_window)
    //     this->confirm_sale->create_confirm_sale_window();

    ImGui::Separator();

    // Change Password button (Pink)
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(220, 75, 150, 255));
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
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(220, 50, 50, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 80, 80, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(180, 30, 30, 255));
    ImGui::Dummy(ImVec2(0.0f, spacing));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
    if (ImGui::Button("Log Out", ImVec2(button_width, button_height)))
    {
        login_window = true;
        finance_window = false;
        marketer_window = false;
        admin_window = false;
        regular_user_window = false;
        showing_admin_extensions = false;
    }
    ImGui::PopStyleColor(3);

    ImGui::End();

    // Pop styles
    ImGui::PopStyleColor(8);
    ImGui::PopStyleVar(4);

    return 1;
}

FUserUi::Profit_Loss_Window::Profit_Loss_Window(mongocxx::database db)
{
    this->operational_costs = db["OperationalCosts"];
    this->products_collection = db["Products"];
    this->components_arrays = get_components_arrays(this->products_collection);
}

int FUserUi::Profit_Loss_Window::create_profit_loss_window() {
    // Push styling for window background and text
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(40, 40, 40, 255));   // Dark background
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255));    // Light gray text
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);               // Rounded corners
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f)); // Padding

    // Set window position and size
    ImGui::SetNextWindowPos(
        ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
        ImGuiCond_Appearing,
        ImVec2(0.5f, 0.5f)
    );
    ImGui::SetNextWindowSize(ImVec2(460, 300), ImGuiCond_Always);

    // Begin window with fixed size and no collapse option
    ImGui::Begin("Profit/Loss", &this->show_window, 
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    // Title styling (left-aligned, greenish color)
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.4f, 1.0f), "Profit or Loss");
    ImGui::PopFont();
    ImGui::Separator();

    // Compute financial metrics
    this->expected_expenditure = get_expected_expenditure(this->operational_costs);
    SALE sale;
    this->total_sales = compute_total_cost(
        this->products_collection, 
        components_arrays, 
        sale, 
        "SellingPrice"
    );
    this->total_expenditure = get_operational_costs_total(this->operational_costs);
    this->profit_loss = this->total_sales - this->total_expenditure;

    // Define uniform starting positions for labels and values
    const float label_width = 220.0f; // Fixed width for labels to ensure uniform alignment
    const float value_start = label_width + 10.0f; // Starting position for values (with small padding)

    // Display financial metrics with vertically aligned labels and values
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::SetCursorPosX(20.0f);
    ImGui::TextColored(ImVec4(0.2f, 0.6f, 1.0f, 1.0f), "Expected Expenditure:"); // Blue
    ImGui::SameLine();
    ImGui::SetCursorPosX(value_start);
    ImGui::TextColored(ImVec4(0.2f, 0.6f, 1.0f, 1.0f), "%s", this->truncate(this->expected_expenditure).c_str());
    ImGui::Separator();

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::SetCursorPosX(20.0f);
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.0f, 1.0f), "Total Sales:"); // Yellow
    ImGui::SameLine();
    ImGui::SetCursorPosX(value_start);
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.0f, 1.0f), "%s", this->truncate(this->total_sales).c_str());
    ImGui::Separator();

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::SetCursorPosX(20.0f);
    ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Total Expenditure:"); // Red
    ImGui::SameLine();
    ImGui::SetCursorPosX(value_start);
    ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "%s", this->truncate((float)this->total_expenditure).c_str());
    ImGui::Separator();

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::SetCursorPosX(20.0f);
    ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Total Profit/Loss:"); // Green
    ImGui::SameLine();
    ImGui::SetCursorPosX(value_start);
    ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "%s", this->truncate(this->profit_loss).c_str());
    ImGui::Separator();

    // End window and pop styles
    ImGui::End();
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(2);

    return 1;
}

std::string FUserUi::Profit_Loss_Window::truncate(double val)
{
    std::string s_val = std::to_string(val);
    size_t decimal_index = s_val.find_first_of('.', 0);
    s_val = s_val.substr(0, decimal_index + 3);
    return s_val;
}

char FUserUi::AddExpenseWindow::c_expense_name[128] = "";

FUserUi::AddExpenseWindow::AddExpenseWindow(mongocxx::database db)
{
    this->operational_costs = db["OperationalCosts"];
}

int FUserUi::AddExpenseWindow::create_expense_window()
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(40, 40, 40, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(60, 60, 60, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(80, 80, 80, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(100, 100, 100, 255));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.40f, 0.75f, 1.0f)); // Updated to professional blue
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 180, 100, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0, 120, 60, 255));
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 5.0f));

    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(345, 300), ImGuiCond_Always);

    ImGui::Begin("Add Expense", &this->show_window, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Add Expense").x) * 0.5f);
    ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.4f, 1.0f), "Add Expense");
    ImGui::PopFont();
    ImGui::Separator();

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.0f); // Shift right by 20 pixels
    ImGui::Text("Expense Name:");
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.0f); // Shift input right by 20 pixels
    ImGui::SetNextItemWidth(200.0f);
    ImGui::InputTextWithHint("###", "Expense Name", this->c_expense_name, IM_ARRAYSIZE(this->c_expense_name));

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.0f); // Shift right by 20 pixels
    ImGui::Text("Expense Cost:");
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.0f); // Shift input right by 20 pixels
    ImGui::SetNextItemWidth(200.0f);
    ImGui::InputFloat("##", &cost, 0);

    if (this->input_error)
        this->handle_error(this->input_error);
    else if (this->success)
        this->notify_sale_status(this->success);

    ImGui::Dummy(ImVec2(0.0f, 20.0f));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 150.0f) * 0.5f);
    if (ImGui::Button("Add", ImVec2(150.0f, 30.0f)))
    {
        this->expense_name = this->c_expense_name;
        if ((this->expense_name.empty()) || this->cost <= 0)
            this->input_error = true;
        else
        {
            insert_expense(this->operational_costs, this->expense_name, this->cost);
            this->success = true;
        }
    }

    ImGui::End();
    ImGui::PopStyleColor(8);
    ImGui::PopStyleVar(4);

    return 1;
}

int FUserUi::Window::handle_error(bool& error_flag)
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(40, 40, 40, 255));
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);

    ImGui::OpenPopup("Error");
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Please Fill in All The Details Correctly or\nMake Sure A Checkbox is Checked.");
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 150.0f) * 0.5f);
        if (ImGui::Button("OK", ImVec2(150.0f, 30.0f)))
        {
            error_flag = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(1);
    return 1;
}

int FUserUi::Window::notify_sale_status(bool& success_flag)
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(40, 40, 40, 255));
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);

    ImGui::OpenPopup("Success");
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Success", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Operation successful");
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 150.0f) * 0.5f);
        if (ImGui::Button("OK", ImVec2(150.0f, 30.0f)))
        {
            success_flag = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(1);
    return 1;
}

int FUserUi::RemoveExpenseWindow::expense_current_item = 0;

FUserUi::RemoveExpenseWindow::RemoveExpenseWindow(mongocxx::database db)
{
    this->operational_costs = db["OperationalCosts"];
}

int FUserUi::RemoveExpenseWindow::remove_expense_window()
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(40, 40, 40, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(60, 60, 60, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(80, 80, 80, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(100, 100, 100, 255));
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 150, 80, 255)); 
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(50, 100, 200, 255)); // Lighter blue for hover
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(30, 80, 160, 255)); // Darker blue for active
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 5.0f));

    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(360, 250), ImGuiCond_Always);

    ImGui::Begin("Remove Expense", &this->show_window, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Remove Expense").x) * 0.5f);
    ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.4f, 1.0f), "Remove Expense");
    ImGui::PopFont();
    ImGui::Separator();

    // Fixed label width for alignment
    const float label_width = 100.0f;
    const float input_width = 160.0f;
    
    ImGui::NewLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
    ImGui::Text("Expense:");
    ImGui::SameLine(label_width);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3.0f);
    ImGui::SetNextItemWidth(input_width);
    this->fetch_expenses();
    create_combo(this->p_expenses, "Expense", this->expense_current_item, ImVec2(input_width, 20.0f));
    delete[] this->p_expenses;
    this->p_expenses = nullptr;

    if (this->confirm == this->ACCEPTED)
    {
        delete_expense(this->operational_costs, this->selected_expense);
        this->success = true;
        this->expense_current_item = 0;
        this->show_confirm_window = false;
        this->confirm = this->NONE;
    }
    else if (this->confirm == this->REJECTED)
    {
        this->show_confirm_window = false;
        this->confirm = this->NONE;
    }

    if (this->show_confirm_window)
        this->finance_confirm_message(this->confirm);
    else if (this->input_error)
        this->handle_error(this->input_error);

    if (this->success)
        this->notify_sale_status(this->success);

    ImGui::Dummy(ImVec2(0.0f, 20.0f));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 150.0f) * 0.5f);
    if (ImGui::Button("Remove", ImVec2(150.0f, 30.0f)))
    {
        this->get_selected_expense(this->expense_current_item);
        if (this->expense_current_item == 0 || this->selected_expense.empty())
            this->input_error = true;
        else
            this->show_confirm_window = true;
    }

    ImGui::End();
    ImGui::PopStyleColor(8);
    ImGui::PopStyleVar(4);

    return 1;
}

int FUserUi::RemoveExpenseWindow::create_combo(char* my_items_names, std::string description, int& current_item, ImVec2 size)
{
    // Apply styling for the combo box
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(70, 70, 70, 255)); // Combo background
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f); // Rounded frame

    // Set the width of the combo box
    ImGui::SetNextItemWidth(size.x);

    // Render the combo box
    ImGui::Combo(("##" + description).c_str(), &current_item, (const char*)my_items_names);

    // Pop styling
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    return 1;
}

void FUserUi::RemoveExpenseWindow::prepare_items_pointer(const std::string& items, char items_separator, char*& items_pointer)
{
    size_t count = std::count(items.begin(), items.end(), items_separator);
    size_t offset = 0;
    for (int index = 0; index < count; index++)
    {
        offset = items.find_first_of(items_separator, offset + 1);
        items_pointer[offset] = '\0';
    }
}

int FUserUi::RemoveExpenseWindow::set_expenses()
{
    this->expense_names = "Select_";
    this->expenses_array = get_expenses_array(this->operational_costs);
    for (const std::string& expense : this->expenses_array)
        this->expense_names += expense + "_";
    return 1;
}

std::string FUserUi::RemoveExpenseWindow::get_selected_expense(int selected_index)
{
    if (selected_index != 0)
        selected_expense = this->expenses_array[selected_index - 1];
    else
        selected_expense = "";
    return selected_expense;
}

int FUserUi::RemoveExpenseWindow::fetch_expenses()
{
    this->set_expenses();
    this->p_expenses = new char[this->expense_names.size() + 1];
    memset(this->p_expenses, 0, this->expense_names.size());
    strncpy_s(this->p_expenses, this->expense_names.size() + 1, this->expense_names.c_str(), this->expense_names.size());
    this->prepare_items_pointer(this->expense_names, '_', this->p_expenses);
    return 1;
}

FUserUi::SetExpenditureWindow::SetExpenditureWindow(mongocxx::database db)
{
    this->operational_costs = db["OperationalCosts"];
}

int FUserUi::SetExpenditureWindow::set_expenditure_window()
{
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

    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(355, 220), ImGuiCond_Always);

    ImGui::Begin("Set Expected Expenditure", &this->show_window, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Set Expected Expenditure").x) * 0.5f);
    ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.4f, 1.0f), "Set Expected Expenditure");
    ImGui::PopFont();
    ImGui::Separator();

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.0f);
    ImGui::Text("Expected Expenditure:");
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.0f);
    ImGui::SetNextItemWidth(200.0f);
    ImGui::InputFloat("##", &this->expenditure, 0);

    if (this->input_error)
        this->handle_error(this->input_error);
    else if (this->success)
        this->notify_sale_status(this->success);

    ImGui::Dummy(ImVec2(0.0f, 20.0f));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 150.0f) * 0.5f);
    if (ImGui::Button("Set", ImVec2(150.0f, 30.0f)))
    {
        if (this->expenditure < 0)
            this->input_error = true;
        else
        {
            set_expected_expenditure(this->operational_costs, this->expenditure);
            this->success = true;
        }
    }

    ImGui::End();
    ImGui::PopStyleColor(8);
    ImGui::PopStyleVar(4);

    return 1;
}

FUserUi::Expenses::Expenses(FUserUi::AddExpenseWindow& add, FUserUi::RemoveExpenseWindow& remove, FUserUi::SetExpenditureWindow& set)
{
    this->add = &add;
    this->remove = &remove;
    this->set = &set;
}

int FUserUi::Window::finance_confirm_message(int& confirm_flag)
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(40, 40, 40, 255));
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255));
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 150, 80, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 180, 100, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0, 120, 60, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);

    ImGui::OpenPopup("Confirm");
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Confirm", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Are You Sure You Want To Complete This Action?");
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 320.0f) * 0.5f);
        if (ImGui::Button("No", ImVec2(150.0f, 30.0f)))
        {
            confirm_flag = REJECTED;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine(0.0f, 20.0f);
        if (ImGui::Button("Yes", ImVec2(150.0f, 30.0f)))
        {
            confirm_flag = ACCEPTED;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::PopStyleColor(5);
    ImGui::PopStyleVar(2);
    return 1;
}

int FUserUi::Expenses::create_handle_expenses_window()
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(40, 40, 40, 255));
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 150, 80, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 180, 100, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0, 120, 60, 255));
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 5.0f));

    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(350, 350), ImGuiCond_Always);

    ImGui::Begin("Manage Expenses", &this->show_window, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Manage Expenses").x) * 0.5f);
    ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.4f, 1.0f), "Manage Expenses");
    ImGui::PopFont();
    ImGui::Separator();

    if (this->success)
        this->notify_sale_status(this->success);

    const float button_width = 150.0f;
    const float button_height = 30.0f;
    const float spacing = 10.0f;

    ImGui::Dummy(ImVec2(0.0f, spacing));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
    if (ImGui::Button("Add Expense", ImVec2(button_width, button_height)))
        this->add->show_window = true;

    ImGui::Dummy(ImVec2(0.0f, spacing));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
    if (ImGui::Button("Remove Expense", ImVec2(button_width, button_height)))
        this->remove->show_window = true;

    ImGui::Dummy(ImVec2(0.0f, spacing));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
    if (ImGui::Button("Reset Expenses", ImVec2(button_width, button_height)))
        this->show_confirm_window = true;

    ImGui::Dummy(ImVec2(0.0f, spacing));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
    if (ImGui::Button("Expected Expenditure", ImVec2(button_width, button_height)))
        this->set->show_window = true;

    if (this->confirm == this->ACCEPTED)
    {
        reset_expenses(this->add->operational_costs);
        this->success = true;
        this->show_confirm_window = false;
        this->confirm = this->NONE;
    }
    else if (this->confirm == this->REJECTED)
    {
        this->show_confirm_window = false;
        this->confirm = this->NONE;
    }

    if (this->show_confirm_window)
        this->finance_confirm_message(this->confirm);
    else if (this->input_error)
        this->handle_error(this->input_error);

    ImGui::End();
    ImGui::PopStyleColor(5);
    ImGui::PopStyleVar(4);

    return 1;
}

FUserUi::ConfirmSaleWindow::ConfirmSaleWindow(mongocxx::database db, RUserUi::Window& window)
{
    this->agents = db["Agents"];
    this->agent_sales = db["AgentSales"];
    this->products_collection = db["Products"];
    this->stock = db["stock"];
    this->p_window = &window;
    this->selected_agent = "All Agents";
    this->table_flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_RowBg;
}

int FUserUi::ConfirmSaleWindow::prepare_agents_details(std::string& m_agent_details, const std::string& agent_details_separator, const std::string& new_separator)
{
    if (m_agent_details.empty())
        return 0;

    size_t start_pos = 0;
    while ((start_pos = m_agent_details.find(agent_details_separator, start_pos)) != std::string::npos)
    {
        m_agent_details.replace(start_pos, agent_details_separator.length(), new_separator);
    }
    return 1;
}

int FUserUi::ConfirmSaleWindow::create_confirm_sale_window()
{
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

    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(720, 460), ImGuiCond_Always);

    ImGui::Begin("Confirm Agent Sale", &this->show_window, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Confirm Agent Sale").x) * 0.5f);
    ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.4f, 1.0f), "Confirm Agent Sale");
    ImGui::PopFont();
    ImGui::Separator();

    if (this->input_error)
        this->handle_error(this->input_error);
    else if (this->success)
        this->notify_sale_status(this->success);

    this->set_agents_details();

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 210.0f) * 0.5f);
    this->p_window->create_listbox_filter(this->agents_details, "Agents", this->selected_agent, '_', ImVec2(210, 150), "Confirm");

    if (this->selected_agent != "All Agents")
    {
        this->set_agent_sales();
        if (!this->v_agent_sales[0].empty())
        {
            ImGui::Dummy(ImVec2(0.0f, 10.0f));
            draw_sales_table(this->TEXT_BASE_HEIGHT);

            size_t sales_count = this->checked_sales.size();
            SALE sale;

            ImGui::Dummy(ImVec2(0.0f, 10.0f));
            ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 150.0f) * 0.5f);
            if (ImGui::Button("Confirm", ImVec2(150.0f, 30.0f)))
            {
                if (this->agent_id == 0 || sales_count == 0)
                    this->input_error = true;
                else
                {
                    for (int i = 0; i < sales_count; i++)
                    {
                        sale.product_name = this->v_agent_sales[0][this->checked_sales[i]];
                        sale.brand = sale.product_name + "_" + this->v_agent_sales[1][this->checked_sales[i]];
                        sale.quantity = this->v_agent_sales[2][this->checked_sales[i]];
                        sale.season = this->v_agent_sales[3][this->checked_sales[i]];
                        sale.age_group = this->v_agent_sales[4][this->checked_sales[i]];
                        sale.gender = this->v_agent_sales[5][this->checked_sales[i]];
                        sale.location = this->v_agent_sales[6][this->checked_sales[i]];
                        sale.item_count = std::stoi(this->v_agent_sales[7][this->checked_sales[i]]);

                        change_amountOf_item(this->products_collection, sale, UPDATE);
                        update_stock_amountOf_item(this->stock, sale, UPDATE);
                        delete_agent_sale(this->agent_sales, this->agent_id, std::stoi(this->v_agent_sales[8][this->checked_sales[i]]));
                    }
                    this->success = true;
                    this->checked_sales.clear();
                }
            }
        }
        else
        {
            ImGui::Dummy(ImVec2(0.0f, 10.0f));
            ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("No Pending Sales for ").x) * 0.5f);
            ImGui::Text("No Pending Sales for %s", this->selected_agent.substr(8).c_str());
        }
    }
    else
    {
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Select an Agent To View Their Pending Sales.").x) * 0.5f);
        ImGui::Text("Select an Agent To View Their Pending Sales.");
    }

    ImGui::End();
    ImGui::PopStyleColor(8);
    ImGui::PopStyleVar(4);

    return 1;
}

int FUserUi::ConfirmSaleWindow::set_agents_details()
{
    this->agents_details = retieve_agents_data(this->agents);
    this->prepare_agents_details(this->agents_details, "_", ": ");
    this->prepare_agents_details(this->agents_details, ",", "_");
    this->agents_details = "All Agents_" + this->agents_details;
    return 1;
}

int FUserUi::ConfirmSaleWindow::set_agent_sales()
{
    int64_t agent_id = this->get_agent_id();
    this->v_agent_sales = get_agent_sales_arrays(this->agent_sales, agent_id);
    return 1;
}

int64_t FUserUi::ConfirmSaleWindow::get_agent_id()
{
    int64_t a_id = 0;
    std::string s_id = this->selected_agent.substr(0, 6);
    try
    {
        a_id = std::stoi(s_id);
    }
    catch (const std::invalid_argument&)
    {
        a_id = 0;
    }
    this->agent_id = a_id;
    return a_id;
}

int FUserUi::ConfirmSaleWindow::draw_sales_table(const float& m_TEXT_BASE_HEIGHT)
{
    bool* checked_statuses = new bool[this->v_agent_sales[0].size()];
    memset(checked_statuses, 0, this->v_agent_sales[0].size());
    for (int index : this->checked_sales)
        checked_statuses[index] = true;

    ImVec2 outer_size = ImVec2(0.0f, m_TEXT_BASE_HEIGHT * 90);
    if (ImGui::BeginTable("table_scrolly", 10, this->table_flags, outer_size))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Product", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Brand", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Quantity", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Season", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Age Group", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Gender", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Location", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Number", ImGuiTableColumnFlags_None);
        ImGui::TableSetupColumn("Check", ImGuiTableColumnFlags_None);

        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin((int)this->v_agent_sales[0].size());
        while (clipper.Step())
        {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 10; column++)
                {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(this->colum_colors[column]));
                    ImGui::TableSetColumnIndex(column);
                    if (column == 0)
                        ImGui::Text("%d", row + 1);
                    else if (column == 1)
                        ImGui::Text("%s", this->v_agent_sales[0][row].c_str());
                    else if (column == 2)
                        ImGui::Text("%s", this->v_agent_sales[1][row].c_str());
                    else if (column == 3)
                        ImGui::Text("%s", this->v_agent_sales[2][row].c_str());
                    else if (column == 4)
                        ImGui::Text("%s", this->v_agent_sales[3][row].c_str());
                    else if (column == 5)
                        ImGui::Text("%s", this->v_agent_sales[4][row].c_str());
                    else if (column == 6)
                        ImGui::Text("%s", this->v_agent_sales[5][row].c_str());
                    else if (column == 7)
                        ImGui::Text("%s", this->v_agent_sales[6][row].c_str());
                    else if (column == 8)
                        ImGui::Text("%s", this->v_agent_sales[7][row].c_str());
                    else
                    {
                        auto position = std::find(this->checked_sales.begin(), this->checked_sales.end(), row);
                        bool present = position != this->checked_sales.end();
                        ImGui::PushID(row);
                        bool clicked = ImGui::Checkbox("##", &checked_statuses[row]);
                        ImGui::PopID();
                        if (clicked && present && !ImGui::IsItemActive())
                            this->checked_sales.erase(position);
                        else if (clicked && !present && !ImGui::IsItemActive())
                            this->checked_sales.push_back(row);
                    }
                }
            }
        }
        ImGui::EndTable();
    }

    delete[] checked_statuses;
    return 1;
}