#include "login_ui.h"
#include "regular_ui/regular_user_ui.h"
#include "finance_ui/finance_ui.h"
#include "marketer_ui/marketer_ui.h"
#include "admin_ui/admin_ui.h"

bool login_window = true;

char LoginUi::LoginWindow::c_password[128] = "";

LoginUi::LoginWindow::LoginWindow(mongocxx::database db, ImFont* default_font, ImFont* large_font, ImFont* emoji_font, BackgroundRenderer& bgRenderer)
    : login(db["LogInData"]), default_font(default_font), large_font(large_font), emoji_font(emoji_font), backgroundRenderer(bgRenderer), showBackgroundSettings(false) {
    this->background_images_coll = db["background_images"];
    initialize_background_images();
    // Initialize blur and solid color settings
    blurRadius = backgroundRenderer.getBlurRadius();
    useSolidColor = backgroundRenderer.isSolidColorMode();
    const float* color = backgroundRenderer.getSolidColor();
    solidColor[0] = color[0];
    solidColor[1] = color[1];
    solidColor[2] = color[2];
}

void LoginUi::LoginWindow::initialize_background_images() {
    try {
        auto count = this->background_images_coll.count_documents({});
        if (count == 0) {
            this->background_images_coll.insert_one(bsoncxx::builder::stream::document{}
                << "path" << "assets/background.jpg"
                << bsoncxx::builder::stream::finalize);
        }
    }
    catch (const mongocxx::v_noabi::exception& e) {
        std::cerr << "Failed to initialize background images: " << e.what() << std::endl;
    }

    try {
        auto cursor = background_images_coll.find({});
        for (auto&& doc : cursor) {
            // Safely extract the "path" field
            if (auto path_elem = doc["path"]; path_elem && path_elem.type() == bsoncxx::type::k_string) {
                auto path = path_elem.get_string().value;
                backgroundRenderer.addImage(std::string(path));
            }
            else {
                std::cerr << "Skipping document: 'path' field missing or not a UTF-8 string" << std::endl;
            }
        }
    }
    catch (const mongocxx::v_noabi::exception& e) {
        std::cerr << "Failed to load background images: " << e.what() << std::endl;
    }
}

