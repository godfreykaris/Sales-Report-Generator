#pragma once

#include <string>
#include <vector>
#include <memory> // For smart pointers

#include "imgui.h"
#include "app_common.h"
#include "change_password_ui/change_password.h"

#include <mongocxx/collection.hpp>
#include <mongocxx/database.hpp>

// External flag for controlling window visibility
extern bool regular_user_window;

// Regular user's user-interface namespace
namespace RUserUi
{
    // Base Window class for shared functionality
    class Window
    {
    public:
        // Constructor and Destructor
        Window();
        virtual ~Window() = default; // Virtual destructor for proper cleanup in derived classes

        /* Properties */
    public:
        // Strings for UI elements
        std::string products = "All Products_"; // Default product list
        std::string brands = "Select_";         // Default brand list
        std::string quantities = "Select_";     // Default quantity list

        // Vectors for storing data
        std::vector<std::string> products_array;
        std::vector<std::string> brands_array;
        std::vector<std::string> quantities_array;
        std::vector<int> amounts_array;

        // Vectors for detailed product data
        std::vector<std::vector<std::string>> brand_quantities_array;
        std::vector<std::vector<int>> brand_amounts_array;
        std::vector<std::vector<std::string>> all_brands_array;
        std::vector<std::vector<std::vector<std::string>>> all_quantities_array;
        std::vector<std::vector<std::vector<int>>> all_amounts_array;

        // Smart pointers for combo box items
        std::unique_ptr<char[]> p_brands;
        std::unique_ptr<char[]> p_quantities;

        // Selected items
        std::string selected_product = "All Products";
        std::string selected_brand = "Select";
        std::string selected_quantity = "Select";

		std::vector<char> unwanted_chars = { '_', '\\', ';', '\'', ',','.', '/' };

        bool show_window = false;
        bool product_changed = false;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;

        // Database collections
        mongocxx::collection products_collection;
        mongocxx::collection stock;
        mongocxx::collection agents;

        /* Methods */
    public:
        void prepare_items_pointer(const std::string& items, char item_separator, std::unique_ptr<char[]>& items_pointer);
        int display_all_products(bool show_low_stock = false, int low_stock_threshold = 0, bool show_out_of_stock = false);
        int display_all_brands(const std::string& product, bool show_low_stock, int low_stock_threshold, bool show_out_of_stock,
            std::vector<std::tuple<std::string, std::string, std::string, int>>& print_data, bool& has_valid_items,
            std::vector<int>& edited_remaining, std::vector<bool>& is_edited, size_t& row_index,
            bool& show_success_popup, bool& show_failure_popup, bool& show_invalid_popup);
int display_brand_quantities(const std::string& product, const std::string& brand, bool show_low_stock, int low_stock_threshold, bool show_out_of_stock,
                 std::vector<std::tuple<std::string, std::string, std::string, int>>& print_data, bool& has_valid_items,
                 std::vector<int>& edited_remaining, std::vector<bool>& is_edited, size_t& row_index,
                 bool& show_success_popup, bool& show_failure_popup, bool& show_invalid_popup);
int display_quantity(const std::string& product, const std::string& brand, const std::string& quantity, bool show_low_stock, int low_stock_threshold, bool show_out_of_stock,
          std::vector<std::tuple<std::string, std::string, std::string, int>>& print_data, bool& has_valid_items,
          std::vector<int>& edited_remaining, std::vector<bool>& is_edited, size_t& row_index,
          bool& show_success_popup, bool& show_failure_popup, bool& show_invalid_popup);
        int set_products();
        int set_brands(const std::string& m_brands);
        int set_quantities(const std::string& m_quantities);
        int set_products_array(const std::vector<std::string>& m_products);
        int set_brands_array(const std::vector<std::string>& m_brands);
        int set_quantities_array(const std::vector<std::string>& m_quantities);
        std::string get_selected_product() const;
        std::string get_selected_brand(int selected_index);
        std::string get_selected_quantity(int selected_index);
        int reset_combo_items();
        int create_listbox_filter(const std::string& items, const std::string& items_description,
            std::string& selected_item, char item_separator, ImVec2 size,
            const std::string& caller = "Regular User");
        int create_combo(const std::unique_ptr<char[]>& items_names, const std::string& description,
            int& current_item, ImVec2 size);

		int notify_sale_status(bool& success_flag);
		bool check_unwanted_characters(std::string entered_string);
		bool check_for_duplicate(std::vector<std::string> items, std::string item);
    };

