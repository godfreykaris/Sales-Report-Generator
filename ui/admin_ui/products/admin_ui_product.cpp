#include "admin_ui.h"
#include "insert_components.h"
#include "delete_components.h"
#include "retrieve_data.h"
#include "imgui_internal.h"

AdminUi::AddProductWindow::AddProductWindow(mongocxx::database db)
{
    this->products_collection = db["Products"];
    this->stock = db["Stock"];
}

char AdminUi::AddProductWindow::c_product_name[128] = "";

int AdminUi::AddProductWindow::create_add_product_window()
{
    // Set dark theme background
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(60, 60, 60, 255));

    // Center window with responsive size
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_Once);

    ImGui::Begin("Add Product", &this->show_window, this->window_flags | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    // Title
    ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1.0f), "Add New Product");
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    // Two-column grid for input
    if (ImGui::BeginTable("product_input_table", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_PadOuterX))
    {
        // Set column widths: fixed for label, stretch for input
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 120.0f); // Increased width for better fit
        ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthStretch);
    
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        // Vertically center the label
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (ImGui::GetTextLineHeightWithSpacing() - ImGui::GetTextLineHeight()) * 0.5f);
        ImGui::Text("Product Name");
        
        ImGui::TableSetColumnIndex(1);
        // Set input field width to avoid over-stretching, with padding
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 10.0f); // Leave some padding
        ImGui::InputTextWithHint("###product_name", "Enter name", this->c_product_name, IM_ARRAYSIZE(this->c_product_name));
    
        ImGui::EndTable();
    }

    // Handle errors
    if (this->input_error == true)
        this->admin_error_message(this->input_error);

    if (this->brand_win.input_error == true)
        this->admin_error_message(this->brand_win.input_error);
    else if (this->brand_win.success == true)
        this->notify_sale_status(this->brand_win.success);

    // Handle brand window finish
    if (this->brand_win.finish == true && this->brand_win.input_error == false)
    {
        std::vector<std::string> previous_brands = this->brand_names;
        previous_brands.pop_back();
        bool status = check_for_duplicate(previous_brands, this->brand_names[this->brand_names.size() - 1]);

        if (status == true)
        {
            this->brand_names[this->brand_names.size() - 1] = "";
            this->input_error = true;
        }

        this->brand_win.show_window = false;
        this->brand_win.finish = false;
        this->brand_index++;
    }

    // Nested brand window
    if (this->brand_win.show_window == true)
        this->brand_win.create_add_brand_window(this->product_name, this->brand_names[brand_index], this->qty_names[brand_index], this->selling_price[brand_index], this->buying_price[brand_index], this->number_of_items[brand_index], "Product");
    else if (this->brand_win.show_window == false && this->brand_names.size() != 0 && this->brand_names[this->brand_names.size() - 1].compare("") == 0)
    {
        this->brand_names.pop_back();
        this->qty_names.pop_back();
        this->selling_price.pop_back();
        this->buying_price.pop_back();
        this->number_of_items.pop_back();

        if (this->brand_index != 0)
            this->brand_index--;
    }

    // Button styling: Blue for actions
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(50, 100, 255, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(70, 120, 255, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(90, 140, 255, 255));

    // Center buttons
    float button_width = 120.0f;
    float window_width = ImGui::GetContentRegionAvail().x;
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::SetCursorPosX((window_width - (button_width * 2 + 10.0f)) * 0.5f);
    if (ImGui::Button("Add Brand", ImVec2(button_width, 30.0f)) && this->brand_win.show_window == false)
    {
        this->product_name = this->c_product_name;
        if (this->product_name.compare("") != 0)
        {
            std::vector<std::string> m_quantities = {};
            this->qty_names.push_back(m_quantities);

            std::vector<double> prices = {};
            this->selling_price.push_back(prices);

            this->buying_price.push_back(prices);

            std::vector<int> nums = {};
            this->number_of_items.push_back(nums);

            this->brand_names.push_back("");

            this->brand_win.show_window = true;
        }
        else
        {
            this->input_error = true;
        }
    }

    ImGui::SameLine(0.0f, 10.0f);
    if (ImGui::Button("Finish", ImVec2(button_width, 30.0f)))
    {
        this->product_name = this->c_product_name;

        bool found_unwanted_char = check_unwanted_characters(this->product_name);

        if ((this->product_name.compare("") == 0) || (this->brand_names.size() == 0) || found_unwanted_char == true)
        {
            this->input_error = true;
        }
        else
        {
            bool status = check_for_duplicate(v_products, this->product_name);

            if (status == true)
            {
                this->input_error = true;
            }
            else
            {
                insert_product(this->products_collection, this->stock, this->product_name, this->brand_names, this->qty_names, this->selling_price, this->buying_price, this->number_of_items);
                this->success = true;

                this->brand_index = 0;
                this->finish = true;
            }
        }
    }

    ImGui::PopStyleColor(3);
    ImGui::PopStyleColor();
    ImGui::End();
    return 1;
}

AdminUi::RemoveProductWindow::RemoveProductWindow(mongocxx::database db)
{
	this->products_collection = db["Products"];
	this->stock = db["Stock"];
	
}

int AdminUi::RemoveProductWindow::create_remove_product_window()
{	
	ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(60, 60, 60, 255));

	ImGui::SetNextWindowPos(ImVec2(400, 350), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(350, 300), ImGuiCond_Once);

	ImGui::Begin("Remove Product", &this->show_window, this->window_flags);

	ImGui::NewLine();
	ImGui::SameLine(80.0, 0.0);
	this->mset_products();
	this->create_listbox_filter(this->products, "Product", this->selected_product, '_', ImVec2(210, 150));
		
	if (this->confirm == this->ACCEPTED)
	{
		delete_product(this->products_collection, this->stock, this->selected_product);

		this->success = true;

		this->show_confirm_window = false;
		this->confirm = this->NONE;

		this->selected_product = "All Products";
		
	}
	else if (this->confirm == this->REJECTED)
	{
		this->show_confirm_window = false;
		this->confirm = this->NONE;
	}

	if(this->show_confirm_window == true)
		this->admin_confirm_message(this->confirm);
	else if (this->input_error)
			this->admin_error_message(this->input_error);

	ImGui::Dummy(ImVec2(0.0, 10.0));
	ImGui::NewLine();
	ImGui::SameLine(100.0, 0.0);

	if (ImGui::Button("Remove", ImVec2(120, 0)))
	{	
		if ((this->selected_product.compare("All Products") == 0))
		{
			this->input_error = true;
		}
		else
		{		
			this->show_confirm_window = true;					
		}

	}

	ImGui::PopStyleColor();
	ImGui::End();
	return 1;
}