﻿#include "admin_ui/admin_ui.h"
#include "regular_ui/regular_user_ui.h"
#include "retrieve_data.h"
#include "agents.h"
#include "change_values.h"
#include "update_stock.h"
#include "login_ui/login_ui.h"
#include "marketer_ui/marketer_ui.h"
#include "imgui_internal.h"

#include <memory> // For smart pointers
#include <algorithm> // For std::count, std::find
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iomanip>
#include <windows.h>
#include <commdlg.h>
#include <hpdf.h>
#include <shellapi.h> // For ShellExecute


// External flag for window visibility
bool regular_user_window = false;

// Constructor for base Window class
RUserUi::Window::Window()
{
    this->selected_product = "All Products";
    this->selected_brand = "Select";
    this->selected_quantity = "Select";

    this->set_brands("Select_");
    this->set_quantities("Select_");

    // Allocate smart pointers for combo box items
    this->p_brands = std::make_unique<char[]>(this->brands.size() + 1);
    std::fill_n(this->p_brands.get(), this->brands.size() + 1, 0);
    std::strncpy(this->p_brands.get(), this->brands.c_str(), this->brands.size());
    this->prepare_items_pointer(this->brands, '_', this->p_brands);

    this->p_quantities = std::make_unique<char[]>(this->quantities.size() + 1);
    std::fill_n(this->p_quantities.get(), this->quantities.size() + 1, 0);
    std::strncpy(this->p_quantities.get(), this->quantities.c_str(), this->quantities.size());
    this->prepare_items_pointer(this->quantities, '_', this->p_quantities);
}

// Constructor for ShowRegularUserWindow
RUserUi::ShowRegularUserWindow::ShowRegularUserWindow(StockWindow& stock_win, SellWindow& sell_win,
    ReturnSaleWindow& return_sale_win, AgentsWindow& agents_win, PassWord& change_password, AddorRemoveItemWindow& add_or_remove_item, AddorRemoveOthersWindow& add_or_remove_others, SalesReportWindow& sales_report_window)
    : stock_win(stock_win), sell_win(sell_win), return_sale_win(return_sale_win),
    agents_win(agents_win), change_password(change_password)
{
    this->add_or_remove_item = &add_or_remove_item;
    this->add_or_remove_others = &add_or_remove_others;
    this->sales_report_window = &sales_report_window;
}

// Create the main regular user window with styled layout
int RUserUi::ShowRegularUserWindow::create_show_regular_user_window()
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
    ImGui::SetNextWindowSize(ImVec2(350.0f, 470.0f), ImGuiCond_Always);

    // Begin window
    ImGui::Begin("Regular User Dashboard", &this->show_window, this->window_flags);

    // Title
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]); // Assuming large font at index 1
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("User Dashboard").x) * 0.5f);
    ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.4f, 1.0f), "User Dashboard");
    ImGui::PopFont();
    ImGui::Separator();

    // Button layout (centered, evenly spaced)
    const float button_width = 200.0f;
    const float button_height = 40.0f;
    const float spacing = 15.0f;

    ImGui::Dummy(ImVec2(0.0f, spacing));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
    if (ImGui::Button("View Stock", ImVec2(button_width, button_height))) {
        this->stock_win.show_window = true;
    }
    if (this->stock_win.show_window) {
        this->stock_win.create_stock_window();
    }

    ImGui::Dummy(ImVec2(0.0f, spacing));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
    if (ImGui::Button("Sell Product", ImVec2(button_width, button_height))) {
        this->sell_win.show_window = true;
    }
    if (this->sell_win.show_window) {
        this->sell_win.create_sell_window();
    }

    /*ImGui::Dummy(ImVec2(0.0f, spacing));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
    if (ImGui::Button("Return Product", ImVec2(button_width, button_height))) {
        this->return_sale_win.show_window = true;
    }
    if (this->return_sale_win.show_window) {
        this->return_sale_win.create_return_sale_window();
    }

    ImGui::Dummy(ImVec2(0.0f, spacing));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
    if (ImGui::Button("View Agents", ImVec2(button_width, button_height))) {
        this->agents_win.show_window = true;
    }
    if (this->agents_win.show_window) {
        this->agents_win.create_agents_window();
    }*/

    ImGui::Separator();

    ImGui::Dummy(ImVec2(0.0f, spacing));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
	if (ImGui::Button("Products", ImVec2(button_width, button_height)))
		this->add_or_remove_item->show_window = true;

	if (this->add_or_remove_item->show_window)-
		this->add_or_remove_item->create_add_or_remove_items_window();


	ImGui::Dummy(ImVec2(0.0f, spacing));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
	if (ImGui::Button("Others", ImVec2(button_width, button_height)))
		this->add_or_remove_others->show_window = true;

	if (this->add_or_remove_others->show_window)
		this->add_or_remove_others->create_add_or_remove_others_window();


    ImGui::Dummy(ImVec2(0.0f, spacing));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
	if (ImGui::Button("Print Sales", ImVec2(button_width, button_height)))
		this->sales_report_window->show_window = true;

	if (this->sales_report_window->show_window)-
		this->sales_report_window->create_sales_report_window();

    ImGui::Separator();

    // Change Password and Log Out buttons with distinct styling
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(220, 75, 150, 255)); // Pink for Change Password
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 105, 180, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(200, 130, 0, 255));
    ImGui::Dummy(ImVec2(0.0f, spacing));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
    if (ImGui::Button("Change Password", ImVec2(button_width, button_height))) {
        this->change_password.show_window = true;
    }
    if (this->change_password.show_window) {
        this->change_password.create_password_window();
    }
    ImGui::PopStyleColor(3);

    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(220, 50, 50, 255)); // Red for Log Out
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 80, 80, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(180, 30, 30, 255));
    ImGui::Dummy(ImVec2(0.0f, spacing));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - button_width) * 0.5f);
    if (ImGui::Button("Log Out", ImVec2(button_width, button_height))) {
        login_window = true;
        regular_user_window = false;
        marketer_window = false;
        admin_window = false;
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

// Prepare items for combo boxes using smart pointers
void RUserUi::Window::prepare_items_pointer(const std::string& items, char item_separator, std::unique_ptr<char[]>& items_pointer)
{
    size_t count = std::count(items.begin(), items.end(), item_separator);
    size_t offset = 0;
    for (size_t index = 0; index < count; ++index) {
        offset = items.find_first_of(item_separator, offset + 1);
        items_pointer[offset] = '\0';
    }
}

int RUserUi::Window::create_listbox_filter(const std::string& items, const std::string& items_description,
    std::string& selected_item, char item_separator, ImVec2 size,
    const std::string& caller)
{
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(50, 50, 50, 255)); // Darker child background
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255)); // Light text
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(60, 60, 65, 255)); // Input field background
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(80, 80, 85, 255)); // Input hover
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(100, 100, 105, 255)); // Input active
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(100, 150, 200, 128)); // Subtle blue border
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f); // Rounded frame
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f)); // Balanced spacing
    ImGui::BeginChild(items_description.c_str(), size, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    // Display selected item
    std::string display_text = items_description + ": " + selected_item;
    ImGui::Text(display_text.c_str());
    ImGui::Dummy(ImVec2(0.0f, 2.0f)); // Spacing

    // Search filter with label to the left
    ImGui::Text("Search");
    ImGui::SameLine();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3.0f);
    static ImGuiTextFilter filter;
    std::string label = "##Search_" + items_description; // Unique ID
    filter.Draw(label.c_str(), size.x/2.0f);
    ImGui::Dummy(ImVec2(0.0f, 2.0f)); // Spacing

    // Parse items into vector
    std::vector<std::string> items_array;
    std::string item;
    size_t end_of_item_name = 0;
    std::string remaining_items = items;
    size_t count = std::count(items.begin(), items.end(), item_separator);
    for (size_t i = 0; i < count; ++i) {
        end_of_item_name = remaining_items.find_first_of(item_separator);
        item = remaining_items.substr(0, end_of_item_name);
        if (filter.PassFilter(item.c_str())) {
            items_array.push_back(item);
        }
        remaining_items = remaining_items.substr(end_of_item_name + 1);
    }
   

    // Listbox
    label = "##Listbox_" + items_description;
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 10.0f);
    if (ImGui::BeginListBox(label.c_str(), ImVec2(size.x * 0.85f, size.y - 92.0f))) { // Adjusted height
        for (const auto& list_item : items_array) {
            if (ImGui::Selectable(list_item.c_str())) {
                if (list_item != " ") {
                    selected_item = list_item;
                    if (caller == "Regular User") {
                        this->product_changed = true;
                    }
                }
            }
        }
        ImGui::EndListBox();
    }

    ImGui::PopStyleColor(6); // ChildBg, Text, FrameBg, FrameBgHovered, FrameBgActive, Border
    ImGui::PopStyleVar(2); // FrameRounding, ItemSpacing
    ImGui::EndChild();

    return 1;
}

// Create combo box with styled appearance
int RUserUi::Window::create_combo(const std::unique_ptr<char[]>& my_items_names, const std::string& description,
    int& current_item, ImVec2 size)
{
    // Apply styling for the combo box
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(70, 70, 70, 255)); // Combo background
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f); // Rounded frame

    // Set the width of the combo box
    ImGui::SetNextItemWidth(size.x);

    // Render the combo box
    ImGui::Combo(("##" + description).c_str(), &current_item, my_items_names.get());

    // Pop styling
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    return 1;
}

int RUserUi::Window::display_all_products(bool show_low_stock, int low_stock_threshold, bool show_out_of_stock)
{
    // Modern styling for the table
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(230, 230, 230, 255));
    ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, IM_COL32(60, 60, 65, 255));
    ImGui::PushStyleColor(ImGuiCol_TableRowBg, IM_COL32(50, 50, 55, 255));
    ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, IM_COL32(45, 45, 50, 255));
    ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, IM_COL32(80, 80, 85, 255));
    ImGui::PushStyleColor(ImGuiCol_TableBorderLight, IM_COL32(60, 60, 65, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 6.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 6.0f));

    // Begin table with modern flags
    if (ImGui::BeginTable("ProductsTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable))
    {
        // Table headers
        ImGui::TableSetupColumn("Product", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableSetupColumn("Brand", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableSetupColumn("Quantity", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Remaining", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableHeadersRow();

        this->products_array = get_products_array(this->products_collection);
        for (const auto& product : this->products_array)
        {
            // Count valid rows for this product to determine if TreeNode should be shown
            size_t valid_row_count = 0;
            this->brands_array = get_product_brands_array(this->products_collection, product);
            for (const auto& brand : this->brands_array)
            {
                this->quantities_array = get_brand_quantities_array(this->products_collection, product + "_" + brand);
                this->amounts_array = get_brand_amounts_array(this->products_collection, this->stock, product, brand);
                for (size_t i = 0; i < this->quantities_array.size(); ++i)
                {
                    int remaining = this->amounts_array[i];
                    bool show_item = true;
                    if (show_out_of_stock && remaining != 0) {
                        show_item = false;
                    }
                    if (!show_out_of_stock && remaining == 0) {
                        show_item = false;
                    }
                    if (show_low_stock && remaining >= low_stock_threshold) {
                        show_item = false;
                    }
                    if (show_item) {
                        valid_row_count++;
                    }
                }
            }

            // Only show TreeNode if there are valid rows
            if (valid_row_count > 0 && ImGui::TreeNode(product.c_str()))
            {
                std::string current_brand;
                std::vector<float> brand_row_heights;
                for (const auto& brand : this->brands_array)
                {
                    this->quantities_array = get_brand_quantities_array(this->products_collection, product + "_" + brand);
                    this->amounts_array = get_brand_amounts_array(this->products_collection, this->stock, product, brand);

                    // Collect row heights for valid rows only
                    std::vector<float> temp_row_heights;
                    std::vector<size_t> valid_indices;
                    for (size_t i = 0; i < this->quantities_array.size(); ++i)
                    {
                        int remaining = this->amounts_array[i];
                        bool show_item = true;
                        if (show_out_of_stock && remaining != 0) {
                            show_item = false;
                        }
                        if (!show_out_of_stock && remaining == 0) {
                            show_item = false;
                        }
                        if (show_low_stock && remaining >= low_stock_threshold) {
                            show_item = false;
                        }
                        if (show_item) {
                            temp_row_heights.push_back(ImGui::GetTextLineHeightWithSpacing());
                            valid_indices.push_back(i);
                        }
                    }

                    if (!valid_indices.empty())
                    {
                        // Add heights to brand_row_heights for centering
                        brand_row_heights.insert(brand_row_heights.end(), temp_row_heights.begin(), temp_row_heights.end());

                        for (size_t idx : valid_indices)
                        {
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.4f, 1.0f), product.c_str());

                            ImGui::TableSetColumnIndex(1);
                            if (brand != current_brand)
                            {
                                float total_height = 0.0f;
                                for (float height : temp_row_heights) {
                                    total_height += height;
                                }
                                float text_height = ImGui::CalcTextSize(brand.c_str()).y;
                                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (total_height - text_height) * 0.5f);

                                float text_width = ImGui::CalcTextSize(brand.c_str()).x;
                                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth() - text_width) * 0.5f);
                                ImGui::Text(brand.c_str());

                                current_brand = brand;
                            }

                            ImGui::TableSetColumnIndex(2);
                            ImGui::Text(this->quantities_array[idx].c_str());
                            ImGui::TableSetColumnIndex(3);
                            ImGui::Text("%d", this->amounts_array[idx]);
                        }
                    }
                }
                ImGui::TreePop();
            }
        }
        ImGui::EndTable();
    }

    ImGui::PopStyleColor(6);
    ImGui::PopStyleVar(2);
    return 1;
}