int LoginUi::LoginWindow::create_login_window() {
    float button_width = 80.0f;
    this->passwords = get_users_data(this->login);
    this->s_password = "";

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
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f); // Rounded input/button corners
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f)); // Window padding
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3.0f, 3.0f)); // Reduced button/input padding

    // Center and size the login window
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f)); // Center
    ImGui::SetNextWindowSize(ImVec2(450.0f, 200.0f), ImGuiCond_Always); // Adjusted height for settings button

    // Begin login window
    ImGui::Begin("Login", NULL, this->window_flags | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    // Title with larger font and key emoji
    ImGui::PushFont(this->large_font);
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("🔑 Sales System Login").x) * 0.5f); // Center title with emoji
    if (this->emoji_font) ImGui::PushFont(this->emoji_font);
    ImGui::Text("🔑"); // Key emoji before title
    if (this->emoji_font) ImGui::PopFont();
    ImGui::SameLine(0.0f, 5.0f); // Small spacing after emoji
    ImGui::Text("Sales System Login");
    ImGui::PopFont();
    ImGui::Separator(); // Visual separator

    // Password label and input (centered) with lock emoji
    ImGui::Dummy(ImVec2(0.0f, 10.0f)); // Reduced vertical spacing
    float input_width = 200.0f;
    float label_width = ImGui::CalcTextSize("🔒 Password: ").x; // Adjusted for emoji
    float total_width = label_width + input_width + button_width + ImGui::GetStyle().ItemSpacing.x * 2;
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - total_width) * 0.5f); // Center input group
    if (this->emoji_font) ImGui::PushFont(this->emoji_font);
    ImGui::Text("🔒"); // Lock emoji before password label
    if (this->emoji_font) ImGui::PopFont();
    ImGui::SameLine(0.0f, 5.0f); // Small spacing after emoji
    ImGui::Text("Password: ");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(input_width);
    static bool show_password = false;
    if (ImGui::InputTextWithHint("##password", "Enter password", this->c_password, IM_ARRAYSIZE(this->c_password), show_password ? 0 : ImGuiInputTextFlags_Password)) {
        this->login_error = false; // Clear error when user types
    }
    ImGui::SameLine();
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3.0f, 3.0f)); // Match existing padding
    if (ImGui::Button("##ShowHidePassword", ImVec2(button_width, 0.0f))) { // Button without label
        show_password = !show_password; // Toggle password visibility
    }
    // Get button position and size for text alignment
    ImVec2 button_pos = ImGui::GetItemRectMin();
    ImVec2 button_size = ImGui::GetItemRectSize();
    // Calculate text and emoji positions (centered horizontally, vertically aligned)
    ImGui::PushFont(this->emoji_font);
    const char* emoji = show_password ? "👁️" : "👁️";
    const char* text = show_password ? " Hide" : " Show";
    float emoji_width = ImGui::CalcTextSize(emoji).x;
    ImGui::PopFont();
    ImGui::PushFont(this->default_font);
    float text_width = ImGui::CalcTextSize(text).x;
    total_width = emoji_width + text_width;
    float text_height = ImGui::CalcTextSize(text).y;
    ImGui::PopFont();
    // Center the combined emoji+text horizontally
    float start_x = button_pos.x + (button_width - total_width) * 0.5f;
    ImVec2 emoji_pos = ImVec2(
        start_x + 10.0f, // Start of emoji
        button_pos.y + (button_size.y - text_height) * 0.5f // Center vertically
    );
    ImVec2 text_pos = ImVec2(
        start_x + 35.0f, // Text starts after emoji
        button_pos.y + (button_size.y - text_height) * 0.5f // Same vertical position
    );
    // Render emoji
    ImGui::SetCursorScreenPos(emoji_pos);
    ImGui::PushFont(this->emoji_font);
    ImGui::Text("%s", emoji);
    ImGui::PopFont();
    // Render text
    ImGui::SetCursorScreenPos(text_pos);
    ImGui::PushFont(this->default_font);
    ImGui::Text("%s", text);
    ImGui::PopFont();
    ImGui::PopStyleVar();

    // Log In button (centered)
    ImGui::Dummy(ImVec2(0.0f, 15.0f)); // Reduced vertical spacing
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 150.0f) * 0.5f); // Center button
    if (ImGui::Button("Log In", ImVec2(150.0f, 30.0f))) {
        this->s_password = this->c_password;
        memset(this->c_password, 0, 128);

        if (this->s_password.compare(this->passwords[0].password) == 0) {
            admin_window = true;
            login_window = false;
        }
        else if (this->s_password.compare(this->passwords[1].password) == 0) {
            finance_window = true;
            login_window = false;
        }
        else if (this->s_password.compare(this->passwords[2].password) == 0) {
            marketer_window = true;
            login_window = false;
        }
        else if (this->s_password.compare(this->passwords[3].password) == 0) {
            regular_user_window = true;
            login_window = false;
        }
        else {
            this->login_error = true;
        }
    }

    ImGui::End();

    // Render error popup
    if (this->login_error) {
        ImGui::OpenPopup("Login Error");
        this->login_error_message(this->login_error);
    }

    // Pop styles
    ImGui::PopStyleColor(8); // Pop all colors
    ImGui::PopStyleVar(4); // Pop all style vars

    return 1;
}

void LoginUi::LoginWindow::apply_window_styles() {
    // Color scheme: Dark slate background with teal accents, soft grays, and high-contrast text
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(24, 30, 38, 245)); // Dark slate blue background
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(40, 48, 60, 255)); // Lighter slate for inputs
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(60, 72, 90, 255)); // Subtle hover effect
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(80, 96, 120, 255)); // Active state
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 128, 160, 255)); // Teal buttons
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 160, 200, 255)); // Lighter teal on hover
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0, 100, 140, 255)); // Darker teal when active
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(230, 235, 240, 255)); // Off-white text for readability
    ImGui::PushStyleColor(ImGuiCol_PopupBg, IM_COL32(30, 38, 48, 245)); // Slightly lighter popup background
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 128, 160, 120)); // Teal border with transparency

    // Style variables for balanced spacing and modern aesthetics
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f); // Soft window corners
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f); // Subtle rounding for buttons/inputs
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16.0f, 16.0f)); // Comfortable window padding
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 6.0f)); // Balanced input/button padding
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 12.0f)); // Enhanced vertical spacing
}

void LoginUi::LoginWindow::cleanup_window_styles() {
    ImGui::PopStyleColor(10);
    ImGui::PopStyleVar(5);
}

