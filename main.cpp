#include <mongocxx/v_noabi/mongocxx/client.hpp>
#include <mongocxx/v_noabi/mongocxx/instance.hpp>
#include <mongocxx/v_noabi/mongocxx/database.hpp>
#include <mongocxx/v_noabi/mongocxx/exception/exception.hpp>
#include <bsoncxx/v_noabi/bsoncxx/builder/stream/document.hpp>
#include <thread>
#include <iostream>

#include "ui/ui_dependences/glew/include/glew.h"
#include "ui/ui_dependences/glfw/include/glfw/glfw3.h"

#include "ui/ui_api/vendor/imgui/imgui.h"
#include "ui/ui_api/vendor/imgui/imgui_impl_glfw.h"
#include "ui/ui_api/vendor/imgui/imgui_impl_opengl3.h"

#include "ui/login_ui/login_ui.h"
#include "ui/admin_ui/admin_ui.h"
#include "ui/finance_ui/finance_ui.h"
#include "ui/marketer_ui/marketer_ui.h"
#include "ui/regular_ui/regular_user_ui.h"
#include "ui/change_password_ui/change_password.h"
#include "server/server.h"

// Global UI flags (defined in respective UI files)
extern bool login_window;
extern bool admin_window;
extern bool finance_window;
extern bool marketer_window;
extern bool regular_user_window;
extern bool showing_admin_extensions;

// Error callback for GLFW
static void glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