// Display all brands for a selected product
int RUserUi::Window::display_all_brands(
    const std::string& product,
    bool show_low_stock,
    int low_stock_threshold,
    bool show_out_of_stock,
    std::vector<std::tuple<std::string, std::string, std::string, int>>& print_data,
    bool& has_valid_items,
    std::vector<int>& edited_remaining,
    std::vector<bool>& is_edited,
    size_t& row_index,
    bool& show_success_popup,
    bool& show_failure_popup,
    bool& show_invalid_popup)
{
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(230, 230, 230, 255));
    ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, IM_COL32(60, 60, 65, 255));
    ImGui::PushStyleColor(ImGuiCol_TableRowBg, IM_COL32(50, 50, 55, 255));
    ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, IM_COL32(45, 45, 50, 255));
    ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, IM_COL32(80, 80, 85, 255));
    ImGui::PushStyleColor(ImGuiCol_TableBorderLight, IM_COL32(60, 60, 65, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 6.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 6.0f));

    this->brands = "Select_";
    // Calculate max column widths based on valid items
    float max_brand_width = 150.0f;
    float max_quantity_width = 100.0f;
    float max_remaining_width = 150.0f;
    this->brands_array = get_product_brands_array(this->products_collection, product);
    for (const auto& brand : this->brands_array) {
        max_brand_width = (std::max)(max_brand_width, ImGui::CalcTextSize(brand.c_str()).x + 20.0f);
        this->quantities_array = get_brand_quantities_array(this->products_collection, product + "_" + brand);
        this->amounts_array = get_brand_amounts_array(this->products_collection, this->stock, product, brand);
        for (size_t i = 0; i < this->quantities_array.size(); ++i) {
            int remaining = this->amounts_array[i];
            bool show_item = true;
            if (show_out_of_stock && remaining != 0) {
                show_item = false;
            }
            if (!show_out_of_stock && remaining == 0) {
                show_item = false;
            }
            if (show_low_stock && remaining >= low_stock_threshold) {
                show_item = false;
            }
            if (show_item) {
                max_quantity_width = (std::max)(max_quantity_width, ImGui::CalcTextSize(this->quantities_array[i].c_str()).x + 20.0f);
            }
        }
    }

    float table_height = ImGui::GetContentRegionAvail().y - 50.0f;
    print_data.clear();
    has_valid_items = false;
    row_index = 0;

    if (ImGui::BeginTable("BrandsTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY, ImVec2(0, table_height))) {
        ImGui::TableSetupColumn("Brand", ImGuiTableColumnFlags_WidthFixed, max_brand_width);
        ImGui::TableSetupColumn("Quantity", ImGuiTableColumnFlags_WidthFixed, max_quantity_width);
        ImGui::TableSetupColumn("Remaining", ImGuiTableColumnFlags_WidthFixed, max_remaining_width);
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        std::string current_brand;
        bool first_brand_row = true;
        for (const auto& brand : this->brands_array) {
            this->brands.append(brand + "_");
            this->quantities_array = get_brand_quantities_array(this->products_collection, product + "_" + brand);
            this->amounts_array = get_brand_amounts_array(this->products_collection, this->stock, product, brand);
            std::vector<size_t> valid_indices;
            for (size_t i = 0; i < this->quantities_array.size(); ++i) {
                int remaining = this->amounts_array[i];
                bool show_item = true;
                if (show_out_of_stock && remaining != 0) {
                    show_item = false;
                }
                if (!show_out_of_stock && remaining == 0) {
                    show_item = false;
                }
                if (show_low_stock && remaining >= low_stock_threshold) {
                    show_item = false;
                }
                if (show_item) {
                    valid_indices.push_back(i);
                    print_data.emplace_back(product, brand, this->quantities_array[i], remaining);
                    has_valid_items = true;
                }
            }

            if (!valid_indices.empty()) {
                for (size_t i : valid_indices) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    if (brand != current_brand || first_brand_row) {
                        ImGui::Text(brand.c_str());
                        current_brand = brand;
                        first_brand_row = false;
                    }
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text(this->quantities_array[i].c_str());
                    ImGui::TableSetColumnIndex(2);

                    // Editable input field for Remaining without step buttons
                    char input_id[32];
                    snprintf(input_id, sizeof(input_id), "##remaining_%zu", row_index);
                    int new_remaining = is_edited[row_index] ? edited_remaining[row_index] : this->amounts_array[i];
                    ImGui::PushItemWidth(80.0f);
                    ImGui::InputInt(input_id, &new_remaining, 0, 0); // Disable + and - buttons
                    if (ImGui::IsItemDeactivatedAfterEdit()) {
                        if (new_remaining >= 0) {
                            edited_remaining[row_index] = new_remaining;
                            is_edited[row_index] = true;
                        } else {
                            show_invalid_popup = true;
                        }
                    }
                    ImGui::PopItemWidth();

                    // Update stock if edited
                    ImGui::SameLine();
                    if (is_edited[row_index] && ImGui::Button(("Update##" + std::to_string(row_index)).c_str())) {
                        SALE sale;
                        sale.product_name = product;
                        sale.brand = product + "_" + brand;
                        sale.quantity = this->quantities_array[i];
                        sale.item_count = this->amounts_array[i] - new_remaining;

                        if (update_stock_amountOf_item(this->stock, sale, UPDATE) == 1) {
                            print_data[row_index] = std::make_tuple(product, brand, this->quantities_array[i], new_remaining);
                            is_edited[row_index] = false;
                            show_success_popup = true;
                        } else {
                            show_failure_popup = true;
                        }
                    }

                    row_index++;
                }
            }
        }
        ImGui::EndTable();
    }

    this->p_brands = std::make_unique<char[]>(this->brands.size() + 1);
    std::fill_n(this->p_brands.get(), this->brands.size() + 1, 0);
    std::strncpy(this->p_brands.get(), this->brands.c_str(), this->brands.size());
    this->prepare_items_pointer(this->brands, '_', this->p_brands);

    ImGui::PopStyleColor(6);
    ImGui::PopStyleVar(2);
    return 1;
}

// Display quantities for a specific brand
int RUserUi::Window::display_brand_quantities(
    const std::string& product,
    const std::string& brand,
    bool show_low_stock,
    int low_stock_threshold,
    bool show_out_of_stock,
    std::vector<std::tuple<std::string, std::string, std::string, int>>& print_data,
    bool& has_valid_items,
    std::vector<int>& edited_remaining,
    std::vector<bool>& is_edited,
    size_t& row_index,
    bool& show_success_popup,
    bool& show_failure_popup,
    bool& show_invalid_popup)
{
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(230, 230, 230, 255));
    ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, IM_COL32(60, 60, 65, 255));
    ImGui::PushStyleColor(ImGuiCol_TableRowBg, IM_COL32(50, 50, 55, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 6.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 6.0f));

    this->quantities = "Select_";
    // Calculate max column widths based on valid items
    float max_quantity_width = 100.0f;
    float max_remaining_width = 150.0f;
    this->quantities_array = get_brand_quantities_array(this->products_collection, product + "_" + brand);
    this->amounts_array = get_brand_amounts_array(this->products_collection, this->stock, product, brand);
    std::vector<size_t> valid_indices;
    for (size_t i = 0; i < this->quantities_array.size(); ++i) {
        int remaining = this->amounts_array[i];
        bool show_item = true;
        if (show_out_of_stock && remaining != 0) {
            show_item = false;
        }
        if (!show_out_of_stock && remaining == 0) {
            show_item = false;
        }
        if (show_low_stock && remaining >= low_stock_threshold) {
            show_item = false;
        }
        if (show_item) {
            valid_indices.push_back(i);
            max_quantity_width = (std::max)(max_quantity_width, ImGui::CalcTextSize(this->quantities_array[i].c_str()).x + 20.0f);
        }
    }

    float table_height = ImGui::GetContentRegionAvail().y - 50.0f;
    print_data.clear();
    has_valid_items = false;
    row_index = 0;

    if (ImGui::BeginTable("QuantitiesTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY, ImVec2(0, table_height))) {
        ImGui::TableSetupColumn("Quantity", ImGuiTableColumnFlags_WidthFixed, max_quantity_width);
        ImGui::TableSetupColumn("Remaining", ImGuiTableColumnFlags_WidthFixed, max_remaining_width);
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        for (size_t i : valid_indices) {
            this->quantities.append(this->quantities_array[i] + "_");
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text(this->quantities_array[i].c_str());
            ImGui::TableSetColumnIndex(1);

            // Editable input field for Remaining without step buttons
            char input_id[32];
            snprintf(input_id, sizeof(input_id), "##remaining_%zu", row_index);
            int new_remaining = is_edited[row_index] ? edited_remaining[row_index] : this->amounts_array[i];
            ImGui::PushItemWidth(80.0f);
            ImGui::InputInt(input_id, &new_remaining, 0, 0); // Disable + and - buttons
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                if (new_remaining >= 0) {
                    edited_remaining[row_index] = new_remaining;
                    is_edited[row_index] = true;
                } else {
                    show_invalid_popup = true;
                }
            }
            ImGui::PopItemWidth();

            // Update stock if edited
            ImGui::SameLine();
            if (is_edited[row_index] && ImGui::Button(("Update##" + std::to_string(row_index)).c_str())) {
                SALE sale;
                sale.product_name = product;
                sale.brand = product + "_" + brand;
                sale.quantity = this->quantities_array[i];
                sale.item_count = this->amounts_array[i] - new_remaining;

                if (update_stock_amountOf_item(this->stock, sale, UPDATE) == 1) {
                    print_data.emplace_back(product, brand, this->quantities_array[i], new_remaining);
                    has_valid_items = true;
                    is_edited[row_index] = false;
                    show_success_popup = true;
                } else {
                    show_failure_popup = true;
                }
            }

            print_data.emplace_back(product, brand, this->quantities_array[i], this->amounts_array[i]);
            has_valid_items = true;
            row_index++;
        }
        ImGui::EndTable();
    }

    this->p_quantities = std::make_unique<char[]>(this->quantities.size() + 1);
    std::fill_n(this->p_quantities.get(), this->quantities.size() + 1, 0);
    std::strncpy(this->p_quantities.get(), this->quantities.c_str(), this->quantities.size());
    this->prepare_items_pointer(this->quantities, '_', this->p_quantities);

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(2);
    return 1;
}

// Display a single quantity
int RUserUi::Window::display_quantity(
    const std::string& product,
    const std::string& brand,
    const std::string& quantity,
    bool show_low_stock,
    int low_stock_threshold,
    bool show_out_of_stock,
    std::vector<std::tuple<std::string, std::string, std::string, int>>& print_data,
    bool& has_valid_items,
    std::vector<int>& edited_remaining,
    std::vector<bool>& is_edited,
    size_t& row_index,
    bool& show_success_popup,
    bool& show_failure_popup,
    bool& show_invalid_popup)
{
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(230, 230, 230, 255));
    ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, IM_COL32(60, 60, 65, 255));
    ImGui::PushStyleColor(ImGuiCol_TableRowBg, IM_COL32(50, 50, 55, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 6.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 6.0f));

    // Calculate max column widths
    float max_product_width = ImGui::CalcTextSize(product.c_str()).x + 20.0f;
    float max_brand_width = ImGui::CalcTextSize(brand.c_str()).x + 20.0f;
    float max_quantity_width = ImGui::CalcTextSize(quantity.c_str()).x + 20.0f;
    float max_remaining_width = 150.0f;

    // Get remaining amount
    SALE sale;
    sale.product_name = product;
    sale.brand = sale.product_name + "_" + brand;
    sale.quantity = quantity;
    int remaining = get_quantity_total(this->stock, sale);

    // Apply filters
    bool show_item = true;
    if (show_out_of_stock && remaining != 0) {
        show_item = false;
    }
    if (!show_out_of_stock && remaining == 0) {
        show_item = false;
    }
    if (show_low_stock && remaining >= low_stock_threshold) {
        show_item = false;
    }

    // Only display table if item passes filters
    print_data.clear();
    has_valid_items = false;
    row_index = 0;

    if (show_item) {
        float table_height = ImGui::GetContentRegionAvail().y - 50.0f;
        if (ImGui::BeginTable("QuantityTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY, ImVec2(0, table_height))) {
            ImGui::TableSetupColumn("Product", ImGuiTableColumnFlags_WidthFixed, max_product_width);
            ImGui::TableSetupColumn("Brand", ImGuiTableColumnFlags_WidthFixed, max_brand_width);
            ImGui::TableSetupColumn("Quantity", ImGuiTableColumnFlags_WidthFixed, max_quantity_width);
            ImGui::TableSetupColumn("Remaining", ImGuiTableColumnFlags_WidthFixed, max_remaining_width);
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.4f, 1.0f), product.c_str());
            ImGui::TableSetColumnIndex(1);
            ImGui::Text(brand.c_str());
            ImGui::TableSetColumnIndex(2);
            ImGui::Text(quantity.c_str());
            ImGui::TableSetColumnIndex(3);

            // Editable input field for Remaining without step buttons
            char input_id[32];
            snprintf(input_id, sizeof(input_id), "##remaining_%zu", row_index);
            int new_remaining = is_edited[row_index] ? edited_remaining[row_index] : remaining;
            ImGui::PushItemWidth(80.0f);
            ImGui::InputInt(input_id, &new_remaining, 0, 0); // Disable + and - buttons
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                if (new_remaining >= 0) {
                    edited_remaining[row_index] = new_remaining;
                    is_edited[row_index] = true;
                } else {
                    show_invalid_popup = true;
                }
            }
            ImGui::PopItemWidth();

            // Update stock if edited
            ImGui::SameLine();
            if (is_edited[row_index] && ImGui::Button(("Update##" + std::to_string(row_index)).c_str())) {
                sale.item_count = remaining - new_remaining;
                if (update_stock_amountOf_item(this->stock, sale, UPDATE) == 1) {
                    print_data.emplace_back(product, brand, quantity, new_remaining);
                    has_valid_items = true;
                    is_edited[row_index] = false;
                    show_success_popup = true;
                } else {
                    show_failure_popup = true;
                }
            }

            print_data.emplace_back(product, brand, quantity, remaining);
            has_valid_items = true;
            row_index++;
            ImGui::EndTable();
        }
    }

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(2);
    return 1;
}

// Set products list
int RUserUi::Window::set_products()
{
    std::vector<std::string> v_products = get_products_array(this->products_collection);
    this->products = "All Products_";
    for (const auto& m_product : v_products) {
        this->products += m_product + "_";
    }
    return 1;
}

// Set brands list
int RUserUi::Window::set_brands(const std::string& m_brands)
{
    this->brands = m_brands;
    return 1;
}

// Set quantities list
int RUserUi::Window::set_quantities(const std::string& m_quantities)
{
    this->quantities = m_quantities;
    return 1;
}

// Set products array
int RUserUi::Window::set_products_array(const std::vector<std::string>& m_products)
{
    this->products_array = m_products;
    return 1;
}

// Set brands array
int RUserUi::Window::set_brands_array(const std::vector<std::string>& m_brands)
{
    this->brands_array = m_brands;
    return 1;
}

// Set quantities array
int RUserUi::Window::set_quantities_array(const std::vector<std::string>& m_quantities)
{
    this->quantities_array = m_quantities;
    return 1;
}

// Get selected product
std::string RUserUi::Window::get_selected_product() const
{
    return selected_product;
}

// Get selected brand
std::string RUserUi::Window::get_selected_brand(int selected_index)
{
    if (selected_index != 0) {
        selected_brand = this->brands_array[selected_index - 1];
    }
    else {
        selected_brand = "";
    }
    return selected_brand;
}

// Get selected quantity
std::string RUserUi::Window::get_selected_quantity(int selected_index)
{
    if (selected_index != 0) {
        selected_quantity = this->quantities_array[selected_index - 1];
    }
    else {
        selected_quantity = "";
    }
    return selected_quantity;
}

// Reset combo box items
int RUserUi::Window::reset_combo_items()
{
    this->brands = "Select_";
    this->p_brands = std::make_unique<char[]>(this->brands.size() + 1);
    std::fill_n(this->p_brands.get(), this->brands.size() + 1, 0);
    std::strncpy(this->p_brands.get(), this->brands.c_str(), this->brands.size());
    this->prepare_items_pointer(this->brands, '_', this->p_brands);

    this->quantities = "Select_";
    this->p_quantities = std::make_unique<char[]>(this->quantities.size() + 1);
    std::fill_n(this->p_quantities.get(), this->quantities.size() + 1, 0);
    std::strncpy(this->p_quantities.get(), this->quantities.c_str(), this->quantities.size());
    this->prepare_items_pointer(this->quantities, '_', this->p_quantities);

    return 1;
}

// Constructor for StockWindow
RUserUi::StockWindow::StockWindow(mongocxx::database db)
{
    this->products_collection = db["Products"];
    this->stock = db["Stock"];
    this->agents = db["Agents"];
    this->set_products();
}