void LoginUi::LoginWindow::setup_window() {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, 20.0f), ImGuiCond_Always, ImVec2(0.5f, 0.0f));
    float maxHeight = io.DisplaySize.y - 30.0f; // Ensure height doesn't exceed display height minus top offset
    ImGui::SetNextWindowSize(ImVec2(480.0f, (std::min)(650.0f, maxHeight)), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.95f);
    ImGui::Begin("Customize Background", &showBackgroundSettings, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
}

void LoginUi::LoginWindow::render_header() {
    ImGui::PushFont(large_font);
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Personalize Your Background").x) * 0.5f);
    ImGui::Text("Personalize Your Background");
    ImGui::PopFont();
    ImGui::Dummy(ImVec2(0.0f, 3.0f)); // Additional spacing before separator
    ImGui::Separator();
}

void LoginUi::LoginWindow::render_popup(const char* title, const char* emoji, const char* message, bool& popup_open) {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f, 12.0f)); // Added padding for popups
    if (ImGui::BeginPopupModal(title, &popup_open, ImGuiWindowFlags_AlwaysAutoResize)) {
        if (emoji_font) ImGui::PushFont(emoji_font);
        ImGui::Text("%s", emoji);
        if (emoji_font) ImGui::PopFont();
        ImGui::SameLine();
        ImGui::Text("%s", message);
        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // Spacing before button
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 100.0f) * 0.5f); // Wider button for balance
        if (ImGui::Button("OK", ImVec2(100.0f, 32.0f))) {
            ImGui::CloseCurrentPopup();
            active_popup.clear();
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
}

void LoginUi::LoginWindow::render_popups() {
    bool popup_open = true;
    if (!active_popup.empty()) {
        ImGui::OpenPopup(active_popup.c_str());
        if (active_popup == "Slideshow Success") {
            render_popup("Slideshow Success", "👍", "Slideshow settings applied successfully!", popup_open);
        }
        else if (active_popup == "Slideshow Disabled") {
            render_popup("Slideshow Disabled", "👍", "Slideshow has been disabled.", popup_open);
        }
        else if (active_popup == "Invalid Interval") {
            render_popup("Invalid Interval", "❌", "Interval must be between 1 and 60 seconds.", popup_open);
        }
        else if (active_popup == "Slideshow Error") {
            render_popup("Slideshow Error", "❌", "Failed to apply slideshow settings. Please try again.", popup_open);
        }
        else if (active_popup == "Image Add Success") {
            render_popup("Image Add Success", "👍", "Image added successfully!", popup_open);
        }
        else if (active_popup == "Image Add Error") {
            render_popup("Image Add Error", "❌", "Failed to add image. Please try again or use another image.", popup_open);
        }
        else if (active_popup == "Invalid Image") {
            render_popup("Invalid Image", "❌", "Invalid image format. Please select a .jpg or .png file.", popup_open);
        }
        else if (active_popup == "Database Error") {
            render_popup("Database Error", "❌", "Failed to save image to database. Try again or use another image.", popup_open);
        }
        else if (active_popup == "Unexpected Error") {
            render_popup("Unexpected Error", "❌", "An unexpected error occurred. Please try again.", popup_open);
        }
        if (!popup_open) {
            active_popup.clear();
        }
    }
}