    // Stock Window for viewing inventory
    class StockWindow : public Window
    {
    public:
        StockWindow(mongocxx::database db);
        StockWindow() = default;
        ~StockWindow() = default;

        /* Properties */
    public:
        static int stock_brand_current_item;
        static int stock_quantity_current_item;
        static bool show_out_of_stock; 
        static bool show_low_stock;    
        static int low_stock_threshold;    
        std::string last_saved_file;

        /* Methods */
    public:
        int create_stock_window();
        int setup_stock_window();
        int display_stock_table(std::vector<std::tuple<std::string, std::string, std::string, int>>& print_data, bool& has_valid_items);
        int handle_stock_print(const std::vector<std::tuple<std::string, std::string, std::string, int>>& print_data, bool has_valid_items);
    };

    // Sell Window for processing sales
    class SellWindow : public Window
    {
    public:
        SellWindow(mongocxx::database db);
        ~SellWindow() = default;

        /* Properties */
    public:
        std::string seasons = "Select_";
        std::string age_groups = "Select_";
        std::string genders = "Select_";
        std::string locations = "Select_";

        std::vector<std::string> seasons_array;
        std::vector<std::string> age_groups_array;
        std::vector<std::string> genders_array;
        std::vector<std::string> locations_array;

        std::unique_ptr<char[]> p_seasons;
        std::unique_ptr<char[]> p_age_groups;
        std::unique_ptr<char[]> p_genders;
        std::unique_ptr<char[]> p_locations;

        mutable std::string selected_season = "Select";
        mutable std::string selected_age_group = "Select";
        mutable std::string selected_gender = "Select";
        mutable std::string selected_location = "Select";

        static int sell_brand_current_item;
        static int sell_quantity_current_item;
        static int sell_season_current_item;
        static int sell_age_group_current_item;
        static int sell_gender_current_item;
        static int sell_location_current_item;

        static bool input_error;
        static bool success;

        static int number_of_items;

        /* Methods */
    public:
        int create_sell_window();
        int set_seasons();
        int set_age_groups();
        int set_genders();
        int set_locations();
        int populate_brands(const std::string& product);
        int populate_quantities(const std::string& brand);
        std::string get_selected_season() const;
        std::string get_selected_age_group() const;
        std::string get_selected_gender() const;
        std::string get_selected_location() const;
        int get_number_of_items() const;
        int handle_error(bool& err_flag);
        int notify_sale_status();
    };

    // Agents Window for viewing agent details
    class AgentsWindow
    {
    public:
        AgentsWindow(mongocxx::database db);
        AgentsWindow() = default;
        ~AgentsWindow() = default;

        /* Properties */
    public:
        std::vector<std::string> ids;
        std::vector<std::string> names;
        std::vector<std::string> phones;
        std::vector<std::string> locations;

        std::vector<std::string> filtered_ids;
        std::vector<std::string> filtered_names;
        std::vector<std::string> filtered_phones;
        std::vector<std::string> filtered_locations;

        std::vector<std::vector<std::string>> agent_details;

        std::string filter_types = "No Criteria Selected_Filter by Name_Filter by Phone_Filter by Location_";
        std::unique_ptr<char[]> p_filter_types;

        static int current_filter_type_index;

        bool show_window = false;
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
        ImGuiTableFlags table_flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg |
            ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
            ImGuiTableFlags_Reorderable;

        mongocxx::collection agents;

        float TEXT_BASE_HEIGHT = 0.0f;

        /* Methods */
    public:
        int create_search_filter_combo(const std::unique_ptr<char[]>& items_names, const std::string& description,
            int& current_item, ImVec2 size);
        void prepare_filter_types_pointer(const std::string& items, char item_separator,
            std::unique_ptr<char[]>& items_pointer);
        int create_agents_filter();
        int draw_agents_table(float text_base_height);
        int create_agents_window();
    };

    // Return Sale Window for handling product returns
    class ReturnSaleWindow : public AgentsWindow, public SellWindow
    {
    public:
        ReturnSaleWindow(mongocxx::database db);
        ~ReturnSaleWindow() = default;

        /* Properties */
    public:
        bool show_window = false;
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
        mongocxx::collection agents;
        static int return_current_filter_type_index;
        std::vector<bool> checked_sales;

        std::vector<std::vector<std::string>> sales_data;