int RUserUi::StockWindow::setup_stock_window()
{
    // Apply modern styling
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(40, 40, 45, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(60, 60, 65, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(80, 80, 85, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(100, 100, 105, 255));
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 120, 160, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 150, 190, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0, 90, 130, 255));
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(230, 230, 230, 255));
    ImGui::PushStyleColor(ImGuiCol_Separator, IM_COL32(80, 80, 85, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 6.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 6.0f));

    // Center and size the window (dynamic sizing)
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f - 400.0f, 20.0f), ImGuiCond_Once, ImVec2(0.0f, 0.0f));
    float maxHeight = io.DisplaySize.y - 40.0f; // Ensure height doesn't exceed display height minus top offset
    ImGui::SetNextWindowSize(ImVec2(800.0f, (std::min)(670.0f, maxHeight)), ImGuiCond_Always);
    ImGui::SetNextWindowSizeConstraints(ImVec2(600.0f, 400.0f), ImVec2(io.DisplaySize.x, maxHeight));

    ImGui::Begin("Stock Inventory", &this->show_window, this->window_flags);

    // Title
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Stock Inventory").x) * 0.5f);
    ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.4f, 1.0f), "Stock Inventory");
    ImGui::PopFont();
    ImGui::Dummy(ImVec2(0.0f, 8.0f));
    ImGui::Separator();

    // Filters layout
    const ImVec2 filter_size = ImVec2(400.0f, 200.0f);
    const float center_offset = (ImGui::GetWindowSize().x - filter_size.x) * 0.5f;

    // Product list box
    ImGui::SetCursorPosX(center_offset);
    ImGui::Text("Select Product");
    ImGui::SetCursorPosX(center_offset);
    create_listbox_filter(this->products, "Selected", this->selected_product, '_', filter_size);

    // Checkboxes and threshold input
    ImGui::Dummy(ImVec2(0.0f, 8.0f));
    ImGui::SetCursorPosX(center_offset);
    ImGui::Checkbox("Show only out-of-stock items", &this->show_out_of_stock);
    ImGui::SetCursorPosX(center_offset);
    ImGui::Checkbox("Show low stock items", &this->show_low_stock);
    if (this->show_low_stock) {
        ImGui::SetCursorPosX(center_offset);
        ImGui::Text("Show items with remaining below:");
        ImGui::SetCursorPosX(center_offset);
        ImGui::PushItemWidth(100.0f);
        ImGui::InputInt("##Threshold", &this->low_stock_threshold);
        ImGui::PopItemWidth();
        this->low_stock_threshold = (std::max)(0, this->low_stock_threshold);
    }
    ImGui::Separator();

    return 1;
}

int RUserUi::StockWindow::display_stock_table(std::vector<std::tuple<std::string, std::string, std::string, int>>& print_data, bool& has_valid_items)
{
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    if (this->selected_product != "All Products") {
        return 1; // Exit early if not showing all products
    }

    ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, IM_COL32(60, 60, 65, 255));
    ImGui::PushStyleColor(ImGuiCol_TableRowBg, IM_COL32(50, 50, 55, 255));
    ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, IM_COL32(45, 45, 50, 255));
    ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, IM_COL32(80, 80, 85, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 6.0f));

    // Calculate max column widths
    float max_product_width = 150.0f, max_brand_width = 150.0f, max_quantity_width = 100.0f, max_remaining_width = 120.0f;
    try {
        this->products_array = get_products_array(this->products_collection);
    } catch (const std::exception& e) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error fetching products: %s", e.what());
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();
        return 0;
    }

    if (this->products_array.empty()) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No products available");
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();
        return 0;
    }

    for (const auto& product : this->products_array) {
        max_product_width = (std::max)(max_product_width, ImGui::CalcTextSize(product.c_str()).x + 20.0f);
        this->brands_array = get_product_brands_array(this->products_collection, product);
        for (const auto& brand : this->brands_array) {
            max_brand_width = (std::max)(max_brand_width, ImGui::CalcTextSize(brand.c_str()).x + 20.0f);
            this->quantities_array = get_brand_quantities_array(this->products_collection, product + "_" + brand);
            for (const auto& quantity : this->quantities_array) {
                max_quantity_width = (std::max)(max_quantity_width, ImGui::CalcTextSize(quantity.c_str()).x + 20.0f);
            }
        }
    }

    // Begin scrollable table or display message
    float table_height = (std::max)(200.0f, ImGui::GetContentRegionAvail().y - 80.0f);
    print_data.clear();
    has_valid_items = false;

    // Collect data for table and printing
    std::vector<int> amounts_array;
    for (const auto& product : this->products_array) {
        this->brands_array = get_product_brands_array(this->products_collection, product);
        for (const auto& brand : this->brands_array) {
            this->quantities_array = get_brand_quantities_array(this->products_collection, product + "_" + brand);
            amounts_array = get_brand_amounts_array(this->products_collection, this->stock, product, brand);
            if (amounts_array.size() != this->quantities_array.size()) {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Mismatched quantities and amounts for %s_%s", product.c_str(), brand.c_str());
                continue;
            }
            for (size_t i = 0; i < this->quantities_array.size(); ++i) {
                int remaining = amounts_array[i];
                bool show_item = true;
                if (this->show_out_of_stock && remaining != 0) {
                    show_item = false;
                }
                if (!this->show_out_of_stock && remaining == 0) {
                    show_item = false;
                }
                if (this->show_low_stock && remaining >= this->low_stock_threshold) {
                    show_item = false;
                }
                if (show_item) {
                    has_valid_items = true;
                    print_data.emplace_back(product, brand, this->quantities_array[i], remaining);
                }
            }
        }
    }

    // Initialize vectors for edited values
    static std::vector<int> edited_remaining;
    static std::vector<bool> is_edited;
    edited_remaining.resize(print_data.size(), 0);
    is_edited.resize(print_data.size(), false);

    // State for popup
    static bool show_success_popup = false;
    static bool show_failure_popup = false;
    static bool show_invalid_popup = false;

    if (!has_valid_items) {
        const char* message = "No items found matching your criteria";
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
        float text_width = ImGui::CalcTextSize(message).x;
        float text_height = ImGui::CalcTextSize(message).y;
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - text_width) * 0.5f);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (table_height - text_height) * 0.5f);
        ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.4f, 1.0f), message);
        ImGui::PopFont();
    }
    else if (ImGui::BeginTable("StockDisplayTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX, ImVec2(0, table_height))) {
        ImGui::TableSetupColumn("Product", ImGuiTableColumnFlags_WidthFixed, max_product_width);
        ImGui::TableSetupColumn("Brand", ImGuiTableColumnFlags_WidthFixed, max_brand_width);
        ImGui::TableSetupColumn("Quantity", ImGuiTableColumnFlags_WidthFixed, max_quantity_width);
        ImGui::TableSetupColumn("Remaining", ImGuiTableColumnFlags_WidthFixed, max_remaining_width);
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        std::string current_product, current_brand;
        bool first_product_row = true;
        size_t row_index = 0;

        for (const auto& product : this->products_array) {
            this->brands_array = get_product_brands_array(this->products_collection, product);
            bool product_has_valid_items = false;
            for (const auto& brand : this->brands_array) {
                this->quantities_array = get_brand_quantities_array(this->products_collection, product + "_" + brand);
                amounts_array = get_brand_amounts_array(this->products_collection, this->stock, product, brand);
                if (amounts_array.size() != this->quantities_array.size()) {
                    continue;
                }
                bool first_brand_row = true;
                for (size_t i = 0; i < this->quantities_array.size(); ++i) {
                    int remaining = amounts_array[i];
                    bool show_item = true;
                    if (this->show_out_of_stock && remaining != 0) {
                        show_item = false;
                    }
                    if (!this->show_out_of_stock && remaining == 0) {
                        show_item = false;
                    }
                    if (this->show_low_stock && remaining >= this->low_stock_threshold) {
                        show_item = false;
                    }

                    if (show_item) {
                        product_has_valid_items = true;
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        if (product != current_product || first_product_row) {
                            ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.4f, 1.0f), product.c_str());
                            current_product = product;
                            first_product_row = false;
                        }
                        ImGui::TableSetColumnIndex(1);
                        if (brand != current_brand || first_brand_row) {
                            ImGui::Text(brand.c_str());
                            current_brand = brand;
                            first_brand_row = false;
                        }
                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text(this->quantities_array[i].c_str());
                        ImGui::TableSetColumnIndex(3);

                        // Ensure row_index is valid
                        if (row_index >= print_data.size()) {
                            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error: row_index out of bounds");
                            ImGui::EndTable();
                            ImGui::PopStyleColor(4);
                            ImGui::PopStyleVar();
                            return 0;
                        }

                        // Editable input field for Remaining without step buttons
                        char input_id[32];
                        snprintf(input_id, sizeof(input_id), "##remaining_%zu", row_index);
                        int new_remaining = is_edited[row_index] ? edited_remaining[row_index] : remaining;
                        ImGui::PushItemWidth(80.0f);
                        ImGui::InputInt(input_id, &new_remaining, 0, 0); // Set step and step_fast to 0 to disable + and - buttons
                        if (ImGui::IsItemDeactivatedAfterEdit()) {
                            if (new_remaining >= 0) {
                                edited_remaining[row_index] = new_remaining;
                                is_edited[row_index] = true;
                            } else {
                                show_invalid_popup = true;
                            }
                        }
                        ImGui::PopItemWidth();

                        // Update stock if edited
                        ImGui::SameLine();
                        if (is_edited[row_index] && ImGui::Button(("Update##" + std::to_string(row_index)).c_str())) {
                            SALE sale;
                            sale.product_name = product;
                            sale.brand = product + "_" + brand;
                            sale.quantity = this->quantities_array[i];
                            sale.item_count = remaining - new_remaining;

                            // Update stock using update_stock_amountOf_item
                            if (update_stock_amountOf_item(this->stock, sale, UPDATE) == 1) {
                                // Update print_data for consistency
                                print_data[row_index] = std::make_tuple(product, brand, this->quantities_array[i], new_remaining);
                                // Clear edited state
                                is_edited[row_index] = false;
                                show_success_popup = true;
                            } else {
                                show_failure_popup = true;
                            }
                        }

                        row_index++;
                    }
                }
            }
            if (!product_has_valid_items) {
                current_product = "";
                first_product_row = true;
            }
        }
        ImGui::EndTable();

        // Render popups outside the table loop
        if (show_success_popup && ImGui::BeginPopup("Update Success")) {
            ImGui::Text("Stock updated successfully!");
            if (ImGui::Button("OK")) {
                ImGui::CloseCurrentPopup();
                show_success_popup = false; // Reset state
            }
            ImGui::EndPopup();
        }
        if (show_failure_popup && ImGui::BeginPopup("Update Failed")) {
            ImGui::Text("Failed to update stock!");
            if (ImGui::Button("OK")) {
                ImGui::CloseCurrentPopup();
                show_failure_popup = false; // Reset state
            }
            ImGui::EndPopup();
        }
        if (show_invalid_popup && ImGui::BeginPopup("Invalid Input")) {
            ImGui::Text("Remaining count cannot be negative!");
            if (ImGui::Button("OK")) {
                ImGui::CloseCurrentPopup();
                show_invalid_popup = false; // Reset state
            }
            ImGui::EndPopup();
        }
    }

    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar();
    return 1;
}

