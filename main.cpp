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
#include "ui/ui_api/vendor/imgui/misc/freetype/imgui_freetype.h"

#include "ui/login_ui/login_ui.h"
#include "ui/admin_ui/admin_ui.h"
#include "ui/finance_ui/finance_ui.h"
#include "ui/marketer_ui/marketer_ui.h"
#include "ui/regular_ui/regular_user_ui.h"
#include "ui/change_password_ui/change_password.h"
#include "server/server.h"
#include "ui/background/background_renderer.h"

#include "ui/ui_dependences/stb_image/stb_image.h"

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

// Initialize GLFW and create the window
GLFWwindow* initialize_glfw() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }

    // Get the primary monitor's resolution
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    if (!primaryMonitor) {
        std::cerr << "Failed to get primary monitor" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    if (!mode) {
        std::cerr << "Failed to get video mode" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    // Set GLFW window hints
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create GLFW window with monitor's width and height
    GLFWwindow* window = glfwCreateWindow(mode->width/1.5, mode->height/1.2, "Inventory Management System", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwSetWindowPos(window, 100, 50);

    // Load icon image
    int icon_width, icon_height, icon_channels;
    unsigned char* icon_data = stbi_load("assets/icon.png", &icon_width, &icon_height, &icon_channels, 4);
    if (icon_data) {
        GLFWimage icon;
        icon.width = icon_width;
        icon.height = icon_height;
        icon.pixels = icon_data;
        glfwSetWindowIcon(window, 1, &icon);
        stbi_image_free(icon_data);
    } else {
        std::cerr << "Failed to load icon: assets/app_icon.png" << std::endl;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    return window;
}

// Initialize GLEW
bool initialize_glew() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return false;
    }
    return true;
}

// Initialize ImGui and load fonts
bool initialize_imgui(GLFWwindow* window, ImFont*& default_font, ImFont*& large_font, ImFont*& emoji_font) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // Font configuration for Roboto
    ImFontConfig font_config;
    font_config.RasterizerMultiply = 1.5f;

    // Load Roboto at 16.0f (default font)
    default_font = io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Medium.ttf", 16.0f, &font_config);
    if (!default_font) {
        std::cerr << "Failed to load Roboto-Medium.ttf (16.0f)!" << std::endl;
        default_font = io.Fonts->AddFontDefault();
    }

    // Merge Segoe UI Emoji into default_font
    static ImWchar ranges[] = { 0x1, 0x1FFFF, 0 };
    ImFontConfig emoji_config;
    emoji_config.MergeMode = true;
    emoji_config.OversampleH = emoji_config.OversampleV = 1;
    emoji_config.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_LoadColor;
    emoji_font = io.Fonts->AddFontFromFileTTF("assets/fonts/seguiemj.ttf", 16.0f, &emoji_config, ranges);
    if (!emoji_font) {
        std::cerr << "Failed to load seguiemj.ttf!" << std::endl;
    }

    // Load Roboto at 20.0f (large font)
    large_font = io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Medium.ttf", 20.0f, &font_config);
    if (!large_font) {
        std::cerr << "Failed to load Roboto-Medium.ttf (20.0f)!" << std::endl;
    }

    // Build font atlas
    if (!io.Fonts->Build()) {
        std::cerr << "Failed to build font atlas!" << std::endl;
        return false;
    }

    io.FontDefault = default_font;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    // Setup ImGui platform/renderer backends
    const char* glsl_version = "#version 130";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    return true;
}

// Initialize MongoDB connection
bool initialize_mongodb(mongocxx::client& client, mongocxx::database& db) {
    try {
        client = mongocxx::client{ mongocxx::uri{"mongodb://localhost:27017"} };
        db = client["inventory_db"];
        auto ping_cmd = bsoncxx::builder::stream::document{} << "ping" << 1 << bsoncxx::builder::stream::finalize;
        auto result = db.run_command(ping_cmd.view());
        if (result.view()["ok"].get_double() == 1.0) {
            std::cout << "MongoDB connection successful: Connected to inventory_db" << std::endl;
            return true;
        } else {
            std::cerr << "MongoDB ping command failed" << std::endl;
        }
    } catch (const mongocxx::exception& e) {
        std::cerr << "MongoDB connection failed: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Standard exception during MongoDB connection: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown error during MongoDB connection" << std::endl;
    }
    return false;
}

// Initialize TCP server
void initialize_server() {
    std::thread server_thread(server);
    server_thread.detach();
}

