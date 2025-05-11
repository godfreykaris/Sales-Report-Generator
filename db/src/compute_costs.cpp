#include "compute_costs.h"
#include "retrieve_data.h"

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;

bsoncxx::stdx::optional<bsoncxx::document::value> get_components_arrays(mongocxx::collection products)
{
    bsoncxx::stdx::optional<bsoncxx::document::value> components_arrays = products.find_one(
        document{} << "name" << "Components" << finalize
    );
    return components_arrays;
}

double compute_total_quantity_cost(mongocxx::collection products, SALE sale, std::string price_type)
{
    double total_quantity_cost = 0.0;
    int quantity_count = 0;
    double quantity_unit_cost = 0.0;

    if (price_type == "BuyingPrice") {
        quantity_count = get_quantity_total(products, sale);
        quantity_unit_cost = get_quantity_unit_cost(products, sale, price_type);
    } else {
        quantity_count = sale.item_count;
        quantity_unit_cost = sale.sale_price;
    }

    total_quantity_cost = quantity_count * quantity_unit_cost;
    return total_quantity_cost;
}

double compute_total_cost(mongocxx::collection products, bsoncxx::stdx::optional<bsoncxx::document::value> components_arrays, SALE sale, std::string price_type)
{
    double total_sales_costs = 0.0;

    if (price_type == "BuyingPrice") {
        // Original logic: Iterate over Components document
        if (!components_arrays) {
            throw std::runtime_error("Components document not found");
        }

        bsoncxx::document::view components_arrays_view = components_arrays.value().view();
        bsoncxx::array::view products_array = components_arrays_view["Products"].get_array();

        for (bsoncxx::array::element my_product : products_array) {
            // Filter by product_name if provided in sale
            std::string product_name = std::string(my_product.get_string().value);
            if (!sale.product_name.empty() && sale.product_name != product_name) {
                continue;
            }

            bsoncxx::array::view brands = components_arrays_view["Brands"][product_name].get_array();

            for (bsoncxx::array::element my_brand : brands) {
                std::string brand_name = std::string(my_brand.get_string().value);
                // Filter by brand if provided in sale
                if (!sale.brand.empty() && sale.brand != brand_name) {
                    continue;
                }

                bsoncxx::array::view quantities = components_arrays_view["Quantities"][brand_name].get_array();

                for (bsoncxx::array::element my_quantity : quantities) {
                    std::string quantity_name = std::string(my_quantity.get_string().value);
                    // Filter by quantity if provided in sale
                    if (!sale.quantity.empty() && sale.quantity != quantity_name) {
                        continue;
                    }

                    SALE temp_sale;
                    temp_sale.product_name = product_name;
                    temp_sale.brand = brand_name;
                    temp_sale.quantity = quantity_name;

                    try {
                        total_sales_costs += compute_total_quantity_cost(products, temp_sale, price_type);
                    } catch (const std::exception& e) {
                        std::cerr << "Error processing product " << temp_sale.product_name << ", brand " << temp_sale.brand 
                                  << ", quantity " << temp_sale.quantity << ": " << e.what() << std::endl;
                        continue;
                    }
                }
            }
        }
    } else {
        // Updated logic: Iterate over SalesData document
        bsoncxx::stdx::optional<bsoncxx::document::value> sales_data = products.find_one(
            document{} << "name" << "SalesData" << finalize
        );

        if (!sales_data) {
            throw std::runtime_error("SalesData document not found");
        }

        bsoncxx::document::view sales_data_view = sales_data.value().view();
        bsoncxx::array::view indices = sales_data_view["indices"].get_array();

        for (bsoncxx::array::element index : indices) {
            std::string index_str = std::string(index.get_string().value);
            bsoncxx::document::view sale_doc = sales_data_view[index_str].get_document().value;

            SALE temp_sale;
            // Extract product name and remove timestamp
            std::string product_full = std::string(sale_doc["Product"].get_string().value);
            size_t underscore_pos = product_full.find('_');
            if (underscore_pos == std::string::npos) {
                std::cerr << "Skipping sale index " << index_str << ": Malformed product name" << std::endl;
                continue;
            }
            temp_sale.product_name = product_full.substr(0, underscore_pos);

            // Filter by product_name if provided in sale
            if (!sale.product_name.empty() && sale.product_name != temp_sale.product_name) {
                continue;
            }

            // Extract brand and remove product prefix
            std::string brand_full = std::string(sale_doc["Brand"].get_string().value);
            underscore_pos = brand_full.find('_');
            if (underscore_pos == std::string::npos) {
                std::cerr << "Skipping sale index " << index_str << ": Malformed brand name" << std::endl;
                continue;
            }
            temp_sale.brand = brand_full.substr(underscore_pos + 1);

            // Filter by brand if provided in sale
            if (!sale.brand.empty() && sale.brand != temp_sale.brand) {
                continue;
            }

            temp_sale.quantity = std::string(sale_doc["Quantity"].get_string().value);
            // Filter by quantity if provided in sale
            if (!sale.quantity.empty() && sale.quantity != temp_sale.quantity) {
                continue;
            }

            temp_sale.item_count = sale_doc["Item Count"].get_int32().value;
            temp_sale.sale_price = sale_doc["Sale Price"].get_double().value;

            try {
                total_sales_costs += compute_total_quantity_cost(products, temp_sale, price_type);
            } catch (const std::exception& e) {
                std::cerr << "Error processing sale index " << index_str << ": " << e.what() << std::endl;
                continue;
            }
        }
    }

    return total_sales_costs;
}

