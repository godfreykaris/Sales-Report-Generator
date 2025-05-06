#include "admin_ui.h"
#include "insert_components.h"
#include "delete_components.h"
#include "retrieve_data.h"
#include "imgui_internal.h"

AdminUi::AddBrandWindow::AddBrandWindow(mongocxx::database db)
{
    this->products_collection = db["Products"];
    this->stock = db["Stock"];
    this->mset_products();
}

char AdminUi::AddBrandWindow::c_brand_name[128] = "";
char AdminUi::AddBrandWindow::c_n_brand_name[128] = "";

int AdminUi::AddBrandWindow::create_add_brand_window(std::string product_name, std::string& brand, std::vector<std::string>& quantities, std::vector<double>& selling_price, std::vector<double>& buying_price, std::vector<int>& number_of_items, std::string caller)
{
    ImVec2 size = ImVec2(300, 190);
    std::string label = "Add Brand";
    std::string quantity_caller = "Brand";
    if (caller.compare("Product") != 0)
    {
        size = ImVec2(350, 400);
        label = "Add New Brand";
        quantity_caller = "New Brand";
    }

    // Consistent styling with add_items_window
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.95f, 1.0f)); // Near-white text
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.13f, 0.14f, 0.15f, 1.0f)); // Dark charcoal background
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 1.0f)); // Soft blue buttons
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.36f, 0.69f, 1.0f, 1.0f)); // Lighter blue on hover
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.20f, 0.49f, 0.88f, 1.0f)); // Darker blue when clicked
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(70, 70, 70, 255)); // Input field background
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(100, 150, 200, 128)); // Subtle blue border
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f); // Rounded window corners
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f); // Rounded input/button corners
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 6.0f)); // Comfortable padding
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f)); // Balanced spacing

    // Center relative to parent and position to the right of add items window
    ImVec2 parent_center = ImGui::GetMainViewport()->GetCenter();
    ImVec2 add_items_size(340, 300);
    float offset_x = add_items_size.x * 0.5f + size.x * 0.5f + 20.0f;
    ImGui::SetNextWindowPos(ImVec2(parent_center.x + offset_x, parent_center.y), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);

    ImGui::Begin(label.c_str(), &this->show_window, this->window_flags | ImGuiWindowFlags_NoCollapse);

    // Title (centered)
    float title_width = ImGui::CalcTextSize(label.c_str()).x;
    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - title_width) * 0.5f);
    ImGui::Text("%s", label.c_str());
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 15.0f));

    // Fixed label width for alignment
    const float label_width = 100.0f;
    const float input_width = 150.0f;

    if (caller.compare("Product") != 0)
    {
        ImGui::Text("Product:");
        ImGui::SameLine(label_width);
        ImGui::SetNextItemWidth(input_width);
        this->mset_products();
        this->create_listbox_filter(this->products, "Product", this->selected_product, '_', ImVec2(input_width, 200));
        ImGui::Dummy(ImVec2(0.0f, 15.0f));
    }

    // Brand Name input
    ImGui::Text("Brand Name:");
    ImGui::SameLine(label_width);
    ImGui::SetNextItemWidth(input_width);
    if (caller.compare("Product") == 0)
        ImGui::InputTextWithHint("##BrandName", "name", this->c_brand_name, IM_ARRAYSIZE(this->c_brand_name));
    else
        ImGui::InputTextWithHint("##NewBrandName", "name", this->c_n_brand_name, IM_ARRAYSIZE(this->c_n_brand_name));

    // Handle error messages
    if (this->quantity_win.input_error == true)
        this->admin_error_message(this->quantity_win.input_error);

    // Quantity window logic
    if (this->quantity_win.finish == true && this->quantity_win.minput_error == false)
    {
        std::vector<std::string> previous_quantities = quantities;
        previous_quantities.pop_back();
        bool status = check_for_duplicate(previous_quantities, quantities[quantities.size() - 1]);

        if (status == true)
        {
            quantities[quantities.size() - 1] = "";
            this->quantity_win.input_error = true;
        }

        this->quantity_win.show_window = false;
        this->quantity_win.finish = false;
        this->quantity_index++;
    }

    if (this->quantity_win.show_window == true)
        quantity_win.create_add_quantity_window(quantities[quantity_index], selling_price[quantity_index], buying_price[quantity_index], number_of_items[quantity_index], quantity_caller);
    else if (this->quantity_win.show_window == false && quantities.size() != 0 && quantities[quantities.size() - 1].compare("") == 0)
    {
        quantities.pop_back();
        selling_price.pop_back();
        buying_price.pop_back();
        number_of_items.pop_back();

        if (this->quantity_index != 0)
            this->quantity_index--;
    }

    ImGui::Dummy(ImVec2(0.0f, 15.0f));

    // Buttons (centered)
    float button_width = 120.0f;
    float total_buttons_width = button_width * 2 + 10.0f; // Two buttons + spacing
    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - total_buttons_width) * 0.5f);

    if (ImGui::Button("Add Quantity", ImVec2(button_width, 0)) && this->quantity_win.show_window == false)
    {
        if (caller.compare("Product") == 0)
            this->brand_name = this->c_brand_name;
        else
            this->brand_name = this->c_n_brand_name;

        if (this->brand_name.compare("") != 0)
        {
            quantities.push_back("");
            selling_price.push_back(0.0);
            buying_price.push_back(0.0);
            number_of_items.push_back(0);

            this->quantity_win.show_window = true;
            this->brand_name = this->selected_product + "_" + this->brand_name;
        }
        else
            this->input_error = true;
    }

    ImGui::SameLine(0.0f, 10.0f);

    if (ImGui::Button("Finish", ImVec2(button_width, 0)))
    {
        if (caller.compare("Product") == 0)
        {
            this->brand_name = this->c_brand_name;
            brand = product_name + "_" + this->brand_name;
        }
        else
        {
            this->brand_name = this->c_n_brand_name;
            brand = this->selected_product + "_" + this->brand_name;
        }

        bool found_unwanted_char = check_unwanted_characters(this->brand_name);

        if ((this->brand_name.compare("") == 0) || quantities.size() == 0 || found_unwanted_char == true)
        {
            this->input_error = true;
        }
        else
        {
            if (caller.compare("Product") != 0)
            {
                if (this->selected_product.compare("All Products") != 0)
                {
                    std::vector<std::string> v_brands = get_product_brands_array(this->products_collection, this->selected_product);
                    bool status = check_for_duplicate(v_brands, this->brand_name);

                    if (status == false)
                    {
                        insert_brand(this->products_collection, this->stock, brand, quantities, selling_price, buying_price, number_of_items, this->selected_product);
                        this->success = true;
                    }
                    else
                        this->input_error = true;
                }
                else
                    this->input_error = true;
            }

            quantity_index = 0;
            this->finish = true;
        }
    }

    ImGui::PopStyleColor(7); // Text, WindowBg, Button, ButtonHovered, ButtonActive, FrameBg, Border
    ImGui::PopStyleVar(4); // WindowRounding, FrameRounding, FramePadding, ItemSpacing
    ImGui::End();
    return 1;
}

