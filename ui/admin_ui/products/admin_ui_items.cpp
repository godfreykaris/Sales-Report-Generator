#include "admin_ui.h"
#include "insert_components.h"
#include "delete_components.h"
#include "retrieve_data.h"
#include "imgui_internal.h"

AdminUi::AddorRemoveItemWindow::AddorRemoveItemWindow(AddItemWindow& add_item, RemoveItemWindow& remove_item)
{
	this->add_item    = &add_item;
	this->remove_item = &remove_item;
}

int AdminUi::AddorRemoveItemWindow::create_add_or_remove_items_window()
{
	// Modern dark theme with rounded corners
	ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(40, 44, 52, 255));
	ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 128, 128, 50)); // Teal border for glassmorphism
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f); // Rounded window corners
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f); // Rounded button corners
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 6.0f)); // Larger button padding

	// Center window with responsive size
	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(320, 240), ImGuiCond_Once);

	ImGui::Begin("Add Or Remove Item", &this->show_window, this->window_flags | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

	// Title with modern typography
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]); // Assuming a bold font; adjust if needed
	ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("Manage Items").x) * 0.5f);
	ImGui::TextColored(ImVec4(0.95f, 0.95f, 0.95f, 1.0f), "Manage Items");
	ImGui::PopFont();

	// Stylish separator with teal accent
	ImGui::PushStyleColor(ImGuiCol_Separator, IM_COL32(0, 128, 128, 100));
	ImGui::Separator();
	ImGui::PopStyleColor();
	ImGui::Dummy(ImVec2(0.0f, 15.0f));

	// Button styling: Teal with neumorphic shadow
	ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 128, 128, 255));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 160, 160, 255));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0, 192, 192, 255));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.95f, 1.0f)); // White text for buttons
	ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 100, 100, 100)); // Subtle shadow effect
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);

	// Center buttons with larger size for modern look
	float button_width = 180.0f;
	float window_width = ImGui::GetContentRegionAvail().x;

	ImGui::SetCursorPosX((window_width - button_width) * 0.5f);
	if (ImGui::Button("Add Item", ImVec2(button_width, 40.0f)))
		this->add_item->show_window = true;

	ImGui::Dummy(ImVec2(0.0f, 15.0f));
	ImGui::SetCursorPosX((window_width - button_width) * 0.5f);
	if (ImGui::Button("Remove Item", ImVec2(button_width, 40.0f)))
		this->remove_item->show_window = true;

	// Pop button styles (5 colors + 1 style var)
	ImGui::PopStyleColor(5);
	ImGui::PopStyleVar(1); // Pop FrameBorderSize

	// Nested window calls (logic unchanged)
	if (this->add_item->show_window)
		this->add_item->create_add_items_window();

	if (this->remove_item->show_window)
		this->remove_item->create_remove_items_window();

	// Pop window styles (2 colors + 4 style vars)
	ImGui::PopStyleColor(2); // WindowBg, Border
	ImGui::PopStyleVar(4); // WindowRounding, WindowBorderSize, FrameRounding, FramePadding
	ImGui::End();

	return 1;
}
AdminUi::AddNewBrandWindow::AddNewBrandWindow(mongocxx::database db)
{
    this->brand_win.products_collection = db["Products"];
    this->brand_win.stock = db["Stock"];
    this->brand_win.mset_products();
}



int AdminUi::AddNewBrandWindow::create_add_new_brand_window()
{
    // Handle any error
    if (this->brand_win.input_error == true)
        this->brand_win.admin_error_message(this->brand_win.input_error);

    if (this->brand_win.finish == true && this->brand_win.input_error == false)
    {
        this->brand_win.show_window = false;
        this->brand_win.finish = false;
    }

    this->brand_win.create_add_brand_window(this->selected_product, this->brand_name, this->n_qty_names, this->n_selling_price, this->n_buying_price, this->n_number_of_items, "New");

    return 1;
}

AdminUi::AddNewQuantityWindow::AddNewQuantityWindow(mongocxx::database db)
{
    this->quantity_win.products_collection = db["Products"];
    this->quantity_win.stock = db["Stock"];
    this->quantity_win.mset_products();
}

int AdminUi::AddNewQuantityWindow::create_add_new_quantity_window()
{
    if (this->quantity_win.finish == true && this->quantity_win.input_error == false)
    {
        this->quantity_win.show_window = false;
        this->quantity_win.finish = false;
    }

    this->quantity_win.create_add_quantity_window(this->quantity_name, this->n_selling_price, this->n_buying_price, this->n_number_of_items, "New Quantity");

    // Handle any error
    if (this->quantity_win.input_error == true)
        this->admin_error_message(this->quantity_win.input_error);

    return 1;
}

AdminUi::AddItemWindow::AddItemWindow(AddProductWindow& add_product, AddNewBrandWindow& add_brand, AddNewQuantityWindow& add_quantity)
{
    this->add_product = &add_product;
    this->add_brand = &add_brand;
    this->add_quantity = &add_quantity;
}

