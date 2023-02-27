#include"EditorLayer.h"
#include "../../SparkRabbit/ThirdParties/GLFW/include/GLFW/glfw3.h"

namespace SparkRabbit {
	void EditorLayer::update()
	{

	}
	void EditorLayer::onImguiRender()
	{
        EditorUI();
	}
	void EditorLayer::onEvent(SparkRabbit::Event& event)
	{

	}

	void EditorLayer::EditorUI()
	{
		EditorMenu(&m_editor_menu_window_open);
		EditorObjectsWindow(&m_asset_window_open);
		EditorGameWindow(&m_game_engine_window_open);
		EditorFileWindow(&m_file_content_window_open);
		EditorDetailWindow(&m_detail_window_open);
	}

	void EditorLayer::EditorMenu(bool* p_open)
	{
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        int width = Application::Get().GetWindow().GetWidth();
        int height = Application::Get().GetWindow().GetHeight();
        ImGuiDockNodeFlags dock_flags = ImGuiDockNodeFlags_DockSpace;
        ImGuiWindowFlags   window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
            ImGuiConfigFlags_NoMouseCursorChange | ImGuiWindowFlags_NoBringToFrontOnFocus;

        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(main_viewport->WorkPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2((float)width, (float)height), ImGuiCond_Always);

        ImGui::SetNextWindowViewport(main_viewport->ID);

        ImGui::Begin("Editor menu", p_open, window_flags);

        ImGuiID main_docking_id = ImGui::GetID("Main Docking");
        if (ImGui::DockBuilderGetNode(main_docking_id) == nullptr)
        {
            ImGui::DockBuilderRemoveNode(main_docking_id);

            ImGui::DockBuilderAddNode(main_docking_id, dock_flags);
            ImGui::DockBuilderSetNodePos(main_docking_id,
                ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y + 18.0f));
            ImGui::DockBuilderSetNodeSize(main_docking_id,
                ImVec2((float)width, (float)height - 18.0f));

            ImGuiID center = main_docking_id;
            ImGuiID left;
            ImGuiID right = ImGui::DockBuilderSplitNode(center, ImGuiDir_Right, 0.25f, nullptr, &left);

            ImGuiID left_other;
            ImGuiID left_file_content = ImGui::DockBuilderSplitNode(left, ImGuiDir_Down, 0.30f, nullptr, &left_other);

            ImGuiID left_game_engine;
            ImGuiID left_asset =
                ImGui::DockBuilderSplitNode(left_other, ImGuiDir_Left, 0.30f, nullptr, &left_game_engine);

            ImGui::DockBuilderDockWindow("World Objects", left_asset);
            ImGui::DockBuilderDockWindow("Components Details", right);
            ImGui::DockBuilderDockWindow("File Content", left_file_content);
            ImGui::DockBuilderDockWindow("Game Engine", left_game_engine);

            ImGui::DockBuilderFinish(main_docking_id);
        }