        std::vector<std::string> sales_product_names;
        std::vector<std::string> sales_brands;
        std::vector<std::string> sales_quantities;
        std::vector<std::string> sales_seasons;
        std::vector<std::string> sales_age_groups;
        std::vector<std::string> sales_genders;
        std::vector<std::string> sales_locations;
        std::vector<std::string> sales_item_counts;
        std::vector<std::string> sales_dates_and_times;
        std::vector<std::string> sales_indices;

        std::vector<std::string> filtered_sales_product_names;
        std::vector<std::string> filtered_sales_brands;
        std::vector<std::string> filtered_sales_quantities;
        std::vector<std::string> filtered_sales_seasons;
        std::vector<std::string> filtered_sales_age_groups;
        std::vector<std::string> filtered_sales_genders;
        std::vector<std::string> filtered_sales_locations;
        std::vector<std::string> filtered_sales_item_counts;
        std::vector<std::string> filtered_sales_dates_and_times;
        std::vector<std::string> filtered_sales_indices;

        size_t sales_count = 0;

        enum confirm_state { ACCEPTED = 0, REJECTED = 1, NONE = 2 };
        int confirm = confirm_state::NONE;
        bool show_confirm_window = false;

        std::vector<ImVec4> colum_colors = {
            ImVec4(0.25f, 0.8f, 0.11f, 0.65f), // Greenish
            ImVec4(0.73f, 0.2f, 0.34f, 0.65f), // Reddish
            ImVec4(0.26f, 0.6f, 0.2f, 0.65f),  // Green
            ImVec4(0.39f, 0.13f, 0.47f, 0.65f), // Purple
            ImVec4(0.8f, 0.55f, 0.22f, 0.65f), // Orange
            ImVec4(0.15f, 0.4f, 0.68f, 0.65f), // Blue
            ImVec4(0.69f, 0.7f, 0.4f, 0.65f),  // Yellowish
            ImVec4(0.31f, 0.21f, 0.33f, 0.65f), // Dark
            ImVec4(0.45f, 0.38f, 0.72f, 0.65f), // Purple
            ImVec4(0.17f, 0.61f, 0.38f, 0.65f), // Green
            ImVec4(0.28f, 0.11f, 0.88f, 0.65f)  // Blue
        };

        std::string sales_filter_types = "No Criteria Selected_Filter by Name_Filter by Location_Filter by Date&Time_";
        std::unique_ptr<char[]> p_sales_filter_types;

        bool success = false;
        bool input_error = false;

        SALE sale;

        /* Methods */
    public:
        int create_return_sale_window();
        int create_sales_data_filter();
        int regular_user_confirm_message(int& confirm_flag);
        int draw_sales_data_table(float text_base_height);
    };

    class AddQuantityWindow : public Window
	{
		/*Constructor and Detructor*/
	public:
		AddQuantityWindow(mongocxx::database db);
		AddQuantityWindow() = default;
		~AddQuantityWindow() = default;

		/*properties*/
	public:
		std::string quantity_name;
		static char c_quantity_name[128];
		static char c_new_brand_quantity_name[128];
		static char c_new_quantity_name[128];

		float buying_price = 0.0;
		float selling_price = 0.0;
		int amount = 0;

		static int a_current_brand_index;
		
		bool finish = false;

		bool minput_error = false;
		bool msuccess = false;

				
		enum confirm_state { ACCEPTED = 0, REJECTED = 1, NONE = 2 };
		int confirm = confirm_state::NONE;

		bool  show_confirm_window = false;

		bool duplicate_error = false;

		mongocxx::collection  products_collection;
		mongocxx::collection stock;

		std::vector<std::string> v_products;
		std::vector<std::string> v_brands;

		std::vector<char> unwanted_chars = { '_', '\\', ';', '\'', ',','.', '/' };

		bool show_window = false;
		ImGuiWindowFlags window_flags;

		/*methods*/
	public:
		int create_add_quantity_window(std::string& quantities, double& selling_price, double& buying_price, int& number_of_items, std::string caller);
		static int admin_error_message(bool& error_flag);
		static int admin_confirm_message(int& confirm_flag);
		
		int mset_quantities(std::vector<std::string> vquantities);
		int mset_brands(std::vector<std::string> vbradns);
		int mset_products();
		
	};

	class AddBrandWindow : public AddQuantityWindow
	{
		/*Constructor and Detructor*/
	public:
		AddBrandWindow(mongocxx::database db);
		AddBrandWindow() = default;
		~AddBrandWindow() = default;