void LoginUi::LoginWindow::render_add_image() {
    ImGui::Dummy(ImVec2(0.0f, 3.0f)); // Section spacing
    if (emoji_font) ImGui::PushFont(emoji_font);
    ImGui::Text("📷");
    if (emoji_font) ImGui::PopFont();
    ImGui::SameLine();
    ImGui::Text("Add New Image:");
    ImGui::InputText("##newImagePath", newImagePath, IM_ARRAYSIZE(newImagePath), ImGuiInputTextFlags_ReadOnly);
    ImGui::SameLine();
    if (ImGui::Button("Browse##AddImage", ImVec2(100.0f, 32.0f))) { // Larger button for consistency
        OPENFILENAME ofn = { 0 };
        wchar_t file_name[MAX_PATH] = L"";
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = GetActiveWindow();
        ofn.lpstrFilter = L"Image Files (*.jpg;*.png)\0*.jpg;*.png\0All Files (*.*)\0*.*\0";
        ofn.lpstrFile = file_name;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrDefExt = L"jpg";
        ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

        if (GetOpenFileName(&ofn)) {
            char file_name_mb[MAX_PATH] = { 0 };
            size_t converted_chars = 0;
            wcstombs_s(&converted_chars, file_name_mb, file_name, MAX_PATH);

            std::string path(file_name_mb);
            std::string pathLower = path;
            std::transform(pathLower.begin(), pathLower.end(), pathLower.begin(), ::tolower);
            try {
                if (std::filesystem::exists(path) &&
                    (pathLower.size() >= 4 &&
                        (pathLower.substr(pathLower.size() - 4) == ".jpg" || pathLower.substr(pathLower.size() - 4) == ".png"))) {
                    backgroundRenderer.addImage(path);
                    background_images_coll.insert_one(
                        bsoncxx::builder::stream::document{} << "path" << path << bsoncxx::builder::stream::finalize);
                    strncpy_s(newImagePath, path.c_str(), sizeof(newImagePath) - 1);
                    newImagePath[sizeof(newImagePath) - 1] = '\0';
                    active_popup = "Image Add Success";
                }
                else {
                    active_popup = "Invalid Image";
                }
            }
            catch (const mongocxx::exception& e) {
                std::cerr << "Failed to save image path: " << e.what() << std::endl;
                active_popup = "Database Error";
            }
            catch (const std::exception& e) {
                std::cerr << "Error adding image: " << e.what() << std::endl;
                active_popup = "Image Add Error";
            }
            catch (...) {
                std::cerr << "Unknown error occurred while adding image" << std::endl;
                active_popup = "Unexpected Error";
            }
        }
    }
}

void LoginUi::LoginWindow::render_select_image() {
    ImGui::Dummy(ImVec2(0.0f, 12.0f));
    if (emoji_font) ImGui::PushFont(emoji_font);
    ImGui::Text("🖼️");
    if (emoji_font) ImGui::PopFont();
    ImGui::SameLine();
    ImGui::Text("Select Background Image:");
    
    if (useSolidColor) {
        ImGui::Text("(Disabled in Solid Color mode)");
        return;
    }

    const auto& paths = backgroundRenderer.getImagePaths();
    for (size_t i = 0; i < paths.size(); ++i) {
        std::string label = std::filesystem::path(paths[i]).filename().string();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));
        if (ImGui::RadioButton(("##Radio" + std::to_string(i)).c_str(), backgroundRenderer.getCurrentImageIndex() == i)) {
            backgroundRenderer.setCurrentImage(i);
        }
        ImGui::PopStyleVar();
        ImGui::SameLine();
        ImGui::Text("%s", label.c_str());
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 160, 200, 50));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0, 100, 140, 80));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(230, 235, 240, 255));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 2.0f));
        if (emoji_font) ImGui::PushFont(emoji_font);
        
        const char* emoji = "❌";
        ImVec2 emoji_size = ImGui::CalcTextSize(emoji);
        ImVec2 button_size = ImVec2(emoji_size.x + 8.0f, emoji_size.y + 8.0f);
        
        if (ImGui::Button(("❌##" + std::to_string(i)).c_str(), button_size) && paths.size() > 1) {
            try {
                background_images_coll.delete_one(
                    bsoncxx::builder::stream::document{} << "path" << paths[i] << bsoncxx::builder::stream::finalize);
                backgroundRenderer.removeImage(i);
            }
            catch (const mongocxx::exception& e) {
                std::cerr << "Failed to remove image path: " << e.what() << std::endl;
                active_popup = "Database Error";
            }
        }
        
        if (emoji_font) ImGui::PopFont();
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();
    }
}