int RUserUi::StockWindow::handle_stock_print(const std::vector<std::tuple<std::string, std::string, std::string, int>>& print_data, bool has_valid_items)
{
    if (!has_valid_items) {
        return 0; // Should not be called if no valid items, but included for safety
    }

    OPENFILENAME ofn = { 0 };
    wchar_t file_name[MAX_PATH] = L"stock_report.pdf";
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetActiveWindow();
    ofn.lpstrFilter = L"PDF Files (*.pdf)\0*.pdf\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = file_name;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = L"pdf";
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

    if (GetSaveFileName(&ofn)) {
        // Convert wchar_t to char
        char file_name_mb[MAX_PATH] = { 0 };
        size_t converted_chars = 0;
        wcstombs_s(&converted_chars, file_name_mb, file_name, MAX_PATH);

        HPDF_Doc pdf = HPDF_New(nullptr, nullptr);
        if (!pdf) {
            return 0;
        }

        HPDF_Page page = HPDF_AddPage(pdf);
        HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);
        HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);

        // Load fonts (fallback to Times-Roman if Helvetica unavailable)
        HPDF_Font title_font = HPDF_GetFont(pdf, "Helvetica-Bold", nullptr);
        if (!title_font) title_font = HPDF_GetFont(pdf, "Times-Roman", nullptr);
        HPDF_Font header_font = HPDF_GetFont(pdf, "Helvetica-Bold", nullptr);
        if (!header_font) header_font = HPDF_GetFont(pdf, "Times-Roman", nullptr);
        HPDF_Font data_font = HPDF_GetFont(pdf, "Helvetica", nullptr);
        if (!data_font) data_font = HPDF_GetFont(pdf, "Times-Roman", nullptr);

        // Calculate maximum lengths for product and brand names
        size_t max_product_len = 20;
        size_t max_brand_len = 20;
        for (const auto& [product, brand, quantity, remaining] : print_data) {
            max_product_len = (std::max)(max_product_len, product.length());
            max_brand_len = (std::max)(max_brand_len, brand.length());
        }

        // Cap lengths to fit within page width (A4 ~595 points, ~500 usable)
        const float char_width = 7.0f; // Approx for Helvetica/Times-Roman at 12pt
        const float max_page_width = 500.0f;
        const size_t max_chars_per_line = static_cast<size_t>(max_page_width / char_width / 2);
        max_product_len = (std::min)(max_product_len, max_chars_per_line);
        max_brand_len = (std::min)(max_brand_len, max_chars_per_line);

        // Define column widths with padding
        const float text_padding = 12.0f;
        const float product_width = (std::max)(50.0f, static_cast<float>(max_product_len) * char_width + 2 * text_padding);
        const float brand_width = (std::max)(50.0f, static_cast<float>(max_brand_len) * char_width + 2 * text_padding);
        const float quantity_width = (std::max)(50.0f, 15.0f * char_width + 2 * text_padding);
        const float remaining_width = (std::max)(50.0f, 10.0f * char_width + 2 * text_padding);
        const float table_width = product_width + brand_width + quantity_width + remaining_width;
        const float x_offset = (HPDF_Page_GetWidth(page) - table_width) / 2;
        const float base_row_height = 20.0f;
        const float line_height = 15.0f;

        float y = HPDF_Page_GetHeight(page) - 50;
        float table_top_y = y - 40 - 30;

        // Title
        HPDF_Page_SetFontAndSize(page, title_font, 16);
        HPDF_Page_SetRGBFill(page, 0.0f, 0.2f, 0.4f);
        HPDF_Page_BeginText(page);
        const char* title = "Stock Inventory Report";
        float title_width = HPDF_Page_TextWidth(page, title);
        HPDF_Page_TextOut(page, (HPDF_Page_GetWidth(page) - title_width) / 2, y, title);
        HPDF_Page_EndText(page);
        y -= 40;

        // Timestamp
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

        // Table header
        HPDF_Page_SetFontAndSize(page, header_font, 12);
        HPDF_Page_SetRGBFill(page, 0.0f, 0.4f, 0.6f);
        HPDF_Page_BeginText(page);
        HPDF_Page_TextOut(page, x_offset + text_padding, y, "Product");
        HPDF_Page_TextOut(page, x_offset + product_width + text_padding, y, "Brand");
        HPDF_Page_TextOut(page, x_offset + product_width + brand_width + text_padding, y, "Quantity");
        HPDF_Page_TextOut(page, x_offset + product_width + brand_width + quantity_width + text_padding, y, "Remaining");
        HPDF_Page_EndText(page);

        // Header background and border
        HPDF_Page_SetRGBFill(page, 0.9f, 0.95f, 1.0f);
        HPDF_Page_SetRGBStroke(page, 0.0f, 0.2f, 0.4f);
        HPDF_Page_Rectangle(page, x_offset, y - 5, table_width, base_row_height + 10);
        HPDF_Page_FillStroke(page);

        y -= base_row_height;
        std::string current_product;
        std::string current_brand;
        bool even_row = false;
        int page_count = 1;
        float table_bottom_y = y;

        // Text splitting function
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
                    while (len > 0 && text[start + len - 1] != ' ') {
                        --len;
                    }
                    if (len == 0) len = max_chars_per_line;
                }
                lines.push_back(text.substr(start, len));
                start += len;
            }
            return lines;
        };

        for (const auto& [product, brand, quantity, remaining] : print_data) {
            const char* product_str = (product != current_product) ? product.c_str() : "";
            const char* brand_str = (brand != current_brand || product != current_product) ? brand.c_str() : "";

            std::vector<std::string> product_lines = split_text(product_str);
            std::vector<std::string> brand_lines = split_text(brand_str);
            size_t max_lines = (std::max)(product_lines.size(), brand_lines.size());
            max_lines = (std::max)(max_lines, size_t(1));
            float row_height = max_lines * line_height + 10.0f;

            if (y - row_height < 50) {
                // Add page number
                HPDF_Page_SetFontAndSize(page, data_font, 10);
                HPDF_Page_SetRGBFill(page, 0.5f, 0.5f, 0.5f);
                HPDF_Page_BeginText(page);
                char page_num[32];
                snprintf(page_num, sizeof(page_num), "Page %d", page_count);
                HPDF_Page_TextOut(page, x_offset, 30, page_num);
                HPDF_Page_EndText(page);

                // Draw vertical lines
                HPDF_Page_SetRGBStroke(page, 0.7f, 0.7f, 0.7f);
                HPDF_Page_SetLineWidth(page, 0.5);
                float x_positions[] = {
                    x_offset,
                    x_offset + product_width,
                    x_offset + product_width + brand_width,
                    x_offset + product_width + brand_width + quantity_width,
                    x_offset + product_width + brand_width + quantity_width + remaining_width
                };
                for (float x : x_positions) {
                    HPDF_Page_MoveTo(page, x, table_top_y + 5);
                    HPDF_Page_LineTo(page, x, table_bottom_y - 5);
                    HPDF_Page_Stroke(page);
                }

                // Start new page
                page = HPDF_AddPage(pdf);
                page_count++;
                HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
                y = HPDF_Page_GetHeight(page) - 50;
                table_top_y = y - 40 - 30;

                // Repeat header
                HPDF_Page_SetFontAndSize(page, header_font, 12);
                HPDF_Page_SetRGBFill(page, 0.0f, 0.4f, 0.6f);
                HPDF_Page_BeginText(page);
                HPDF_Page_TextOut(page, x_offset + text_padding, y, "Product");
                HPDF_Page_TextOut(page, x_offset + product_width + text_padding, y, "Brand");
                HPDF_Page_TextOut(page, x_offset + product_width + brand_width + text_padding, y, "Quantity");
                HPDF_Page_TextOut(page, x_offset + product_width + brand_width + quantity_width + text_padding, y, "Remaining");
                HPDF_Page_EndText(page);

                HPDF_Page_SetRGBFill(page, 0.9f, 0.95f, 1.0f);
                HPDF_Page_SetRGBStroke(page, 0.0f, 0.2f, 0.4f);
                HPDF_Page_Rectangle(page, x_offset, y - 5, table_width, base_row_height + 10);
                HPDF_Page_FillStroke(page);

                y -= base_row_height;
                table_bottom_y = y;
            }

            // Draw row background
            HPDF_Page_SetRGBFill(page, even_row ? 0.98f : 1.0f, even_row ? 0.98f : 1.0f, even_row ? 0.98f : 1.0f);
            HPDF_Page_Rectangle(page, x_offset, y - 5, table_width, row_height);
            HPDF_Page_Fill(page);
            even_row = !even_row;

            // Draw horizontal borders
            HPDF_Page_SetRGBStroke(page, 0.7f, 0.7f, 0.7f);
            HPDF_Page_SetLineWidth(page, 0.5);
            HPDF_Page_MoveTo(page, x_offset, y - 2);
            HPDF_Page_LineTo(page, x_offset + table_width, y - 2);
            HPDF_Page_MoveTo(page, x_offset, y - row_height - 2);
            HPDF_Page_LineTo(page, x_offset + table_width, y - row_height - 2);
            HPDF_Page_Stroke(page);

            // Print data
            HPDF_Page_SetFontAndSize(page, data_font, 12);

            // Product
            HPDF_Page_SetRGBFill(page, product_str[0] ? 0.0f : 0.0f, product_str[0] ? 0.5f : 0.0f, 0.0f);
            HPDF_Page_BeginText(page);
            for (size_t i = 0; i < product_lines.size(); ++i) {
                HPDF_Page_TextOut(page, x_offset + text_padding, y - (i * line_height), product_lines[i].c_str());
            }
            HPDF_Page_EndText(page);

            // Brand
            HPDF_Page_SetRGBFill(page, 0.0f, 0.0f, 0.0f);
            HPDF_Page_BeginText(page);
            for (size_t i = 0; i < brand_lines.size(); ++i) {
                HPDF_Page_TextOut(page, x_offset + product_width + text_padding, y - (i * line_height), brand_lines[i].c_str());
            }
            HPDF_Page_EndText(page);

            // Quantity
            std::string quantity_trunc = quantity.substr(0, max_chars_per_line);
            HPDF_Page_BeginText(page);
            HPDF_Page_TextOut(page, x_offset + product_width + brand_width + text_padding, y, quantity_trunc.c_str());
            HPDF_Page_EndText(page);

            // Remaining
            HPDF_Page_SetRGBFill(page, remaining == 0 ? 1.0f : 0.0f, 0.0f, 0.0f);
            HPDF_Page_BeginText(page);
            std::string remaining_str = std::to_string(remaining);
            HPDF_Page_TextOut(page, x_offset + product_width + brand_width + quantity_width + text_padding, y, remaining_str.c_str());
            HPDF_Page_EndText(page);

            y -= row_height;
            table_bottom_y = y;
            current_product = product;
            current_brand = brand;
        }

        // Draw vertical lines for final page
        HPDF_Page_SetRGBStroke(page, 0.7f, 0.7f, 0.7f);
        HPDF_Page_SetLineWidth(page, 0.5);
        float x_positions[] = {
            x_offset,
            x_offset + product_width,
            x_offset + product_width + brand_width,
            x_offset + product_width + brand_width + quantity_width,
            x_offset + product_width + brand_width + quantity_width + remaining_width
        };
        for (float x : x_positions) {
            HPDF_Page_MoveTo(page, x, table_top_y);
            HPDF_Page_LineTo(page, x, table_bottom_y - 5);
            HPDF_Page_Stroke(page);
        }

        // Final page number
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
            return 1; // Success
        }
    }

    return 0; // Failure
}

int RUserUi::StockWindow::create_stock_window()
{
    // Setup window and filters
    if (!setup_stock_window()) {
        return 0;
    }

    // Prepare print data and valid items flag
    std::vector<std::tuple<std::string, std::string, std::string, int>> print_data;
    bool has_valid_items = false;

    // Initialize vectors for edited values
    static std::vector<int> edited_remaining;
    static std::vector<bool> is_edited;
    static size_t row_index = 0;

    // State for popup
    static bool show_success_popup = false;
    static bool show_failure_popup = false;
    static bool show_invalid_popup = false;

    this->set_products();

    // Handle non-"All Products" selection
    if (this->selected_product != "All Products") {
        if (this->stock_brand_current_item == 0 && !product_changed) {
            display_all_brands(this->selected_product, this->show_low_stock, this->low_stock_threshold, this->show_out_of_stock, print_data, has_valid_items, edited_remaining, is_edited, row_index, show_success_popup, show_failure_popup, show_invalid_popup);
        }
        else if (product_changed) {
            display_all_brands(this->selected_product, this->show_low_stock, this->low_stock_threshold, this->show_out_of_stock, print_data, has_valid_items, edited_remaining, is_edited, row_index, show_success_popup, show_failure_popup, show_invalid_popup);
            this->stock_brand_current_item = 0;
            this->stock_quantity_current_item = 0;
            this->reset_combo_items(); // Reset only when product changes
            product_changed = false;
        }
        else if (this->stock_brand_current_item != 0 && this->stock_quantity_current_item == 0 && !product_changed) {
            this->get_selected_brand(this->stock_brand_current_item);
            display_brand_quantities(this->selected_product, this->selected_brand, this->show_low_stock, this->low_stock_threshold, this->show_out_of_stock, print_data, has_valid_items, edited_remaining, is_edited, row_index, show_success_popup, show_failure_popup, show_invalid_popup);
        }
        else if (this->stock_quantity_current_item != 0 && !product_changed) {
            this->get_selected_quantity(this->stock_quantity_current_item);
            display_quantity(this->selected_product, this->selected_brand, this->selected_quantity, this->show_low_stock, this->low_stock_threshold, this->show_out_of_stock, print_data, has_valid_items, edited_remaining, is_edited, row_index, show_success_popup, show_failure_popup, show_invalid_popup);
        }
    }
    else {
        // Display table and collect print data for "All Products"
        if (!display_stock_table(print_data, has_valid_items)) {
            return 0;
        }
    }

    // Resize edited vectors based on print_data
    edited_remaining.resize(print_data.size(), 0);
    is_edited.resize(print_data.size(), false);

    // Render popups
    if (show_success_popup && ImGui::BeginPopup("Update Success")) {
        ImGui::Text("Stock updated successfully!");
        if (ImGui::Button("OK")) {
            ImGui::CloseCurrentPopup();
            show_success_popup = false;
        }
        ImGui::EndPopup();
    }
    if (show_failure_popup && ImGui::BeginPopup("Update Failed")) {
        ImGui::Text("Failed to update stock!");
        if (ImGui::Button("OK")) {
            ImGui::CloseCurrentPopup();
            show_failure_popup = false;
        }
        ImGui::EndPopup();
    }
    if (show_invalid_popup && ImGui::BeginPopup("Invalid Input")) {
        ImGui::Text("Remaining count cannot be negative!");
        if (ImGui::Button("OK")) {
            ImGui::CloseCurrentPopup();
            show_invalid_popup = false;
        }
        ImGui::EndPopup();
    }

    // Render the Print button directly in the stock window
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 100.0f) * 0.5f); // Center the button
    bool button_clicked = ImGui::Button("Print", ImVec2(100.0f, 30.0f));
    if (!has_valid_items) {
        ImGui::SameLine();
        ImGui::Text("No data to print");
    }
    if (button_clicked && has_valid_items) {
        if (!handle_stock_print(print_data, has_valid_items)) {
            ImGui::OpenPopup("Print Error");
        }
    }

    // Stock Error popup
    if (ImGui::BeginPopupModal("Stock Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Invalid selection. Please choose a valid product, brand, or quantity.");
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 120.0f) * 0.5f);
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // Print feedback popups
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
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::Separator();
    ImGui::PopStyleColor(9);
    ImGui::PopStyleVar(6);
    ImGui::End();
    return 1;
}

// Initialize static members for StockWindow
int RUserUi::StockWindow::stock_brand_current_item = 0;
int RUserUi::StockWindow::stock_quantity_current_item = 0;
bool RUserUi::StockWindow::show_out_of_stock = false;
bool RUserUi::StockWindow::show_low_stock = false;
int RUserUi::StockWindow::low_stock_threshold = 5;

// Constructor for SellWindow
RUserUi::SellWindow::SellWindow(mongocxx::database db)
{
    this->products_collection = db["Products"];
    this->stock = db["Stock"];
    this->agents = db["Agents"];

    this->set_products();
    this->set_seasons();
    this->set_age_groups();
    this->set_genders();
    this->set_locations();

    // Initialize smart pointers for combo boxes
    this->p_seasons = std::make_unique<char[]>(this->seasons.size() + 1);
    std::fill_n(this->p_seasons.get(), this->seasons.size() + 1, 0);
    std::strncpy(this->p_seasons.get(), this->seasons.c_str(), this->seasons.size());
    this->prepare_items_pointer(this->seasons, '_', this->p_seasons);

    this->p_age_groups = std::make_unique<char[]>(this->age_groups.size() + 1);
    std::fill_n(this->p_age_groups.get(), this->age_groups.size() + 1, 0);
    std::strncpy(this->p_age_groups.get(), this->age_groups.c_str(), this->age_groups.size());
    this->prepare_items_pointer(this->age_groups, '_', this->p_age_groups);

    this->p_genders = std::make_unique<char[]>(this->genders.size() + 1);
    std::fill_n(this->p_genders.get(), this->genders.size() + 1, 0);
    std::strncpy(this->p_genders.get(), this->genders.c_str(), this->genders.size());
    this->prepare_items_pointer(this->genders, '_', this->p_genders);

    this->p_locations = std::make_unique<char[]>(this->locations.size() + 1);
    std::fill_n(this->p_locations.get(), this->locations.size() + 1, 0);
    std::strncpy(this->p_locations.get(), this->locations.c_str(), this->locations.size());
    this->prepare_items_pointer(this->locations, '_', this->p_locations);
}

// Set seasons list
int RUserUi::SellWindow::set_seasons()
{
    this->seasons = "Select_";
    this->seasons_array = get_seasons_array(this->products_collection);
    for (const auto& season : this->seasons_array) {
        this->seasons += season + "_";
    }
    return 1;
}

// Set age groups list
int RUserUi::SellWindow::set_age_groups()
{
    this->age_groups = "Select_";
    this->age_groups_array = get_age_groups_array(this->products_collection);
    for (const auto& age_group : this->age_groups_array) {
        this->age_groups += age_group + "_";
    }
    return 1;
}

// Set genders list
int RUserUi::SellWindow::set_genders()
{
    this->genders = "Select_";
    this->genders_array = get_genders_array(this->products_collection);
    for (const auto& gender : this->genders_array) {
        this->genders += gender + "_";
    }
    return 1;
}

// Set locations list
int RUserUi::SellWindow::set_locations()
{
    this->locations = "Select_";
    this->locations_array = get_locations_array(this->products_collection);
    for (const auto& location : this->locations_array) {
        this->locations += location + "_";
    }
    return 1;
}

// Get selected season
std::string RUserUi::SellWindow::get_selected_season() const
{
    if (this->sell_season_current_item != 0) {
        selected_season = this->seasons_array[this->sell_season_current_item - 1];
    }
    else {
        selected_season = "";
    }
    return selected_season;
}

// Get selected age group
std::string RUserUi::SellWindow::get_selected_age_group() const
{
    if (this->sell_age_group_current_item != 0) {
        selected_age_group = this->age_groups_array[this->sell_age_group_current_item - 1];
    }
    else {
        selected_age_group = "";
    }
    return selected_age_group;
}

// Get selected gender
std::string RUserUi::SellWindow::get_selected_gender() const
{
    if (this->sell_gender_current_item != 0) {
        selected_gender = this->genders_array[this->sell_gender_current_item - 1];
    }
    else {
        selected_gender = "";
    }
    return selected_gender;
}