	public:
		/*properties*/
		
		std::string brand_name;
		static char c_brand_name[128];
		static char c_n_brand_name[128];


		int quantity_index = 0;

		bool finish = false;

		AddQuantityWindow quantity_win;

		bool show_window = false;
		ImGuiWindowFlags window_flags;

		/*methods*/
	public:
		int create_add_brand_window(std::string product_name, std::string& brand, std::vector<std::string>& quantities, std::vector<double>& selling_price, std::vector<double>& buying_price, std::vector<int>& number_of_items, std::string caller);
	};



	class AddProductWindow : public AddBrandWindow
	{
		/*Constructor and Detructor*/
	public:
		AddProductWindow(mongocxx::database db);
		AddProductWindow() = default;
		~AddProductWindow() = default;

		/*properties*/
	public:
		std::string product_name;
		static char c_product_name[128];

		int brand_index = 0;
				
		std::vector<std::string> brand_names;
		std::vector<std::vector<std::string>> qty_names;
		std::vector<std::vector<double>> selling_price;
		std::vector<std::vector<double>> buying_price;
		std::vector<std::vector<int>> number_of_items;

		AddBrandWindow  brand_win;

		bool show_window = false;
		ImGuiWindowFlags window_flags;

		/*methods*/
	public:
		int create_add_product_window();
	};


	class AddNewBrandWindow : public AddQuantityWindow
	{
		/*Constructor and Detructor*/
	public:
		AddNewBrandWindow(mongocxx::database db);
		AddNewBrandWindow() = default;
		~AddNewBrandWindow() = default;

		/*properties*/
	public:
		std::string brand_name;
		

		int n_quantity_index = 0;

		std::vector<std::string> n_qty_names;
		std::vector<double> n_selling_price;
		std::vector<double> n_buying_price;
		std::vector<int> n_number_of_items;

		AddBrandWindow  brand_win;

		bool show_window = false;
		ImGuiWindowFlags window_flags;

		/*methods*/
	public:
		int create_add_new_brand_window();
	};


	class AddNewQuantityWindow : public AddQuantityWindow
	{
		/*Constructor and Detructor*/
	public:
		AddNewQuantityWindow(mongocxx::database db);
		AddNewQuantityWindow() = default;
		~AddNewQuantityWindow() = default;

		/*properties*/
	public:
		std::string quantity_name;
		static char c_n_quantity_name[128];
		
		double n_selling_price = 0.0;
		double n_buying_price = 0.0;
		int n_number_of_items = 0;

		AddQuantityWindow quantity_win;
		
		bool show_window = false;
		ImGuiWindowFlags window_flags;

		/*methods*/
	public:
		int create_add_new_quantity_window();
	};
	

	class AddItemWindow
	{
		/*Constructor and Detructor*/
		public:
			AddItemWindow(AddProductWindow &add_product, AddNewBrandWindow&add_brand, AddNewQuantityWindow &add_quantity);
			~AddItemWindow() = default;

		/*properties*/
		public:
			AddProductWindow* add_product;
			AddNewBrandWindow* add_brand;
			AddNewQuantityWindow* add_quantity;

			bool show_window = false;
			ImGuiWindowFlags window_flags;

		/*methods*/
		public:
			int create_add_items_window();

	};

	class RemoveProductWindow : public AddQuantityWindow
	{
		/*Constructor and Detructor*/
	public:
		RemoveProductWindow(mongocxx::database db);
		RemoveProductWindow() = default;
		~RemoveProductWindow() = default;

		/*properties*/
	public:
		
		bool show_window = false;
		ImGuiWindowFlags window_flags;

		/*methods*/
	public:
		int create_remove_product_window();
	};

	class RemoveBrandWindow : public AddQuantityWindow
	{
		/*Constructor and Detructor*/
	public:
		RemoveBrandWindow(mongocxx::database db);
		RemoveBrandWindow() = default;
		~RemoveBrandWindow() = default;

		/*properties*/
	public:
		std::string brand_name;
		static int r_brand_current_item;
		
		bool show_window = false;
		ImGuiWindowFlags window_flags;

		/*methods*/
	public:
		int create_remove_brand_window();
	};

	class RemoveQuantityWindow : public AddQuantityWindow
	{
		/*Constructor and Detructor*/
	public:
		RemoveQuantityWindow(mongocxx::database db);
		RemoveQuantityWindow() = default;
		~RemoveQuantityWindow() = default;