void LoginUi::LoginWindow::render_slideshow_settings() {
    ImGui::Dummy(ImVec2(0.0f, 12.0f)); // Section spacing
    if (emoji_font) ImGui::PushFont(emoji_font);
    ImGui::Text("⏲️");
    if (emoji_font) ImGui::PopFont();
    ImGui::SameLine();
    ImGui::Text("Slideshow Settings:");
    static bool slideshowEnabled = false;
    static float slideshowInterval = 5.0f;
    ImGui::Checkbox("Enable Automatic Slideshow", &slideshowEnabled);
    ImGui::Text("Switch Interval (seconds):");
    ImGui::SetNextItemWidth(100.0f); // Fixed width for input field
    ImGui::InputFloat("##SlideshowInterval", &slideshowInterval, 0.1f, 1.0f, "%.1f");
    if (slideshowInterval < 1.0f) slideshowInterval = 1.0f;
    if (slideshowInterval > 60.0f) slideshowInterval = 60.0f;

    ImGui::Dummy(ImVec2(0.0f, 5.0f)); // Spacing before buttons
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 320.0f) * 0.5f); // Center buttons
    if (ImGui::Button("Apply Slideshow", ImVec2(150.0f, 35.0f))) {
        try {
            if (slideshowInterval >= 1.0f && slideshowInterval <= 60.0f) {
                backgroundRenderer.enableSlideshow(slideshowEnabled, slideshowInterval);
                active_popup = "Slideshow Success";
            }
            else {
                active_popup = "Invalid Interval";
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Failed to apply slideshow: " << e.what() << std::endl;
            active_popup = "Slideshow Error";
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Disable Slideshow", ImVec2(150.0f, 35.0f))) {
        try {
            slideshowEnabled = false;
            backgroundRenderer.enableSlideshow(false, slideshowInterval);
            active_popup = "Slideshow Disabled";
        }
        catch (const std::exception& e) {
            std::cerr << "Failed to disable slideshow: " << e.what() << std::endl;
            active_popup = "Slideshow Error";
        }
    }
}

void LoginUi::LoginWindow::render_close_button() {
    ImGui::Dummy(ImVec2(0.0f, 3.0f)); // Increased spacing for prominence
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 140.0f) * 0.5f); // Wider button
    if (ImGui::Button("Close Window", ImVec2(140.0f, 38.0f))) { // Larger, more prominent button
        showBackgroundSettings = false;
    }
}

void LoginUi::LoginWindow::render_background_effects() {
    ImGui::Dummy(ImVec2(0.0f, 12.0f));
    if (emoji_font) ImGui::PushFont(emoji_font);
    ImGui::Text("🌫️");
    if (emoji_font) ImGui::PopFont();
    ImGui::SameLine();
    ImGui::Text("Background Effects:");

    // Blur Radius Slider
    ImGui::Text("Control Background Blur:");
    ImGui::SetNextItemWidth(200.0f);
    if (ImGui::SliderFloat("##BlurRadius", &blurRadius, 0.0f, 2.0f, "%.2f")) { // Adjusted range
        backgroundRenderer.setBlurRadius(blurRadius);
    }

    // Solid Color Toggle and Picker
    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    if (ImGui::Checkbox("Use Solid Color", &useSolidColor)) {
        backgroundRenderer.setSolidColorMode(useSolidColor);
        if (useSolidColor) {
            backgroundRenderer.setSolidColor(solidColor[0], solidColor[1], solidColor[2]);
        }
    }

    if (useSolidColor) {
        ImGui::SameLine();
        if (ImGui::ColorButton("##ColorPicker", ImVec4(solidColor[0], solidColor[1], solidColor[2], 1.0f))) {
            ImGui::OpenPopup("ColorPickerPopup");
        }
        if (ImGui::BeginPopup("ColorPickerPopup")) {
            if (ImGui::ColorPicker3("##ColorPicker3", solidColor, ImGuiColorEditFlags_DisplayRGB)) {
                backgroundRenderer.setSolidColor(solidColor[0], solidColor[1], solidColor[2]);
            }
            ImGui::EndPopup();
        }
    }
}

int LoginUi::LoginWindow::create_background_settings_window() {
    apply_window_styles();
    setup_window(); // Note: You may want to increase window height in setup_window()

    render_header();
    render_popups();
    render_background_effects(); // New section for blur and color
    render_add_image();
    render_select_image();
    render_slideshow_settings();
    render_close_button();

    ImGui::End();
    cleanup_window_styles();

    return 1;
}

