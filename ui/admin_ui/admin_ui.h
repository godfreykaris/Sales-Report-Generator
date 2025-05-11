#pragma once

#include <string>
#include <vector>
#include <memory>

#include <imgui.h>
#include "finance_ui/finance_ui.h"
#include "regular_ui/regular_user_ui.h"
#include "db/admin_ui_db_management.h"
#include "change_password.h"

extern bool admin_window;
extern bool showing_admin_extensions;
extern bool log_out;

namespace AdminUi
{
    // Agents Window for viewing agent details
    class AgentsWindow: public RUserUi::Window, public RUserUi::AgentsWindow
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
        int create_agents_filter();
        int draw_agents_table(float text_base_height);
        int create_agents_window();
    };

    class RemoveAgentWindow : public AgentsWindow
    {
    public:
        RemoveAgentWindow(mongocxx::database db);
        RemoveAgentWindow() = default;
        ~RemoveAgentWindow() = default;

        /* Properties */
    public:
        static int remove_current_filter_type_index;
        std::vector<int> checked_agents;

        std::string agent_name = "";
        std::string agent_id = "";

        int agents_count = 0;

        enum confirm_state { ACCEPTED = 0, REJECTED = 1, NONE = 2 };
        int confirm = confirm_state::NONE;
        bool show_confirm_window = false;

        std::vector<ImVec4> colum_colors = {
            ImVec4(0.25f, 0.8f, 0.11f, 0.65f),
            ImVec4(0.73f, 0.2f, 0.34f, 0.65f),
            ImVec4(0.26f, 0.6f, 0.2f, 0.65f),
            ImVec4(0.39f, 0.13f, 0.47f, 0.65f),
            ImVec4(0.8f, 0.55f, 0.22f, 0.65f)
        };

        std::string admin_filter_types = "No Criteria Selected_Filter by ID_Filter by Name_Filter by Phone_Filter by Location_";
        std::unique_ptr<char[]> p_admin_filter_types;

        mongocxx::collection agents;
        mongocxx::collection agent_sales;

        bool show_window = false;
        ImGuiWindowFlags window_flags;

        bool success = false; // Added for notify_sale_status
        bool input_error = false; // Added for admin_error_message

        /* Methods */
    public:
        int create_remove_agent_window();
        int create_admin_agents_filter();
        int draw_admin_agents_table(const float& m_TEXT_BASE_HEIGHT);
        // Note: admin_error_message and admin_confirm_message are assumed to be in RUserUi::AddQuantityWindow
    };

    class AddAgentWindow : public RemoveAgentWindow
    {
    public:
        AddAgentWindow(mongocxx::database db);
        AddAgentWindow() = default;
        ~AddAgentWindow() = default;

        /* Properties */
    public:
        std::string s_agent_name = "";
        std::string s_agent_location = "";
        std::string s_agent_phone = "";
        std::string s_agent_id = "";

        static char agent_name[128];
        static char agent_phone[13];
        static char agent_location[128];

        bool show_id = false;
        bool success = false; // Added for notify_sale_status
        bool input_error = false; // Added for admin_error_message

        bool show_window = false;
        ImGuiWindowFlags window_flags;

        /* Methods */
    public:
        int create_add_agent_window();
    };

    class AddorRemoveAgentsWindow
    {
    public:
        AddorRemoveAgentsWindow(AddAgentWindow& add_agent, RemoveAgentWindow& remove_agent);
        AddorRemoveAgentsWindow() = default;
        ~AddorRemoveAgentsWindow() = default;

        /* Properties */
    public:
        AddAgentWindow* add_agent;
        RemoveAgentWindow* remove_agent;

        bool show_window = false;
        ImGuiWindowFlags window_flags;

        /* Methods */
    public:
        int create_add_or_remove_agents_window();
    };

    class ShowAdminWindow : public AddorRemoveAgentsWindow, public PassWord
    {
    public:
        ShowAdminWindow(AddorRemoveAgentsWindow& add_or_remove_agents, PassWord& change_password, DBManagementWindow& db_management, mongocxx::database db);
        ~ShowAdminWindow() = default;

        AddorRemoveAgentsWindow* add_or_remove_agents;
        PassWord* change_password;
        DBManagementWindow* db_management;

        mongocxx::collection products; // Store products collection
        bool show_reset_success = false; // Flag for success message

        bool show_window = false;
        ImGuiWindowFlags window_flags;

        int create_show_admin_window();
    };
}