		/*properties*/
	public:
		static int r_quantity_brand_current_item;
		static int r_quantity_current_item;

		bool show_window = false;
		ImGuiWindowFlags window_flags;

		/*methods*/
	public:
		int create_remove_quantity_window();
	};

	
	class RemoveItemWindow
	{
		/*Constructor and Detructor*/
	public:
		RemoveItemWindow(RemoveProductWindow&  remove_product, RemoveBrandWindow& remove_brand, RemoveQuantityWindow& remove_quantity);
		~RemoveItemWindow() = default;

		/*properties*/
	public:
		RemoveProductWindow* remove_product;
		RemoveBrandWindow* remove_brand;
		RemoveQuantityWindow* remove_quantity;
		

		bool show_window = false;
		ImGuiWindowFlags window_flags;

		/*methods*/
	public:
		int create_remove_items_window();

	};

	class AddorRemoveItemWindow
	{
		/*Constructor and Detructor*/
	public:
		AddorRemoveItemWindow(AddItemWindow& add_item, RemoveItemWindow& remove_item);
		AddorRemoveItemWindow() = default;
		~AddorRemoveItemWindow() = default;

		/*properties*/
	public:
		AddItemWindow* add_item;
		RemoveItemWindow* remove_item;

		bool show_window = false;
		ImGuiWindowFlags window_flags;

		/*methods*/
	public:
		int create_add_or_remove_items_window();

	};

	class AddOthersWindow : public AddQuantityWindow
	{
			/*Constructor and Detructor*/
		public:
			AddOthersWindow(mongocxx::database db);
			AddOthersWindow() = default;
			~AddOthersWindow() = default;

			/*properties*/
		public:
			
			std::unique_ptr<char[]> p_others;
			std::string others = "Select_Season_Age Group_Location_";
			std::vector<std::string> v_others = { "Season" , "Age Group",  "Location"};

			static int others_current_item;
			static char other_name[128];
			
			std::string s_other_name;
			

			bool show_window = false;
			ImGuiWindowFlags window_flags;

			/*methods*/
		public:
			int create_add_others_window();
	};


	class RemoveOthersWindow : public AddQuantityWindow
	{	
			/*Constructor and Detructor*/
		public:
			RemoveOthersWindow(mongocxx::database db);
			RemoveOthersWindow() = default;
			~RemoveOthersWindow() = default;

			/*properties*/
		public:

			std::unique_ptr<char[]> p_others;
			std::string others = "Select_Season_Age Group_Location_";

			std::unique_ptr<char[]> p_selected_others;
			std::string selected_others = "Select_";

			std::string selected_other;
			std::vector<std::string> v_selected_others;

			static int r_others_current_item;
			static int r_other_selected_current_item;

			std::string s_other_name;


			bool show_window = false;
			ImGuiWindowFlags window_flags;

			/*methods*/
		public:
			int create_remove_others_window();
			int mset_others(std::vector<std::string> v_s_others);
			
		
	};

	class AddorRemoveOthersWindow: public Window
	{
			/*Constructor and Detructor*/
		public:
			AddorRemoveOthersWindow(AddOthersWindow& add_other, RemoveOthersWindow& remove_other);
			AddorRemoveOthersWindow() = default;
			~AddorRemoveOthersWindow() = default;

			/*properties*/
		public:
			AddOthersWindow* add_other;
			RemoveOthersWindow* remove_other;

			bool show_window = false;
			ImGuiWindowFlags window_flags;

			/*methods*/
		public:
			int create_add_or_remove_others_window();

	};

    // Main Regular User Window to orchestrate sub-windows
    class ShowRegularUserWindow
    {
    public:
        ShowRegularUserWindow(StockWindow& stock_win, SellWindow& sell_win, ReturnSaleWindow& return_sale_win,
            AgentsWindow& agents_win, PassWord& change_password, AddorRemoveItemWindow& add_or_remove_item, AddorRemoveOthersWindow& add_or_remove_others);
        ~ShowRegularUserWindow() = default;

        /* Properties */
    public:
        StockWindow& stock_win;
        SellWindow& sell_win;
        ReturnSaleWindow& return_sale_win;
        AgentsWindow& agents_win;
        PassWord& change_password;
        AddorRemoveItemWindow* add_or_remove_item;
	    AddorRemoveOthersWindow* add_or_remove_others;
    
        bool show_window = false;
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;

        /* Methods */
    public:
        int create_show_regular_user_window();
    };
}
