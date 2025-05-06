#include "admin_ui.h"
#include "insert_components.h"
#include "delete_components.h"
#include "retrieve_data.h"
#include "imgui_internal.h"
#include <regex>
#include <algorithm>

AdminUi::AddQuantityWindow::AddQuantityWindow(mongocxx::database db)
{
    this->products_collection = db["Products"];
    this->stock = db["tock"];
    this->mset_products();
}

int AdminUi::AddQuantityWindow::a_current_brand_index = 0;
char AdminUi::AddQuantityWindow::c_quantity_name[128] = "";
char AdminUi::AddQuantityWindow::c_new_brand_quantity_name[128] = "";
char AdminUi::AddQuantityWindow::c_new_quantity_name[128] = "";

int AdminUi::AddQuantityWindow::create_add_quantity_window(std::string& quantity, double& selling_price, double& buying_price, int& number_of_items, std::string caller)
{
    ImVec2 size = ImVec2(300, 350);
    std::string label = "Add Quantity";
    if (caller.compare("New Brand") == 0)
    {
        label = "Add New Brand Quantity ";
    }
    else if (caller.compare("New Quantity") == 0)
    {
        size = ImVec2(400, 500);
        label = "Add New Quantity ";
    }

    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(60, 60, 60, 255));

    ImGui::SetNextWindowPos(ImVec2(400, 400), ImGuiCond_Once);
    ImGui::SetNextWindowSize(size, ImGuiCond_Once);

    ImGui::Begin(label.c_str(), &this->show_window, this->window_flags);

    if (caller.compare("New Quantity") == 0)
    {
        ImGui::SameLine(120.0, 0.0);
        this->mset_products();
        this->create_listbox_filter(this->products, "Products", this->selected_product, '_', ImVec2(210, 150));

        ImVec2 combo_size = ImVec2(230, 50);

        ImGui::NewLine();

        if (this->selected_product.compare("All Products") != 0)
        {
            this->brands_array = get_product_brands_array(this->products_collection, this->selected_product);

            mset_brands(this->brands_array);

            // Reallocate this->p_brands with new size
            this->p_brands = std::make_unique<char[]>(this->brands.size() + 1);

            // Clear the memory
            std::fill(this->p_brands.get(), this->p_brands.get() + this->brands.size(), 0);

            // Copy the string
            std::strncpy(this->p_brands.get(), this->brands.c_str(), this->brands.size() + 1);

            // Call prepare_items_pointer
            this->prepare_items_pointer(this->brands, '_', this->p_brands);
        }

        create_combo(this->p_brands, "Brand", this->a_current_brand_index, combo_size);
    }

    ImGui::Dummy(ImVec2(0.0, 10.0));
    ImGui::NewLine();
    ImGui::Text("Quantity Name: ");

    ImGui::SameLine(100.0, 0.0);
    ImGui::SetNextItemWidth(150);

    if (caller.compare("Brand") == 0)
        ImGui::InputTextWithHint("###", "name", this->c_quantity_name, IM_ARRAYSIZE(this->c_quantity_name));
    else if (caller.compare("New Brand") == 0)
        ImGui::InputTextWithHint("###", "name", this->c_new_brand_quantity_name, IM_ARRAYSIZE(this->c_new_brand_quantity_name));
    else if (caller.compare("New Quantity") == 0)
        ImGui::InputTextWithHint("###", "name", this->c_new_quantity_name, IM_ARRAYSIZE(this->c_new_quantity_name));

    ImGui::NewLine();
    ImGui::Text("Buying Price: ");

    ImGui::SameLine(100.0, 0.0);
    ImGui::SetNextItemWidth(150);

    ImGui::PushID("buying price");
    ImGui::InputFloat("##", &this->buying_price, 0);
    ImGui::PopID();

    ImGui::NewLine();
    ImGui::Text("Selling Price: ");

    ImGui::SameLine(100.0, 0.0);
    ImGui::SetNextItemWidth(150);

    ImGui::PushID("selling price");
    ImGui::InputFloat("##", &this->selling_price, 0);
    ImGui::PopID();

    ImGui::NewLine();
    ImGui::Text("Number: ");

    ImGui::SameLine(100.0, 0.0);
    ImGui::SetNextItemWidth(150);

    ImGui::InputInt("##", &this->amount, 0);

    ImGui::Dummy(ImVec2(0.0, 10.0));
    ImGui::NewLine();
    ImGui::SameLine(90.0, 0.0);


    if (ImGui::Button("Finish", ImVec2(120, 0)))
    {
        int current_brand_index = 1;

        if (caller.compare("Brand") == 0)
            this->quantity_name = this->c_quantity_name;
        else if (caller.compare("New Brand") == 0)
            this->quantity_name = this->c_new_brand_quantity_name;
        else if (caller.compare("New Quantity") == 0)
        {
            current_brand_index = a_current_brand_index;
            this->quantity_name = this->c_new_quantity_name;
        }


        bool found_unwanted_char = check_unwanted_characters(this->quantity_name);

        quantity = this->quantity_name;
        buying_price = this->buying_price;
        selling_price = this->selling_price;
        number_of_items = this->amount;

        if (this->quantity_name.compare("") == 0 || this->buying_price <= 0 || this->selling_price <= 0 || this->amount < 0 || found_unwanted_char == true || current_brand_index == 0)
        {
            this->input_error = true;
        }
        else
        {
            if (caller.compare("New Quantity") == 0)
            {
                std::vector<std::string> v_quantites = get_brand_quantities_array(this->products_collection, this->selected_product + "_" + this->brands_array[a_current_brand_index - 1]);
                bool status = check_for_duplicate(v_quantites, this->quantity_name);

                if (status == false)
                {
                    insert_quantity(this->products_collection, this->stock, this->selected_product, this->brands_array[a_current_brand_index - 1], this->quantity_name, this->selling_price, this->buying_price, this->amount);
                    this->success = true;
                }
                else
                    this->input_error = true;
            }
            this->finish = true;
        }
    }

    ImGui::PopStyleColor();
    ImGui::End();
    return 1;
}