        ImGui::DockSpace(main_docking_id);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Menu"))
            {
                if (ImGui::MenuItem("Reload Current Level"))
                {

                }
                if (ImGui::MenuItem("Save Current Level"))
                {

                }
                if (ImGui::BeginMenu("Debug"))
                {
                    if (ImGui::BeginMenu("Animation"))
                    {
                        if (ImGui::MenuItem( "show skeleton"))
                        {
                        }
                        if (ImGui::MenuItem("show bone name"))
                        {
                        }
                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Camera"))
                    {
                        if (ImGui::MenuItem("show runtime info"))
                        {
                        }
                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Game Object"))
                    {
                        if (ImGui::MenuItem("show bounding box"))
                        {

                        }
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::MenuItem("Exit"))
                {
                    Application::Get().Close();
                    exit(0);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Window"))
            {
                ImGui::MenuItem("World Objects", nullptr, &m_asset_window_open);
                ImGui::MenuItem("Game", nullptr, &m_game_engine_window_open);
                ImGui::MenuItem("File Content", nullptr, &m_file_content_window_open);
                ImGui::MenuItem("Detail", nullptr, &m_detail_window_open);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::End();
	}

	void EditorLayer::EditorObjectsWindow(bool* p_open)
	{

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

        if (!*p_open)
            return;

        if (!ImGui::Begin("World Objects", p_open, window_flags))
        {
            ImGui::End();
            return;
        }
        //TODO
        //需要实现场景管理获取object
        ImGui::End();
	}

	void EditorLayer::EditorFileWindow(bool* p_open)
	{
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

        if (!*p_open)
            return;

        if (!ImGui::Begin("File Content", p_open, window_flags))
        {
            ImGui::End();
            return;
        }

        static ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH |
            ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg |
            ImGuiTableFlags_NoBordersInBody;

        if (ImGui::BeginTable("File Content", 2, flags))
        {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();

            auto current_time = std::chrono::steady_clock::now();
            //TODO
            //文件管理
            ImGui::EndTable();
        }

        // file image list

        ImGui::End();
	}

	void EditorLayer::EditorGameWindow(bool* p_open)
	{
        ImGuiIO& io = ImGui::GetIO();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar;

        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

        if (!*p_open)
            return;

        if (!ImGui::Begin("Game Engine", p_open, window_flags))
        {
            ImGui::End();
            return;
        }

        static bool trans_button_ckecked = false;
        static bool rotate_button_ckecked = false;
        static bool scale_button_ckecked = false;

        if (ImGui::BeginMenuBar())
        {
            ImGui::Indent(10.f);
            drawAxisToggleButton("Trans", trans_button_ckecked, 1);
            ImGui::Unindent();

            ImGui::SameLine();

            drawAxisToggleButton("Rotate", rotate_button_ckecked, 2);

            ImGui::SameLine();

            drawAxisToggleButton("Scale", scale_button_ckecked, 3);

            ImGui::SameLine();


            if (editor_mode)
            {
                ImGui::PushID("Editor Mode");
                if (ImGui::Button("Editor Mode"))
                {
                    editor_mode = false;
                }
                ImGui::PopID();
            }
            else
            {
                if (ImGui::Button("Game Mode"))
                {
                    editor_mode = true;
                }
            }

            ImGui::Unindent();
            ImGui::EndMenuBar();
        }

        ImVec2 render_target_window_pos = { 0.0f, 0.0f };
        ImVec2 render_target_window_size = { 0.0f, 0.0f };

        auto menu_bar_rect = ImGui::GetCurrentWindow()->MenuBarRect();

        render_target_window_pos.x = ImGui::GetWindowPos().x;
        render_target_window_pos.y = menu_bar_rect.Max.y;
        render_target_window_size.x = ImGui::GetWindowSize().x;
        render_target_window_size.y = (ImGui::GetWindowSize().y + ImGui::GetWindowPos().y) - menu_bar_rect.Max.y;


        ImGui::End();
	}

	void EditorLayer::EditorDetailWindow(bool* p_open)
	{
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

        if (!*p_open)
            return;

        if (!ImGui::Begin("Components Details", p_open, window_flags))
        {
            ImGui::End();
            return;
        }

        //ECS未实现，整不了
        //const std::string& name = selected_object->getName();
        const std::string& name = "untitled";
        static char        cname[128];
        memset(cname, 0, 128);
        memcpy(cname, name.c_str(), name.size());

        ImGui::Text("Name");
        ImGui::SameLine();
        ImGui::InputText("##Name", cname, IM_ARRAYSIZE(cname), ImGuiInputTextFlags_ReadOnly);

        static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings;
        //后面实现ECS再写
        /*auto&& selected_object_components = selected_object->getComponents();
        for (auto component_ptr : selected_object_components)
        {
            m_editor_ui_creator["TreeNodePush"](("<" + component_ptr.getTypeName() + ">").c_str(), nullptr);
            auto object_instance = Reflection::ReflectionInstance(
                Piccolo::Reflection::TypeMeta::newMetaFromName(component_ptr.getTypeName().c_str()),
                component_ptr.operator->());
            createClassUI(object_instance);
            m_editor_ui_creator["TreeNodePop"](("<" + component_ptr.getTypeName() + ">").c_str(), nullptr);
        }*/
        ImGui::End();
	}

    void EditorLayer::drawAxisToggleButton(const char* string_id, bool check_state, int axis_mode)
    {
        if (check_state)
        {
            ImGui::PushID(string_id);
            ImVec4 check_button_color = ImVec4(93.0f / 255.0f, 10.0f / 255.0f, 66.0f / 255.0f, 1.00f);
            ImGui::PushStyleColor(ImGuiCol_Button,
                ImVec4(check_button_color.x, check_button_color.y, check_button_color.z, 0.40f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, check_button_color);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, check_button_color);
            ImGui::Button(string_id);
            ImGui::PopStyleColor(3);
            ImGui::PopID();
        }
        else
        {
            if (ImGui::Button(string_id))
            {
                check_state = true;
            }
        }
    }


	
}
