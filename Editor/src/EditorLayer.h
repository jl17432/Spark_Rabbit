#pragma once
#include<SparkRabbit.h>
#include"imgui.h"
#include"imgui_internal.h"


namespace SparkRabbit {
	class EditorLayer : public Layer
	{
	public:
		EditorLayer() : Layer("Editor") {}
		virtual ~EditorLayer() = default;

		virtual void update() override;
		virtual void onImguiRender() override;
		virtual void onEvent(Event& event) override;


	private:
		void EditorUI();
		void EditorMenu(bool* p_open);
		void EditorObjectsWindow(bool* p_open);
		void EditorFileWindow(bool* p_open);
		void EditorGameWindow(bool* p_open);
		void EditorDetailWindow(bool* p_open);

		void drawAxisToggleButton(const char* string_id, bool check_state, int axis_mode);

	private:
		bool m_editor_menu_window_open = true;
		bool m_asset_window_open = true;
		bool m_game_engine_window_open = true;
		bool m_file_content_window_open = true;
		bool m_detail_window_open = true;
		bool m_scene_lights_window_open = true;
		bool m_scene_lights_data_window_open = true;

		bool editor_mode = false;


	};
}