// Get selected location
std::string RUserUi::SellWindow::get_selected_location() const
{
    if (this->sell_location_current_item != 0) {
        selected_location = this->locations_array[this->sell_location_current_item - 1];
    }
    else {
        selected_location = "";
    }
    return selected_location;
}

// Get number of items
int RUserUi::SellWindow::get_number_of_items() const
{
    return this->number_of_items;
}

// Populate brands for a product
int RUserUi::SellWindow::populate_brands(const std::string& product)
{
    this->brands_array = get_product_brands_array(this->products_collection, product);
    this->brands = "Select_";
    for (const auto& my_brand : this->brands_array) {
        this->brands += my_brand + "_";
    }

    this->p_brands = std::make_unique<char[]>(this->brands.size() + 1);
    std::fill_n(this->p_brands.get(), this->brands.size() + 1, 0);
    std::strncpy(this->p_brands.get(), this->brands.c_str(), this->brands.size());
    this->prepare_items_pointer(this->brands, '_', this->p_brands);
    return 1;
}

// Populate quantities for a brand
int RUserUi::SellWindow::populate_quantities(const std::string& brand)
{
    this->quantities_array = get_brand_quantities_array(this->products_collection, brand);
    this->quantities = "Select_";
    for (const auto& my_quantity : this->quantities_array) {
        this->quantities += my_quantity + "_";
    }

    this->p_quantities = std::make_unique<char[]>(this->quantities.size() + 1);
    std::fill_n(this->p_quantities.get(), this->quantities.size() + 1, 0);
    std::strncpy(this->p_quantities.get(), this->quantities.c_str(), this->quantities.size());
    this->prepare_items_pointer(this->quantities, '_', this->p_quantities);
    return 1;
}

int RUserUi::SellWindow::create_sell_window()
{
    // Apply modern styling without affecting data logic
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(40, 40, 45, 255)); // Lighter dark background
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(60, 60, 65, 255)); // Input background
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(80, 80, 85, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(100, 100, 105, 255));
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 130, 180, 255)); // Teal button
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 160, 210, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0, 100, 150, 255));
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(230, 230, 230, 255)); // Brighter text
    ImGui::PushStyleColor(ImGuiCol_Separator, IM_COL32(80, 80, 85, 255)); // Subtle separator
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f); // Rounded window
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f); // Rounded inputs/buttons
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f)); // Balanced padding
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 6.0f)); // Input padding
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f); // Subtle input borders
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 6.0f)); // Reduced vertical spacing

    // Center and size the window (increased height to fit all content)
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f - 260.0f, 20.0f), ImGuiCond_Once, ImVec2(0.0f, 0.0f));
    float maxHeight = io.DisplaySize.y - 25.0f; // Ensure height doesn't exceed display height minus top offset
    ImGui::SetNextWindowSize(ImVec2(520.0f, (std::min)(650.0f, maxHeight)), ImGuiCond_Always);

    ImGui::Begin("Create Sale", &this->show_window, this->window_flags);

    // Title
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]); // Large font
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Create Sale").x) * 0.5f);
    ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "Create Sale");
    ImGui::PopFont();
    ImGui::Dummy(ImVec2(0.0f, 8.0f)); // Reduced spacing
    ImGui::Separator();

    // Filters layout (two-column grid, labels on one line, selects on next)
    const ImVec2 filter_size = ImVec2(230.0f, 150.0f); // Reduced list box height
    const ImVec2 combo_size = ImVec2(230.0f, 30.0f); // Compact combo height
    const float column_offset = (ImGui::GetWindowSize().x - filter_size.x - combo_size.x - 15.0f) * 0.5f;
    const float center_offset = (ImGui::GetWindowSize().x - filter_size.x) * 0.5f;

    // Product list box
    ImGui::SetCursorPosX(center_offset);
    ImGui::Text("Select Product");
    ImGui::SetCursorPosX(center_offset);
    this->set_products();
    create_listbox_filter(this->products, "Selected", this->selected_product, '_', filter_size); // Empty label

    // Brand and Quantity (first row)
    ImGui::Dummy(ImVec2(0.0f, 15.0f));
    ImGui::SetCursorPosX(column_offset);
    ImGui::Text("Select Brand");
    ImGui::SameLine(0.0f, 15.0f + filter_size.x);
    ImGui::Text("Select Quantity");
    ImGui::SetCursorPosX(column_offset);
    if (this->selected_product != "All Products") {
        this->populate_brands(this->selected_product);
    } else {
        this->reset_combo_items();
        this->sell_brand_current_item = 0;
        this->sell_quantity_current_item = 0;
        this->prices_valid = false; // Reset prices when no product is selected
        this->default_selling_price = 0.0;
        this->buying_price = 0.0;
        this->entered_sale_price = 0.0;
    }
    create_combo(this->p_brands, "##Brand", this->sell_brand_current_item, combo_size);
    ImGui::SameLine(0.0f, 15.0f);
    if (this->sell_brand_current_item != 0) {
        this->get_selected_brand(this->sell_brand_current_item);
        this->populate_quantities(this->selected_product + "_" + this->selected_brand);
    }
    create_combo(this->p_quantities, "##Quantity", this->sell_quantity_current_item, combo_size);

    // Fetch prices only if a valid quantity is selected
    if (this->sell_quantity_current_item != 0 && !this->get_selected_quantity(this->sell_quantity_current_item).empty()) {
        SALE temp_sale;
        temp_sale.product_name = this->selected_product;
        temp_sale.brand = this->selected_product + "_" + this->selected_brand;
        temp_sale.quantity = this->get_selected_quantity(this->sell_quantity_current_item);
        try {
            this->default_selling_price = get_quantity_unit_cost(this->products_collection, temp_sale, "SellingPrice");
            this->buying_price = get_quantity_unit_cost(this->stock, temp_sale, "BuyingPrice");
            if (this->entered_sale_price == 0.0 || !this->prices_valid) {
                this->entered_sale_price = this->default_selling_price; // Initialize only if not set
            }
            this->prices_valid = true;
        } catch (const std::runtime_error& e) {
            this->prices_valid = false;
            this->default_selling_price = 0.0;
            this->buying_price = 0.0;
            this->entered_sale_price = 0.0;
            this->input_error = true; // Trigger error popup for failed price fetch
        }
    } else {
        this->prices_valid = false;
        this->default_selling_price = 0.0;
        this->buying_price = 0.0;
        this->entered_sale_price = 0.0;
    }

    // Season and Age Group (second row)
    ImGui::Dummy(ImVec2(0.0f, 15.0f));
    ImGui::SetCursorPosX(column_offset);
    ImGui::Text("Season");
    ImGui::SameLine(0.0f, 15.0f + filter_size.x);
    ImGui::Text("Age Group");
    ImGui::SetCursorPosX(column_offset);
    create_combo(this->p_seasons, "##Season", this->sell_season_current_item, combo_size);
    ImGui::SameLine(0.0f, 15.0f);
    create_combo(this->p_age_groups, "##AgeGroup", this->sell_age_group_current_item, combo_size);

    // Gender and Location (third row)
    ImGui::Dummy(ImVec2(0.0f, 15.0f));
    ImGui::SetCursorPosX(column_offset);
    ImGui::Text("Gender");
    ImGui::SameLine(0.0f, 15.0f + filter_size.x);
    ImGui::Text("Location");
    ImGui::SetCursorPosX(column_offset);
    create_combo(this->p_genders, "##Gender", this->sell_gender_current_item, combo_size);
    ImGui::SameLine(0.0f, 15.0f);
    create_combo(this->p_locations, "##Location", this->sell_location_current_item, combo_size);

    // Quantity to Sell input
    ImGui::Dummy(ImVec2(0.0f, 15.0f));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 150.0f) * 0.5f);
    ImGui::Text("Quantity to Sell");
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 150.0f) * 0.5f);
    ImGui::SetNextItemWidth(150.0f);
    ImGui::InputInt("##QuantityToSell", &this->number_of_items, 1, 10);

    // Sale Price input (only shown if prices are valid)
    if (this->prices_valid) {
        ImGui::Dummy(ImVec2(0.0f, 15.0f));
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 200.0f) * 0.5f);
        // Change text color to red if price is below minimum
        if (this->entered_sale_price < this->buying_price) {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 80, 80, 255)); // Red text
        }
        ImGui::Text("Sale Price (Selling: %.2f, Buying: %.2f)", this->default_selling_price, this->buying_price);
        if (this->entered_sale_price < this->buying_price) {
            ImGui::PopStyleColor();
        }
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 150.0f) * 0.5f);
        ImGui::SetNextItemWidth(150.0f);
        ImGui::InputDouble("##SalePrice", &this->entered_sale_price, 0.0f, 0.0f, "%.2f");
        // Clamp value to minimum during editing, but don't trigger popup
        if (this->entered_sale_price < this->buying_price) {
            this->entered_sale_price = this->buying_price;
        }
    }

    // Buttons
    ImGui::Dummy(ImVec2(0.0f, 15.0f));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 330.0f) * 0.5f);
    if (ImGui::Button("Confirm Sale", ImVec2(150.0f, 40.0f))) {
        SALE sale;
        sale.product_name = this->get_selected_product();
        sale.brand = sale.product_name + "_" + this->get_selected_brand(this->sell_brand_current_item);
        sale.quantity = this->get_selected_quantity(this->sell_quantity_current_item);
        sale.season = this->get_selected_season();
        sale.age_group = this->get_selected_age_group();
        sale.gender = this->get_selected_gender();
        sale.location = this->get_selected_location();
        sale.item_count = this->get_number_of_items();
        sale.sale_price = this->entered_sale_price; // Use the latest entered price

        int available_items = 0;
        if (!sale.quantity.empty()) {
            available_items = get_quantity_total(this->stock, sale);
        }

        if (sale.product_name.empty() || sale.brand.empty() || sale.quantity.empty() ||
            sale.season.empty() || sale.age_group.empty() || sale.gender.empty() ||
            sale.location.empty() || sale.item_count <= 0 || available_items == 0 ||
            sale.item_count > available_items || !this->prices_valid || 
            sale.sale_price < this->buying_price) {
            this->input_error = true;
        } else {
            change_amountOf_item(this->products_collection, sale, UPDATE);
            update_stock_amountOf_item(this->stock, sale, UPDATE);
            this->success = true;
        }
    }

    ImGui::SameLine(0.0f, 20.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(200, 50, 50, 255)); // Red cancel button
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(230, 80, 80, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(170, 30, 30, 255));
    if (ImGui::Button("Cancel", ImVec2(150.0f, 40.0f))) {
        this->show_window = false; // Close window
    }
    ImGui::PopStyleColor(3);

    // Handle error or success popups
    if (this->input_error) {
        ImGui::OpenPopup("Sale Error");
        this->input_error = false;
    }
    if (ImGui::BeginPopupModal("Sale Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Sale failed. Please ensure all fields are filled, sufficient stock is available, and sale price is at least %.2f.", this->buying_price);
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 120.0f) * 0.5f);
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    if (this->success) {
        ImGui::OpenPopup("Sale Success");
        this->success = false;
    }
    if (ImGui::BeginPopupModal("Sale Success", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Sale completed successfully!");
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 120.0f) * 0.5f);
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::PopStyleColor(9);
    ImGui::PopStyleVar(6);
    ImGui::End();
    return 1;
}


// Initialize static members for SellWindow
int RUserUi::SellWindow::sell_brand_current_item = 0;
int RUserUi::SellWindow::sell_quantity_current_item = 0;
int RUserUi::SellWindow::sell_season_current_item = 1;
int RUserUi::SellWindow::sell_age_group_current_item = 1;
int RUserUi::SellWindow::sell_gender_current_item = 1;
int RUserUi::SellWindow::sell_location_current_item = 1;
bool RUserUi::SellWindow::input_error = false;
bool RUserUi::SellWindow::success = false;
int RUserUi::SellWindow::number_of_items = 0;

// Handle error popup
int RUserUi::SellWindow::handle_error(bool& err_flag)
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(50, 50, 50, 255)); // Dark popup background
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(255, 80, 80, 255)); // Red border
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 200, 200, 255)); // Light red text
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 150, 80, 255)); // Green button
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 180, 100, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0, 120, 60, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 5.0f));

    ImGui::OpenPopup("Error");
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]); // Large font
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Invalid Input").x) * 0.5f);
        ImGui::Text("Invalid Input");
        ImGui::PopFont();
        ImGui::Separator();
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Please fill in all details and ensure stock availability.").x) * 0.5f);
        ImGui::Text("Please fill in all details and ensure stock availability.");

        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 120.0f) * 0.5f);
        if (ImGui::Button("OK", ImVec2(120.0f, 30.0f))) {
            err_flag = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::PopStyleColor(6);
    ImGui::PopStyleVar(4);
    return 1;
}

// Notify sale status popup
int RUserUi::SellWindow::notify_sale_status()
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(50, 50, 50, 255)); // Dark popup background
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 150, 80, 255)); // Green border
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 255, 200, 255)); // Light green text
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 150, 80, 255)); // Green button
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 180, 100, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0, 120, 60, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 5.0f));

    ImGui::OpenPopup("Success");
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Success", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]); // Large font
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Sale Successful").x) * 0.5f);
        ImGui::Text("Sale Successful");
        ImGui::PopFont();
        ImGui::Separator();
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Transaction completed.").x) * 0.5f);
        ImGui::Text("Transaction completed.");

        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 120.0f) * 0.5f);
        if (ImGui::Button("OK", ImVec2(120.0f, 30.0f))) {
            this->success = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::PopStyleColor(6);
    ImGui::PopStyleVar(4);
    return 1;
}

int RUserUi::Window::notify_sale_status(bool& success_flag)
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

bool RUserUi::Window::check_unwanted_characters(std::string entered_string)
{
    for (char c : entered_string)
    {
        auto iterator = std::find(this->unwanted_chars.begin(), this->unwanted_chars.end(), c);

        if (iterator != this->unwanted_chars.end())
            return true;
    }

    return false;
}


bool RUserUi::Window::check_for_duplicate(std::vector<std::string> items, std::string item)
{
    std::vector<std::string> items2;
    items2.reserve(items.size());

    std::transform(items.begin(), items.end(), std::back_inserter(items2),
        [](const std::string& in)
        {
            std::string out;
            out.reserve(in.size());
            std::transform(in.begin(), in.end(), std::back_inserter(out), ::tolower);
            return out;
        });

    std::transform(item.begin(), item.end(), item.begin(),
        [](unsigned char c)
        {
            return std::tolower(c);
        });

    auto count = std::count(items2.begin(), items2.end(), item);

    if (count > 0)
        return true;

    return false;
}

// Initialize static members for ReturnSaleWindow
int RUserUi::ReturnSaleWindow::return_current_filter_type_index = 0;

// Constructor for ReturnSaleWindow
RUserUi::ReturnSaleWindow::ReturnSaleWindow(mongocxx::database db) : SellWindow(db), AgentsWindow(db)
{
    this->products_collection = db["Products"];
    this->stock = db["Stock"];
    this->agents = db["Agents"];

    // Initialize table flags
    table_flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Reorderable;
    TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

    // Initialize sales filter types with smart pointer
    this->p_sales_filter_types = std::make_unique<char[]>(this->sales_filter_types.size() + 1);
    std::fill_n(this->p_sales_filter_types.get(), this->sales_filter_types.size() + 1, 0);
    std::strncpy(this->p_sales_filter_types.get(), this->sales_filter_types.c_str(), this->sales_filter_types.size());
    this->prepare_items_pointer(this->sales_filter_types, '_', this->p_sales_filter_types);
}