// Render UI based on global flags
void render_ui(bool db_connected, GLFWwindow* window, BackgroundRenderer& backgroundRenderer,
               LoginUi::LoginWindow& login_ui, AdminUi::ShowAdminWindow& admin_ui,
               FUserUi::ShowFinanceWindow& finance_ui, MUserUi::ShowMarketerWindow& marketer_ui,
               RUserUi::ShowRegularUserWindow& regular_user_ui) {
    // Poll and handle events
    glfwPollEvents();

    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Clear the screen
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    backgroundRenderer.resizeFramebuffer(display_w, display_h);

    glViewport(0, 0, display_w, display_h);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Disable depth testing for 2D rendering
    glDisable(GL_DEPTH_TEST);

    // Render the gradient background
    backgroundRenderer.render();

    // Re-enable depth testing if needed for ImGui
    glEnable(GL_DEPTH_TEST);

    // Show connection error in UI if not connected
    if (!db_connected) {
        ImGui::Begin("Database Error");
        ImGui::Text("Failed to connect to MongoDB. Please check the server and try again.");
        if (ImGui::Button("Exit")) {
            glfwSetWindowShouldClose(window, true);
        }
        ImGui::End();
    } else {
        login_ui.render_login_extras(display_w, display_h);
            if (login_ui.showBackgroundSettings) {
                login_ui.create_background_settings_window();
            }
            
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
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap buffers
    glfwSwapBuffers(window);
}

// Cleanup resources
void cleanup(GLFWwindow* window) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

int main(int, char**) {
    // Initialize MongoDB instance
    mongocxx::instance instance{};

    // Initialize GLFW and create window
    GLFWwindow* window = initialize_glfw();
    if (!window) {
        return -1;
    }

    // Initialize GLEW
    if (!initialize_glew()) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Initialize ImGui and fonts
    ImFont* default_font = nullptr;
    ImFont* large_font = nullptr;
    ImFont* emoji_font = nullptr;
    if (!initialize_imgui(window, default_font, large_font, emoji_font)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Initialize MongoDB connection
    mongocxx::client client;
    mongocxx::database db;
    bool db_connected = initialize_mongodb(client, db);
    if (!db_connected) {
        std::cerr << "Application will run in limited mode without database access" << std::endl;
    }

    // Initialize TCP server
    initialize_server();

    // Initialize BackgroundRenderer
    std::string default_image = "assets/background.jpg";
    BackgroundRenderer backgroundRenderer(default_image);

    // Initialize UI components (restored as original)
    LoginUi::LoginWindow login_ui(db, default_font, large_font, emoji_font, backgroundRenderer);
    RUserUi::AddProductWindow add_product_ui(db);
    RUserUi::AddNewBrandWindow add_new_brand_ui(db);
    RUserUi::AddNewQuantityWindow add_new_quantity_ui(db);
    RUserUi::AddItemWindow add_item_ui(add_product_ui, add_new_brand_ui, add_new_quantity_ui);
    RUserUi::RemoveProductWindow remove_product_ui(db);
    RUserUi::RemoveBrandWindow remove_brand_ui(db);
    RUserUi::RemoveQuantityWindow remove_quantity_ui(db);
    RUserUi::RemoveItemWindow remove_item_ui(remove_product_ui, remove_brand_ui, remove_quantity_ui);
    RUserUi::AddorRemoveItemWindow add_or_remove_item_ui(add_item_ui, remove_item_ui);
    RUserUi::AddOthersWindow add_others_ui(db);
    RUserUi::RemoveOthersWindow remove_others_ui(db);
    RUserUi::AddorRemoveOthersWindow add_or_remove_others_ui(add_others_ui, remove_others_ui);
    AdminUi::AddAgentWindow add_agent_ui(db);
    AdminUi::RemoveAgentWindow remove_agent_ui(db);
    AdminUi::AddorRemoveAgentsWindow add_or_remove_agents_ui(add_agent_ui, remove_agent_ui);
    PassWord change_password_ui(db);
    AdminUi::DBManagementWindow db_management(db);
    AdminUi::ShowAdminWindow admin_ui(add_or_remove_agents_ui, change_password_ui, db_management);

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
    RUserUi::ShowRegularUserWindow regular_user_ui(stock_ui, sell_ui, return_sale_ui, agents_ui, change_password_ui, add_or_remove_item_ui, add_or_remove_others_ui);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        render_ui(db_connected, window, backgroundRenderer, login_ui, admin_ui, finance_ui, marketer_ui, regular_user_ui);
    }

    // Cleanup
    cleanup(window);

    return 0;
}