int AdminUi::AddQuantityWindow::mset_brands(std::vector<std::string> vbrands)
{
    this->brands = "Select_";

    for (std::string brand : vbrands)
        this->brands += brand + "_";
    return 1;
}

int AdminUi::AddQuantityWindow::mset_products()
{
    this->v_products = get_products_array(this->products_collection);

    this->products = "All Products_";

    for (std::string m_product : v_products)
    {
        this->products += m_product + "_";
    }

    return 1;
}

int AdminUi::AddQuantityWindow::admin_error_message(bool& error_flag)
{
    ImGui::OpenPopup("Error");
    // Always center this window when appearing
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Fill In The Details or,\nThe Item already exists! or, \nRemove Unwanted Character(s)");
        ImGui::Separator();

        ImGui::SameLine(60.0f, 0.0f);
        if (ImGui::Button("OK", ImVec2(90, 0)))
        {
            error_flag = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    return 1;
}

bool AdminUi::AddQuantityWindow::check_unwanted_characters(std::string entered_string)
{
    for (char c : entered_string)
    {
        auto iterator = std::find(this->unwanted_chars.begin(), this->unwanted_chars.end(), c);

        if (iterator != this->unwanted_chars.end())
            return true;
    }

    return false;
}

bool AdminUi::AddQuantityWindow::check_for_duplicate(std::vector<std::string> items, std::string item)
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

int AdminUi::AddQuantityWindow::admin_confirm_message(int& confirm_flag)
{
	ImGui::OpenPopup("Confirm");
	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Confirm", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Are You Sure You Want To Complete This Action?");
		ImGui::Separator();

		ImGui::SameLine(20.0f, 0.0f);
		if (ImGui::Button("No", ImVec2(90, 0)))
		{
			confirm_flag = REJECTED;
			ImGui::CloseCurrentPopup();

		}

		ImGui::SameLine(200.0f, 0.0f);
		if (ImGui::Button("Yes", ImVec2(90, 0)))
		{
			confirm_flag = ACCEPTED;
			ImGui::CloseCurrentPopup();

		}
		
	
		ImGui::EndPopup();
	}	

	return 1;
}