int AdminUi::RemoveBrandWindow::r_brand_current_item = 0;
AdminUi::RemoveBrandWindow::RemoveBrandWindow(mongocxx::database db)
{
	this->products_collection = db["Products"];
	this->stock = db["Stock"];
	
}

int AdminUi::RemoveBrandWindow::create_remove_brand_window()
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(60, 60, 60, 255));

	ImGui::SetNextWindowPos(ImVec2(400, 400), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(350, 400), ImGuiCond_Once);

	ImGui::Begin("Remove Brand", &this->show_window, this->window_flags);

	
	ImGui::SameLine(80.0, 0.0);
	this->mset_products();

	this->create_listbox_filter(this->products, "Products", this->selected_product, '_', ImVec2(210, 150));


	ImVec2 combo_size = ImVec2(230, 50);
	ImGui::NewLine();

	if (this->selected_product.compare("All Products") != 0)
	{
		this->brands_array = get_product_brands_array(this->products_collection, this->selected_product);

		mset_brands(this->brands_array);
	}
	else
	{
		this->brands = "Select_";
	}

	// Reallocate this->p_brands with new size
	this->p_brands = std::make_unique<char[]>(this->brands.size() + 1);

	// Clear the memory
	std::fill(this->p_brands.get(), this->p_brands.get() + this->brands.size(), 0);

	// Copy the string
	std::strncpy(this->p_brands.get(), this->brands.c_str(), this->brands.size() + 1);

	// Call prepare_items_pointer
	this->prepare_items_pointer(this->brands, '_', this->p_brands);

	ImGui::SameLine(60.0, 0.0);
	create_combo(this->p_brands, "Brand", this->r_brand_current_item, combo_size);
	
	if (this->confirm == this->ACCEPTED)
	{
		delete_brand(this->products_collection, this->stock, this->selected_product, this->brands_array[r_brand_current_item - 1]);
		
		this->success = true;

		this->show_confirm_window = false;
		this->confirm = this->NONE;

		this->selected_product = "All Products";
		r_brand_current_item = 0;
	}
	else if (this->confirm == this->REJECTED)
	{
		this->show_confirm_window = false;
		this->confirm = this->NONE;
	}

	if (this->show_confirm_window == true)
		this->admin_confirm_message(this->confirm);
	else if (this->input_error)
		this->admin_error_message(this->input_error);
		
	
	ImGui::Dummy(ImVec2(0.0, 10.0));
	ImGui::NewLine();
	ImGui::SameLine(70.0, 0.0);
	if (ImGui::Button("Remove", ImVec2(120, 0)))
	{
		if (this->selected_product.compare("All Products") == 0  || r_brand_current_item == 0)
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