// Helper function to fetch buying price from Stock collection
double get_buying_price(mongocxx::collection stock, const SALE& sale) {
    try {
        // Construct the query for the Stock collection
        // Stock document structure: { "name": product_name, "Brands": { brand: { quantity: { "BuyingPrice": value } } } }
        auto doc = stock.find_one(
            document{} 
                << "name" << sale.product_name
                << "Brands." + sale.product_name + "_" + sale.brand + "." + sale.quantity + ".BuyingPrice" 
                << bsoncxx::builder::stream::open_document << "$exists" << true << bsoncxx::builder::stream::close_document
                << finalize
        );

        if (doc) {
            bsoncxx::document::view view = doc.value().view();
            return view["Brands"][sale.product_name + "_" + sale.brand][sale.quantity]["BuyingPrice"].get_double().value;
        } else {
            std::cerr << "No buying price found for product: " << sale.product_name 
                      << ", brand: " << sale.brand << ", quantity: " << sale.quantity << std::endl;
            return 0.0;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error fetching buying price for product: " << sale.product_name 
                  << ", brand: " << sale.brand << ", quantity: " << sale.quantity 
                  << ": " << e.what() << std::endl;
        return 0.0;
    }
}

// New function to compute total buying prices of sold items
double compute_total_buying_cost(mongocxx::collection products, mongocxx::collection stock, SALE sale) {
    double total_buying_cost = 0.0;

    try {
        // Find SalesData document in products collection
        bsoncxx::stdx::optional<bsoncxx::document::value> sales_data = products.find_one(
            document{} << "name" << "SalesData" << finalize
        );

        if (!sales_data) {
            throw std::runtime_error("SalesData document not found");
        }

        bsoncxx::document::view sales_data_view = sales_data.value().view();
        bsoncxx::array::view indices = sales_data_view["indices"].get_array();

        for (bsoncxx::array::element index : indices) {
            std::string index_str = std::string(index.get_string().value);
            bsoncxx::document::view sale_doc = sales_data_view[index_str].get_document().value;

            SALE temp_sale;
            // Extract product name (remove timestamp)
            std::string product_full = std::string(sale_doc["Product"].get_string().value);
            size_t underscore_pos = product_full.find('_');
            if (underscore_pos == std::string::npos) {
                std::cerr << "Skipping sale index " << index_str << ": Malformed product name" << std::endl;
                continue;
            }
            temp_sale.product_name = product_full.substr(0, underscore_pos);

            // Filter by product_name if provided
            if (!sale.product_name.empty() && sale.product_name != temp_sale.product_name) {
                continue;
            }

            // Extract brand (remove product prefix)
            std::string brand_full = std::string(sale_doc["Brand"].get_string().value);
            underscore_pos = brand_full.find('_');
            if (underscore_pos == std::string::npos) {
                std::cerr << "Skipping sale index " << index_str << ": Malformed brand name" << std::endl;
                continue;
            }
            temp_sale.brand = brand_full.substr(underscore_pos + 1);

            // Filter by brand if provided
            if (!sale.brand.empty() && sale.brand != temp_sale.brand) {
                continue;
            }

            temp_sale.quantity = std::string(sale_doc["Quantity"].get_string().value);
            // Filter by quantity if provided
            if (!sale.quantity.empty() && sale.quantity != temp_sale.quantity) {
                continue;
            }

            temp_sale.item_count = sale_doc["Item Count"].get_int32().value;

            // Fetch buying price from Stock collection
            double buying_price = get_buying_price(stock, temp_sale);
            if (buying_price == 0.0) {
                // Log warning but continue to avoid skewing totals
                continue;
            }

            // Compute cost for this sale
            double sale_cost = temp_sale.item_count * buying_price;
            total_buying_cost += sale_cost;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error computing total buying cost: " << e.what() << std::endl;
        return 0.0;
    }

    return total_buying_cost;
}