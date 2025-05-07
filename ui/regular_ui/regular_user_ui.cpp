#include "admin_ui/admin_ui.h"
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
    ReturnSaleWindow& return_sale_win, AgentsWindow& agents_win, PassWord& change_password)
    : stock_win(stock_win), sell_win(sell_win), return_sale_win(return_sale_win),
    agents_win(agents_win), change_password(change_password)
{
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
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(350.0f, 400.0f), ImGuiCond_Always);

    // Begin window
    ImGui::Begin("Regular User Dashboard", &this->show_window, this->window_flags | ImGuiWindowFlags_NoMove);

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
    ImGui::Dummy(ImVec2(0.0f, 8.0f)); // Spacing

    // Search filter with label to the left
    ImGui::Text("Search");
    ImGui::SameLine();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3.0f);
    ImGui::SetNextItemWidth(size.x - ImGui::CalcTextSize("Search").x - 20.0f); // Adjust width to fit
    static ImGuiTextFilter filter;
    std::string label = "##Search_" + items_description; // Unique ID
    filter.Draw(label.c_str(), size.x - ImGui::CalcTextSize("Search").x - 20.0f);
    ImGui::Dummy(ImVec2(0.0f, 8.0f)); // Spacing

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
    items_array.push_back(" ");
    items_array.push_back(" ");

    // Listbox
    label = "##Listbox_" + items_description;
    if (ImGui::BeginListBox(label.c_str(), ImVec2(size.x - 10.0f, size.y - 80.0f))) { // Adjusted height
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
int RUserUi::Window::display_all_brands(const std::string& product, bool show_low_stock, int low_stock_threshold, bool show_out_of_stock)
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
    float max_brand_width = 150.0f, max_quantity_width = 100.0f, max_remaining_width = 100.0f;
    this->brands_array = get_product_brands_array(this->products_collection, product);
    for (const auto& brand : this->brands_array) {
        max_brand_width = ((std::max))(max_brand_width, ImGui::CalcTextSize(brand.c_str()).x + 20.0f);
        this->quantities_array = get_brand_quantities_array(this->products_collection, product + "_" + brand);
        this->amounts_array = get_brand_amounts_array(this->products_collection, this->stock, product, brand);
        for (size_t i = 0; i < this->quantities_array.size(); ++i) {
            int remaining = this->amounts_array[i];
            bool show_item = true;
            if (show_out_of_stock && remaining != 0) {
                show_item = false;
            }
            if (show_out_of_stock && remaining == 0) {
                show_item = false;
            }
            if (show_low_stock && remaining >= low_stock_threshold) {
                show_item = false;
            }
            if (show_item) {
                max_quantity_width = ((std::max))(max_quantity_width, ImGui::CalcTextSize(this->quantities_array[i].c_str()).x + 20.0f);
            }
        }
    }

    float table_height = ImGui::GetContentRegionAvail().y - 50.0f;
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
                    ImGui::Text("%d", this->amounts_array[i]);
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
int RUserUi::Window::display_brand_quantities(const std::string& product, const std::string& brand, bool show_low_stock, int low_stock_threshold, bool show_out_of_stock)
{
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(230, 230, 230, 255));
    ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, IM_COL32(60, 60, 65, 255));
    ImGui::PushStyleColor(ImGuiCol_TableRowBg, IM_COL32(50, 50, 55, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 6.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 6.0f));

    this->quantities = "Select_";
    // Calculate max column widths based on valid items
    float max_quantity_width = 100.0f, max_remaining_width = 100.0f;
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
            max_quantity_width = ((std::max))(max_quantity_width, ImGui::CalcTextSize(this->quantities_array[i].c_str()).x + 20.0f);
        }
    }

    float table_height = ImGui::GetContentRegionAvail().y - 50.0f;
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
            ImGui::Text("%d", this->amounts_array[i]);
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
int RUserUi::Window::display_quantity(const std::string& product, const std::string& brand, const std::string& quantity, bool show_low_stock, int low_stock_threshold, bool show_out_of_stock)
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
    float max_remaining_width = 100.0f;

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
            ImGui::Text("%d", remaining);

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
    const ImVec2 filter_size = ImVec2(400.0f, 120.0f);
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
    if (this->selected_product == "All Products") {
        ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, IM_COL32(60, 60, 65, 255));
        ImGui::PushStyleColor(ImGuiCol_TableRowBg, IM_COL32(50, 50, 55, 255));
        ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, IM_COL32(45, 45, 50, 255));
        ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, IM_COL32(80, 80, 85, 255));
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 6.0f));

        // Calculate max column widths
        float max_product_width = 150.0f, max_brand_width = 150.0f, max_quantity_width = 100.0f, max_remaining_width = 100.0f;
        this->products_array = get_products_array(this->products_collection);
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
        print_data.clear(); // Ensure print_data is empty before populating

        // Check and collect data for table and printing
        for (const auto& product : this->products_array) {
            this->brands_array = get_product_brands_array(this->products_collection, product);
            for (const auto& brand : this->brands_array) {
                this->quantities_array = get_brand_quantities_array(this->products_collection, product + "_" + brand);
                this->amounts_array = get_brand_amounts_array(this->products_collection, this->stock, product, brand);
                for (size_t i = 0; i < this->quantities_array.size(); ++i) {
                    int remaining = this->amounts_array[i];
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
            for (const auto& product : this->products_array) {
                this->brands_array = get_product_brands_array(this->products_collection, product);
                bool product_has_valid_items = false;
                for (const auto& brand : this->brands_array) {
                    this->quantities_array = get_brand_quantities_array(this->products_collection, product + "_" + brand);
                    this->amounts_array = get_brand_amounts_array(this->products_collection, this->stock, product, brand);
                    bool first_brand_row = true;
                    for (size_t i = 0; i < this->quantities_array.size(); ++i) {
                        int remaining = this->amounts_array[i];
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
                            ImGui::Text("%d", remaining);
                        }
                    }
                }
                if (!product_has_valid_items) {
                    current_product = "";
                    first_product_row = true;
                }
            }
            ImGui::EndTable();
        }

        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();
    }
    return 1;
}