// Create return sale window with styled layout
int RUserUi::ReturnSaleWindow::create_return_sale_window()
{
    // Apply modern styling
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(45, 45, 45, 255)); // Slightly darker background
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(60, 60, 60, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(80, 80, 80, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(100, 100, 100, 255));
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 150, 80, 255)); // Green button
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 180, 100, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0, 120, 60, 255));
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 5.0f));

    // Center and size the window
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f - 500.0f, io.DisplaySize.y * 0.5f - 300.0f), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(1000.0f, 600.0f), ImGuiCond_Once);

    ImGui::Begin("Return Sale", &this->show_window, this->window_flags | ImGuiWindowFlags_NoMove);

    // Title
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]); // Large font
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Return Sale").x) * 0.5f);
    ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.4f, 1.0f), "Return Sale");
    ImGui::PopFont();
    ImGui::Separator();

    // Fetch sales data
    this->sales_data = get_sales_data(this->products_collection);
    this->sales_count = this->sales_data.size();

    // Populate sales data vectors
    this->sales_product_names.clear();
    this->sales_brands.clear();
    this->sales_quantities.clear();
    this->sales_seasons.clear();
    this->sales_age_groups.clear();
    this->sales_genders.clear();
    this->sales_locations.clear();
    this->sales_item_counts.clear();
    this->sales_dates_and_times.clear();
    this->sales_indices.clear();

    for (size_t i = 0; i < this->sales_data.size(); ++i) {
        this->sales_product_names.push_back(this->sales_data[i][0]);
        this->sales_brands.push_back(this->sales_data[i][1]);
        this->sales_quantities.push_back(this->sales_data[i][2]);
        this->sales_seasons.push_back(this->sales_data[i][3]);
        this->sales_age_groups.push_back(this->sales_data[i][4]);
        this->sales_genders.push_back(this->sales_data[i][5]);
        this->sales_locations.push_back(this->sales_data[i][6]);
        this->sales_item_counts.push_back(this->sales_data[i][7]);
        this->sales_dates_and_times.push_back(this->sales_data[i][8]);
        this->sales_indices.push_back(std::to_string(i));
    }

    if (!this->sales_product_names.empty()) {
        // Filter combo
        const ImVec2 combo_size = ImVec2(300.0f, 50.0f);
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - combo_size.x) * 0.5f);
        create_combo(this->p_sales_filter_types, "Filter Criteria", this->return_current_filter_type_index, combo_size);

        // Filter data
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        if (this->return_current_filter_type_index != 0) {
            create_sales_data_filter();
        }
        else {
            this->filtered_sales_product_names = this->sales_product_names;
            this->filtered_sales_brands = this->sales_brands;
            this->filtered_sales_quantities = this->sales_quantities;
            this->filtered_sales_seasons = this->sales_seasons;
            this->filtered_sales_age_groups = this->sales_age_groups;
            this->filtered_sales_genders = this->sales_genders;
            this->filtered_sales_locations = this->sales_locations;
            this->filtered_sales_item_counts = this->sales_item_counts;
            this->filtered_sales_dates_and_times = this->sales_dates_and_times;
        }

        // Table title
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Sales Data").x) * 0.5f);
        ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.4f, 1.0f), "Sales Data");
        ImGui::Separator();

        // Sales table
        draw_sales_data_table(this->TEXT_BASE_HEIGHT);

        // Confirmation handling
        if (this->confirm == confirm_state::ACCEPTED) {
            for (size_t i = 0; i < this->checked_sales.size(); ++i) {
                if (this->checked_sales[i]) {
                    SALE sale;
                    sale.product_name = this->filtered_sales_product_names[i];
                    sale.brand = this->filtered_sales_brands[i];
                    sale.quantity = this->filtered_sales_quantities[i];
                    sale.season = this->filtered_sales_seasons[i];
                    sale.age_group = this->filtered_sales_age_groups[i];
                    sale.gender = this->filtered_sales_genders[i];
                    sale.location = this->filtered_sales_locations[i];
                    sale.item_count = std::stoi(this->filtered_sales_item_counts[i]);

                    change_amountOf_item(this->products_collection, sale, UPDATE);
                    update_stock_amountOf_item(this->stock, sale, UPDATE);
                    long m_index = std::stol(this->filtered_sales_indices[i]);
                    delete_sale_data(this->products_collection, size_t(m_index));
                }
            }
            this->success = true;
            this->show_confirm_window = false;
            this->confirm = confirm_state::NONE;
            this->checked_sales.clear();
            this->return_current_filter_type_index = 0;
        }
        else if (this->confirm == confirm_state::REJECTED) {
            this->show_confirm_window = false;
            this->confirm = confirm_state::NONE;
        }

        // Popups
        if (this->show_confirm_window) {
            regular_user_confirm_message(this->confirm);
        }
        else if (this->input_error) {
            handle_error(this->input_error);
        }
        else if (this->success) {
            ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(50, 50, 50, 255)); // Dark popup background
            ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 150, 80, 255)); // Green border
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 255, 200, 255)); // Light green text
            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 150, 80, 255)); // Green button
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 180, 100, 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0, 120, 60, 255));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 5.0f));

            ImGui::OpenPopup("Success");
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            if (ImGui::BeginPopupModal("Success", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]); // Large font
                ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Return Successful").x) * 0.5f);
                ImGui::Text("Return Successful");
                ImGui::PopFont();
                ImGui::Separator();
                ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Selected items have been returned.").x) * 0.5f);
                ImGui::Text("Selected items have been returned.");

                ImGui::Dummy(ImVec2(0.0f, 10.0f));
                ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 120.0f) * 0.5f);
                if (ImGui::Button("OK", ImVec2(120.0f, 30.0f))) {
                    this->success = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            ImGui::PopStyleColor(6);
            ImGui::PopStyleVar(4);
        }

        // Return button
        ImGui::Dummy(ImVec2(0.0f, 15.0f));
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 150.0f) * 0.5f);
        if (ImGui::Button("Return Selected", ImVec2(150.0f, 40.0f))) {
            bool any_selected = false;
            for (const auto& checked : this->checked_sales) {
                if (checked) {
                    any_selected = true;
                    break;
                }
            }
            if (any_selected) {
                this->show_confirm_window = true;
            }
            else {
                this->input_error = true;
            }
        }
    }
    else {
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("No Sales Found").x) * 0.5f);
        ImGui::Text("No Sales Found");
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
    }

    ImGui::PopStyleColor(8);
    ImGui::PopStyleVar(4);
    ImGui::End();
    return 1;
}

// Create sales data filter
int RUserUi::ReturnSaleWindow::create_sales_data_filter()
{
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(50, 50, 50, 255)); // Darker child background
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(70, 70, 70, 255)); // Combo background
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255)); // Light text
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f); // Rounded frame

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 300.0f) * 0.5f);
    ImGui::BeginChild("Search##SalesFilter", ImVec2(300.0f, 50.0f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    static ImGuiTextFilter filter;
    filter.Draw("Search##SalesFilter");

    size_t count = this->sales_product_names.size();

    this->filtered_sales_product_names.clear();
    this->filtered_sales_brands.clear();
    this->filtered_sales_quantities.clear();
    this->filtered_sales_seasons.clear();
    this->filtered_sales_age_groups.clear();
    this->filtered_sales_genders.clear();
    this->filtered_sales_locations.clear();
    this->filtered_sales_item_counts.clear();
    this->filtered_sales_dates_and_times.clear();
    this->filtered_sales_indices.clear();

    for (size_t item_num = 0; item_num < count; ++item_num) {
        if (this->return_current_filter_type_index == 1) { // Filter by Name
            if (filter.PassFilter(this->sales_product_names[item_num].c_str())) {
                this->filtered_sales_product_names.push_back(this->sales_product_names[item_num]);
                this->filtered_sales_brands.push_back(this->sales_brands[item_num]);
                this->filtered_sales_quantities.push_back(this->sales_quantities[item_num]);
                this->filtered_sales_seasons.push_back(this->sales_seasons[item_num]);
                this->filtered_sales_age_groups.push_back(this->sales_age_groups[item_num]);
                this->filtered_sales_genders.push_back(this->sales_genders[item_num]);
                this->filtered_sales_locations.push_back(this->sales_locations[item_num]);
                this->filtered_sales_item_counts.push_back(this->sales_item_counts[item_num]);
                this->filtered_sales_dates_and_times.push_back(this->sales_dates_and_times[item_num]);
                this->filtered_sales_indices.push_back(this->sales_indices[item_num]);
            }
        }
        else if (this->return_current_filter_type_index == 2) { // Filter by Location
            if (filter.PassFilter(this->sales_locations[item_num].c_str())) {
                this->filtered_sales_product_names.push_back(this->sales_product_names[item_num]);
                this->filtered_sales_brands.push_back(this->sales_brands[item_num]);
                this->filtered_sales_quantities.push_back(this->sales_quantities[item_num]);
                this->filtered_sales_seasons.push_back(this->sales_seasons[item_num]);
                this->filtered_sales_age_groups.push_back(this->sales_age_groups[item_num]);
                this->filtered_sales_genders.push_back(this->sales_genders[item_num]);
                this->filtered_sales_locations.push_back(this->sales_locations[item_num]);
                this->filtered_sales_item_counts.push_back(this->sales_item_counts[item_num]);
                this->filtered_sales_dates_and_times.push_back(this->sales_dates_and_times[item_num]);
                this->filtered_sales_indices.push_back(this->sales_indices[item_num]);
            }
        }
        else if (this->return_current_filter_type_index == 3) { // Filter by Date&Time
            if (filter.PassFilter(this->sales_dates_and_times[item_num].c_str())) {
                this->filtered_sales_product_names.push_back(this->sales_product_names[item_num]);
                this->filtered_sales_brands.push_back(this->sales_brands[item_num]);
                this->filtered_sales_quantities.push_back(this->sales_quantities[item_num]);
                this->filtered_sales_seasons.push_back(this->sales_seasons[item_num]);
                this->filtered_sales_age_groups.push_back(this->sales_age_groups[item_num]);
                this->filtered_sales_genders.push_back(this->sales_genders[item_num]);
                this->filtered_sales_locations.push_back(this->sales_locations[item_num]);
                this->filtered_sales_item_counts.push_back(this->sales_item_counts[item_num]);
                this->filtered_sales_dates_and_times.push_back(this->sales_dates_and_times[item_num]);
                this->filtered_sales_indices.push_back(this->sales_indices[item_num]);
            }
        }
    }

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();
    ImGui::EndChild();
    return 1;
}

// Draw sales data table with styled appearance
int RUserUi::ReturnSaleWindow::draw_sales_data_table(float text_base_height)
{
    ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, IM_COL32(0, 150, 80, 255)); // Green header
    ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, IM_COL32(80, 80, 80, 255)); // Dark border
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255)); // Light text

    if (this->checked_sales.size() < this->filtered_sales_product_names.size()) {
        this->checked_sales.resize(this->filtered_sales_product_names.size(), false);
    }

    if (ImGui::BeginTable("SalesTable", 10, this->table_flags, ImVec2(0.0f, text_base_height * 10))) {
        ImGui::TableSetupColumn("Select", ImGuiTableColumnFlags_WidthFixed, 50.0f);
        ImGui::TableSetupColumn("Product", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Brand", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Quantity", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Season", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Age Group", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Gender", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Location", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Items", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Date & Time", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin((int)this->filtered_sales_product_names.size());
        while (clipper.Step()) {
            for (size_t row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::PushID(static_cast<int>(row));
                // Checkbox column
                bool temp_checked = checked_sales[row]; // Copy to temporary bool
                if (ImGui::Checkbox("##select", &temp_checked))
                {
                    checked_sales[row] = temp_checked; // Update vector
                }
                ImGui::PopID();
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", this->filtered_sales_product_names[row].c_str());
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%s", this->filtered_sales_brands[row].c_str());
                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%s", this->filtered_sales_quantities[row].c_str());
                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%s", this->filtered_sales_seasons[row].c_str());
                ImGui::TableSetColumnIndex(5);
                ImGui::Text("%s", this->filtered_sales_age_groups[row].c_str());
                ImGui::TableSetColumnIndex(6);
                ImGui::Text("%s", this->filtered_sales_genders[row].c_str());
                ImGui::TableSetColumnIndex(7);
                ImGui::Text("%s", this->filtered_sales_locations[row].c_str());
                ImGui::TableSetColumnIndex(8);
                ImGui::Text("%s", this->filtered_sales_item_counts[row].c_str());
                ImGui::TableSetColumnIndex(9);
                ImGui::Text("%s", this->filtered_sales_dates_and_times[row].c_str());
            }
        }
        ImGui::EndTable();
    }

    ImGui::PopStyleColor(3);
    return 1;
}