int LoginUi::LoginWindow::render_login_extras(int display_w, int display_h) {
    // Modern button styling, matching apply_window_styles teal palette
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 128, 160, 255));       // Teal button
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 160, 200, 255)); // Lighter teal for hover
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0, 100, 140, 255));  // Darker teal for active
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(230, 235, 240, 255));       // Off-white text
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);                  // Rounded corners
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 8.0f));     // Comfortable button padding

    // Customize Background button (top-left, in a window)
    float button_width = 190.0f;  // Wide enough for text and emoji
    float button_height = 30.0f;  // Taller for prominence
    float window_padding = 10.0f; // Uniform padding around button
    float window_width = button_width + 2 * window_padding;  // Account for padding
    float window_height = button_height + 2 * window_padding; // Account for padding
    ImGui::SetNextWindowPos(ImVec2(20.0f, 20.0f), ImGuiCond_Always); // Top-left
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 0));   // Fully transparent window
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(window_padding, window_padding)); // Uniform window padding
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);         // Slightly rounder window
    ImGui::Begin("##CustomizeButton", nullptr, 
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | 
                 ImGuiWindowFlags_NoBackground);
    ImGui::SetWindowSize(ImVec2(window_width, window_height)); // Match button + padding

    // Subtle shadow effect using a semi-transparent border
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 100)); // Shadow
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.5f);       // Thicker border

    // Render button without label
    if (ImGui::Button("##CustomizeBackground", ImVec2(button_width, button_height))) {
        this->showBackgroundSettings = !this->showBackgroundSettings;
    }
    // Get button position for text alignment
    ImVec2 button_pos = ImGui::GetItemRectMin();
    ImVec2 text_pos = ImVec2(button_pos.x + 12.0f, button_pos.y + 8.0f); // Align with FramePadding
    ImGui::SetCursorScreenPos(text_pos);
    ImGui::PushFont(this->emoji_font); // Use emoji font for gear icon
    ImGui::Text("⚙️");
    ImGui::PopFont();
    ImGui::PushFont(this->default_font); // Use default font for text
    text_pos = ImVec2(button_pos.x + 35.0f, button_pos.y + 6.0f);
    ImGui::SetCursorScreenPos(text_pos);
    ImGui::Text("Customize Background");
    ImGui::PopFont();
    // Render emoji and text like the signature
  
    // Subtle hover animation
    if (ImGui::IsItemHovered()) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 1.5f); // Gentle lift effect
    }
    ImGui::PopStyleVar(1); // FrameBorderSize
    ImGui::PopStyleColor(1); // Border
    ImGui::PopStyleVar(2); // WindowPadding, WindowRounding
    ImGui::PopStyleColor(1); // WindowBg
    ImGui::End();

    // Developer signature (top-right)
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(150, 150, 150, 180)); // Subtle gray
    float signature_width = 240.0f;  // Increased width as requested
    float signature_height = 30.0f;  // Compact
    ImGui::SetNextWindowPos(ImVec2(display_w - signature_width - 20.0f, 20.0f), ImGuiCond_Always);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 0)); // Transparent window
    ImGui::Begin("##DeveloperSignature", nullptr, 
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | 
                 ImGuiWindowFlags_NoBackground);
    ImGui::SetWindowSize(ImVec2(signature_width, signature_height));
    ImGui::PushFont(this->default_font);
    ImGui::Text("Crafted with");
    ImGui::SameLine(0.0f, 5.0f); // Tight spacing
    ImGui::PopFont();
    ImGui::PushFont(this->emoji_font);
    ImGui::Text("💕");
    ImGui::PopFont();
    ImGui::SameLine(0.0f, 5.0f);
    ImGui::PushFont(this->default_font);
    ImGui::Text("by #KarisDevs");
    ImGui::PopFont();
    ImGui::End();
    ImGui::PopStyleColor(1); // WindowBg
    ImGui::PopStyleColor(1); // Signature text

    ImGui::PopStyleColor(4); // Button colors and text
    ImGui::PopStyleVar(2);   // FramePadding, FrameRounding

    return 1;
}


int LoginUi::LoginWindow::login_error_message(bool& error_flag)
{
    // Center the popup
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    // Apply error popup styling
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(50, 50, 50, 255)); // Dark background
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(255, 80, 80, 255)); // Red border
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 200, 200, 255)); // Light red text
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 150, 80, 255)); // Green button
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 180, 100, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0, 120, 60, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f); // Rounded corners
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f)); // Padding
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f); // Thicker border
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3.0f, 3.0f)); // Reduced button padding

    bool popup_open = true;
    if (ImGui::BeginPopupModal("Login Error", &popup_open, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::PushFont(this->large_font); // Use 24.0f font
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Invalid Password!").x) * 0.5f); // Center title
        ImGui::Text("Invalid Password!");
        ImGui::PopFont();
        ImGui::Separator();
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("Please check your password and try again.").x) * 0.5f); // Center text
        ImGui::Text("Please check your password and try again.");

        ImGui::Dummy(ImVec2(0.0f, 10.0f)); // Spacing
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 100.0f) * 0.5f); // Center button
        if (ImGui::Button("OK", ImVec2(100.0f, 30.0f))) {
            error_flag = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    else {
        // Popup was closed (e.g., via "X" button)
        error_flag = false; // Reset error flag to prevent reopening
    }

    // Pop styles
    ImGui::PopStyleColor(6);
    ImGui::PopStyleVar(4);

    return 1;
}