int AdminUi::AddQuantityWindow::mset_quantities(std::vector<std::string> vquantities)
{
    this->quantities = "Select_";

    for (std::string quantity : vquantities)
        this->quantities += quantity + "_";
    return 1;
}

int AdminUi::RemoveQuantityWindow::r_quantity_brand_current_item = 0;
int AdminUi::RemoveQuantityWindow::r_quantity_current_item = 0;
AdminUi::RemoveQuantityWindow::RemoveQuantityWindow(mongocxx::database db)
{
	this->products_collection = db["Products"];
	this->stock = db["Stock"];

}


int AdminUi::RemoveQuantityWindow::create_remove_quantity_window()
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(60, 60, 60, 255));

	ImGui::SetNextWindowPos(ImVec2(400, 400), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(350, 400), ImGuiCond_Once);

	ImGui::Begin("Remove Quantity", &this->show_window, this->window_flags);


	ImGui::SameLine(70.0, 0.0);
	this->mset_products();
	this->create_listbox_filter(this->products, "Products", this->selected_product, '_', ImVec2(210, 150));
		

	if (this->selected_product.compare("All Products") != 0)
	{
		this->brands_array = get_product_brands_array(this->products_collection, this->selected_product);

		mset_brands(this->brands_array);
	}
	else
	{
		this->brands = "Select_";		
	}

	if (r_quantity_brand_current_item != 0)
	{
		this->quantities_array = get_brand_quantities_array(this->products_collection, this->selected_product + "_" + this->brands_array[r_quantity_brand_current_item - 1]);

		mset_quantities(this->quantities_array);
	}
	else
	{
		this->quantities = "Select_";
	}

	// Reallocate this->p_brands with new size
	this->p_brands = std::make_unique<char[]>(this->brands.size() + 1);

	// Clear the memory
	std::fill(this->p_brands.get(), this->p_brands.get() + this->brands.size(), 0);

	// Copy the string
	std::strncpy(this->p_brands.get(), this->brands.c_str(), this->brands.size() + 1);

	// Call prepare_items_pointer
	this->prepare_items_pointer(this->brands, '_', this->p_brands);

	ImVec2 combo_size = ImVec2(230, 50);
	ImGui::NewLine();
	ImGui::SameLine(60.0, 0.0);
	create_combo(this->p_brands, "Brand", this->r_quantity_brand_current_item, combo_size);
	
	// Reallocate this->p_quantities with new size
	this->p_quantities = std::make_unique<char[]>(this->quantities.size() + 1);

	// Clear the memory
	std::fill(this->p_quantities.get(), this->p_quantities.get() + this->quantities.size(), 0);

	// Copy the string
	std::strncpy(this->p_quantities.get(), this->quantities.c_str(), this->quantities.size() + 1);

	// Call prepare_items_pointer
	this->prepare_items_pointer(this->quantities, '_', this->p_quantities);

	ImGui::NewLine();
	ImGui::SameLine(60.0, 0.0);
	create_combo(this->p_quantities, "Quantity", this->r_quantity_current_item, combo_size);

	if (this->confirm == this->ACCEPTED)
	{
		delete_quantity(this->products_collection, this->stock, this->selected_product, this->brands_array[r_quantity_brand_current_item - 1], this->quantities_array[r_quantity_current_item - 1]);

		this->success = true;

		this->show_confirm_window = false;
		this->confirm = this->NONE;

		this->selected_product = "All Products";
		r_quantity_current_item = 0;
		r_quantity_brand_current_item = 0;
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
	ImGui::SameLine(100.0, 0.0);
	if (ImGui::Button("Remove", ImVec2(120, 0)))
	{																 //no item selected, current selected item is Select
		if (this->selected_product.compare("All Products") == 0 || r_quantity_brand_current_item == 0 || r_quantity_current_item == 0)
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