// Create confirmation message popup
int RUserUi::ReturnSaleWindow::regular_user_confirm_message(int& confirm_flag)
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(50, 50, 50, 255)); // Dark popup background
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(255, 165, 0, 255)); // Orange border
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255)); // Light text
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 150, 80, 255)); // Green button
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 180, 100, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0, 120, 60, 255));
    ImGui::PushStyleColor(ImGuiCol_Button + 1, IM_COL32(220, 50, 50, 255)); // Red cancel button
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered + 1, IM_COL32(255, 80, 80, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive + 1, IM_COL32(180, 30, 30, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 5.0f));

    ImGui::OpenPopup("Confirm Return");
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Confirm Return", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]); // Large font
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Confirm Return").x) * 0.5f);
        ImGui::Text("Confirm Return");
        ImGui::PopFont();
        ImGui::Separator();
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Are you sure you want to return the selected items?").x) * 0.5f);
        ImGui::Text("Are you sure you want to return the selected items?");

        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 260.0f) * 0.5f);
        if (ImGui::Button("Confirm", ImVec2(120.0f, 30.0f))) {
            confirm_flag = confirm_state::ACCEPTED;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine(0.0f, 20.0f);
        if (ImGui::Button("Cancel", ImVec2(120.0f, 30.0f))) {
            confirm_flag = confirm_state::REJECTED;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::PopStyleColor(9);
    ImGui::PopStyleVar(4);
    return 1;
}


RUserUi::SalesReportWindow::SalesReportWindow(mongocxx::database db)
{
    this->products_collection = db["Products"];

    // Initialize dates to today
    time_t now = time(nullptr);
    start_date = *localtime(&now);
    end_date = *localtime(&now);
    start_date.tm_hour = 0;
    start_date.tm_min = 0;
    start_date.tm_sec = 0;
    end_date.tm_hour = 23;
    end_date.tm_min = 59;
    end_date.tm_sec = 59;
}
int RUserUi::SalesReportWindow::create_sales_report_window()
{
    // Restore original dark background with modern styling
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(40, 40, 45, 255)); // Restored dark background
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(60, 60, 65, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(80, 80, 85, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(100, 100, 105, 255));
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(50, 150, 250, 255)); // Vibrant blue for contrast
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(80, 180, 255, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(30, 120, 200, 255));
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(230, 230, 230, 255)); // Light text for dark background
    ImGui::PushStyleColor(ImGuiCol_Separator, IM_COL32(80, 80, 85, 255));
    ImGui::PushStyleColor(ImGuiCol_PopupBg, IM_COL32(50, 50, 55, 255)); // Slightly lighter popup background
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 8.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, 8.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 8.0f);

    // Responsive window size
    ImGuiIO& io = ImGui::GetIO();
    float window_width = (std::min)(600.0f, io.DisplaySize.x * 0.8f);
    float window_height = 450.0f;
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f - window_width * 0.5f, io.DisplaySize.y * 0.5f - window_height * 0.5f), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(window_width, window_height), ImGuiCond_Always);

    ImGui::Begin("Sales Report", &this->show_window, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

    // Title with larger font and gradient effect
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.45f, 0.85f, 1.0f));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Sales Report Generator").x) * 0.5f);
    ImGui::Text("Sales Report Generator");
    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::Dummy(ImVec2(0.0f, 12.0f));
    ImGui::Separator();

    // Date range selection with grouping
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
    ImGui::Text("Select Date Range");
    ImGui::Dummy(ImVec2(0.0f, 8.0f));

    // Combo box arrays
    static const char* days[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31"};
    static const char* months[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
    static const char* years[100];
    static bool years_initialized = false;
    if (!years_initialized) {
        static char year_buffers[100][5];
        for (int i = 0; i < 100; ++i) {
            snprintf(year_buffers[i], sizeof(year_buffers[i]), "%d", 2000 + i);
            years[i] = year_buffers[i];
        }
        years_initialized = true;
    }

    // Start Date
    ImGui::BeginGroup();
    ImGui::Text("Start Date");
    ImGui::SetNextItemWidth(100.0f);
    int start_day_index = start_date.tm_mday - 1;
    if (ImGui::Combo("##StartDay", &start_day_index, days, IM_ARRAYSIZE(days))) {
        start_date.tm_mday = start_day_index + 1;
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(120.0f);
    if (ImGui::Combo("##StartMonth", &start_date.tm_mon, months, IM_ARRAYSIZE(months))) {
        // Update month
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80.0f);
    int start_year_index = start_date.tm_year - 100;
    if (ImGui::Combo("##StartYear", &start_year_index, years, IM_ARRAYSIZE(years))) {
        start_date.tm_year = start_year_index + 100;
    }
    ImGui::EndGroup();

    // End Date
    ImGui::Dummy(ImVec2(0.0f, 12.0f));
    ImGui::BeginGroup();
    ImGui::Text("End Date");
    ImGui::SetNextItemWidth(100.0f);
    int end_day_index = end_date.tm_mday - 1;
    if (ImGui::Combo("##EndDay", &end_day_index, days, IM_ARRAYSIZE(days))) {
        end_date.tm_mday = end_day_index + 1;
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(120.0f);
    if (ImGui::Combo("##EndMonth", &end_date.tm_mon, months, IM_ARRAYSIZE(months))) {
        // Update month
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80.0f);
    int end_year_index = end_date.tm_year - 100;
    if (ImGui::Combo("##EndYear", &end_year_index, years, IM_ARRAYSIZE(years))) {
        end_date.tm_year = end_year_index + 100;
    }
    ImGui::EndGroup();

    // Real-time date validation
    time_t start_time = mktime(&start_date);
    time_t end_time = mktime(&end_date);
    date_valid = (start_time != -1 && end_time != -1 && start_time <= end_time);
    if (!date_valid) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.1f, 0.1f, 1.0f));
        ImGui::TextWrapped("Please ensure the end date is after the start date and both are valid.");
        ImGui::PopStyleColor();
    }

    // Generate PDF button (tooltip removed)
    ImGui::Dummy(ImVec2(0.0f, 20.0f));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 200.0f) * 0.5f);
    ImGui::BeginDisabled(!date_valid);
    if (ImGui::Button("Generate PDF Report", ImVec2(180.0f, 40.0f))) {
        std::vector<std::tuple<std::string, std::string, std::string, int, double>> sales_data;
        if (fetch_sales_data(sales_data, start_time, end_time) && !sales_data.empty()) {
            if (handle_sales_print(sales_data)) {
                this->success = true;
            } else {
                this->input_error = true;
            }
        } else {
            this->input_error = true;
        }
    }
    ImGui::EndDisabled();

    // Help text
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f)); // Adjusted for dark background
    ImGui::TextWrapped("Select a date range and click 'Generate PDF Report' to create a sales report.");
    ImGui::PopStyleColor();

    // Popups with icons
    if (this->input_error) {
        ImGui::OpenPopup("Report Error");
        this->input_error = false;
    }
    if (ImGui::BeginPopupModal("Report Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.1f, 0.1f, 1.0f));
        ImGui::Text("Error: No sales found or failed to save PDF.");
        ImGui::PopStyleColor();
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 120.0f) * 0.5f);
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (this->success) {
        ImGui::OpenPopup("Report Success");
        this->success = false;
    }
    if (ImGui::BeginPopupModal("Report Success", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.7f, 0.1f, 1.0f));
        ImGui::Text("Success: Report saved to %s", this->last_saved_file.c_str());
        ImGui::PopStyleColor();
        ImGui::TextWrapped("Open the PDF to print or review.");
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 250.0f) * 0.5f);
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("View PDF", ImVec2(120, 0))) {
            wchar_t file_name_w[MAX_PATH] = {0};
            size_t converted_chars = 0;
            mbstowcs_s(&converted_chars, file_name_w, this->last_saved_file.c_str(), MAX_PATH);
            ShellExecute(NULL, L"open", file_name_w, NULL, NULL, SW_SHOWNORMAL);
        }
        ImGui::EndPopup();
    }

    ImGui::PopFont();
    ImGui::PopStyleColor(10);
    ImGui::PopStyleVar(7);
    ImGui::End();
    return 1;
}

bool RUserUi::SalesReportWindow::fetch_sales_data(std::vector<std::tuple<std::string, std::string, std::string, int, double>>& sales_data, time_t start, time_t end)
{
    auto all_sales = get_sales_data(this->products_collection);
    if (all_sales.empty() || all_sales[0].empty()) {
        return false;
    }
    // all_sales indices: 0=Product, 1=Brand, 2=Quantity, 3=Season, 4=Age Group, 5=Gender, 6=Location, 7=Item Count, 8=Index, 9=DateTime, 10=Sale Price
    for (size_t i = 0; i < all_sales[0].size(); ++i) {
        std::string date_time = all_sales[9][i]; // e.g., "Thu May 8 21:43:05 2025"
        struct tm sale_time = {};
        std::istringstream ss(date_time);
        ss >> std::get_time(&sale_time, "%a %b %d %H:%M:%S %Y");
        if (ss.fail()) {
            continue; // Skip invalid dates
        }
        time_t sale_timestamp = mktime(&sale_time);
        if (sale_timestamp == -1 || sale_timestamp < start || sale_timestamp > end) {
            continue; // Skip sales outside date range
        }
        std::string product = all_sales[0][i];
        std::string brand = all_sales[1][i];
        std::string quantity = all_sales[2][i];
        int item_count = std::stoi(all_sales[7][i]);
        double sale_price = all_sales[10].empty() ? 0.0 : std::stod(all_sales[10][i]);
        sales_data.emplace_back(product, brand, quantity, item_count, sale_price);
    }
    return !sales_data.empty();
}
int RUserUi::SalesReportWindow::handle_sales_print(const std::vector<std::tuple<std::string, std::string, std::string, int, double>>& sales_data)
{
    // Note: Ensure start_date and end_date are defined (e.g., as class members)
    // struct tm start_date, end_date; // Must be initialized

    OPENFILENAME ofn = { 0 };
    wchar_t file_name[MAX_PATH] = L"sales_report.pdf";
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetActiveWindow();
    ofn.lpstrFilter = L"PDF Files (*.pdf)\0*.pdf\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = file_name;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = L"pdf";
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

    if (GetSaveFileName(&ofn)) {
        char file_name_mb[MAX_PATH] = { 0 };
        size_t converted_chars = 0;
        errno_t err = wcstombs_s(&converted_chars, file_name_mb, file_name, MAX_PATH);
        if (err != 0 || converted_chars == 0) {
            MessageBox(NULL, L"Failed to convert filename", L"Error", MB_OK | MB_ICONERROR);
            return 0;
        }

        HPDF_Doc pdf = HPDF_New(nullptr, nullptr);
        if (!pdf) {
            MessageBox(NULL, L"Failed to create PDF document", L"Error", MB_OK | MB_ICONERROR);
            return 0;
        }

        HPDF_Page page = HPDF_AddPage(pdf);
        if (!page) {
            MessageBox(NULL, L"Failed to create PDF page", L"Error", MB_OK | MB_ICONERROR);
            HPDF_Free(pdf);
            return 0;
        }

        HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);
        HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);

        // Load fonts with safety checks
        HPDF_Font title_font = HPDF_GetFont(pdf, "Helvetica-Bold", nullptr);
        if (!title_font) title_font = HPDF_GetFont(pdf, "Times-Bold", nullptr);
        if (!title_font) {
            MessageBox(NULL, L"Failed to load title font", L"Error", MB_OK | MB_ICONERROR);
            HPDF_Free(pdf);
            return 0;
        }
        HPDF_Font header_font = HPDF_GetFont(pdf, "Helvetica-Bold", nullptr);
        if (!header_font) header_font = HPDF_GetFont(pdf, "Times-Bold", nullptr);
        if (!header_font) {
            MessageBox(NULL, L"Failed to load header font", L"Error", MB_OK | MB_ICONERROR);
            HPDF_Free(pdf);
            return 0;
        }
        HPDF_Font data_font = HPDF_GetFont(pdf, "Helvetica", nullptr);
        if (!data_font) data_font = HPDF_GetFont(pdf, "Times-Roman", nullptr);
        if (!data_font) {
            MessageBox(NULL, L"Failed to load data font", L"Error", MB_OK | MB_ICONERROR);
            HPDF_Free(pdf);
            return 0;
        }

        // Check for empty sales_data
        if (sales_data.empty()) {
            MessageBox(NULL, L"No sales data to display", L"Error", MB_OK | MB_ICONERROR);
            HPDF_Free(pdf);
            return 0;
        }

        // Calculate maximum lengths
        size_t max_product_len = 20;
        size_t max_brand_len = 20;
        size_t max_quantity_len = 10;
        double total_sales = 0.0;
        for (const auto& [product, brand, quantity, item_count, sale_price] : sales_data) {
            max_product_len = (std::max)(max_product_len, product.length());
            max_brand_len = (std::max)(max_brand_len, brand.length());
            max_quantity_len = (std::max)(max_quantity_len, quantity.length());
            total_sales += item_count * sale_price;
        }

        // Cap lengths to fit A4 (~595 points, ~500 usable)
        const float char_width = 5.5f;
        const float max_page_width = 500.0f;
        const size_t max_chars_per_line = static_cast<size_t>(max_page_width / char_width / 4);
        max_product_len = (std::min)(max_product_len, max_chars_per_line);
        max_brand_len = (std::min)(max_brand_len, max_chars_per_line);
        max_quantity_len = (std::min)(max_quantity_len, max_chars_per_line);

        // Define column widths
        float product_width = (std::max)(35.0f, static_cast<float>(max_product_len) * char_width + 2 * 8.0f);
        float brand_width = (std::max)(35.0f, static_cast<float>(max_brand_len) * char_width + 2 * 8.0f);
        float quantity_width = (std::max)(40.0f, static_cast<float>(max_quantity_len) * char_width + 2 * 8.0f);
        float item_count_width = (std::max)(40.0f, 8.0f * char_width + 2 * 8.0f);
        float total_price_width = (std::max)(40.0f, 10.0f * char_width + 2 * 8.0f);
        float table_width = product_width + brand_width + quantity_width + item_count_width + total_price_width;

        // Cap table width to prevent overflow
        const float max_table_width = 500.0f;
        if (table_width > max_table_width) {
            float scale = max_table_width / table_width;
            product_width *= scale;
            brand_width *= scale;
            quantity_width *= scale;
            item_count_width *= scale;
            total_price_width *= scale;
            table_width = max_table_width;
        }

        const float x_offset = 50.0f;
        const float base_row_height = 20.0f;
        const float line_height = 12.0f;
        float y = HPDF_Page_GetHeight(page) - 50;
        float table_top_y = y - 40 - 30;

        // Title
        HPDF_Page_SetFontAndSize(page, title_font, 16);
        HPDF_Page_SetRGBFill(page, 0.0f, 0.2f, 0.4f);
        HPDF_Page_BeginText(page);
        const char* title = "Sales Report";
        HPDF_Page_TextOut(page, x_offset, y, title);
        HPDF_Page_EndText(page);
        y -= 40;

        // Date Range
        char date_range[64];
        char start_date_str[32];
        char end_date_str[32];
        strftime(start_date_str, sizeof(start_date_str), "%Y-%m-%d", &start_date);
        strftime(end_date_str, sizeof(end_date_str), "%Y-%m-%d", &end_date);
        snprintf(date_range, sizeof(date_range), "From %s to %s", start_date_str, end_date_str);
        HPDF_Page_SetFontAndSize(page, data_font, 10);
        HPDF_Page_SetRGBFill(page, 0.5f, 0.5f, 0.5f);
        HPDF_Page_BeginText(page);
        HPDF_Page_TextOut(page, x_offset, y, date_range);
        HPDF_Page_EndText(page);
        y -= 30;

        // Table header (Fixed: Draw background before text)
        HPDF_Page_SetRGBFill(page, 0.9f, 0.95f, 1.0f);
        HPDF_Page_SetRGBStroke(page, 0.0f, 0.2f, 0.4f);
        HPDF_Page_Rectangle(page, x_offset, y - base_row_height, table_width, base_row_height); // Adjusted y
        HPDF_Page_FillStroke(page);

        HPDF_Page_SetFontAndSize(page, header_font, 10);
        HPDF_Page_SetRGBFill(page, 0.0f, 0.4f, 0.6f);
        HPDF_Page_BeginText(page);
        HPDF_Page_TextOut(page, x_offset + 8.0f, y - line_height, "Product"); // Adjusted y for text
        HPDF_Page_TextOut(page, x_offset + product_width + 8.0f, y - line_height, "Brand");
        HPDF_Page_TextOut(page, x_offset + product_width + brand_width + 8.0f, y - line_height, "Quantity");
        HPDF_Page_TextOut(page, x_offset + product_width + brand_width + quantity_width + 8.0f, y - line_height, "Item Count");
        HPDF_Page_TextOut(page, x_offset + product_width + brand_width + quantity_width + item_count_width + 8.0f, y - line_height, "Total Price");
        HPDF_Page_EndText(page);
        y -= base_row_height;

        bool even_row = false;
        int page_count = 1;
        float table_bottom_y = y;

        // Text splitting function
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
                    while (len > 0 && text[start + len - 1] != ' ') {
                        --len;
                    }
                    if (len == 0) len = max_chars_per_line;
                }
                lines.push_back(text.substr(start, len));
                start += len;
            }
            return lines;
        };

        for (const auto& [product, brand, quantity, item_count, sale_price] : sales_data) {
            std::string product_name = product.substr(0, product.find('_'));
            const char* product_str = product_name.c_str();
            const char* brand_str = brand.c_str();
            const char* quantity_str = quantity.c_str();
            std::vector<std::string> product_lines = split_text(product_str);
            std::vector<std::string> brand_lines = split_text(brand_str);
            std::vector<std::string> quantity_lines = split_text(quantity_str);
            size_t max_lines = (std::max)((std::max)(product_lines.size(), brand_lines.size()), quantity_lines.size());
            max_lines = (std::max)(max_lines, size_t(1));
            max_lines = (std::min)(max_lines, size_t(3));
            float row_height = max_lines * line_height + 8.0f;

            if (y - row_height < 50) {
                // Add page number
                HPDF_Page_SetFontAndSize(page, data_font, 10);
                HPDF_Page_SetRGBFill(page, 0.5f, 0.5f, 0.5f);
                HPDF_Page_BeginText(page);
                char page_num[32];
                snprintf(page_num, sizeof(page_num), "Page %d", page_count);
                HPDF_Page_TextOut(page, x_offset, 30, page_num);
                HPDF_Page_EndText(page);

                // Draw vertical lines
                HPDF_Page_SetRGBStroke(page, 0.7f, 0.7f, 0.7f);
                HPDF_Page_SetLineWidth(page, 0.5);
                float x_positions[] = {
                    x_offset,
                    x_offset + product_width,
                    x_offset + product_width + brand_width,
                    x_offset + product_width + brand_width + quantity_width,
                    x_offset + product_width + brand_width + quantity_width + item_count_width,
                    x_offset + product_width + brand_width + quantity_width + item_count_width + total_price_width
                };
                for (float x : x_positions) {
                    HPDF_Page_MoveTo(page, x, table_top_y + 5);
                    HPDF_Page_LineTo(page, x, table_bottom_y - 5);
                    HPDF_Page_Stroke(page);
                }

                // Start new page
                page = HPDF_AddPage(pdf);
                if (!page) {
                    MessageBox(NULL, L"Failed to create new PDF page", L"Error", MB_OK | MB_ICONERROR);
                    HPDF_Free(pdf);
                    return 0;
                }
                page_count++;
                HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
                y = HPDF_Page_GetHeight(page) - 50;
                table_top_y = y - 40 - 30;

                // Repeat title
                HPDF_Page_SetFontAndSize(page, title_font, 16);
                HPDF_Page_SetRGBFill(page, 0.0f, 0.2f, 0.4f);
                HPDF_Page_BeginText(page);
                HPDF_Page_TextOut(page, x_offset, y, title);
                HPDF_Page_EndText(page);
                y -= 40;

                // Repeat date range
                HPDF_Page_SetFontAndSize(page, data_font, 10);
                HPDF_Page_SetRGBFill(page, 0.5f, 0.5f, 0.5f);
                HPDF_Page_BeginText(page);
                HPDF_Page_TextOut(page, x_offset, y, date_range);
                HPDF_Page_EndText(page);
                y -= 30;

                // Repeat table header
                HPDF_Page_SetRGBFill(page, 0.9f, 0.95f, 1.0f);
                HPDF_Page_SetRGBStroke(page, 0.0f, 0.2f, 0.4f);
                HPDF_Page_Rectangle(page, x_offset, y - base_row_height, table_width, base_row_height);
                HPDF_Page_FillStroke(page);

                HPDF_Page_SetFontAndSize(page, header_font, 10);
                HPDF_Page_SetRGBFill(page, 0.0f, 0.4f, 0.6f);
                HPDF_Page_BeginText(page);
                HPDF_Page_TextOut(page, x_offset + 8.0f, y - line_height, "Product");
                HPDF_Page_TextOut(page, x_offset + product_width + 8.0f, y - line_height, "Brand");
                HPDF_Page_TextOut(page, x_offset + product_width + brand_width + 8.0f, y - line_height, "Quantity");
                HPDF_Page_TextOut(page, x_offset + product_width + brand_width + quantity_width + 8.0f, y - line_height, "Item Count");
                HPDF_Page_TextOut(page, x_offset + product_width + brand_width + quantity_width + item_count_width + 8.0f, y - line_height, "Total Price");
                HPDF_Page_EndText(page);
                y -= base_row_height;
                table_bottom_y = y;
            }

            // Draw row background (Fixed: Draw before text)
            HPDF_Page_SetRGBFill(page, even_row ? 0.98f : 1.0f, even_row ? 0.98f : 1.0f, even_row ? 0.98f : 1.0f);
            HPDF_Page_Rectangle(page, x_offset, y - row_height, table_width, row_height); // Adjusted y
            HPDF_Page_Fill(page);
            even_row = !even_row;

            // Draw horizontal borders
            HPDF_Page_SetRGBStroke(page, 0.7f, 0.7f, 0.7f);
            HPDF_Page_SetLineWidth(page, 0.5);
            HPDF_Page_MoveTo(page, x_offset, y);
            HPDF_Page_LineTo(page, x_offset + table_width, y);
            HPDF_Page_MoveTo(page, x_offset, y - row_height);
            HPDF_Page_LineTo(page, x_offset + table_width, y - row_height);
            HPDF_Page_Stroke(page);

            // Print data
            HPDF_Page_SetFontAndSize(page, data_font, 10);
            // Product
            HPDF_Page_SetRGBFill(page, 0.0f, 0.5f, 0.0f);
            HPDF_Page_BeginText(page);
            for (size_t i = 0; i < product_lines.size(); ++i) {
                HPDF_Page_TextOut(page, x_offset + 8.0f, y - (i + 1) * line_height, product_lines[i].c_str());
            }
            HPDF_Page_EndText(page);
            // Brand
            HPDF_Page_SetRGBFill(page, 0.0f, 0.0f, 0.0f);
            HPDF_Page_BeginText(page);
            for (size_t i = 0; i < brand_lines.size(); ++i) {
                HPDF_Page_TextOut(page, x_offset + product_width + 8.0f, y - (i + 1) * line_height, brand_lines[i].c_str());
            }
            HPDF_Page_EndText(page);
            // Quantity
            HPDF_Page_SetRGBFill(page, 0.0f, 0.0f, 0.0f);
            HPDF_Page_BeginText(page);
            for (size_t i = 0; i < quantity_lines.size(); ++i) {
                HPDF_Page_TextOut(page, x_offset + product_width + brand_width + 8.0f, y - (i + 1) * line_height, quantity_lines[i].c_str());
            }            
            HPDF_Page_EndText(page);

            // Item Count
            HPDF_Page_SetRGBFill(page, 0.0f, 0.0f, 0.0f);
            HPDF_Page_BeginText(page);
            std::string item_count_str = std::to_string(item_count);
            HPDF_Page_TextOut(page, x_offset + product_width + brand_width + quantity_width + 8.0f, y - line_height, item_count_str.c_str());
            HPDF_Page_EndText(page);
            // Total Price
            HPDF_Page_SetRGBFill(page, 0.0f, 0.0f, 0.0f);
            HPDF_Page_BeginText(page);
            char total_price_str[32];
            snprintf(total_price_str, sizeof(total_price_str), "%.2f", item_count * sale_price);
            HPDF_Page_TextOut(page, x_offset + product_width + brand_width + quantity_width + item_count_width + 8.0f, y - line_height, total_price_str);
            HPDF_Page_EndText(page);

            y -= row_height;
            table_bottom_y = y;
        }

        // Draw vertical lines for final page
        HPDF_Page_SetRGBStroke(page, 0.7f, 0.7f, 0.7f);
        HPDF_Page_SetLineWidth(page, 0.5);
        float x_positions[] = {
            x_offset,
            x_offset + product_width,
            x_offset + product_width + brand_width,
            x_offset + product_width + brand_width + quantity_width,
            x_offset + product_width + brand_width + quantity_width + item_count_width,
            x_offset + product_width + brand_width + quantity_width + item_count_width + total_price_width
        };
        for (float x : x_positions) {
            HPDF_Page_MoveTo(page, x, table_top_y + 5);
            HPDF_Page_LineTo(page, x, table_bottom_y - 5);
            HPDF_Page_Stroke(page);
        }

        // Total Sales
        y -= 20;
        HPDF_Page_SetFontAndSize(page, header_font, 10);
        HPDF_Page_SetRGBFill(page, 0.0f, 0.4f, 0.6f);
        HPDF_Page_BeginText(page);
        char total_sales_str[64];
        snprintf(total_sales_str, sizeof(total_sales_str), "Total Sales: %.2f", total_sales);
        HPDF_Page_TextOut(page, x_offset + 8.0f, y, total_sales_str);
        HPDF_Page_EndText(page);

        // Final page number
        HPDF_Page_SetFontAndSize(page, data_font, 10);
        HPDF_Page_SetRGBFill(page, 0.5f, 0.5f, 0.5f);
        HPDF_Page_BeginText(page);
        char page_num[32];
        snprintf(page_num, sizeof(page_num), "Page %d", page_count);
        HPDF_Page_TextOut(page, x_offset, 30, page_num);
        HPDF_Page_EndText(page);

        // Save PDF
        HPDF_STATUS error = HPDF_SaveToFile(pdf, file_name_mb);
        if (error != HPDF_OK) {
            wchar_t error_msg[256];
            swprintf_s(error_msg, sizeof(error_msg) / sizeof(wchar_t), L"Failed to save PDF: Error code %u", error);
            MessageBox(NULL, error_msg, L"Error", MB_OK | MB_ICONERROR);
            HPDF_Free(pdf);
            return 0;
        }

        HPDF_Free(pdf);
        this->last_saved_file = file_name_mb; // Note: If std::wstring, convert file_name_mb
        return 1;
    }
    return 0;
}

