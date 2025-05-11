#pragma once


/*
* Handles the computation of the costs from the sales
*/

#include "app_basic.h"

/*Get the document containing the components arrays*/
bsoncxx::stdx::optional<bsoncxx::document::value> get_components_arrays(mongocxx::collection products);

/*Get the total cost of a quantity of a particular brand and document*/
/*
*price_type: "BuyingPrice" or "SellingPrice"
*/
double compute_total_quantity_cost(mongocxx::collection products, SALE sale, std::string price_type);

/*Get the total sales cost*/
/*
*price_type: "BuyingPrice" or "SellingPrice"
*/
double compute_total_cost(mongocxx::collection products, bsoncxx::stdx::optional<bsoncxx::document::value> components_arrays, SALE sale, std::string price_type);

// Helper function to fetch buying price from Stock collection
double get_buying_price(mongocxx::collection stock, const SALE& sale);
// New function to compute total buying prices of sold items
double compute_total_buying_cost(mongocxx::collection products, mongocxx::collection stock, SALE sale);