int main(int, char**) {
    // Initialize GLFW
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create GLFW window
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Inventory Management System", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImFontConfig font_config;
    font_config.RasterizerMultiply = 1.5f; // Improve rendering quality
    ImFont* default_font = io.Fonts->AddFontFromFileTTF("J:\\source\\PointOfSaleSystem\\PointOfSaleSystem\\ui\\ui_api\\vendor\\imgui\\misc\\fonts\\Roboto-Medium.ttf", 16.0f, &font_config);
    ImFont* large_font = io.Fonts->AddFontFromFileTTF("J:\\source\\PointOfSaleSystem\\PointOfSaleSystem\\ui\\ui_api\\vendor\\imgui\\misc\\fonts\\Roboto-Medium.ttf", 20.0f, &font_config);
    if (!default_font || !large_font) {
        std::cerr << "Failed to load font!" << std::endl;
        io.Fonts->AddFontDefault(); // Fallback to default font
    }
    io.Fonts->TexDesiredWidth = 4096; // Larger texture size
    io.Fonts->Build(); // Rebuild the font atlas
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard navigation
    ImGui::StyleColorsDark(); // Set dark theme

    // Setup ImGui platform/renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Initialize MongoDB and check connection
    mongocxx::instance instance{}; // Must be created once
    mongocxx::client client;
    mongocxx::database db;
    bool db_connected = false;

    try {
        // Attempt to initialize the client
        client = mongocxx::client{ mongocxx::uri{"mongodb://localhost:27017"} };

        // Access the database
        db = client["inventory_db"];

        // Test the connection with a ping command
        auto ping_cmd = bsoncxx::builder::stream::document{} << "ping" << 1 << bsoncxx::builder::stream::finalize;
        auto result = db.run_command(ping_cmd.view());

        // Check if the ping command was successful
        if (result.view()["ok"].get_double() == 1.0) {
            std::cout << "MongoDB connection successful: Connected to inventory_db" << std::endl;
            db_connected = true;
        }
        else {
            std::cerr << "MongoDB ping command failed" << std::endl;
        }
    }
    catch (const mongocxx::exception& e) {
        std::cerr << "MongoDB connection failed: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Standard exception during MongoDB connection: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown error during MongoDB connection" << std::endl;
    }

    // Decide how to proceed if connection failed
    if (!db_connected) {
        std::cerr << "Application will run in limited mode without database access" << std::endl;
        // Optionally, terminate the application:
        // glfwDestroyWindow(window);
        // glfwTerminate();
        // return -1;
    }

    // Initialize TCP server in a separate thread
    std::thread server_thread(server); // Call server() from server.cpp
    server_thread.detach(); // Run server independently

    // Initialize UI components
    LoginUi::LoginWindow login_ui(db, default_font, large_font);
    AdminUi::AddProductWindow add_product_ui(db);
    AdminUi::AddNewBrandWindow add_new_brand_ui(db);
    AdminUi::AddNewQuantityWindow add_new_quantity_ui(db);
    AdminUi::AddItemWindow add_item_ui(add_product_ui, add_new_brand_ui, add_new_quantity_ui);
    AdminUi::RemoveProductWindow remove_product_ui(db);
    AdminUi::RemoveBrandWindow remove_brand_ui(db);
    AdminUi::RemoveQuantityWindow remove_quantity_ui(db);
    AdminUi::RemoveItemWindow remove_item_ui(remove_product_ui, remove_brand_ui, remove_quantity_ui);
    AdminUi::AddorRemoveItemWindow add_or_remove_item_ui(add_item_ui, remove_item_ui);
    AdminUi::AddOthersWindow add_others_ui(db);
    AdminUi::RemoveOthersWindow remove_others_ui(db);
    AdminUi::AddorRemoveOthersWindow add_or_remove_others_ui(add_others_ui, remove_others_ui);
    AdminUi::AddAgentWindow add_agent_ui(db);
    AdminUi::RemoveAgentWindow remove_agent_ui(db);
    AdminUi::AddorRemoveAgentsWindow add_or_remove_agents_ui(add_agent_ui, remove_agent_ui);
    PassWord change_password_ui(db);
    AdminUi::ShowAdminWindow admin_ui(add_or_remove_item_ui, add_or_remove_others_ui, add_or_remove_agents_ui, change_password_ui);

    FUserUi::Profit_Loss_Window profit_loss_ui(db);
    FUserUi::AddExpenseWindow add_expense_ui(db);
    FUserUi::RemoveExpenseWindow remove_expense_ui(db);
    FUserUi::SetExpenditureWindow set_expenditure_ui(db);
    FUserUi::Expenses expenses_ui(add_expense_ui, remove_expense_ui, set_expenditure_ui);
    RUserUi::StockWindow stock_ui(db);
    FUserUi::ConfirmSaleWindow confirm_sale_ui(db, stock_ui);
    FUserUi::ShowFinanceWindow finance_ui(profit_loss_ui, expenses_ui, confirm_sale_ui, change_password_ui);

    MUserUi::GeneralReportWindow general_report_ui(db, window);
    MUserUi::CustomizedReportWindow customized_report_ui(db, window);
    MUserUi::ShowMarketerWindow marketer_ui(general_report_ui, customized_report_ui, change_password_ui);

    RUserUi::SellWindow sell_ui(db);
    RUserUi::ReturnSaleWindow return_sale_ui(db);
    RUserUi::AgentsWindow agents_ui(db);
    RUserUi::ShowRegularUserWindow regular_user_ui(stock_ui, sell_ui, return_sale_ui, agents_ui, change_password_ui);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Poll and handle events
        glfwPollEvents();

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Show connection error in UI if not connected
        if (!db_connected) {
            ImGui::Begin("Database Error");
            ImGui::Text("Failed to connect to MongoDB. Please check the server and try again.");
            if (ImGui::Button("Exit")) {
                glfwSetWindowShouldClose(window, true);
            }
            ImGui::End();
        }
        else {
            // Render active UI based on global flags
            if (login_window) {
                login_ui.create_login_window();
            }
            if (admin_window) {
                admin_ui.create_show_admin_window();
            }
            if (finance_window) {
                finance_ui.create_show_finance_window();
            }
            if (marketer_window) {
                marketer_ui.create_show_marketer_window();
            }
            if (regular_user_window) {
                regular_user_ui.create_show_regular_user_window();
            }
        }

        // Render ImGui
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}