/*-----------------------------Agents Window-------------------------------*/

// Initialize static members for AgentsWindow
int RUserUi::AgentsWindow::current_filter_type_index = 0;

// Constructor for AgentsWindow
RUserUi::AgentsWindow::AgentsWindow(mongocxx::database db)
{
    this->agents = db["Agents"];

    // Initialize table flags
    table_flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Reorderable;
    TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

    // Fetch agent details using get_agent_details
    std::vector<std::vector<std::string>> agent_details = get_agent_details(this->agents);
    if (agent_details.size() >= 4) {
        this->ids = agent_details[0];
        this->names = agent_details[1];
        this->phones = agent_details[2];
        this->locations = agent_details[3];
    }
    else {
        // Handle case where agent_details doesn't have enough data
        this->ids = {};
        this->names = {};
        this->phones = {};
        this->locations = {};
    }

    // Initialize filtered data
    this->filtered_ids = this->ids;
    this->filtered_names = this->names;
    this->filtered_phones = this->phones;
    this->filtered_locations = this->locations;

    // Initialize filter types with smart pointer
    this->p_filter_types = std::make_unique<char[]>(this->filter_types.size() + 1);
    std::fill_n(this->p_filter_types.get(), this->filter_types.size() + 1, 0);
    std::strncpy(this->p_filter_types.get(), this->filter_types.c_str(), this->filter_types.size());
    this->prepare_filter_types_pointer(this->filter_types, '_', this->p_filter_types);
}

// Create agents window with styled layout
int RUserUi::AgentsWindow::create_agents_window()
{
    // Apply modern styling
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(45, 45, 45, 255)); // Slightly darker background
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(60, 60, 60, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(80, 80, 80, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(100, 100, 100, 255));
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 5.0f));

    // Center and size the window
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f - 300.0f, io.DisplaySize.y * 0.5f - 250.0f), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(600.0f, 500.0f), ImGuiCond_Once);

    ImGui::Begin("Agents", &this->show_window, this->window_flags | ImGuiWindowFlags_NoMove);

    // Title
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]); // Large font
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Agents").x) * 0.5f);
    ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.4f, 1.0f), "Agents");
    ImGui::PopFont();
    ImGui::Separator();

    // Filter combo and search
    const ImVec2 combo_size = ImVec2(300.0f, 50.0f);
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - combo_size.x) * 0.5f);
    create_search_filter_combo(this->p_filter_types, "Filter By", this->current_filter_type_index, combo_size);

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    create_agents_filter();

    ImGui::Separator();

    // Agents table
    draw_agents_table(this->TEXT_BASE_HEIGHT);

    ImGui::PopStyleColor(5);
    ImGui::PopStyleVar(4);
    ImGui::End();
    return 1;
}

// Draw agents table
int RUserUi::AgentsWindow::draw_agents_table(float text_base_height)
{
    ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, IM_COL32(0, 150, 80, 255)); // Green header
    ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, IM_COL32(80, 80, 80, 255)); // Dark border
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255)); // Light text

    if (ImGui::BeginTable("AgentsTable", 4, this->table_flags, ImVec2(0.0f, text_base_height * 10))) {
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableSetupColumn("Phone", ImGuiTableColumnFlags_WidthFixed, 120.0f);
        ImGui::TableSetupColumn("Location", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin((int)this->filtered_names.size());
        while (clipper.Step()) {
            for (size_t row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", this->filtered_ids[row].c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", this->filtered_names[row].c_str());
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%s", this->filtered_phones[row].c_str());
                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%s", this->filtered_locations[row].c_str());
            }
        }
        ImGui::EndTable();
    }

    ImGui::PopStyleColor(3);
    return 1;
}

// Create agents filter
int RUserUi::AgentsWindow::create_agents_filter()
{
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(50, 50, 50, 255)); // Darker child background
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(70, 70, 70, 255)); // Combo background
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255)); // Light text
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f); // Rounded frame

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 300.0f) * 0.5f);
    ImGui::BeginChild("Search##AgentsFilter", ImVec2(300.0f, 50.0f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    static ImGuiTextFilter filter;
    filter.Draw("Search##AgentsFilter");

    size_t count = this->names.size();

    this->filtered_ids.clear();
    this->filtered_names.clear();
    this->filtered_phones.clear();
    this->filtered_locations.clear();

    for (size_t item_num = 0; item_num < count; ++item_num) {
        if (this->current_filter_type_index == 1) { // Filter by Name
            if (filter.PassFilter(this->names[item_num].c_str())) {
                this->filtered_ids.push_back(this->ids[item_num]);
                this->filtered_names.push_back(this->names[item_num]);
                this->filtered_phones.push_back(this->phones[item_num]);
                this->filtered_locations.push_back(this->locations[item_num]);
            }
        }
        else if (this->current_filter_type_index == 2) { // Filter by Phone
            if (filter.PassFilter(this->phones[item_num].c_str())) {
                this->filtered_ids.push_back(this->ids[item_num]);
                this->filtered_names.push_back(this->names[item_num]);
                this->filtered_phones.push_back(this->phones[item_num]);
                this->filtered_locations.push_back(this->locations[item_num]);
            }
        }
        else if (this->current_filter_type_index == 3) { // Filter by Location
            if (filter.PassFilter(this->locations[item_num].c_str())) {
                this->filtered_ids.push_back(this->ids[item_num]);
                this->filtered_names.push_back(this->names[item_num]);
                this->filtered_phones.push_back(this->phones[item_num]);
                this->filtered_locations.push_back(this->locations[item_num]);
            }
        }
    }

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();
    ImGui::EndChild();
    return 1;
}

// Create search filter combo for agents
int RUserUi::AgentsWindow::create_search_filter_combo(const std::unique_ptr<char[]>& my_items_names, const std::string& description,
    int& current_item, ImVec2 size)
{
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(50, 50, 50, 255)); // Darker child background
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(70, 70, 70, 255)); // Combo background
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255)); // Light text
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f); // Rounded frame
    ImGui::BeginChild(description.c_str(), size, true);

    ImGui::Combo(("##" + description).c_str(), &current_item, my_items_names.get());

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();
    ImGui::EndChild();
    return 1;
}

void RUserUi::AgentsWindow::prepare_filter_types_pointer(const std::string& items, char items_separator, std::unique_ptr<char[]>& items_pointer)
{
    size_t count = std::count(items.begin(), items.end(), items_separator);
    size_t offset = 0;
    for (size_t index = 0; index < count; ++index) {
        offset = items.find_first_of(items_separator, offset + 1);
        items_pointer[offset] = '\0';
    }
}