int RUserUi::StockWindow::handle_stock_print(const std::vector<std::tuple<std::string, std::string, std::string, int>>& print_data, bool has_valid_items)
{
    if (has_valid_items && ImGui::Button("Print", ImVec2(150.0f, 40.0f))) {
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
                ImGui::OpenPopup("Print Error");
                return 1;
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
            const float text_padding = 12.0f; // Increased padding to prevent text cutting
            const float product_width = (std::max)(50.0f, static_cast<float>(max_product_len) * char_width + 2 * text_padding);
            const float brand_width = (std::max)(50.0f, static_cast<float>(max_brand_len) * char_width + 2 * text_padding);
            const float quantity_width = (std::max)(50.0f, 15.0f * char_width + 2 * text_padding);
            const float remaining_width = (std::max)(50.0f, 10.0f * char_width + 2 * text_padding);
            const float table_width = product_width + brand_width + quantity_width + remaining_width;
            const float x_offset = (HPDF_Page_GetWidth(page) - table_width) / 2;
            const float base_row_height = 20.0f;
            const float line_height = 15.0f;

            float y = HPDF_Page_GetHeight(page) - 50;
            float table_top_y = y - 40 - 30; // After title and timestamp

            // Title
            HPDF_Page_SetFontAndSize(page, title_font, 16);
            HPDF_Page_SetRGBFill(page, 0.0f, 0.2f, 0.4f); // Dark blue
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
            float table_bottom_y = y; // Track bottom of table for vertical lines

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
                    // Add page number to current page
                    HPDF_Page_SetFontAndSize(page, data_font, 10);
                    HPDF_Page_SetRGBFill(page, 0.5f, 0.5f, 0.5f);
                    HPDF_Page_BeginText(page);
                    char page_num[32];
                    snprintf(page_num, sizeof(page_num), "Page %d", page_count);
                    HPDF_Page_TextOut(page, x_offset, 30, page_num);
                    HPDF_Page_EndText(page);

                    // Draw vertical lines for current page
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

                    // Repeat header on new page
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

                // Draw horizontal borders (top and bottom of row)
                HPDF_Page_SetRGBStroke(page, 0.7f, 0.7f, 0.7f);
                HPDF_Page_SetLineWidth(page, 0.5);
                HPDF_Page_MoveTo(page, x_offset, y - 2);
                HPDF_Page_LineTo(page, x_offset + table_width, y - 2);
                HPDF_Page_MoveTo(page, x_offset, y - row_height - 2);
                HPDF_Page_LineTo(page, x_offset + table_width, y - row_height - 2);
                HPDF_Page_Stroke(page);

                // Print data
                HPDF_Page_SetFontAndSize(page, data_font, 12);

                // Product (green when printed)
                HPDF_Page_SetRGBFill(page, product_str[0] ? 0.0f : 0.0f, product_str[0] ? 0.5f : 0.0f, 0.0f);
                HPDF_Page_BeginText(page);
                for (size_t i = 0; i < product_lines.size(); ++i) {
                    HPDF_Page_TextOut(page, x_offset + text_padding, y - (i * line_height), product_lines[i].c_str());
                }
                HPDF_Page_EndText(page);

                // Brand (black)
                HPDF_Page_SetRGBFill(page, 0.0f, 0.0f, 0.0f);
                HPDF_Page_BeginText(page);
                for (size_t i = 0; i < brand_lines.size(); ++i) {
                    HPDF_Page_TextOut(page, x_offset + product_width + text_padding, y - (i * line_height), brand_lines[i].c_str());
                }
                HPDF_Page_EndText(page);

                // Quantity (black)
                std::string quantity_trunc = quantity.substr(0, max_chars_per_line);
                HPDF_Page_BeginText(page);
                HPDF_Page_TextOut(page, x_offset + product_width + brand_width + text_padding, y, quantity_trunc.c_str());
                HPDF_Page_EndText(page);

                // Remaining (red if 0)
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
                ImGui::OpenPopup("Print Success");
            }
            else {
                ImGui::OpenPopup("Print Error");
            }
        }
    }

    // Popups for print feedback
    if (ImGui::BeginPopupModal("Print Success", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Report saved to %s", this->last_saved_file.c_str());
        ImGui::Text("Open the PDF to print or review.");
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 250.0f) * 0.5f); // Adjusted for two buttons (120 + 10 + 120)
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("View", ImVec2(120, 0))) {
            // Convert last_saved_file to wide string for ShellExecute
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

    return 1;
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

    // Handle non-"All Products" selection
    if (this->selected_product != "All Products") {
        if (this->stock_brand_current_item == 0 && !product_changed) {
            display_all_brands(this->selected_product, this->show_low_stock, this->low_stock_threshold, this->show_out_of_stock);
        }
        else if (product_changed) {
            display_all_brands(this->selected_product, this->show_low_stock, this->low_stock_threshold, this->show_out_of_stock);
            this->stock_brand_current_item = 0;
            this->stock_quantity_current_item = 0;
            product_changed = false;
        }
        else if (this->stock_brand_current_item != 0 && this->stock_quantity_current_item == 0 && !product_changed) {
            this->get_selected_brand(this->stock_brand_current_item);
            display_brand_quantities(this->selected_product, this->selected_brand, this->show_low_stock, this->low_stock_threshold, this->show_out_of_stock);
        }
        else if (this->stock_quantity_current_item != 0 && !product_changed) {
            this->get_selected_quantity(this->stock_quantity_current_item);
            display_quantity(this->selected_product, this->selected_brand, this->selected_quantity, this->show_low_stock, this->low_stock_threshold, this->show_out_of_stock);
        }

        // Collect print data for the selected product/brand/quantity
        if (this->stock_brand_current_item != 0 && this->stock_quantity_current_item != 0) {
            // Specific quantity selected
            this->quantities_array = get_brand_quantities_array(this->products_collection, this->selected_product + "_" + this->selected_brand);
            this->amounts_array = get_brand_amounts_array(this->products_collection, this->stock, this->selected_product, this->selected_brand);
            for (size_t i = 0; i < this->quantities_array.size(); ++i) {
                if (this->quantities_array[i] == this->selected_quantity) {
                    int remaining = this->amounts_array[i];
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
                        print_data.emplace_back(this->selected_product, this->selected_brand, this->selected_quantity, remaining);
                        has_valid_items = true;
                    }
                    break;
                }
            }
        }
        else if (this->stock_brand_current_item != 0) {
            // Specific brand selected, include all quantities
            this->quantities_array = get_brand_quantities_array(this->products_collection, this->selected_product + "_" + this->selected_brand);
            this->amounts_array = get_brand_amounts_array(this->products_collection, this->stock, this->selected_product, this->selected_brand);
            for (size_t i = 0; i < this->quantities_array.size(); ++i) {
                int remaining = this->amounts_array[i];
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
                    print_data.emplace_back(this->selected_product, this->selected_brand, this->quantities_array[i], remaining);
                    has_valid_items = true;
                }
            }
        }
        else {
            // Only product selected, include all brands and quantities
            this->brands_array = get_product_brands_array(this->products_collection, this->selected_product);
            for (const auto& brand : this->brands_array) {
                this->quantities_array = get_brand_quantities_array(this->products_collection, this->selected_product + "_" + brand);
                this->amounts_array = get_brand_amounts_array(this->products_collection, this->stock, this->selected_product, brand);
                for (size_t i = 0; i < this->quantities_array.size(); ++i) {
                    int remaining = this->amounts_array[i];
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
                        print_data.emplace_back(this->selected_product, brand, this->quantities_array[i], remaining);
                        has_valid_items = true;
                    }
                }
            }
        }

        this->reset_combo_items();
        this->stock_brand_current_item = 0;
        this->stock_quantity_current_item = 0;
    }
    else {
        // Display table and collect print data for "All Products"
        if (!display_stock_table(print_data, has_valid_items)) {
            return 0;
        }
    }

    // Handle printing (for both cases)
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 150.0f) * 0.5f); // Center the Print button
    if (!handle_stock_print(print_data, has_valid_items)) {
        return 0;
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
    float maxHeight = io.DisplaySize.y - 40.0f; // Ensure height doesn't exceed display height minus top offset
    ImGui::SetNextWindowSize(ImVec2(520.0f, (std::min)(630.0f, maxHeight)), ImGuiCond_Always);

    ImGui::Begin("Create Sale", &this->show_window, this->window_flags);

    // Title
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]); // Large font
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Create Sale").x) * 0.5f);
    ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "Create Sale");
    ImGui::PopFont();
    ImGui::Dummy(ImVec2(0.0f, 8.0f)); // Reduced spacing
    ImGui::Separator();

    // Filters layout (two-column grid, labels on one line, selects on next)
    const ImVec2 filter_size = ImVec2(230.0f, 120.0f); // Reduced list box height
    const ImVec2 combo_size = ImVec2(230.0f, 30.0f); // Compact combo height
    const float column_offset = (ImGui::GetWindowSize().x - filter_size.x - combo_size.x - 15.0f) * 0.5f;

    // Product list box (single column due to height)
    ImGui::SetCursorPosX(column_offset);
    ImGui::Text("Select Product");
    ImGui::SetCursorPosX(column_offset);
    create_listbox_filter(this->products, "Selected", this->selected_product, '_', filter_size); // Empty label

    // Brand and Quantity (first row)
    ImGui::Dummy(ImVec2(0.0f, 15.0f)); // Reduced spacing
    ImGui::SetCursorPosX(column_offset);
    ImGui::Text("Select Brand");
    ImGui::SameLine(0.0f, 15.0f + filter_size.x); // Align with second column
    ImGui::Text("Select Quantity");
    ImGui::SetCursorPosX(column_offset);
    if (this->selected_product != "All Products") {
        this->populate_brands(this->selected_product);
    }
    else {
        this->reset_combo_items();
        this->sell_brand_current_item = 0;
        this->sell_quantity_current_item = 0;
    }
    create_combo(this->p_brands, "##Brand", this->sell_brand_current_item, combo_size); // Empty label
    ImGui::SameLine(0.0f, 15.0f);
    if (this->sell_brand_current_item != 0) {
        this->get_selected_brand(this->sell_brand_current_item);
        this->populate_quantities(this->selected_product + "_" + this->selected_brand);
    }
    create_combo(this->p_quantities, "##Quantity", this->sell_quantity_current_item, combo_size); // Empty label

    // Season and Age Group (second row)
    ImGui::Dummy(ImVec2(0.0f, 15.0f));
    ImGui::SetCursorPosX(column_offset);
    ImGui::Text("Season");
    ImGui::SameLine(0.0f, 15.0f + filter_size.x);
    ImGui::Text("Age Group");
    ImGui::SetCursorPosX(column_offset);
    create_combo(this->p_seasons, "##Season", this->sell_season_current_item, combo_size); // Empty label
    ImGui::SameLine(0.0f, 15.0f);
    create_combo(this->p_age_groups, "##AgeGroup", this->sell_age_group_current_item, combo_size); // Empty label

    // Gender and Location (third row)
    ImGui::Dummy(ImVec2(0.0f, 15.0f));
    ImGui::SetCursorPosX(column_offset);
    ImGui::Text("Gender");
    ImGui::SameLine(0.0f, 15.0f + filter_size.x);
    ImGui::Text("Location");
    ImGui::SetCursorPosX(column_offset);
    create_combo(this->p_genders, "##Gender", this->sell_gender_current_item, combo_size); // Empty label
    ImGui::SameLine(0.0f, 15.0f);
    create_combo(this->p_locations, "##Location", this->sell_location_current_item, combo_size); // Empty label

    // Quantity to Sell input
    ImGui::Dummy(ImVec2(0.0f, 15.0f));
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 150.0f) * 0.5f);
    ImGui::Text("Quantity to Sell");
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 150.0f) * 0.5f);
    ImGui::SetNextItemWidth(150.0f);
    ImGui::InputInt("##QuantityToSell", &this->number_of_items, 1, 10); // Hidden label

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

        int available_items = 0;
        if (!sale.quantity.empty()) {
            available_items = get_quantity_total(this->stock, sale);
        }

        if (sale.product_name.empty() || sale.brand.empty() || sale.quantity.empty() ||
            sale.season.empty() || sale.age_group.empty() || sale.gender.empty() ||
            sale.location.empty() || sale.item_count <= 0 || available_items == 0 ||
            sale.item_count > available_items) {
            this->input_error = true;
        }
        else {
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
        ImGui::Text("Sale failed. Please check all inputs and ensure sufficient stock.");
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // Add spacing
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 120.0f) * 0.5f); // Center button
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
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // Add spacing
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 120.0f) * 0.5f); // Center button
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
int RUserUi::SellWindow::sell_season_current_item = 0;
int RUserUi::SellWindow::sell_age_group_current_item = 0;
int RUserUi::SellWindow::sell_gender_current_item = 0;
int RUserUi::SellWindow::sell_location_current_item = 0;
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