int AdminUi::AddItemWindow::create_add_items_window()
{
    // Push UX-friendly colors
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.95f, 1.0f)); // Near-white text for high contrast
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.13f, 0.14f, 0.15f, 1.0f)); // Dark charcoal background
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 1.0f)); // Soft blue buttons
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.36f, 0.69f, 1.0f, 1.0f)); // Lighter blue on hover
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.20f, 0.49f, 0.88f, 1.0f)); // Darker blue when clicked
    ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.26f, 0.59f, 0.98f, 0.5f)); // Soft blue separator
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f); // Slightly rounded window corners
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f); // Rounded button corners
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 6.0f)); // Comfortable button padding

    // Set window position and size (centered, responsive)
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(340, 300), ImGuiCond_Once);

    // Create window with basic flags
    ImGui::Begin("Add Item", &this->show_window, this->window_flags | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    // Title (centered)
    float title_width = ImGui::CalcTextSize("Add New Item").x;
    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - title_width) * 0.5f);
    ImGui::Text("Add New Item");
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 20.0f));

    // Buttons for Product, Brand, Quantity
    float button_width = 200.0f;
    float window_width = ImGui::GetContentRegionAvail().x;

    // Product button
    ImGui::SetCursorPosX((window_width - button_width) * 0.5f);
    if (ImGui::Button("Product", ImVec2(button_width, 40.0f)))
        this->add_product->show_window = true;

    ImGui::Dummy(ImVec2(0.0f, 15.0f));

    // Brand button
    ImGui::SetCursorPosX((window_width - button_width) * 0.5f);
    if (ImGui::Button("Brand", ImVec2(button_width, 40.0f)))
        this->add_brand->brand_win.show_window = true;

    ImGui::Dummy(ImVec2(0.0f, 15.0f));

    // Quantity button
    ImGui::SetCursorPosX((window_width - button_width) * 0.5f);
    if (ImGui::Button("Quantity", ImVec2(button_width, 40.0f)))
        this->add_quantity->quantity_win.show_window = true;

    // Nested window calls (unchanged)
    if (this->add_product->show_window)
        this->add_product->create_add_product_window();

    if (this->add_product->success == true)
        this->add_product->notify_sale_status(this->add_product->success);

    if (this->add_product->finish)
    {
        this->add_product->show_window = false;
        this->add_product->finish = false;
    }

    if (this->add_brand->brand_win.show_window)
        this->add_brand->create_add_new_brand_window();

    if (this->add_brand->brand_win.success == true)
        this->add_brand->brand_win.notify_sale_status(this->add_brand->brand_win.success);

    if (this->add_quantity->quantity_win.show_window)
        this->add_quantity->create_add_new_quantity_window();

    if (this->add_quantity->quantity_win.success == true)
        this->add_quantity->notify_sale_status(this->add_quantity->quantity_win.success);

    // Pop styles
    ImGui::PopStyleColor(6); // Text, WindowBg, Button, ButtonHovered, ButtonActive, Separator
    ImGui::PopStyleVar(3);  // WindowRounding, FrameRounding, FramePadding
    ImGui::End();

    return 1;
}

AdminUi::RemoveItemWindow::RemoveItemWindow(RemoveProductWindow& remove_product, RemoveBrandWindow& remove_brand, RemoveQuantityWindow& remove_quantity)
{
	this->remove_product = &remove_product;
	this->remove_brand = &remove_brand;
	this->remove_quantity = &remove_quantity;
}

int AdminUi::RemoveItemWindow::create_remove_items_window()
{
    // Push UX-friendly colors
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.95f, 1.0f)); // Near-white text for high contrast
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.13f, 0.14f, 0.15f, 1.0f)); // Dark charcoal background
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 1.0f)); // Soft blue buttons
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.36f, 0.69f, 1.0f, 1.0f)); // Lighter blue on hover
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.20f, 0.49f, 0.88f, 1.0f)); // Darker blue when clicked
    ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.26f, 0.59f, 0.98f, 0.5f)); // Soft blue separator
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f); // Slightly rounded window corners
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f); // Rounded button corners
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 6.0f)); // Comfortable button padding

    // Set window position and size (centered, responsive)
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(340, 300), ImGuiCond_Once);

    // Create window with basic flags
    ImGui::Begin("Remove Item", &this->show_window, this->window_flags | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    // Title (centered)
    float title_width = ImGui::CalcTextSize("Remove Item").x;
    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - title_width) * 0.5f);
    ImGui::Text("Remove Item");
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 20.0f));

    // Buttons for Product, Brand, Quantity
    float button_width = 200.0f;
    float window_width = ImGui::GetContentRegionAvail().x;

    // Product button
    ImGui::SetCursorPosX((window_width - button_width) * 0.5f);
    if (ImGui::Button("Product", ImVec2(button_width, 40.0f)))
        this->remove_product->show_window = true;

    ImGui::Dummy(ImVec2(0.0f, 15.0f));

    // Brand button
    ImGui::SetCursorPosX((window_width - button_width) * 0.5f);
    if (ImGui::Button("Brand", ImVec2(button_width, 40.0f)))
        this->remove_brand->show_window = true;

    ImGui::Dummy(ImVec2(0.0f, 15.0f));

    // Quantity button
    ImGui::SetCursorPosX((window_width - button_width) * 0.5f);
    if (ImGui::Button("Quantity", ImVec2(button_width, 40.0f)))
        this->remove_quantity->show_window = true;

    // Nested window calls and state handling
    if (this->remove_product->show_window)
        this->remove_product->create_remove_product_window();

    if (this->remove_product->success == true)
    {
        this->remove_product->notify_sale_status(this->remove_product->success);
        this->remove_product->show_window = false;
    }

    if (this->remove_brand->show_window)
        this->remove_brand->create_remove_brand_window();

    if (this->remove_brand->success == true)
    {
        this->remove_brand->notify_sale_status(this->remove_brand->success);
        this->remove_brand->show_window = false;
    }

    if (this->remove_quantity->show_window)
        this->remove_quantity->create_remove_quantity_window();

    if (this->remove_quantity->success == true)
    {
        this->remove_quantity->notify_sale_status(this->remove_quantity->success);
        this->remove_quantity->show_window = false; // Added to ensure consistency
    }

    // Pop styles
    ImGui::PopStyleColor(6); // Text, WindowBg, Button, ButtonHovered, ButtonActive, Separator
    ImGui::PopStyleVar(3);  // WindowRounding, FrameRounding, FramePadding
    ImGui::End();

    return 1;
}
