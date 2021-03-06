#pragma once

#include "Editor/EditorInterface.h"
#include "Logger.h"





#include "SDL_syswm.h"




static void import_scene_node(EditorInterface* editor, AssetImport_Scene* scene, AssetImport_SceneNode* parent_node, Entity parent_entity) {

	EntityManager* entity_manager = editor->api.entity_manager;
	AssetManager* asset_manager = editor->api.asset_manager;
	AssetImport_SceneNode* children = parent_node->children;
	for (u32 i = 0; i < parent_node->children_count; i++) {
		AssetImport_SceneNode* child_node = &children[i];
		Entity child_entity = create_entity(entity_manager, child_node->name);




		//add_component(manager, child_entity, ComponentType::Transform);
		//set_name(manager, child_entity, child_node->name);
		set_position(entity_manager, child_entity, child_node->translation);
		set_scale(entity_manager, child_entity, child_node->scale);
		set_rotation(entity_manager, child_entity, euler_to_quat(child_node->get_rotation));
		attach_child_entity(entity_manager, parent_entity, child_entity);

		import_scene_node(editor, scene, child_node, child_entity);
	}

	if (parent_node->mesh_count > 0) {
		for (u32 i = 0; i < parent_node->mesh_count; i++) {
			add_component(entity_manager, parent_entity, ComponentType::StaticMesh);
			add_component(entity_manager, parent_entity, ComponentType::Render);



			u32 mesh_index = parent_node->meshes[i];
			AssetID mesh_id = scene->mesh_infos[mesh_index];
			set_static_mesh(entity_manager, parent_entity, mesh_id.mesh);

			// TODO: What happens when the mesh has multiple child nodes, does that mean is has multiple materials?
			//assert(parent_node->children_count < 2);
			// Get material for this mesh
			if (parent_node->children_count != 0) {
				AssetImport_SceneNode* child_node = &parent_node->children[i];
				if (child_node->material_count > 0) {
					u32 mat_index = child_node->materials[0];
					AssetID material_id = scene->material_infos[mat_index];
					set_render_material(entity_manager, parent_entity, material_id.material);
				} else {
					// use default material
					set_render_material(entity_manager, parent_entity, asset_manager->default_mat.material);
				}
			} else {
				// use default material
				set_render_material(entity_manager, parent_entity, asset_manager->default_mat.material);

			}


		}
	}

}

Entity import_scene(EditorInterface* editor, SceneID id) {

	AssetID scene_id;
	scene_id.id = id.id;
	scene_id.type = AssetType::Scene;
	
	InternalAsset asset = get_internal_asset_by_id(editor->api.asset_manager, scene_id);

	AssetImport_Scene* scene = asset.scene;
	EntityManager* manager = editor->api.entity_manager;


	Entity root = create_entity(manager, scene->root->name);
	//add_component(manager, root, ComponentType::Transform);
	//set_name(manager, root, scene->root->name);

	set_position(manager, root, scene->root->translation);
	set_scale(manager, root, scene->root->scale);
	set_rotation(manager, root, euler_to_quat(scene->root->get_rotation));


	import_scene_node(editor, scene, scene->root, root);
	return root;
}

static void set_editor_layout(EditorInterface* editor) {
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	bool p_open = true;
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_PassthruCentralNode;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen) {
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (opt_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", &p_open, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// Dockspace
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
		editor->dockspace_id = ImGui::GetID("EditorDockspace");
	}

	if (ImGui::DockBuilderGetNode(editor->dockspace_id) == NULL) {
		// Set default layout
		ImGui::DockBuilderRemoveNode(editor->dockspace_id); // Clear out existing layout
		ImGui::DockBuilderAddNode(editor->dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty node
		ImGui::DockBuilderSetNodeSize(editor->dockspace_id, ImGui::GetMainViewport()->Size);

		ImGuiID dock_main_id = editor->dockspace_id; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
		
		
		ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.20f, NULL, &dock_main_id);
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.25f, NULL, &dock_main_id);
		ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.35f, NULL, &dock_main_id);
		ImGuiID dock_id_top = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.1f, NULL, &dock_main_id);
		

		ImGuiID dock_id_left_bottom = ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Down, 0.20f, NULL, &dock_id_left);



		
		ImGui::DockBuilderDockWindow("Toolbar", dock_id_top);

		ImGui::DockBuilderDockWindow(ICON_FA_VIDEO " Scene", dock_main_id);
		ImGui::DockBuilderDockWindow(ICON_FA_GAMEPAD " Game", dock_main_id);

		ImGui::DockBuilderDockWindow("Top - Y Axis", dock_main_id);
		ImGui::DockBuilderDockWindow("Front - Z Axis", dock_main_id);
		ImGui::DockBuilderDockWindow("Side - x Axis", dock_main_id);

		ImGui::DockBuilderDockWindow(ICON_FA_SITEMAP " Entity Scene Tree", dock_id_left);
		ImGui::DockBuilderDockWindow(ICON_FA_LIST " Entity Components", dock_id_right);
		
		

		ImGui::DockBuilderDockWindow(ICON_FA_FOLDER  " Asset Browser", dock_id_bottom);
		ImGui::DockBuilderDockWindow(ICON_FA_CHART_LINE " Render Stats", dock_id_bottom);
		ImGui::DockBuilderDockWindow(ICON_FA_TERMINAL " Log", dock_id_bottom);
		ImGui::DockBuilderDockWindow(ICON_FA_HISTORY " Command History", dock_id_left_bottom);

		
		
		

		ImGui::DockBuilderFinish(editor->dockspace_id);
	}

	ImGui::DockSpace(editor->dockspace_id, ImVec2(0,0), opt_flags);

	ImGui::End();
}

static void set_editor_style(EditorInterface* editor) {

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();
	
	ImFontConfig config;
	
	config.OversampleH = 4;
	config.OversampleV = 1;

	

	
	
	static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

	ImFont* font = io.Fonts->AddFontFromFileTTF("editor_resources/fonts/Lato-Bold.ttf", 14.0f, &config);
	config.MergeMode = true;
	config.GlyphMinAdvanceX = 13.0f;
	font = io.Fonts->AddFontFromFileTTF("editor_resources/fonts/fa-regular-400.ttf", 14.0f, &config, icon_ranges);
	font = io.Fonts->AddFontFromFileTTF("editor_resources/fonts/fa-solid-900.ttf", 14.0f, &config, icon_ranges);
	
	// If the font cant be loaded, load the default font
	if (font == NULL) {
		font = io.Fonts->AddFontDefault();
	}
	editor->editor_font = font;
	

	ImGuiStyle& style = ImGui::GetStyle();
	
	style.WindowPadding = ImVec2(8, 8);
	style.FramePadding = ImVec2(4, 7);
	style.ItemSpacing = ImVec2(8, 6);
	style.ItemInnerSpacing = ImVec2(8, 7);
	style.TouchExtraPadding = ImVec2(0, 0);

	style.IndentSpacing = 22;
	style.ScrollbarSize = 20;
	style.GrabMinSize = 16;
	
	style.WindowBorderSize = 1;
	style.ChildBorderSize = 0;
	style.PopupBorderSize = 1;
	style.FrameBorderSize = 1;
	
	style.WindowRounding = 0;
	style.ChildRounding = 0;
	style.FrameRounding = 2;
	style.PopupRounding = 2;
	style.ScrollbarRounding = 2;
	style.GrabRounding = 2;
	style.TabRounding = 2;
	
	style.WindowTitleAlign = ImVec2(0, 0.5f);
	style.ButtonTextAlign= ImVec2(0.5f, 0.5f);
	
	//ImVec4* colors = ImGui::GetStyle().Colors; 

	
	

	ImVec4 dark_accent = ImVec4(0.12f, 0.29f, 0.62f, 1.00f);
	ImVec4 light_accent = ImVec4(0.29f, 0.53f, 1.00f, 1.00f);
	

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
	colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.29f, 0.53f, 1.00f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.53f, 0.53f, 0.53f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.30f, 0.30f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.12f, 0.29f, 0.62f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.29f, 0.53f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.12f, 0.29f, 0.62f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.12f, 0.29f, 0.62f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.29f, 0.53f, 1.00f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.12f, 0.29f, 0.62f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.12f, 0.29f, 0.62f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.29f, 0.53f, 1.00f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.43f, 0.43f, 0.43f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.203f, 0.203f, 0.203f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.29f, 0.53f, 1.00f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.29f, 0.53f, 1.00f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.12f, 0.29f, 0.62f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.12f, 0.29f, 0.62f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
	colors[ImGuiCol_DockingPreview] = ImVec4(0.12f, 0.29f, 0.62f, 1.00f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.29f, 0.53f, 1.00f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.99f, 0.08f, 0.08f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.29f, 0.53f, 1.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.29f, 0.53f, 1.00f, 1.00f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.29f, 0.53f, 1.00f, 1.00f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.29f, 0.53f, 1.00f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.29f, 0.53f, 1.00f, 1.00f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.59f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.59f);


}

static void ShowStyleEditor(ImGuiStyle* ref)
{
	// You can pass in a reference ImGuiStyle structure to compare to, revert to and save to (else it compares to an internally stored reference)
	ImGuiStyle& style = ImGui::GetStyle();
	static ImGuiStyle ref_saved_style;

	// Default to using internal storage as reference
	static bool init = true;
	if (init && ref == NULL)
		ref_saved_style = style;
	init = false;
	if (ref == NULL)
		ref = &ref_saved_style;

	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);

	if (ImGui::ShowStyleSelector("Colors##Selector"))
		ref_saved_style = style;
	ImGui::ShowFontSelector("Fonts##Selector");

	// Simplified Settings
	if (ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f"))
		style.GrabRounding = style.FrameRounding; // Make GrabRounding always the same value as FrameRounding
	{ bool window_border = (style.WindowBorderSize > 0.0f); if (ImGui::Checkbox("WindowBorder", &window_border)) style.WindowBorderSize = window_border ? 1.0f : 0.0f; }
	ImGui::SameLine();
	{ bool frame_border = (style.FrameBorderSize > 0.0f); if (ImGui::Checkbox("FrameBorder", &frame_border)) style.FrameBorderSize = frame_border ? 1.0f : 0.0f; }
	ImGui::SameLine();
	{ bool popup_border = (style.PopupBorderSize > 0.0f); if (ImGui::Checkbox("PopupBorder", &popup_border)) style.PopupBorderSize = popup_border ? 1.0f : 0.0f; }

	// Save/Revert button
	if (ImGui::Button("Save Ref"))
		* ref = ref_saved_style = style;
	ImGui::SameLine();
	if (ImGui::Button("Revert Ref"))
		style = *ref;
	ImGui::SameLine();
	

	ImGui::Separator();

	if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem("Sizes"))
		{
			ImGui::Text("Main");
			ImGui::SliderFloat2("WindowPadding", (float*)& style.WindowPadding, 0.0f, 20.0f, "%.0f");
			ImGui::SliderFloat2("FramePadding", (float*)& style.FramePadding, 0.0f, 20.0f, "%.0f");
			ImGui::SliderFloat2("ItemSpacing", (float*)& style.ItemSpacing, 0.0f, 20.0f, "%.0f");
			ImGui::SliderFloat2("ItemInnerSpacing", (float*)& style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
			ImGui::SliderFloat2("TouchExtraPadding", (float*)& style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
			ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
			ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
			ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
			ImGui::Text("Borders");
			ImGui::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
			ImGui::SliderFloat("ChildBorderSize", &style.ChildBorderSize, 0.0f, 1.0f, "%.0f");
			ImGui::SliderFloat("PopupBorderSize", &style.PopupBorderSize, 0.0f, 1.0f, "%.0f");
			ImGui::SliderFloat("FrameBorderSize", &style.FrameBorderSize, 0.0f, 1.0f, "%.0f");
			ImGui::SliderFloat("TabBorderSize", &style.TabBorderSize, 0.0f, 1.0f, "%.0f");
			ImGui::Text("Rounding");
			ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
			ImGui::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
			ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
			ImGui::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
			ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
			ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
			ImGui::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");
			ImGui::Text("Alignment");
			ImGui::SliderFloat2("WindowTitleAlign", (float*)& style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
			ImGui::SliderFloat2("ButtonTextAlign", (float*)& style.ButtonTextAlign, 0.0f, 1.0f, "%.2f"); ImGui::SameLine();
			ImGui::SliderFloat2("SelectableTextAlign", (float*)& style.SelectableTextAlign, 0.0f, 1.0f, "%.2f"); ImGui::SameLine();
			ImGui::Text("Safe Area Padding"); ImGui::SameLine();
			ImGui::SliderFloat2("DisplaySafeAreaPadding", (float*)& style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Colors"))
		{
			static int output_dest = 0;
			static bool output_only_modified = true;
			if (ImGui::Button("Export Unsaved"))
			{
				if (output_dest == 0)
					ImGui::LogToClipboard();
				else
					ImGui::LogToTTY();
				ImGui::LogText("ImVec4* colors = ImGui::GetStyle().Colors;");
				for (int i = 0; i < ImGuiCol_COUNT; i++)
				{
					const ImVec4& col = style.Colors[i];
					const char* name = ImGui::GetStyleColorName(i);
					if (!output_only_modified || memcmp(&col, &ref->Colors[i], sizeof(ImVec4)) != 0)
						ImGui::LogText("colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);", name, 23 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
				}
				ImGui::LogFinish();
			}
			ImGui::SameLine(); ImGui::PushItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0"); ImGui::PopItemWidth();
			ImGui::SameLine(); ImGui::Checkbox("Only Modified Colors", &output_only_modified);

			static ImGuiTextFilter filter;
			filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

			static ImGuiColorEditFlags alpha_flags = 0;
			ImGui::RadioButton("Opaque", &alpha_flags, 0); ImGui::SameLine();
			ImGui::RadioButton("Alpha", &alpha_flags, ImGuiColorEditFlags_AlphaPreview); ImGui::SameLine();
			ImGui::RadioButton("Both", &alpha_flags, ImGuiColorEditFlags_AlphaPreviewHalf); ImGui::SameLine();
			

			ImGui::BeginChild("##colors", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NavFlattened);
			ImGui::PushItemWidth(-160);
			for (int i = 0; i < ImGuiCol_COUNT; i++)
			{
				const char* name = ImGui::GetStyleColorName(i);
				if (!filter.PassFilter(name))
					continue;
				ImGui::PushID(i);
				ImGui::ColorEdit4("##color", (float*)& style.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
				if (memcmp(&style.Colors[i], &ref->Colors[i], sizeof(ImVec4)) != 0)
				{
					// Tips: in a real user application, you may want to merge and use an icon font into the main font, so instead of "Save"/"Revert" you'd use icons.
					// Read the FAQ and misc/fonts/README.txt about using icon fonts. It's really easy and super convenient!
					ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Save")) ref->Colors[i] = style.Colors[i];
					ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Revert")) style.Colors[i] = ref->Colors[i];
				}
				ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
				ImGui::TextUnformatted(name);
				ImGui::PopID();
			}
			ImGui::PopItemWidth();
			ImGui::EndChild();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Fonts"))
		{
			ImGuiIO& io = ImGui::GetIO();
			ImFontAtlas* atlas = io.Fonts;
			
			ImGui::PushItemWidth(120);
			for (int i = 0; i < atlas->Fonts.Size; i++)
			{
				ImFont* font = atlas->Fonts[i];
				ImGui::PushID(font);
				bool font_details_opened = ImGui::TreeNode(font, "Font %d: \"%s\"\n%.2f px, %d glyphs, %d file(s)", i, font->ConfigData ? font->ConfigData[0].Name : "", font->FontSize, font->Glyphs.Size, font->ConfigDataCount);
				ImGui::SameLine(); if (ImGui::SmallButton("Set as default")) { io.FontDefault = font; }
				if (font_details_opened)
				{
					ImGui::PushFont(font);
					ImGui::Text("The quick brown fox jumps over the lazy dog");
					ImGui::PopFont();
					ImGui::DragFloat("Font scale", &font->Scale, 0.005f, 0.3f, 2.0f, "%.1f");   // Scale only this font
					ImGui::SameLine();
					ImGui::InputFloat("Font offset", &font->DisplayOffset.y, 1, 1, "%.0f");
					ImGui::Text("Ascent: %f, Descent: %f, Height: %f", font->Ascent, font->Descent, font->Ascent - font->Descent);
					ImGui::Text("Fallback character: '%c' (%d)", font->FallbackChar, font->FallbackChar);
					const float surface_sqrt = sqrtf((float)font->MetricsTotalSurface);
					ImGui::Text("Texture surface: %d pixels (approx) ~ %dx%d", font->MetricsTotalSurface, (int)surface_sqrt, (int)surface_sqrt);
					for (int config_i = 0; config_i < font->ConfigDataCount; config_i++)
						if (const ImFontConfig * cfg = &font->ConfigData[config_i])
							ImGui::BulletText("Input %d: \'%s\', Oversample: (%d,%d), PixelSnapH: %d", config_i, cfg->Name, cfg->OversampleH, cfg->OversampleV, cfg->PixelSnapH);
					if (ImGui::TreeNode("Glyphs", "Glyphs (%d)", font->Glyphs.Size))
					{
						// Display all glyphs of the fonts in separate pages of 256 characters
						for (int base = 0; base < 0x10000; base += 256)
						{
							int count = 0;
							for (int n = 0; n < 256; n++)
								count += font->FindGlyphNoFallback((ImWchar)(base + n)) ? 1 : 0;
							if (count > 0 && ImGui::TreeNode((void*)(intptr_t)base, "U+%04X..U+%04X (%d %s)", base, base + 255, count, count > 1 ? "glyphs" : "glyph"))
							{
								float cell_size = font->FontSize * 1;
								float cell_spacing = style.ItemSpacing.y;
								ImVec2 base_pos = ImGui::GetCursorScreenPos();
								ImDrawList* draw_list = ImGui::GetWindowDrawList();
								for (int n = 0; n < 256; n++)
								{
									ImVec2 cell_p1(base_pos.x + (n % 16) * (cell_size + cell_spacing), base_pos.y + (n / 16) * (cell_size + cell_spacing));
									ImVec2 cell_p2(cell_p1.x + cell_size, cell_p1.y + cell_size);
									const ImFontGlyph * glyph = font->FindGlyphNoFallback((ImWchar)(base + n));
									draw_list->AddRect(cell_p1, cell_p2, glyph ? IM_COL32(255, 255, 255, 100) : IM_COL32(255, 255, 255, 50));
									if (glyph)
										font->RenderChar(draw_list, cell_size, cell_p1, ImGui::GetColorU32(ImGuiCol_Text), (ImWchar)(base + n)); // We use ImFont::RenderChar as a shortcut because we don't have UTF-8 conversion functions available to generate a string.
									if (glyph && ImGui::IsMouseHoveringRect(cell_p1, cell_p2))
									{
										ImGui::BeginTooltip();
										ImGui::Text("Codepoint: U+%04X", base + n);
										ImGui::Separator();
										ImGui::Text("AdvanceX: %.1f", glyph->AdvanceX);
										ImGui::Text("Pos: (%.2f,%.2f)->(%.2f,%.2f)", glyph->X0, glyph->Y0, glyph->X1, glyph->Y1);
										ImGui::Text("UV: (%.3f,%.3f)->(%.3f,%.3f)", glyph->U0, glyph->V0, glyph->U1, glyph->V1);
										ImGui::EndTooltip();
									}
								}
								ImGui::Dummy(ImVec2((cell_size + cell_spacing) * 16, (cell_size + cell_spacing) * 16));
								ImGui::TreePop();
							}
						}
						ImGui::TreePop();
					}
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
			if (ImGui::TreeNode("Atlas texture", "Atlas texture (%dx%d pixels)", atlas->TexWidth, atlas->TexHeight))
			{
				ImGui::Image(atlas->TexID, ImVec2((float)atlas->TexWidth, (float)atlas->TexHeight), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
				ImGui::TreePop();
			}

			static float window_scale = 1.0f;
			if (ImGui::DragFloat("this window scale", &window_scale, 0.005f, 0.3f, 2.0f, "%.2f"))   // scale only this window
				ImGui::SetWindowFontScale(window_scale);
			ImGui::DragFloat("global scale", &io.FontGlobalScale, 0.005f, 0.3f, 2.0f, "%.2f");      // scale everything
			ImGui::PopItemWidth();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Rendering"))
		{
			ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines); ImGui::SameLine();
			ImGui::Checkbox("Anti-aliased fill", &style.AntiAliasedFill);
			ImGui::PushItemWidth(100);
			ImGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, FLT_MAX, "%.2f", 2.0f);
			if (style.CurveTessellationTol < 0.10f) style.CurveTessellationTol = 0.10f;
			ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
			ImGui::PopItemWidth();

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::PopItemWidth();
}


// Helper function to setup all the editor cameras
static void init_viewport(EditorInterface* editor, PanelViewports::Viewport* viewport, const char* camera_name, Vec3f pos, Vec3f rotation, CameraProjection projection_type) {
	
	viewport->size = Vec2i(1920, 1080);
	u32 width = viewport->size.x;
	u32 height = viewport->size.y;

	viewport->camera = create_entity(editor->api.entity_manager, camera_name);
	add_component(editor->api.entity_manager, viewport->camera, ComponentType::Transform);
	add_component(editor->api.entity_manager, viewport->camera, ComponentType::Camera);
	set_camera_params(editor->api.entity_manager, viewport->camera, 0.0001f, 100.0f, 90.0f, viewport->size.x / cast(float) viewport->size.y);
	set_camera_projection(editor->api.entity_manager, viewport->camera, projection_type);
	set_position(editor->api.entity_manager, viewport->camera, pos);
	set_rotation(editor->api.entity_manager, viewport->camera, euler_to_quat(rotation));


	

	Texture2D color_texture = Texture2D(); color_texture.data = NULL; color_texture.width = width; color_texture.height = height;
	Texture2D depth_texture = Texture2D(); depth_texture.data = NULL; depth_texture.width = width; depth_texture.height = height;



	viewport->render_texture = create_texture_resource(editor->api.renderer, &color_texture, false);
	viewport->depth_texture = create_texture_resource(editor->api.renderer, &depth_texture, false, true);

	FrameBufferAttachement color_texture_attachement = FrameBufferAttachement(FrameBufferAttachementType::COLOR, viewport->render_texture, 0);
	FrameBufferAttachement depth_texture_attachement = FrameBufferAttachement(FrameBufferAttachementType::DEPTH, viewport->depth_texture, 0);
	FrameBufferAttachement attachments[2] = { color_texture_attachement, depth_texture_attachement };

	viewport->framebuffer = create_frame_buffer(editor->api.renderer, (u32)2, attachments);
	set_camera_framebuffer(editor->api.entity_manager, viewport->camera, &viewport->framebuffer);
}

bool init_editor_interface(EditorInterface* editor, EngineAPI api) {

	set_editor_style(editor);

	map_init(&editor->entity_selected);
	map_init(&editor->panel_asset_browser.asset_thumbnail_cache);
	

	editor->panel_log.show_info = true;
	editor->panel_log.show_warning = true;
	editor->panel_log.show_fatal = true;
	editor->panel_log.log_start_offset = 0;
	editor->panel_log.show_time = true;
	editor->panel_log.show_tag = true;
	editor->panel_log.show_thread_id = true;
	editor->panel_log.show_filename = true;
	editor->panel_log.show_function = true;
	editor->panel_log.show_line = true;
	editor->panel_log.show_message = true;
	editor->panel_log.panel_open = true;

	editor->editor_control_data.gizmo_operation = ImGuizmo::TRANSLATE;


	
	
	editor->panel_asset_browser.panel_open = true;
	editor->panel_asset_details.panel_open = false;
	editor->panel_asset_details.current_asset = NULL;

	editor->panel_scenetree.panel_open = true;
	editor->panel_components.panel_open= true;
	editor->panel_render_stats.panel_open = true;

	editor->viewports.current_viewport_capture = PanelViewports::ViewportType::None;
	editor->viewports.right_click_down = false;

	editor->api = api;
	

	arena_init(&editor->arena);
	size_t mem_size = EDITOR_MEMORY;
	void* mem_block = arena_alloc(&editor->arena, mem_size);
	mem_size = editor->arena.end - cast(char*) mem_block;
	stack_alloc_init(&editor->stack, mem_block, mem_size);

	
	
	init_asset_importer(&editor->importer, &editor->api.asset_manager->asset_tracker);
	construct_asset_brower_tree(editor);

	
	// Setup the scene, top,front, and side viewports
	float initial_ortho_distance = 10.0f;
	init_viewport(editor, &editor->viewports.scene, "Editor Camera", Vec3f(0, 0, 0), Vec3f(0, 0, 0), CameraProjection::Perspective);
	init_viewport(editor, &editor->viewports.top, "Top Camera", Vec3f(0, initial_ortho_distance, 0), Vec3f(-90.0f, 0, 0), CameraProjection::Orthographic);
	init_viewport(editor, &editor->viewports.front, "Front Camera", Vec3f(0, 0, initial_ortho_distance), Vec3f(0, 0, 0), CameraProjection::Orthographic);
	init_viewport(editor, &editor->viewports.side, "Side Camera", Vec3f(initial_ortho_distance, 0, 0), Vec3f(0, 90.0f, 0), CameraProjection::Orthographic);
	
	
	//load_hdr_skymap(&editor->hdr_skymap, &editor->stack, "InternalAssets/skyboxes/hdr/Alexs_Apartment/Alexs_Apt_2k.hdr");
	load_hdr_skymap(&editor->hdr_skymap, &editor->stack, "InternalAssets/skyboxes/hdr/Mono_Lake_B/Mono_Lake_B_Ref.hdr");
	//load_hdr_skymap(&editor->hdr_skymap, &editor->stack, "InternalAssets/skyboxes/hdr/Newport_Loft/Newport_Loft_Ref.hdr");

	create_skymap(api.renderer, &editor->hdr_skymap);
	create_shadowmap(api.renderer);

	
	//AssetID tcolor = import_texture(editor->api.asset_manager,		String("Assets/textures/plastic/scuffed-plastic-alb.png"), true);
	//AssetID tnormal = import_texture(editor->api.asset_manager,		String("Assets/textures/plastic/scuffed-plastic-normal.png"), true);
	//AssetID tmetallic = import_texture(editor->api.asset_manager,	String("Assets/textures/plastic/scuffed-plastic-metal.png"), true);
	//AssetID troughness = import_texture(editor->api.asset_manager,	String("Assets/textures/plastic/scuffed-plastic-rough.png"), true);
	//AssetID tao = import_texture(editor->api.asset_manager,			String("Assets/textures/plastic/scuffed-plastic-ao.png"), true);
	//////AssetID theight = import_texture(editor->api.asset_manager, String("Assets/textures/wet_stone/slipperystonework-height.png"), true);
	////
	//Material test_mat;
	//init_material_defaults(&test_mat);
	//
	//test_mat.albedo = tcolor.texture;
	//test_mat.normal = tnormal.texture;
	//test_mat.metal = tmetallic.texture;
	//test_mat.roughness = troughness.texture;
	//test_mat.ao = tao.texture;
	////test_mat.height = theight.texture;
	//
	//AssetID mat_id = create_material_asset(editor->api.asset_manager, "Assets\\textures\\plastic\\", "plastic", &test_mat);
	//editor->test_mat = mat_id;

	//AssetID tcolor = import_texture(editor->api.asset_manager, String("Assets/textures/gold/gold-scuffed_basecolor.png"), true);
	//AssetID tnormal = import_texture(editor->api.asset_manager, String("Assets/textures/gold/gold-scuffed_normal.png"), true);
	//AssetID tmetallic = import_texture(editor->api.asset_manager, String("Assets/textures/gold/gold-scuffed_metallic.png"), true);
	//AssetID troughness = import_texture(editor->api.asset_manager, String("Assets/textures/gold/gold-scuffed_roughness.png"), true);
	//AssetID tao = import_texture(editor->api.asset_manager, String("Assets/textures/gold/gold-scuffed_metallic.png"), true);
	//////AssetID theight = import_texture(editor->api.asset_manager, String("Assets/textures/wet_stone/slipperystonework-height.png"), true);
	////
	//Material test_mat;
	//init_material_defaults(&test_mat);
	//
	//test_mat.albedo = tcolor.texture;
	//test_mat.normal = tnormal.texture;
	//test_mat.metal = tmetallic.texture;
	//test_mat.roughness = troughness.texture;
	//test_mat.ao = tao.texture;
	////test_mat.height = theight.texture;
	//
	//AssetID mat_id = create_material_asset(editor->api.asset_manager, "Assets\\textures\\gold\\", "gold", &test_mat);
	//editor->test_mat = mat_id;





	

	//AssetID mat_id = load_asset_by_name(editor->api.asset_manager, "Assets/textures/plastic/plastic_mat_mat.easset");
	//editor->default_mat = load_asset_by_name(editor->api.asset_manager, "Assets/textures/bamboo-wood/bamboo-wood_mat.easset");
	//editor->default_mat = load_asset_by_name(editor->api.asset_manager, "Assets/textures/plastic/plastic_mat.easset");
	//editor->default_mat = load_asset_by_name(editor->api.asset_manager, "Assets/textures/plastic/plastic_green_mat.easset");
	//editor->default_mat = load_asset_by_name(editor->api.asset_manager, "Assets/textures/wet_stone/slipperystonework_mat.easset");
	editor->default_mat = load_asset_by_name(editor->api.asset_manager, "Assets/textures/gold/gold_mat.easset");

	//editor->test_mat = load_asset_by_name(editor->api.asset_manager, "Assets/textures/rust_iron/rust_iron_mat.easset");
	//editor->test_mat = load_asset_by_name(editor->api.asset_manager, "Assets/textures/paint_cement/paint_cement_mat_mat.easset");
	
	//AssetID import_scene_fbx = import_fbx(&editor->importer, "Assets/test_fbx/mill.fbx", false);
	//Entity e = import_scene(editor, import_scene_fbx.scene);

	
	


	EntityManager* entity_manager = editor->api.entity_manager;
	AssetManager* asset_manager = editor->api.asset_manager;

	
	editor->test_mesh = create_entity(entity_manager, "Test mesh");
	editor->entity_test_light = create_entity(entity_manager, "Test Light");

	add_component(entity_manager, editor->entity_test_light, ComponentType::Light);


	//attach_child_entity(entity_manager, e3, e4);
	//attach_child_entity(entity_manager, e3, e8);
	//attach_child_entity(entity_manager, e4, e5);
	//attach_child_entity(entity_manager, e3, e6);
	//attach_child_entity(entity_manager, e3, editor->test_mesh);




	



	
	for (int i = 0; i < 10; i++) {
		Entity new_entity = create_entity(entity_manager, "Entity");

		add_component(entity_manager, new_entity, ComponentType::StaticMesh);
		add_component(entity_manager, new_entity, ComponentType::Render);
		set_render_material(entity_manager, new_entity, editor->default_mat.material);
		set_static_mesh(entity_manager, new_entity, editor->api.asset_manager->sphere_mesh.mesh);
		set_position(entity_manager, new_entity, Vec3f(i * 2, i, 0));
	}

	


	
	

	//add_component(entity_manager, editor->test_mesh, ComponentType::StaticMesh);
	//add_component(entity_manager, editor->test_mesh, ComponentType::Render);
	//set_render_material(entity_manager, editor->test_mesh, mat_id.material);
	//set_render_visibility(entity_manager, editor->test_mesh, true);
	//set_static_mesh(entity_manager, editor->test_mesh, editor->api.asset_manager->cube_mesh.mesh);
	


	
	
	


	Light light;
	light.type = LightType::DirectionalLight;
	light.dir_light.direction = Vec3f(0.00000000001f, -1.0f, 0.0f);;
	light.dir_light.color = Vec3f(1, 1, 1);
	set_light(entity_manager, editor->entity_test_light, light);




	
	

	//AssetImporter* importer = &editor->importer;
	//AssetID scene_asset;
	//scene_asset = find_asset_by_name(importer->tracker, "mill.fbx.easset");
	//if (scene_asset.id == 0) {
	//	scene_asset = import_fbx(importer, "Assets/test_fbx/mill.fbx", true);
	//}
	

	
	return true;
}

void destroy_editor_interface(EditorInterface* editor) {

	map_destroy(&editor->entity_selected);
	map_destroy(&editor->panel_asset_browser.asset_thumbnail_cache);
	destroy_asset_importer(&editor->importer);
	arena_free(&editor->arena);
}

static void draw_gizmo_for_entity_with_camera(EditorInterface* editor, PanelViewports::Viewport viewport, Entity entity) {

	
	EntityManager* entity_manager = editor->api.entity_manager;
	
	Mat4x4f* view_mat = get_camera_view_matrix(entity_manager, viewport.camera);
	Mat4x4f proj_mat = get_camera_projection_matrix(entity_manager, viewport.camera);
	Mat4x4f* object_mat = get_world_mat(entity_manager, entity);
	
	
	bool world = editor->editor_control_data.gizmo_mode == ImGuizmo::WORLD;
	
	
	Vec3f old_pos = get_position(entity_manager, entity);
	Vec3f old_scale = get_scale(entity_manager, entity);
	Quat old_rot = get_rotation(entity_manager, entity);
		
	
	float default_snap = 0;
	float* snap = &default_snap;
	
	switch (editor->editor_control_data.gizmo_operation) {
		case ImGuizmo::TRANSLATE:
			snap = editor->editor_control_data.snap_translate.data;
			break;
		case ImGuizmo::ROTATE:
			snap = &editor->editor_control_data.snap_rotate;
			break;
		case ImGuizmo::SCALE:
			snap = editor->editor_control_data.snap_scale.data;
			break;
	}
	
	
	
	
	Mat4x4f object_mat_copy;
	memcpy(object_mat_copy.mat1d, object_mat->mat1d, sizeof(object_mat->mat1d));
	

	ImGuizmo::Enable(true);
	ImGuizmo::SetRect(editor->viewports.scene.pos.x, editor->viewports.scene.pos.y, editor->viewports.scene.size.x, editor->viewports.scene.size.y);
	ImGuizmo::Manipulate(view_mat->mat1d,
		proj_mat.mat1d,
		editor->editor_control_data.gizmo_operation,
		editor->editor_control_data.gizmo_mode,
		object_mat_copy.mat1d,
		NULL,
		editor->editor_control_data.use_gizmo_snapping ? snap : NULL
		//editor->editor_control_data.gizmo_operation == ImGuizmo::SCALE ? bounds_snap.data : NULL,
		//editor->editor_control_data.gizmo_operation == ImGuizmo::SCALE ? snap : NULL
		);
	
	
	if (ImGuizmo::IsUsing()) {
		editor->editor_control_data.was_using_gizmo_last_frame = true;

		// Get parent world transform
		Entity par = parent(entity_manager, entity);
		Mat4x4f* par_mat = get_world_mat(entity_manager, par);

		// Invert the parent world transform
		Mat4x4f inv_mat;
		mat4x4f_invert(*par_mat, &inv_mat);

		// Transform the new world transform of the object we just transformed back into world space
		// local = inverse (world parent) * world of object
		object_mat_copy = inv_mat * object_mat_copy;

		Vec3f local_new_pos;
		Vec3f local_new_rot;
		Vec3f local_new_scale;
		ImGuizmo::DecomposeMatrixToComponents(object_mat_copy.mat1d, local_new_pos.data, local_new_rot.data, local_new_scale.data);

		

		if (magnitude(old_pos - local_new_pos) > 0.0f) {
			set_position(entity_manager, entity, local_new_pos);
			cmd_edtior_set_transform_component(editor, entity, old_pos, old_rot, old_scale, local_new_pos, old_rot, old_scale, true);
		}

		Quat local_new_rot_quat = euler_to_quat(local_new_rot);
		Vec4f rot_diff = quat_to_vec(old_rot) - quat_to_vec(local_new_rot_quat);
		if (magnitude(rot_diff) > 0.0f) {
			set_rotation(entity_manager, entity, local_new_rot_quat);
			cmd_edtior_set_transform_component(editor, entity, old_pos, old_rot, old_scale, old_pos, local_new_rot_quat, old_scale, true);
		}

		if (magnitude(old_scale - local_new_scale) > 0.0f) {
			set_scale(entity_manager, entity, local_new_scale);
			cmd_edtior_set_transform_component(editor, entity, old_pos, old_rot, old_scale, old_pos, old_rot, local_new_scale, true);
		}
	
	
	} else if (editor->editor_control_data.was_using_gizmo_last_frame) {
		editor->editor_control_data.was_using_gizmo_last_frame = false;
		cmd_editor_force_no_merge(editor);
	} 
	
}

void editor_update(EditorInterface* editor) {

	ImGui::PushFont(editor->editor_font);
	
	Input* input = editor->api.input;
	GameTimer* timer = editor->api.game_loop;
	EntityManager* entity_manager = editor->api.entity_manager;

	Renderer* renderer = editor->api.renderer;
	Window* window = editor->api.window;

	Camera* camera = get_camera(entity_manager, editor->viewports.scene.camera);
	EditorControlsData* editor_control_data = &editor->editor_control_data;
	
	
	//  Used to style the editor
	ImGuiStyle* style = &ImGui::GetStyle();
	if (ImGui::Begin("srtles")) {
		ShowStyleEditor(style);
	}
	ImGui::End();
	
	
	float delta_time = get_delta_time(timer);
	
	// Check for undo key and undo repeat
	bool is_ctrl_down = is_keymod_down(input, KEYMOD_CTRL);
	if (is_key_pressed(input, KEYCODE_Z) && is_ctrl_down) {
		// If the user pressed ctrl + z, always do it right away, do not wait for repeat/delay
		editor_control_data->last_undo_time = timer->milliseconds;
		editor_control_data->undo_repeat_count = 0;
		perform_undo_operation(editor);
	} else {	
		// If ctrl + z is repeated, check for the key delay
		if (is_key_down(input, KEYCODE_Z) && is_ctrl_down) {
			if (timer->milliseconds - editor_control_data->last_undo_time > (EditorControlsData::REPEAT_DELAY_TIME - (editor_control_data->undo_repeat_count * EditorControlsData::INCREMENT_REPEAT_TIME))) {
				editor_control_data->last_undo_time = timer->milliseconds;
				editor_control_data->undo_repeat_count = MIN(editor_control_data->undo_repeat_count + 1, EditorControlsData::MAX_INCREMENT);
				perform_undo_operation(editor);
			}
		}
	}

	

	// Check for redo key and redo repeat
	if (is_key_pressed(input, KEYCODE_Y) && is_ctrl_down) {
		// If the user pressed ctrl + y, always do it right away, do not wait for repeat/delay
		editor_control_data->last_redo_time = timer->milliseconds;
		editor_control_data->redo_repeat_count = 0;
		perform_redo_operation(editor);
	} else {
		if (is_key_down(input, KEYCODE_Y) && is_ctrl_down) {
			// If ctrl + y is repeated, check for the key delay
			if (timer->milliseconds - editor_control_data->last_redo_time > (EditorControlsData::REPEAT_DELAY_TIME - (editor_control_data->redo_repeat_count * EditorControlsData::INCREMENT_REPEAT_TIME))) {
				editor_control_data->last_redo_time = timer->milliseconds;
				editor_control_data->redo_repeat_count = MIN(editor_control_data->redo_repeat_count + 1, EditorControlsData::MAX_INCREMENT);
				perform_redo_operation(editor);
			}
		}
	}




	if (is_key_pressed(input, KEYCODE_C) && is_ctrl_down) {
		LOG_INFO("Editor", "perform copy command");
	}
	if (is_key_pressed(input, KEYCODE_V) && is_ctrl_down) {
		LOG_INFO("Editor", "perform paste command");
	}
	if (is_key_pressed(input, KEYCODE_D) && is_ctrl_down) {
		LOG_INFO("Editor", "perform duplicate command");
	}



	
	

	//for (int i = 8; i < 18; i++) {
	//	Entity e3 = Entity(i);
	//	Vec3f epos = get_position(entity_manager, e3);
	//	epos.y =  epos.y + 5 * sinf_(timer->seconds * i) * timer->delta_time;
	//	//epos.x = epos.x + (1 * timer->delta_time);
	//	set_position(entity_manager, e3, epos);
	//}

	enum class OrthoPlaneMovement {
		XZ,
		YZ,
		XY,
	};
	
	OrthoPlaneMovement plane;




	

	if (editor->viewports.current_viewport_capture == PanelViewports::ViewportType::Scene) {
		if (is_mouse_pressed(input, MouseButton::Left)) {
			Vec2i mouse_pos = get_mouse_pos(input);
			
			// Do a raycast to check which object is pressed
			//LOG_INFO("MOUSE", "Mouse pos %d, %d\n", mouse_pos.x, mouse_pos.y);
		}

		
		Vec2i scroll = get_scroll_delta(input);

		// Capture scolling to move camera forward and back
		if (scroll.y != 0) {
			Vec3f new_cam_direction = (delta_time * -forward(entity_manager, editor->viewports.scene.camera));

			// TODO: make this configurable
			float scroll_scale = 10.0f;

			float cam_move_scale = scroll_scale * scroll.y;
			Vec3f cam_pos = get_position(entity_manager, editor->viewports.scene.camera);
			set_position(entity_manager, editor->viewports.scene.camera, cam_pos + (cam_move_scale * new_cam_direction));
		}

		int x = input->mouse.pos.x;
		int y = input->mouse.pos.y;

		int gx = input->mouse.global_pos.x;
		int gy = input->mouse.global_pos.y;

		int sx = window->size.x;
		int sy = window->size.y;

		Vec2i delta_pos = input->mouse.delta_pos;




		// Only apply editor movement if right mouse button is clicked
		if (is_mouse_down(input, MouseButton::Right)) {
			//SDL_SetWindowGrab(window->sdl_window, SDL_TRUE);
			SDL_ShowCursor(SDL_DISABLE);


			SDL_SetRelativeMouseMode(SDL_TRUE);

			Vec2i rel_pos = Vec2i(0, 0);
			// TODO: Move this to input
			SDL_GetRelativeMouseState(&rel_pos.x, &rel_pos.y);
			//SDL_SetRelativeMouseMode(SDL_FALSE);

			Vec3f new_cam_direction;


			// Since the camera always looks down -z
			if (is_key_down(input, KEYCODE_W)) { new_cam_direction += (delta_time * -forward(entity_manager, editor->viewports.scene.camera)); }
			if (is_key_down(input, KEYCODE_S)) { new_cam_direction += (delta_time * forward(entity_manager, editor->viewports.scene.camera)); }
			if (is_key_down(input, KEYCODE_D)) { new_cam_direction += (delta_time * right(entity_manager, editor->viewports.scene.camera)); }
			if (is_key_down(input, KEYCODE_A)) { new_cam_direction += (delta_time * -right(entity_manager, editor->viewports.scene.camera)); }
			if (is_key_down(input, KEYCODE_LSHIFT)) { new_cam_direction += (delta_time * up(entity_manager, editor->viewports.scene.camera)); }
			if (is_key_down(input, KEYCODE_LCTRL)) { new_cam_direction += (delta_time * -up(entity_manager, editor->viewports.scene.camera)); }

			float cam_move_scale = 10;
			Vec3f cam_pos = get_position(entity_manager, editor->viewports.scene.camera);
			set_position(entity_manager, editor->viewports.scene.camera, cam_pos + (cam_move_scale * new_cam_direction));


			// Prevent camera rotation jump, when the delta between the last time the right mouse was down and now
			if (editor->viewports.right_click_down) {
				// TODO: this will be exposed to the user, we still need to implement proper control handling in engine
				float sensitivity = 0.25f;
				Quat old_cam_rot = get_rotation(entity_manager, editor->viewports.scene.camera);
				Quat new_cam_rot = quat_from_axis_angle(Vec3f_Up, -rel_pos.x * sensitivity) * old_cam_rot;
				new_cam_rot = new_cam_rot * quat_from_axis_angle(Vec3f_Right, -rel_pos.y * sensitivity);

				set_rotation(entity_manager, editor->viewports.scene.camera, new_cam_rot);
			}
			editor->viewports.right_click_down = true;
		}
		else {

			if (is_key_pressed(input, KEYCODE_W)) {
				editor->editor_control_data.gizmo_operation = ImGuizmo::TRANSLATE;
			}
			if (is_key_pressed(input, KEYCODE_E)) {
				editor->editor_control_data.gizmo_operation = ImGuizmo::ROTATE;
			}
			if (is_key_pressed(input, KEYCODE_R)) {
				editor->editor_control_data.gizmo_operation = ImGuizmo::SCALE;
			}

			editor->viewports.right_click_down = false;
			SDL_ShowCursor(SDL_ENABLE);
			SDL_SetRelativeMouseMode(SDL_FALSE);

		}
	}
	else {
		Entity ortho_cam;
		bool ortho_capture = false;
		if (editor->viewports.current_viewport_capture == PanelViewports::ViewportType::Top) {
			ortho_cam = editor->viewports.top.camera;
			ortho_capture = true;
			plane = OrthoPlaneMovement::XZ;
		}

		else if (editor->viewports.current_viewport_capture == PanelViewports::ViewportType::Side) {
			ortho_cam = editor->viewports.side.camera;
			ortho_capture = true;
			plane = OrthoPlaneMovement::YZ;
		}

		else if (editor->viewports.current_viewport_capture == PanelViewports::ViewportType::Front) {
			ortho_cam = editor->viewports.front.camera;
			ortho_capture = true;
			plane = OrthoPlaneMovement::XY;
		}


		if (ortho_capture) {
			Vec2i scroll = get_scroll_delta(input);

			float size = get_camera_ortho_size(entity_manager, ortho_cam);
			// Capture scolling to move camera forward and back
			if (scroll.y != 0) {

				// TODO: make this configurable
				float scroll_scale = 1.0f;

				float cam_move_scale = scroll_scale * -scroll.y;

				size = size + cam_move_scale;
				if (size < 1) {
					size = 1;
				}
				set_camera_ortho_size(entity_manager, ortho_cam, size);
			}

			// Only apply editor movement if right mouse button is clicked
			if (is_mouse_down(input, MouseButton::Right)) {

				Vec2f mouse_pos = Vec2f(0, 0);
				mouse_pos = Vec2f(input->mouse.delta_pos.x, input->mouse.delta_pos.y);
				// For now until we can do exact mouse movement, we'll just scale down the movement for the ortho viewports
				mouse_pos.x *= (0.01f * size);
				mouse_pos.y *= (0.01f * size);
				Vec3f cam_pos = get_position(entity_manager, ortho_cam);
				
				Vec3f new_cam_pos;
				switch (plane) {
					case OrthoPlaneMovement::XZ: {
						new_cam_pos = Vec3f(cam_pos.x - mouse_pos.x, cam_pos.y, cam_pos.z - mouse_pos.y);
						break;
					}
					case OrthoPlaneMovement::YZ:  {
						new_cam_pos = Vec3f(cam_pos.x, cam_pos.y + mouse_pos.y, cam_pos.z + mouse_pos.x);
						break;
					}
					case OrthoPlaneMovement::XY: {
						new_cam_pos = Vec3f(cam_pos.x - mouse_pos.x, cam_pos.y + mouse_pos.y, cam_pos.z);
						break;
					}

				}
				
				set_position(entity_manager, ortho_cam, new_cam_pos);

				editor->viewports.right_click_down = true;
			} else {
				editor->viewports.right_click_down = false;
			}

		}
	}
	
	





	

	

	set_editor_layout(editor);
	draw_main_menu_bar(editor);
	draw_toolbar(editor);
	draw_panel_components(editor);
	draw_panel_scene_tree(editor);
	draw_panel_log(editor);
	draw_panel_asset_browser(editor);
	draw_panel_viewports(editor);
	draw_panel_render_stats(editor);
	draw_editor_command_undo_and_redo_stack(editor);
	draw_panel_asset_details(editor);
	


	process_editor_command_buffer(editor);

	ImGui::PopFont();

}

void editor_post_update(EditorInterface* editor) {
	EntityManager* entity_manager = editor->api.entity_manager;

	for (int i = 0; i < entity_manager->entity_count; i++) {
		Entity e = entity_manager->entity_list[i];
		MapResult<bool> result = map_get(&editor->entity_selected, e.id);
		if (!result.found) continue;
		if (result.value) {
			draw_gizmo_for_entity_with_camera(editor, editor->viewports.scene, e);
		}
	}

	
}

static bool is_entity_selected(EditorInterface* editor, Entity entity) {
	bool selected = map_get(&editor->entity_selected, entity.id, false);
	return selected;
}

static void draw_main_menu_bar(EditorInterface* editor) {
	// Menu bar
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New Project", "Ctrl+N")) { /* Do stuff */ }
			if (ImGui::MenuItem("Open Project", "Ctrl+O")) {}
			if (ImGui::MenuItem("Build", "Ctrl+B")) { /* Do stuff */ }
			if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
			if (ImGui::MenuItem("Exit")) {
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Scene")) {
			if (ImGui::MenuItem("Create Empty Entity")) {
				cmd_editor_create_emtpy_entity(editor);
			}
			if (ImGui::MenuItem("Create Camera")) {
				cmd_editor_create_camera(editor);
			}
			if (ImGui::MenuItem("Create Light")) {
				cmd_editor_create_light(editor);
			}
			if (ImGui::MenuItem("Create Plane")) {
				cmd_editor_create_plane(editor);
			}
			if (ImGui::MenuItem("Create Cube")) {
				cmd_editor_create_cube(editor);
			}
			if (ImGui::MenuItem("Create Sphere")) {
				cmd_editor_create_sphere(editor);
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Assets")) {

			if (ImGui::MenuItem("Import")) {}
			if (ImGui::MenuItem("Create Material")) {}
			if (ImGui::MenuItem("Create Folder")) {}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window")) {

			
			
			if (ImGui::BeginMenu("Layout")) {
				ImGui::MenuItem("Default");
				ImGui::MenuItem("4 Panel");

				ImGui::EndMenu();
			}
			ImGui::MenuItem("Scene Tree", NULL, &editor->panel_scenetree.panel_open);
			ImGui::MenuItem("Entity Components", NULL, &editor->panel_components.panel_open);
			ImGui::MenuItem("Log", NULL, &editor->panel_log.panel_open);
			ImGui::MenuItem("Asset Browser", NULL, &editor->panel_asset_browser.panel_open);
			ImGui::MenuItem("Render Stats", NULL, &editor->panel_render_stats.panel_open);
			
			
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

}

static void draw_toolbar(EditorInterface* editor) {
	bool open = true;
	auto flags = ImGuiWindowFlags_NoResize  | ImGuiWindowFlags_NoTitleBar;
	
	if (ImGui::Begin("Toolbar", &open)) {
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2.0f, 7.0f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

		// Save button
		ImGui::Button(ICON_FA_SAVE); ImGui::SameLine();
		if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Save Project");}

		// Undo button
		if (ImGui::Button(ICON_FA_UNDO)) { perform_undo_operation(editor);}
		if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Undo"); }
		ImGui::SameLine();

		// Redo button
		if (ImGui::Button(ICON_FA_REDO)) { perform_redo_operation(editor);}
		if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Redo"); }

		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::VerticalSeparator();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();

		
		// Translate button
		if (ImGui::Button(ICON_FA_ARROWS_ALT)) {
			editor->editor_control_data.gizmo_operation = ImGuizmo::TRANSLATE;
		}
		ImGui::SameLine();
		if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Translate"); }

		// Rotate button
		if (ImGui::Button(ICON_FA_SYNC_ALT)) {
			editor->editor_control_data.gizmo_operation = ImGuizmo::ROTATE;
		}
		ImGui::SameLine();
		if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Rotate"); }

		// Scale button
		if (ImGui::Button(ICON_FA_EXPAND_ARROWS_ALT)) {
			editor->editor_control_data.gizmo_operation = ImGuizmo::SCALE;
		}
		ImGui::SameLine();
		if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Scale"); }


		if (editor->editor_control_data.gizmo_mode == ImGuizmo::LOCAL) {
			if (ImGui::Button("Set to World")) {
				editor->editor_control_data.gizmo_mode = ImGuizmo::WORLD;
			}
			if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Set the transform to world space"); }
		} else {
			if (ImGui::Button("Set to Local")) {
				editor->editor_control_data.gizmo_mode = ImGuizmo::LOCAL;
			}
			if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Set the transform to local space"); }
		}

		ImGui::SameLine();
		
		if (ImGui::Button("Snap Settings")) {
			ImGui::OpenPopup("Snap Settings Popup");

			
		}

		if (ImGui::BeginPopup("Snap Settings Popup", NULL)) {
			ImGui::Checkbox("Snap", &editor->editor_control_data.use_gizmo_snapping);
			ImGui::InputFloat3("Translate Snap", editor->editor_control_data.snap_translate.data, 1);
			ImGui::InputFloat("Rotate Snap", &editor->editor_control_data.snap_rotate, 1);
			ImGui::InputFloat3("Scale Snap", editor->editor_control_data.snap_scale.data, 1);
			ImGui::EndPopup();
		}


		ImGui::SameLine();
		

		
		

		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::VerticalSeparator();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();

		// Play button
		ImGui::Button(ICON_FA_PLAY); ImGui::SameLine();
		if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Play Game"); }
		// Pause button
		ImGui::Button(ICON_FA_PAUSE); ImGui::SameLine();
		if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Pause Game"); }


		ImGui::PopStyleColor(1);
		ImGui::PopStyleVar(2);
	}
	ImGui::End();
}

static AssetBrowserFileNode* try_find_child_node_already_created_in_parent_node(AssetBrowserFileNode* parent, String token) {

	// If the parent has no children then there are no nodes to find
	if (parent->first_child != NULL) {
		AssetBrowserFileNode* child_node = parent->first_child;
		while (child_node != NULL) {
			if (strncmp(token.buffer, child_node->name.buffer, token.length) == 0) {
				return child_node;
			}
			child_node = child_node->next_sibling;
		}
	}
	return NULL;
}

static RenderResource get_asset_browser_thumbnail_for_asset(EditorInterface* editor, AssetID id) {
	if (id.id == 0) {
		// No thumnbail for non assets
		return editor->panel_asset_browser.res_asset_icon_texture;
	}
	MapResult<RenderResource> result = map_get(&editor->panel_asset_browser.asset_thumbnail_cache, id.id);
	if (result.found) {
		return result.value;
	}

	InternalAsset internal_asset = get_internal_asset_by_id(editor->api.asset_manager, id);
	switch (id.type) {
		case AssetType::Texture: {
			Texture2D* txt = internal_asset.texture;
			RenderResource tex_resource = create_texture_resource(editor->api.renderer, txt, false, false);
			map_put(&editor->panel_asset_browser.asset_thumbnail_cache, id.id, tex_resource);
			return tex_resource;
		}
		default: {
			return editor->panel_asset_browser.res_asset_icon_texture;
		}
								 
	}
	
}

static void construct_asset_brower_tree(EditorInterface* editor) {
	
	Texture2D folder_texture;
	load_texture("editor_resources/thumbnails/Folder-Icon.png", &folder_texture, &editor->stack, false);
	editor->panel_asset_browser.res_folder_icon_texture = create_texture_resource(editor->api.renderer, &folder_texture, false);
	stack_pop(&editor->stack);

	Texture2D default_asset_icon;
	load_texture("editor_resources/thumbnails/Asset-Icon.png", &default_asset_icon, &editor->stack, false);
	editor->panel_asset_browser.res_asset_icon_texture = create_texture_resource(editor->api.renderer, &default_asset_icon, false);
	stack_pop(&editor->stack);


	AssetTracker* tracker = &editor->api.asset_manager->asset_tracker;
	size_t map_size = tracker->track_map.size;
	editor->panel_asset_browser.root = (AssetBrowserFileNode*)arena_alloc(&tracker->mem, sizeof(AssetBrowserFileNode));

	
	const char* root_name_str = "Root";
	size_t root_name_str_length = 4;
	// Copy name
	char* root_name = (char*)arena_alloc(&tracker->mem, root_name_str_length + 1); // +1 for '\0' terminator
	memcpy(root_name, root_name_str, root_name_str_length);
	root_name[root_name_str_length + 1] = '\0';

	
	editor->panel_asset_browser.root->node_type = AssetBrowserFileNode::Type::Directory;
	editor->panel_asset_browser.root->asset = AssetID();
	editor->panel_asset_browser.root->name = String(root_name, root_name_str_length);
	editor->panel_asset_browser.root->children_count = 0;
	editor->panel_asset_browser.root->parent = NULL;
	editor->panel_asset_browser.root->first_child = NULL;
	editor->panel_asset_browser.root->next_sibling = NULL;
	editor->panel_asset_browser.root->has_child_directorys = false;

	for (size_t i = 0; i < map_size; i++) {
		CompactMapItem<AssetTrackData> track_item = tracker->track_map.map[i];

		if (track_item.key != 0 && track_item.key != TOMBSTONE) {
			String fullpath = track_item.value.file;


			AssetBrowserFileNode* parent_node = editor->panel_asset_browser.root;

			char* data = (char*)fullpath.buffer;
			char* next;
			char* curr = data;


			while ((next = strchr(curr, '\\')) != NULL) {


				size_t len = next - curr;
				String token(curr, len);
				AssetBrowserFileNode* dir_node;

				AssetBrowserFileNode* tryfind_dir_node = try_find_child_node_already_created_in_parent_node(parent_node, token);
				if (tryfind_dir_node != NULL) {
					dir_node = tryfind_dir_node;
				} else {
					dir_node = (AssetBrowserFileNode*)arena_alloc(&tracker->mem, sizeof(AssetBrowserFileNode));

					if (*next == '\\') {
						dir_node->node_type = AssetBrowserFileNode::Type::Directory;
						dir_node->asset = AssetID();
					} else {
						assert(false && "This is not a directory, but shouldnt get here");
					}

					// Copy name
					char* name = (char*)arena_alloc(&tracker->mem, token.length + 1); // +1 for '\0' terminator
					memcpy(name, token.buffer, token.length);
					name[token.length] = '\0';
					dir_node->name = String(name, token.length);

					dir_node->parent = parent_node;
					dir_node->children_count = 0;
					dir_node->has_child_directorys = false;
					parent_node->children_count++;

					parent_node->has_child_directorys = true;
					// Attach this child node to the parent
					// If there is no parent first child, then we assign this node to the first_child
					if (parent_node->first_child == NULL) {
						parent_node->first_child = dir_node;

					} else {
						// If the parent does have a first child, then we need to assign this node to the right most next sibling
						AssetBrowserFileNode* child_node = parent_node->first_child;
						while (child_node->next_sibling != NULL) {
							child_node = child_node->next_sibling;
						}
						child_node->next_sibling = dir_node;
					}
				}




				parent_node = dir_node;

				curr = next + 1;
			}
			AssetBrowserFileNode* file_node = (AssetBrowserFileNode*)arena_alloc(&tracker->mem, sizeof(AssetBrowserFileNode));
			file_node->node_type = AssetBrowserFileNode::Type::File;

			// Copy name
			String token = String(curr, strlen(curr));
			char* name = (char*)arena_alloc(&tracker->mem, token.length + 1);
			memcpy(name, token.buffer, token.length);
			name[token.length] = '\0';
			file_node->name = String(name, token.length);
			file_node->asset = track_item.value.assetid;
			file_node->children_count = 0;
			file_node->parent = parent_node;
			file_node->first_child = NULL;
			file_node->next_sibling = NULL;
			parent_node->children_count++;

			if (parent_node->first_child == NULL) {
				parent_node->first_child = file_node;
			} else {
				// If the parent does have a first child, then we need to assign this node to the right most next sibling
				AssetBrowserFileNode* child_node = parent_node->first_child;
				while (child_node->next_sibling != NULL) {
					child_node = child_node->next_sibling;
				}
				child_node->next_sibling = file_node;
			}






		}
	}

	editor->panel_asset_browser.current_directory = editor->panel_asset_browser.root;

}

static void draw_component_transform(EditorInterface* editor, Entity e) {
	EntityManager* entity_manager = editor->api.entity_manager;
	
	ImGui::PushID("transform_component");
	
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
	if (ImGui::CollapsingHeader(ICON_FA_ASTERISK " Transform", 0, ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
		Vec3f old_pos = get_position(entity_manager, e);
		Vec3f new_pos = old_pos;

		Quat old_rot = get_rotation(entity_manager, e);
		Vec3f euler = quat_to_euler(old_rot);
		Quat new_rot = old_rot;

		Vec3f old_scale = get_scale(entity_manager, e);
		Vec3f new_scale = old_scale;

		
		
		

		ImGui::PushID("pos_default");
		if (ImGui::Button("z")) {
			cmd_edtior_set_transform_component(editor, e, old_pos, old_rot, old_scale, Vec3f(0, 0, 0), old_rot, old_scale, true);
		}
		ImGui::SameLine();

		
		
		if (ImGui::DragFloat3("Position", new_pos.data, 1.0f)) {
			// No Command is sent here, this is just to show that the objects updates as we drag
			// Gives good instant visual feedback
			set_position(entity_manager, e, new_pos);

			// Merge this command with previous command. Update with new pos,rot,scale
			cmd_edtior_set_transform_component(editor, e, old_pos, old_rot, old_scale, new_pos, old_rot, old_scale, true);
		}
		

		if (ImGui::IsItemDeactivatedAfterEdit()) {
			// Send this command when we are finished dragging
			// Once we are done dragging we want to a force a barrier between future drags,
			// so that multiple drags dont merge into one command
			cmd_editor_force_no_merge(editor);
		}
		
		
		
		ImGui::PopID();
		
		ImGui::PushID("rot_default");

		if (ImGui::Button("z")) {
			cmd_edtior_set_transform_component(editor, e, old_pos, old_rot, old_scale, old_pos, Quat(), old_scale, false);
		}
		ImGui::SameLine();
		if (ImGui::DragFloat3("Rotation", euler.data, 1.0f)) {
			// No Command is sent here, this is just to show that the objects updates as we drag
			// Gives good instant visual feedback
			new_rot = euler_to_quat(euler);
			set_rotation(entity_manager, e, new_rot);			
			// Merge this command with previous command. Update with new pos,rot,scale
			cmd_edtior_set_transform_component(editor, e, old_pos, old_rot, old_scale, old_pos, new_rot, old_scale, true);
		}

	

		if (ImGui::IsItemDeactivatedAfterEdit()) {
			// Send this command when we are finished dragging
			// Once we are done dragging we want to a force a barrier between future drags,
			// so that multiple drags dont merge into one command
			cmd_editor_force_no_merge(editor);
		}

		

		ImGui::PopID();


		ImGui::PushID("scale_default");
		
		if (ImGui::Button("z")) {
			cmd_edtior_set_transform_component(editor, e, old_pos, old_rot, old_scale, old_pos, old_rot, Vec3f(1, 1, 1), false);
		}
		ImGui::SameLine();
		if (ImGui::DragFloat3("Scale", new_scale.data, 1.0f)) {
			// No Command is sent here, this is just to show that the objects updates as we drag
			// Gives good instant visual feedback
			set_scale(entity_manager, e, new_scale);
			// Merge this command with previous command. Update with new pos,rot,scale
			cmd_edtior_set_transform_component(editor, e, old_pos, old_rot, old_scale, old_pos, old_rot, new_scale, true);
		}


		if (ImGui::IsItemDeactivatedAfterEdit()) {
			// Send this command when we are finished dragging
			// Once we are done dragging we want to a force a barrier between future drags,
			// so that multiple drags dont merge into one command
			cmd_editor_force_no_merge(editor);
		}
		ImGui::PopStyleVar(1);
		ImGui::PopID();	
	}
	ImGui::PopStyleVar(1);

	ImGui::PopID();
}

static void draw_component_camera(EditorInterface* editor, Entity e) {
	EntityManager* entity_manager = editor->api.entity_manager;

	ImGui::PushID("camera_component");
	bool open = true;
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
	if (ImGui::CollapsingHeader(ICON_FA_VIDEO " Camera", &open, ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
		bool comp_enabled = true;
		ImGui::Checkbox("Enabled", &comp_enabled);
			
		
		
		const Camera* const old_cam = get_camera(entity_manager, e);
		Camera new_camera = *old_cam;

		ImGui::Text("Aspect Ratio: %f", old_cam->aspect_ratio);

		int projection = (int)old_cam->projection;
		if (ImGui::Combo("Projection", &projection, "Perspective\0Orthographic\0\0")) {
			new_camera.projection = (CameraProjection)projection;
			cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, false);
			cmd_editor_force_no_merge(editor);
		}



		ImGui::PushID("camera_near_default");
		if (ImGui::SmallButton("z")) {
			new_camera.near_clip = 0.01f;
			cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);
		}
		ImGui::SameLine();
		if (ImGui::DragFloat("Near", &new_camera.near_clip, 1.0f, 0.0f, FLT_MAX)) {
			cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);
		}

		if (ImGui::IsItemDeactivatedAfterEdit()) {
			// Send this command when we are finished dragging
			// Once we are done dragging we want to a force a barrier between future drags,
			// so that multiple drags dont merge into one command
			cmd_editor_force_no_merge(editor);
		}

		ImGui::PopID();

		ImGui::PushID("camera_far_default");
		if (ImGui::SmallButton("z")) {
			new_camera.far_clip = 100.0f;
			cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);
		}
		ImGui::SameLine();

		if (ImGui::DragFloat("Far", &new_camera.far_clip, 1.0f, 0.0f, FLT_MAX)) {
			cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);
		}

		if (ImGui::IsItemDeactivatedAfterEdit()) {
			// Send this command when we are finished dragging
			// Once we are done dragging we want to a force a barrier between future drags,
			// so that multiple drags dont merge into one command
			cmd_editor_force_no_merge(editor);
		}

		ImGui::PopID();

		if (old_cam->projection == CameraProjection::Perspective) {
			ImGui::PushID("camera_fov_default");
			if (ImGui::SmallButton("z")) {
				new_camera.fov = 90.0f;
				cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);

			}
			ImGui::SameLine();
			if (ImGui::SliderFloat("Field Of View", &new_camera.fov, 30.0f, 120.0f)) {
				cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);
			}
			if (ImGui::IsItemDeactivatedAfterEdit()) {
				// Send this command when we are finished dragging
				// Once we are done dragging we want to a force a barrier between future drags,
				// so that multiple drags dont merge into one command
				cmd_editor_force_no_merge(editor);
			}
			ImGui::PopID();
		}

		if (old_cam->projection == CameraProjection::Orthographic) {

			//ImGui::Separator();
			//ImGui::Text("Orthographic Settings");

			
			if (ImGui::DragFloat("Size", &new_camera.size, 1.0f, 1.0f, FLT_MAX)) {
				cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);
			}

			if (ImGui::IsItemDeactivatedAfterEdit()) {
				// Send this command when we are finished dragging
				// Once we are done dragging we want to a force a barrier between future drags,
				// so that multiple drags dont merge into one command
				cmd_editor_force_no_merge(editor);
			}
		}

		set_camera(entity_manager, e, new_camera);
		ImGui::PopStyleVar(1);
	}
	if (!open) {
		remove_component(entity_manager, e, ComponentType::Camera);
	}
	ImGui::PopStyleVar(1);
	ImGui::PopID();
}

static void draw_component_light(EditorInterface* editor, Entity e) {
	EntityManager* entity_manager = editor->api.entity_manager;
	ImGui::PushID("light_component");
	bool open = true;
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
	if (ImGui::CollapsingHeader(ICON_FA_LIGHTBULB " Light", &open, ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
		bool comp_enabled = is_component_enabled(entity_manager, e, ComponentType::Light);
		ImGui::PushID("light_enabled");
		if (ImGui::Checkbox("Enabled", &comp_enabled)) {
			enable_component(editor->api.entity_manager, e, ComponentType::Light, comp_enabled);
		}
		ImGui::PopID();
			
		
		
		

		const Light old_light =  get_light(entity_manager, e);
		Light new_light = old_light;
		int light_type = (int)old_light.type;
		if (ImGui::Combo("Type", &light_type, "None\0Directional Light\0Point Light\0\0")) {
			new_light.type = (LightType)light_type;
			cmd_editor_set_light_component(editor, e, old_light, new_light, true);
		}

		switch (old_light.type) {
			case LightType::DirectionalLight: {


				ImGui::PushID("dir_direction_default");
				if (ImGui::Button("z")) {
					new_light.dir_light.direction = Vec3f(0, 0, 0);
					cmd_editor_set_light_component(editor, e, old_light, new_light, true);
				}
				ImGui::SameLine();
				if (ImGui::DragFloat3("Direction", new_light.dir_light.direction.data)) {
					cmd_editor_set_light_component(editor, e, old_light, new_light, true);
				}

				if (ImGui::IsItemDeactivatedAfterEdit()) {
					// Send this command when we are finished dragging
					// Once we are done dragging we want to a force a barrier between future drags,
					// so that multiple drags dont merge into one command
					cmd_editor_force_no_merge(editor);
				}

				ImGui::PopID();

				ImGui::PushID("dir_color_default");
				if (ImGui::Button("z")) {
					new_light.dir_light.color = Vec3f(1, 1, 1);
					cmd_editor_set_light_component(editor, e, old_light, new_light, true);
				}

				ImGui::SameLine();
				if (ImGui::ColorEdit3("Color", new_light.dir_light.color.data)) {
					cmd_editor_set_light_component(editor, e, old_light, new_light, true);
				}

				if (ImGui::IsItemDeactivatedAfterEdit()) {
					// Send this command when we are finished dragging
					// Once we are done dragging we want to a force a barrier between future drags,
					// so that multiple drags dont merge into one command
					cmd_editor_force_no_merge(editor);
				}
				ImGui::PopID();

				

				ImGui::PushID("dir_intensity_default");
				if (ImGui::Button("z")) {
					new_light.dir_light.intensity = 0;
					cmd_editor_set_light_component(editor, e, old_light, new_light, true);
				}
				ImGui::SameLine();

				if (ImGui::DragFloat("Intensity", &new_light.dir_light.intensity, 1.0f, 0.0f, 100000.0f)) {
					cmd_editor_set_light_component(editor, e, old_light, new_light, true);
				}

				if (ImGui::IsItemDeactivatedAfterEdit()) {
					// Send this command when we are finished dragging
					// Once we are done dragging we want to a force a barrier between future drags,
					// so that multiple drags dont merge into one command
					cmd_editor_force_no_merge(editor);
				}
				ImGui::PopID();

				

				break;
			}
			case LightType::PointLight: {
				ImGui::PushID("point_color_default");
				if (ImGui::SmallButton("z")) {
					new_light.point_light.color = Vec3f(1,1,1);
					cmd_editor_set_light_component(editor, e, old_light, new_light, true);
				}
				ImGui::SameLine();
				if (ImGui::ColorEdit3("Color", new_light.point_light.color.data)) {
					cmd_editor_set_light_component(editor, e, old_light, new_light, true);
				}
				if (ImGui::IsItemDeactivatedAfterEdit()) {
					// Send this command when we are finished dragging
					// Once we are done dragging we want to a force a barrier between future drags,
					// so that multiple drags dont merge into one command
					cmd_editor_force_no_merge(editor);
				}
				ImGui::PopID();
				break;
			}
		}

		set_light(entity_manager, e, new_light);
		
		ImGui::PopStyleVar(1);
	}

	if (!open) {
		remove_component(entity_manager, e, ComponentType::Light);
	}
	ImGui::PopStyleVar(1);
	ImGui::PopID();
}

static void dragdrop_target_asset(EditorInterface* editor, Entity e, AssetType accept_type) {

	if (ImGui::BeginDragDropTarget())
	{
		EntityManager* em = editor->api.entity_manager;
		if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("ASSETID"))
		{
			AssetID asset = *(AssetID*)payload->Data;
			// Only accept assets that we want to accept
			if (asset.type == accept_type) {
				
				if (asset.type == AssetType::Material) {

					if (has_component(em, e, ComponentType::Render)) {
						MaterialID old_mat = get_render_material(em, e);
						cmd_editor_set_material_component(editor, e, old_mat, asset.material);
					}
					
				}
				if (asset.type == AssetType::StaticMesh) {
					if (has_component(em, e, ComponentType::StaticMesh)) {
							StaticMeshID old_mesh = get_static_mesh(em, e);
							cmd_editor_set_staticmesh_component(editor, e, old_mesh, asset.mesh);;
					}
					
				}
			}
			

		}
		ImGui::EndDragDropTarget();
	}
}

static void draw_component_static_mesh(EditorInterface* editor, Entity e) {
	EntityManager* entity_manager = editor->api.entity_manager;
	ImGui::PushID("mesh_component");
	bool open = true;
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
	if (ImGui::CollapsingHeader(ICON_FA_TH_LARGE " Static Mesh", &open, ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
		StaticMeshID mesh_id = get_static_mesh(entity_manager, e);
		AssetID assetid;
		assetid.mesh = mesh_id;

		if (mesh_id.id == 0) {
			char buffer[1];
			ImGui::InputText("Mesh", buffer, sizeof(buffer), ImGuiInputTextFlags_ReadOnly);
		}
		else {
			String name = name_of_asset(&editor->api.asset_manager->asset_tracker, assetid);
			//ImGuiInputTextFlags_ReadOnly
			ImGui::InputText("Mesh", (char*)name.buffer, name.length, ImGuiInputTextFlags_ReadOnly);
			//ImGui::Text(name.buffer);
		}
		dragdrop_target_asset(editor, e, AssetType::StaticMesh);


		ImGui::SameLine();
		if (ImGui::SmallButton("...")) {
			ImGui::OpenPopup("Select Mesh");
		}
		
		
		ImGui::SetNextWindowPosCenter(ImGuiCond_Always);
		if (ImGui::BeginPopup("Select Mesh", NULL))
		{
			
			AssetTracker* tracker = &editor->api.asset_manager->asset_tracker;
			size_t map_size = tracker->track_map.size;
			// Go over our track map, and look for filename
			for (size_t i = 0; i < map_size; i++) {
				CompactMapItem<AssetTrackData> track_item = tracker->track_map.map[i];
				// Check if this is a valid track data
				if (track_item.key != 0 && track_item.key != TOMBSTONE) {

					// Only show Static mesh assets
					if (track_item.value.assetid.type == AssetType::StaticMesh) {
						if (ImGui::Button(track_item.value.file.buffer)) {
							cmd_editor_set_staticmesh_component(editor, e, mesh_id, track_item.value.assetid.mesh);
							
						}
					}
				}
			}

			
			if (ImGui::Button("Close")) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		ImGui::PopStyleVar(1);
	}
	if (!open) {
		remove_component(entity_manager, e, ComponentType::StaticMesh);
	}
	ImGui::PopStyleVar(1);
	ImGui::PopID();
}

static void draw_component_render(EditorInterface* editor, Entity e) {
	EntityManager* entity_manager = editor->api.entity_manager;
	ImGui::PushID("render_component");
	bool open = true;
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
	if (ImGui::CollapsingHeader(ICON_FA_TH_LARGE " Render", &open, ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
		bool comp_enabled = is_component_enabled(entity_manager, e, ComponentType::Render);
		ImGui::PushID("render_enabled");
		if (ImGui::Checkbox("Enabled", &comp_enabled)) {
			enable_component(editor->api.entity_manager, e, ComponentType::Render, comp_enabled);
		}
		ImGui::PopID();



		MaterialID mat_id = get_render_material(entity_manager, e);
		AssetID assetid;
		assetid.material = mat_id;


		if (mat_id.id == 0) {
			char buffer[1];
			ImGui::InputText("Material", buffer, sizeof(buffer), ImGuiInputTextFlags_ReadOnly);
		}
		else {
			String name = name_of_asset(&editor->api.asset_manager->asset_tracker, assetid);
			//ImGuiInputTextFlags_ReadOnly
			ImGui::InputText("Material", (char*)name.buffer, name.length, ImGuiInputTextFlags_ReadOnly);
			//ImGui::Text(name.buffer);
		}

		dragdrop_target_asset(editor, e, AssetType::Material);

		

		ImGui::SameLine();
		if (ImGui::SmallButton("...")) {
			ImGui::OpenPopup("Select Material");
		}

		ImGui::SetNextWindowPosCenter(ImGuiCond_Always);
		if (ImGui::BeginPopup("Select Material", 0))
		{

			AssetTracker* tracker = &editor->api.asset_manager->asset_tracker;
			size_t map_size = tracker->track_map.size;
			// Go over our track map, and look for filename
			for (size_t i = 0; i < map_size; i++) {
				CompactMapItem<AssetTrackData> track_item = tracker->track_map.map[i];
				// Check if this is a valid track data
				if (track_item.key != 0 && track_item.key != TOMBSTONE) {

					if (track_item.value.assetid.type == AssetType::Material) {
						if (ImGui::Button(track_item.value.file.buffer)) {
							cmd_editor_set_material_component(editor, e, mat_id, track_item.value.assetid.material);
						}
					}
				}
			}


			if (ImGui::Button("Close")) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		ImGui::PopStyleVar(1);
	}

	if (!open) {
		remove_component(entity_manager, e, ComponentType::Render);
	}
	ImGui::PopStyleVar(1);
	ImGui::PopID();
}

static void draw_panel_components(EditorInterface* editor) {
	EntityManager* entity_manager = editor->api.entity_manager;

	if (!editor->panel_components.panel_open) {
		return;
	}

	if (ImGui::Begin(ICON_FA_LIST " Entity Components", &editor->panel_components.panel_open)) {
		
	//colors[ImGuiCol_Header] = dark_accent;
	//colors[ImGuiCol_HeaderHovered] = light_accent;
	//colors[ImGuiCol_HeaderActive] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);

		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0, 0, 0, 0));

		for (int i = 0; i < entity_manager->entity_count; i++) {
			Entity e = entity_manager->entity_list[i];
			MapResult<bool> result = map_get(&editor->entity_selected, e.id);
			if (!result.found) continue;
			if (result.value) {
				ImGui::PushID(e.id);
				String e_name = get_name(&entity_manager->meta_manager, e);



				//ImGui::InputText("", e_name.buffer, e_name.length);
				//ImGui::SameLine();
				ImGui::Text(e_name.buffer);
				
				//ImGui::SameLine();


				
				ComponentType comp_types[] = {
					ComponentType::Transform,
					ComponentType::Camera,
					ComponentType::StaticMesh,
					ComponentType::Light,
					ComponentType::Render
				};

				const char* comp_names[] = { "Transform", "Camera", "StaticMesh", "Light", "Render" };
				

			

				bool enabled = 1;
				if (ImGui::Checkbox("Enabled", &enabled)) {
					
				}
				ImGui::SameLine();
				if (ImGui::Button("Add Component")) {
					ImGui::OpenPopup("component_popup");
				}
				ImGui::SameLine();
				if (ImGui::BeginPopup("component_popup")) {
					
					for (int i = 0; i < IM_ARRAYSIZE(comp_names); i++)
						if (ImGui::Selectable(comp_names[i])) {
							
							add_component(entity_manager, e, comp_types[i]);
						}
							
					ImGui::EndPopup();
				}
				ImGui::NewLine();
				ImGui::Separator();
				if (has_component(entity_manager, e, ComponentType::Transform)) {
					draw_component_transform(editor, e);
					ImGui::Separator();
				}
				

				if (has_component(entity_manager, e, ComponentType::Camera)) {
					draw_component_camera(editor, e);
					ImGui::Separator();
				}
				
				if (has_component(entity_manager, e, ComponentType::Light)) {
					draw_component_light(editor, e);
					ImGui::Separator();
				}
				
				if (has_component(entity_manager, e, ComponentType::StaticMesh)) {
					draw_component_static_mesh(editor, e);
					ImGui::Separator();
				}
				
				if (has_component(entity_manager, e, ComponentType::Render)) {
					draw_component_render(editor, e);
					ImGui::Separator();
				}
				
				ImGui::PopID();




				ImGui::Spacing();

			}





		}
		ImGui::PopStyleColor(3);
	}
	ImGui::End();

	//if (ImGui::Begin("Render Components")) {
	//	RenderManager* rm = &entity_manager->render_manager;
	//	ImGui::Text("Capacity %d", rm->total_count);
	//	ImGui::Text("Enabled %d", rm->enabled_count);
	//
	//	for (u64 i = 0; i < rm->enabled_count; i++) {
	//		ImGui::Text("%d ", rm->entitys[i].id);
	//		ImGui::SameLine();
	//	}
	//	ImGui::NewLine();
	//	for (u64 i = 0; i < rm->total_count; i++) {
	//		ImGui::Text("%d ", rm->entitys[i].id);
	//		ImGui::SameLine();
	//	}
	//
	//	ImGui::NewLine();
	//	size_t map_size = rm->id_map.size;
	//	// Go over our track map, and look for filename
	//	for (int i = 0; i < map_size; i++) {
	//		CompactMapItem<u64> item = rm->id_map.map[i];
	//		
	//		// Check if this is a valid track data
	//		if (item.key!= 0 && item.key != TOMBSTONE) {
	//			ImGui::Text("%llu -> %llu", item.key, item.value);
	//		}
	//	}
	//
	//}
	//ImGui::End();
		

}

static void draw_entity_item_context_menu(EditorInterface* editor, Entity e) {
	if (ImGui::BeginPopupContextItem("Entity Item Context Menu")) {


		if (ImGui::Selectable("Copy")) {

		}
		if (ImGui::Selectable("Paste")) {

		}

		if (ImGui::Selectable("Duplicate")) {

		}
		if (ImGui::Selectable("Rename")) {

		}
		ImGui::Separator();
		if (ImGui::Selectable("Delete")) {
			EditorCommand command;
			command.type = EditorCommandType::DELETE_ENTITY;
			command.cmd.delete_entity.entity_to_delete = e;
			push_editor_command(editor, command);
		}


		ImGui::EndPopup();
	}
}

static void dragdrop_source_entity_tree(EditorInterface* editor, Entity e) {
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			ImGui::SetDragDropPayload("ENTITY_DRAGDROP_REPARENT", &e.id, sizeof(e.id));
			String name = get_name(&editor->api.entity_manager->meta_manager, e);
			ImGui::Text(name.buffer);
			ImGui::EndDragDropSource();
		}
}

static void dragdrop_target_entity_tree(EditorInterface* editor, Entity e) {

	if (ImGui::BeginDragDropTarget())
	{
		EntityManager* em = editor->api.entity_manager;
		if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("ENTITY_DRAGDROP_REPARENT"))
		{
			cmd_editor_group_begin(editor);
			// First reparent the entity that was dragged
			u64 payload_entity_id = *(u64*)payload->Data;
			Entity payload_entity(payload_entity_id);
			Entity old_parent_of_payload_entity = parent(em, payload_entity);
			cmd_editor_reparent_entity(editor, payload_entity, old_parent_of_payload_entity, e);

			// Then drag the remaining selected entities
			if (editor->entity_selected_count > 0) {
				for (int i = 0; i < em->entity_count; i++) {
					Entity selected_entity = em->entity_list[i];
					// Only deselect the ones that are selected
					if (is_entity_selected(editor, selected_entity)) {
						Entity old_parent = parent(em, selected_entity);
						cmd_editor_reparent_entity(editor, selected_entity, old_parent, e);
					}
				}
			}
			cmd_editor_group_end(editor);

		}
		ImGui::EndDragDropTarget();
	}
}

static void draw_entity_tree(EditorInterface* editor, Entity e) {

	//if (editor->editor_camera == e) {
	//	// Dont draw the editor camera in the tree
	//	return;
	//}


	bool entity_selected = is_entity_selected(editor, e);

	EntityManager* em = editor->api.entity_manager;
	// We need to recursivly draw the children
	Entity child = first_child(em, e);


	String name = get_name(&em->meta_manager, e);
	
	

	ImGui::PushID(e.id);
	if (child.id == NO_ENTITY_ID) {
		// This is a leaf node
		ImGui::Indent();
		if (ImGui::Selectable(name.buffer, entity_selected)) {
			cmd_editor_group_begin(editor);
			if (!ImGui::GetIO().KeyCtrl) {
				cmd_editor_deselect_all_entitys(editor);
			}
			cmd_editor_select_entity(editor, e.id, !entity_selected);
			cmd_editor_group_end(editor);
		}
		dragdrop_source_entity_tree(editor, e);
		dragdrop_target_entity_tree(editor, e);
		dragdrop_target_asset(editor, e, AssetType::StaticMesh);
		dragdrop_target_asset(editor, e, AssetType::Material);

		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("EntityID %llu", e.id);
		}
		draw_entity_item_context_menu(editor, e);

		

		ImGui::Unindent();
	} else {


		// This is a node that has children

		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ((entity_selected) ? ImGuiTreeNodeFlags_Selected : 0);
		
		

		bool node_open = ImGui::TreeNodeEx(name.buffer, node_flags);

		
		dragdrop_source_entity_tree(editor, e);
		dragdrop_target_entity_tree(editor, e);
		dragdrop_target_asset(editor, e, AssetType::StaticMesh);
		dragdrop_target_asset(editor, e, AssetType::Material);


		if (ImGui::IsItemClicked() && (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) > ImGui::GetTreeNodeToLabelSpacing())
		{
			// The item is clicked
			cmd_editor_group_begin(editor);
			if (!ImGui::GetIO().KeyCtrl) {
				// Clear selection when CTRL is not held
				cmd_editor_deselect_all_entitys(editor);
			}
			cmd_editor_select_entity(editor, e.id, !entity_selected);
			cmd_editor_group_end(editor);
		}
		else {
			// Arrow is clicked
		}

		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("EntityID %llu", e.id);
		}

		draw_entity_item_context_menu(editor, e);

		
		if (node_open) {
			while (child.id != NO_ENTITY_ID) {
				draw_entity_tree(editor, child);
				child = next_sibling(em, child);
			}
			ImGui::TreePop();
		}


	}
	ImGui::PopID();


}

static void draw_panel_scene_tree(EditorInterface* editor) {
	if (!editor->panel_scenetree.panel_open) {
		return;
	}
	EntityManager* entity_manager = editor->api.entity_manager;
	if (ImGui::Begin(ICON_FA_SITEMAP " Entity Scene Tree", &editor->panel_scenetree.panel_open)) {
		
		
		
		ImGui::Text("Total Entitys Created %llu", entity_manager->entitys_created);
		ImGui::Text("Current Entity Count %llu", entity_manager->entity_count);
		ImGui::Text("Entity Selected Count %llu", editor->entity_selected_count);
		ImGui::Separator();


		//static char str0[128] = "";
		//ImGui::Text("Filter Entitys");
		//ImGui::InputText("", str0, IM_ARRAYSIZE(str0));
		

		editor->panel_scenetree.scene_tree_entity_filter.Draw(ICON_FA_SEARCH);

		ImGui::Separator();

		if (editor->panel_scenetree.scene_tree_entity_filter.IsActive()) {
			// Draw as linear list when filtered
			u64 count = entity_manager->entity_count;
			for (int i = 0; i < count; i++) {
				Entity e = entity_manager->entity_list[i];
				String name = get_name(&editor->api.entity_manager->meta_manager, e);

				if (editor->panel_scenetree.scene_tree_entity_filter.PassFilter(name.buffer, name.buffer + name.length)) {
					bool entity_selected = is_entity_selected(editor, e);

					if (ImGui::Selectable(name.buffer, entity_selected)) {
						cmd_editor_group_begin(editor);
						if (!ImGui::GetIO().KeyCtrl) {
							cmd_editor_deselect_all_entitys(editor);
						}
						cmd_editor_select_entity(editor, e.id, !entity_selected);
						cmd_editor_group_end(editor);
					}
				}
				

			}

		}
		else {
			// Draw as tree when not filtered

			// Draw all the child entities of the root entity
			Entity root_child = first_child(entity_manager, entity_manager->root);
			while (root_child.id != NO_ENTITY_ID) {
				draw_entity_tree(editor, root_child);
				root_child = next_sibling(entity_manager, root_child);
			}
		}

		
		
		
		ImGui::BeginChild("scene_internal");
		
		// Allow dropping entitys on the root
		ImGui::Dummy(ImGui::GetWindowSize());
		dragdrop_target_entity_tree(editor, entity_manager->root);

		// Click in the scene hierarchy to deselect all the entities
		if (ImGui::IsMouseHoveringWindow() && ImGui::IsMouseClicked(0, false)) {
			cmd_editor_deselect_all_entitys(editor);
		}
		
		// Right click to  open the context menu
		if (ImGui::BeginPopupContextWindow()) {
			

			if (ImGui::MenuItem("Copy")) {
			
			}

			if (ImGui::MenuItem("Paste")) {
				
			}
			if (ImGui::MenuItem("Duplicate")) {

			}

			

			ImGui::Separator();

			if (ImGui::MenuItem("Create Empty Entity")) {
				cmd_editor_create_emtpy_entity(editor);
			}
			if (ImGui::MenuItem("Create Camera")) {
				cmd_editor_create_camera(editor);
			}
			if (ImGui::MenuItem("Create Light")) {
				cmd_editor_create_light(editor);
			}
			if (ImGui::MenuItem("Create Plane")) {
				cmd_editor_create_plane(editor);
			}
			if (ImGui::MenuItem("Create Cube")) {
				cmd_editor_create_cube(editor);
			}
			if (ImGui::MenuItem("Create Sphere")) {
				cmd_editor_create_sphere(editor);
			}
			ImGui::EndPopup();
		}

		ImGui::EndChild();
		

	}
	ImGui::End();

	
}


static void draw_panel_log(EditorInterface* editor) {

	PanelLog* panel_log = &editor->panel_log;
	if (!panel_log->panel_open) {
		return;
	}

	if (ImGui::Begin(ICON_FA_TERMINAL " Log", &panel_log->panel_open)) {

		LogList logs = g_get_loglist();

		panel_log->log_filter.Draw(ICON_FA_SEARCH " Filter");
		ImGui::SameLine();
		

		const float ItemSpacing = ImGui::GetStyle().ItemSpacing.x;
		static float HostButtonWidth = 100.0f; //The 100.0f is just a guess size for the first frame.
		float pos = HostButtonWidth + ItemSpacing;

		ImGui::SameLine(ImGui::GetWindowWidth() - pos);

		if (ImGui::Button("Clear")) {
			panel_log->log_start_offset = logs.log_count;
		}
		HostButtonWidth = ImGui::GetItemRectSize().x; //Get the actual width for next frame.

		ImGui::Separator();


		ImGui::Checkbox("Info", &panel_log->show_info);
		ImGui::SameLine();
		ImGui::Checkbox("Warning", &panel_log->show_warning);
		ImGui::SameLine();
		ImGui::Checkbox("Fatal", &panel_log->show_fatal);
		ImGui::SameLine();
		


		static const char* str_time = "Time";
		static const char* str_tag = "Tag";
		static const char* str_thread_id = "Thread Id";
		static const char* str_filename = "Filename";
		static const char* str_function = "Function";
		static const char* str_line = "Line";
		static const char* str_msg = "Message";
		
		
		

		const float ItemSpacing2 = ImGui::GetStyle().ItemSpacing.x;
		static float HostButtonWidth2 = 100.0f; //The 100.0f is just a guess size for the first frame.
		float pos2 = HostButtonWidth2 + ItemSpacing2;

		ImGui::SameLine(ImGui::GetWindowWidth() - pos2);

		if (ImGui::Button(ICON_FA_COG " Options")) {
			ImGui::OpenPopup("log_options_popup");
		}
		HostButtonWidth2 = ImGui::GetItemRectSize().x; //Get the actual width for next frame.

		

		
		
		

		if (ImGui::BeginPopup("log_options_popup"))
		{
			ImGui::Checkbox("Auto Scroll", &panel_log->auto_scroll);
			ImGui::Separator();
			ImGui::Text("Column Filters");
			ImGui::Checkbox(str_time, &panel_log->show_time); ImGui::SameLine();
			ImGui::Checkbox(str_tag, &panel_log->show_tag); ImGui::SameLine();
			ImGui::Checkbox(str_thread_id, &panel_log->show_thread_id); ImGui::SameLine();
			ImGui::Checkbox(str_filename, &panel_log->show_filename); ImGui::SameLine();
			ImGui::Checkbox(str_function, &panel_log->show_function); ImGui::SameLine();
			ImGui::Checkbox(str_line, &panel_log->show_line); ImGui::SameLine();
			//ImGui::Checkbox(str_msg, &panel_log->show_message); ImGui::SameLine();
			ImGui::EndPopup();
		}

		ImGui::Spacing();
		ImGui::Separator();
		int col_count =
			panel_log->show_time
			+ panel_log->show_tag
			+ panel_log->show_thread_id
			+ panel_log->show_filename
			+ panel_log->show_function
			+ panel_log->show_line
			+ panel_log->show_message;

		ImGui::BeginGroup();
		ImGui::BeginChild(1);
		if (col_count != 0) {
			ImGui::Columns(col_count, "log_columns");
			if (panel_log->show_time) {
				ImGui::Text(str_time); ImGui::NextColumn();
			}
			if (panel_log->show_tag) {
				ImGui::Text(str_tag); ImGui::NextColumn();
			}
			if (panel_log->show_thread_id) {
				ImGui::Text(str_thread_id); ImGui::NextColumn();
			}
			if (panel_log->show_filename) {
				ImGui::Text(str_filename); ImGui::NextColumn();
			}
			if (panel_log->show_function) {
				ImGui::Text(str_function); ImGui::NextColumn();
			}
			if (panel_log->show_line) {
				ImGui::Text(str_line); ImGui::NextColumn();
			}

			if (panel_log->show_message) {
				ImGui::Text(str_msg); ImGui::NextColumn();
			}
			ImGui::Separator();

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 5));

			
				for (int i = panel_log->log_start_offset; i < logs.log_count; i++) {
					LogItem* log_item = &logs.logs[i];
					
					bool found = false;

					if (panel_log->log_filter.PassFilter(log_item->msg, log_item->msg + log_item->msg_length)) {
						found = true;
					}

					if (panel_log->log_filter.PassFilter(log_item->tag, log_item->tag + log_item->tag_length)) {
						found = true;
					}

					if (panel_log->log_filter.PassFilter(log_item->function, log_item->function + log_item->function_length)) {
						found = true;
					}

					if (panel_log->log_filter.PassFilter(log_item->filename, log_item->filename + log_item->filename_length)) {
						found = true;
					}
					if (!found) {
						continue;
					}
					
					
					bool pop_color = false;

					const char* log_type_str;
					switch (log_item->verbosity) {
						case LoggerVerbosity::INFO: {
							if (!panel_log->show_info) { continue; }
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); pop_color = true;
							log_type_str = "[INFO]"; break;
						}
						case LoggerVerbosity::WARN: {
							if (!panel_log->show_warning) { continue; }
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.90f, 0.25f, 1.0f)); pop_color = true;
							log_type_str = "[WARN]"; break;
						}
						case LoggerVerbosity::FATAL: {
							if (!panel_log->show_fatal) { continue; }
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f)); pop_color = true;
							log_type_str = "[FATAL]"; break;
						}
						case LoggerVerbosity::VERBOSE: {
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); pop_color = true;
							log_type_str = "[VERBOSE]"; break;
						}
						default: {
							log_type_str = "[NONE]"; break;
						}
					}

					if (panel_log->show_time) {
						ImGui::Text("%llu", log_item->time);
						ImGui::NextColumn();
					}
					if (panel_log->show_tag) {
						ImGui::Text(log_item->tag);
						ImGui::NextColumn();
					}
					if (panel_log->show_thread_id) {
						ImGui::Text("%d", log_item->thread_id);
						ImGui::NextColumn();
					}
					if (panel_log->show_filename) {
						ImGui::TextUnformatted(log_item->filename, log_item->filename + log_item->filename_length - 1);
						ImGui::NextColumn();
					}
					if (panel_log->show_function) {
						ImGui::TextUnformatted(log_item->function, log_item->function + log_item->function_length - 1);
						ImGui::NextColumn();
					}
					if (panel_log->show_line) {
						ImGui::Text("%d", log_item->line);
						ImGui::NextColumn();
					}

					if (panel_log->show_message) {
						const char* end = (log_item->msg + log_item->msg_length - 1);
						ImGui::TextUnformatted(log_item->msg, end);
						ImGui::NextColumn();
					}


					if (pop_color) {
						ImGui::PopStyleColor();
					}

					ImGui::Separator();
				}
			
			


			ImGui::PopStyleVar();

			if (panel_log->auto_scroll) {
				ImGui::SetScrollHereY(1.0f);
			}

			ImGui::Columns(1);
			ImGui::EndChild();
			ImGui::EndGroup();
		}
	}
	ImGui::End();
}

static void recursive_deselect(AssetBrowserFileNode* root) {
	root->selected = false;
	AssetBrowserFileNode* child = root->first_child;
	while (child != NULL) {
		recursive_deselect(child);
		child = child->next_sibling;
	}
}

static void push_assetbrowserfilenode_selected(PanelAssetBrowser* asset_browser, AssetBrowserFileNode* node, bool additive_select, bool selected) {
	if (!additive_select) {
		// if we are not a additive/multi selection, then we need to clear all the previously selected nodes
		recursive_deselect(asset_browser->root);
	}
	node->selected = selected;
}

static void push_asset_directory_change(PanelAssetBrowser* asset_browser, AssetBrowserFileNode* node) {
	asset_browser->current_directory = node;
}

static void draw_path_reverse(EditorInterface* editor, AssetTracker* tracker, AssetBrowserFileNode* leaf_node) {
	PanelAssetBrowser* asset_browser = &editor->panel_asset_browser;
	if (leaf_node == NULL) {
		return;
	} 
	;
	draw_path_reverse(editor, tracker, leaf_node->parent);
	
	
	
	
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::ImColor(0.0f, 0.0f, 0.0f, 0.0f));

	
	
	if (ImGui::Button(leaf_node->name.buffer)) {
		push_asset_directory_change(asset_browser, leaf_node);
	}
	ImGui::PopStyleColor(1);

	ImGui::SameLine();
	if (leaf_node->has_child_directorys) {
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::ImColor(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushID(leaf_node->name.buffer);
		if (ImGui::ArrowButton(leaf_node->name.buffer, ImGuiDir_Right)) {
			// Show popups listing all child directories
			ImGui::OpenPopup("asset_path_popup");
		}
		if (ImGui::BeginPopup( "asset_path_popup")) {
			AssetBrowserFileNode* children_directories = leaf_node->first_child;
			while (children_directories != NULL) {
				if (children_directories->node_type == AssetBrowserFileNode::Type::Directory) {
					if (ImGui::Selectable(children_directories->name.buffer)) {
						push_asset_directory_change(asset_browser, children_directories);
					}
				}
				children_directories = children_directories->next_sibling;
			}
			ImGui::EndPopup();
		}
		ImGui::PopID();
		

		ImGui::PopStyleColor(1);
		
	}
	ImGui::SameLine();
	
	
	
}

// We want to know if we should draw a tree based on the filter text. If a node is deep in the tree hierarchy
// we want to draw that node and all the parent nodes
static bool should_draw_asset_tree(EditorInterface* editor, AssetBrowserFileNode* node) {
	PanelAssetBrowser* asset_browser = &editor->panel_asset_browser;

	if (asset_browser->asset_tree_filter.PassFilter(node->name.buffer, node->name.buffer + node->name.length)) {
		return true;
	}
	bool should_draw = false;
	AssetBrowserFileNode* child = node->first_child;
	while (child != NULL) {
		should_draw = should_draw_asset_tree(editor, child);
		if (should_draw) { return should_draw; }
		child = child->next_sibling;
	}
	
	return should_draw;
}

static void draw_asset_tree(EditorInterface* editor, AssetBrowserFileNode* node) {
	
	if (!should_draw_asset_tree(editor, node)) {
		return;
	}

	PanelAssetBrowser* asset_browser = &editor->panel_asset_browser;
	
	

	if (node->node_type == AssetBrowserFileNode::Type::Directory ) {

		if (node->has_child_directorys) {
			bool selected = asset_browser->current_directory == node;
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ((selected) ? ImGuiTreeNodeFlags_Selected : 0);
			bool open = ImGui::TreeNodeEx(node->name.buffer, node_flags, "%s %s", ICON_FA_FOLDER, node->name.buffer);
			
			
			if (ImGui::IsItemClicked() && (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) > ImGui::GetTreeNodeToLabelSpacing())
			{
				// The item is clicked
				push_asset_directory_change(asset_browser, node);
			}
			else {
				// Arrow is clicked
			}
			
			if (open) {
				AssetBrowserFileNode* child = node->first_child;
				while (child != NULL) {
					draw_asset_tree(editor, child);
					child = child->next_sibling;
				}

				ImGui::TreePop();
			}
		}
		else {
			ImGui::Indent();
			bool selected = asset_browser->current_directory == node;
			ImGui::Text(ICON_FA_FOLDER); ImGui::SameLine();
			bool open = ImGui::Selectable(node->name.buffer, &selected);
			if (ImGui::IsItemClicked()) {
				push_asset_directory_change(asset_browser, node);
			}
			ImGui::Unindent();
			
		}
		

	}
	
	
	
	
}

static void draw_asset_browser_context_menu(EditorInterface* editor, AssetTracker* tracker) {
	if (ImGui::BeginPopupContextWindow()) {

		if (ImGui::BeginMenu("Create")) {
			if (ImGui::MenuItem("Folder")) {}
			ImGui::Separator();
			if (ImGui::MenuItem("Create Material")) {}
			ImGui::EndMenu();
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Show In File Explorer")) {}
		if (ImGui::MenuItem("Open")) {}
		if (ImGui::MenuItem("Delete")) {}

		if (ImGui::MenuItem("Rename")) {}

		if (ImGui::MenuItem("Copy path to clipboard")) {}

		ImGui::Separator();

		if (ImGui::MenuItem("Import")) {}
		ImGui::Separator();

		if (ImGui::MenuItem("Refresh")) {}
		if (ImGui::MenuItem("Reimport")) {}

		ImGui::EndPopup();
	}
}

static void dragdrop_source_asset(EditorInterface* editor, AssetID assetid, String name) {
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		void* data = (void*)&assetid;
		ImGui::SetDragDropPayload("ASSETID", data, sizeof(AssetID));
		ImGui::Text(name.buffer);
		ImGui::EndDragDropSource();
	}
}

static void draw_asset_browser(EditorInterface* editor, AssetTracker* tracker) {
	PanelAssetBrowser* asset_browser = &editor->panel_asset_browser;

	AssetBrowserFileNode* node = asset_browser->current_directory->first_child;

	
	
	asset_browser->asset_browser_filter.Draw(ICON_FA_SEARCH " Search");

	ImGui::Checkbox("Scene", &asset_browser->asset_scene_filter); ImGui::SameLine();
	ImGui::Checkbox("Static Mesh", &asset_browser->asset_mesh_filter); ImGui::SameLine();
	ImGui::Checkbox("Material", &asset_browser->asset_material_filter); ImGui::SameLine();
	ImGui::Checkbox("Texture", &asset_browser->asset_texture_filter); ImGui::SameLine();
	
	

	static int scale_count = 0;


	const float ItemSpacing3 = ImGui::GetStyle().ItemSpacing.x;
	static float HostButtonWidth3 = 100.0f; //The 100.0f is just a guess size for the first frame.
	float pos3 = HostButtonWidth3 + ItemSpacing3;
	
	ImGui::SameLine(ImGui::GetColumnWidth() - pos3);
	

	if (ImGui::Button(ICON_FA_COG " Options")) {
		ImGui::OpenPopup("asset_options_popup");
	}
	HostButtonWidth3 = ImGui::GetItemRectSize().x; //Get the actual width for next frame.


	if (ImGui::BeginPopup("asset_options_popup")) {
		ImGui::SetNextItemWidth(50);
		ImGui::SliderInt("Scale", &scale_count, 0, 4, "");
		ImGui::EndPopup();
	}

	


	ImGui::BeginChild("Asset Browser Child");

	if (ImGui::IsMouseHoveringWindow() && ImGui::IsMouseClicked(0, false)) {
		recursive_deselect(asset_browser->root);
	}

	draw_asset_browser_context_menu(editor, tracker);
	

	

	ImGui::Separator();

	
	
			
			
	if (scale_count == 0) {
		// Detail View
		ImGui::Columns(3);

		ImGui::Text("Name");
		ImGui::NextColumn();

		ImGui::Text("Asset ID");
		ImGui::NextColumn();

		ImGui::Text("Asset Type");
		ImGui::Columns(1);

		ImGui::Separator();


		while (node != NULL) {


			ImGui::Columns(3);

			
			if (node->node_type == AssetBrowserFileNode::Type::Directory) {
				bool filter_pass = false;
				if (asset_browser->asset_browser_filter.PassFilter(node->name.buffer, node->name.buffer + node->name.length)) {
					filter_pass = true;
				}
				if (filter_pass) {
					ImGui::BeginGroup();
					bool selected = node->selected;

					
						
					ImGui::Text(ICON_FA_FOLDER); ImGui::SameLine();
					if (ImGui::Selectable(node->name.buffer, &selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap | ImGuiSelectableFlags_AllowDoubleClick)) {
						if (ImGui::IsMouseDoubleClicked(0)) {
							push_asset_directory_change(asset_browser, node);
						}
						else {
							bool multi_select = ImGui::GetIO().KeyCtrl;
							push_assetbrowserfilenode_selected(asset_browser, node, multi_select, !node->selected);
						}
					}
					ImGui::SetItemAllowOverlap();
					ImGui::NextColumn();
					ImGui::Text("-");

					ImGui::SetItemAllowOverlap();
					ImGui::NextColumn();
					ImGui::Text("-");


					ImGui::Columns(1);
					ImGui::EndGroup();

				}


			}
			else if (node->node_type == AssetBrowserFileNode::Type::File) {


				bool filter_pass = false;
				if (asset_browser->asset_browser_filter.PassFilter(node->name.buffer, node->name.buffer + node->name.length)) {
							if (asset_browser->asset_scene_filter && node->asset.type == AssetType::Scene) { filter_pass = true; }
							else if (asset_browser->asset_mesh_filter && node->asset.type == AssetType::StaticMesh) { filter_pass = true; }
							else if (asset_browser->asset_material_filter && node->asset.type == AssetType::Material) { filter_pass = true; }
							else if (asset_browser->asset_texture_filter && node->asset.type == AssetType::Texture) { filter_pass = true; }
						}

				if (filter_pass) {
					ImGui::BeginGroup();
					bool selected = node->selected;
					ImGui::Text(ICON_FA_FILE); ImGui::SameLine();
					if (ImGui::Selectable(node->name.buffer, &selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap | ImGuiSelectableFlags_AllowDoubleClick)) {
						
						if (ImGui::IsMouseDoubleClicked(0)) {
							// open asset viewer for that specific asset type
							editor->panel_asset_details.current_asset = node;
							editor->panel_asset_details.panel_open = true;
							
						}
						else {
							bool multi_select = ImGui::GetIO().KeyCtrl;
							push_assetbrowserfilenode_selected(asset_browser, node, multi_select, !node->selected);
						}
					}
					ImGui::SetItemAllowOverlap();
					ImGui::NextColumn();
					ImGui::Text("%llu", node->asset.id);

					ImGui::NextColumn();
					const char* asset_type_name = asset_type_to_string(node->asset.type);
					ImGui::Text(asset_type_name);


					ImGui::EndGroup();
				}


			}
			
			if (node->node_type == AssetBrowserFileNode::Type::File) {
				dragdrop_source_asset(editor, node->asset, node->name);
			}
			ImGui::Columns(1);
			node = node->next_sibling;
		}
	} else {
		
		// Tile View
		int buttons_count = asset_browser->current_directory->children_count;
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec2 button_sz(scale_count * 40, scale_count * 40);
		int n = 0;
		
		
		float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
		while (node != NULL) {

			bool filter_pass = false;
			void* icon_id = NULL;

			if (node->node_type == AssetBrowserFileNode::Type::Directory) {
				if (asset_browser->asset_browser_filter.PassFilter(node->name.buffer, node->name.buffer + node->name.length)) {
					filter_pass = true;
					icon_id = render_resource_to_id(editor->api.renderer, editor->panel_asset_browser.res_folder_icon_texture);
				}

			}
			else if (node->node_type == AssetBrowserFileNode::Type::File) {

				if (asset_browser->asset_browser_filter.PassFilter(node->name.buffer, node->name.buffer + node->name.length)) {
					if (asset_browser->asset_scene_filter && node->asset.type == AssetType::Scene) { filter_pass = true; }
					else if (asset_browser->asset_mesh_filter && node->asset.type == AssetType::StaticMesh) { filter_pass = true; }
					else if (asset_browser->asset_material_filter && node->asset.type == AssetType::Material) { filter_pass = true; }
					else if (asset_browser->asset_texture_filter && node->asset.type == AssetType::Texture) { filter_pass = true; }
					
					icon_id = render_resource_to_id(editor->api.renderer, get_asset_browser_thumbnail_for_asset(editor, node->asset));
				}
			}


			if (filter_pass) {
				ImGui::BeginGroup();

				// Get the starting position for where this group will be in the layout
				ImVec2 start_group_pos = ImGui::GetCursorScreenPos();
				ImGui::Image(icon_id, button_sz);

				ImVec2 txt_size = ImGui::CalcTextSize(node->name.buffer);
				ImVec2 txt_pos = ImGui::GetCursorScreenPos();
				ImVec4 txt_clip_rect(txt_pos.x, txt_pos.y, txt_pos.x + button_sz.x, txt_pos.y + button_sz.y);
				if (txt_size.x > button_sz.x) {
					// Text will clip
					// So just draw all the way to left, and dont center
					ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(txt_pos.x, txt_pos.y), IM_COL32(255, 255, 255, 255), node->name.buffer, NULL, 0.0f, &txt_clip_rect);
				}
				else {
					// Center text if there is room
					txt_pos.x = txt_pos.x + ((button_sz.x - txt_size.x) * 0.5f);
					ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(txt_pos.x, txt_pos.y), IM_COL32(255, 255, 255, 255), node->name.buffer, NULL, 0.0f, &txt_clip_rect);
				}
				// Add dummy for spacing
				ImGui::Dummy(ImVec2(button_sz.x, txt_size.y));

				bool selected = node->selected;
				if (selected) {
					// If this node is selected, we want to put a higlight over it
					ImVec4 color = style.Colors[ImGuiCol_Header];
					ImVec2 rect(start_group_pos.x + button_sz.x, start_group_pos.y + button_sz.y + txt_size.y + style.ItemSpacing.y);
					ImGui::GetWindowDrawList()->AddRectFilled(start_group_pos, rect, IM_COL32((int)(color.x * 255), (int)(color.y * 255), (int)(color.z * 255), 128));
				}

				ImGui::EndGroup();
				// Drag and drop
				if (node->node_type == AssetBrowserFileNode::Type::File) {
					dragdrop_source_asset(editor, node->asset, node->name);
				}

				if (ImGui::IsItemHovered()) {
					if (ImGui::IsMouseDoubleClicked(0)) {
						if (node->node_type == AssetBrowserFileNode::Type::Directory) {
							// If this node is a directory, we want to go inside that directory
							push_asset_directory_change(asset_browser, node);
						}
						else if (node->node_type == AssetBrowserFileNode::Type::File) {
							// If this node is a file, we want to open the file with a viewer

							// open asset viewer for that specific asset type
							editor->panel_asset_details.current_asset = node;
							editor->panel_asset_details.panel_open = true;
						}
					}
					else if (ImGui::IsItemClicked()) {
						// Select/Deselect
						bool multi_select = ImGui::GetIO().KeyCtrl;
						push_assetbrowserfilenode_selected(asset_browser, node, multi_select, !node->selected);
					}

					ImGui::SetTooltip(node->name.buffer);
				}

				float last_button_x2 = ImGui::GetItemRectMax().x;
				float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_sz.x; // Expected position if next button was on same line
				if (n + 1 < buttons_count && next_button_x2 < window_visible_x2)
					ImGui::SameLine();


			}

			node = node->next_sibling;
			n++;
		}
		
	}

	
	ImGui::EndChild();


	
}

static void draw_panel_asset_browser(EditorInterface* editor) {
	PanelAssetBrowser* asset_browser = &editor->panel_asset_browser;

	if (!asset_browser->panel_open) {
		return;
	}

	if (ImGui::Begin(ICON_FA_FOLDER  " Asset Browser", &asset_browser->panel_open)) {
		
		

		AssetTracker* tracker = &editor->api.asset_manager->asset_tracker;

		

		// TODO: Need to implement history
		//if (ImGui::ArrowButton("##left", ImGuiDir_Left)) {
		//	if (asset_browser->current_directory->parent->node_type != AssetBrowserFileNodeType::Root) {
		//		asset_browser->current_directory = asset_browser->current_directory->parent;
		//	}
		//}
		//ImGui::SameLine();
		//if (ImGui::ArrowButton("##right", ImGuiDir_Right)) {  }
		//ImGui::SameLine();
		//ImGui::VerticalSeparator();
		//ImGui::SameLine();
		
		//ImGui::Button("Import");
		//ImGui::SameLine();

		ImGui::PushID("asset_browser_path");
		// Since imgui draws buttons left to right, we need to use recursion to draw right to left
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
		draw_path_reverse(editor, tracker, asset_browser->current_directory);
		ImGui::PopStyleVar(1);
		ImGui::NewLine();
		ImGui::Separator();
		ImGui::PopID();

		

		ImGui::Columns(2, "asset_tree_and_browser", true);
		//static float initial_spacing = 350.f; if (initial_spacing) ImGui::SetColumnWidth(0, initial_spacing), initial_spacing = 0;
		asset_browser->asset_tree_filter.Draw(ICON_FA_SEARCH " Search Folders");
		ImGui::BeginChild("Asset Tree View");
		draw_asset_tree(editor, editor->panel_asset_browser.root);
		ImGui::EndChild();
		ImGui::NextColumn();

		draw_asset_browser(editor, tracker);
		ImGui::Columns(1);

	}
	ImGui::End();
}

static void draw_panel_render_stats(EditorInterface* editor) {

	
	if (!editor->panel_render_stats.panel_open) {
		return;
	}

	if (ImGui::Begin(ICON_FA_CHART_LINE " Render Stats", &editor->panel_render_stats.panel_open)) {
		Renderer* renderer = editor->api.renderer;

		char renderer_type_str[16];

		switch (renderer->type) {
			case BackenedRendererType::OpenGL: {
				snprintf(renderer_type_str, 16, "OpenGL");
				break;
			}
			default: {
				snprintf(renderer_type_str, 16, "Unknown");
				break;
			}

		}
		GameTimer* timer = editor->api.game_loop;

		ImGui::Checkbox("Cap framerate", &timer->cap_framerate);
		if (timer->cap_framerate) {
			ImGui::SliderInt("Target Framerate", &timer->target_fps, 0, 240);
		}
		editor->panel_render_stats.fps_history[editor->panel_render_stats.fps_history_index] = timer->fps;
		editor->panel_render_stats.fps_history_index = ((editor->panel_render_stats.fps_history_index + 1) % FPS_HISTORY_COUNT);
		static char fps_text[32];
		snprintf(fps_text, 32, "FPS %d - dt %f", timer->fps, timer->delta_time);
		ImGui::PlotLines("Frame Times", editor->panel_render_stats.fps_history, IM_ARRAYSIZE(editor->panel_render_stats.fps_history), 0, fps_text, 0.0f, 120.0f, ImVec2(0, 100));


		ImGui::Text("Time %f", timer->seconds);
		ImGui::Text("Physics Time %f", timer->physics_time);
		ImGui::Text("Physics Time Step %f", timer->time_step);

		ImGui::Text("Frame Count %d", timer->frame_count);
		ImGui::Text("Ticks %llu", timer->ticks);
		ImGui::Text("Delta Ticks %llu", timer->delta_ticks);
		ImGui::Text("Ticks per sec %llu", timer->ticks_per_sec);

		ImGui::Separator();

		ImGui::Text("Renderer Backend: %s", renderer_type_str);
		ImGui::Text("Shader Count: %d", renderer->opengl.shader_count);
		ImGui::Text("Texture Count: %d", renderer->opengl.texture_count);
		ImGui::Text("VAO Count: %d", renderer->opengl.vao_count);
		ImGui::Text("VBO Count: %d", renderer->opengl.vbo_count);
		ImGui::Text("EBO Count: %d", renderer->opengl.ebo_count);
		ImGui::Text("FBO Count: %d", renderer->opengl.fbo_count);
		ImGui::Text("RBO Count: %d", renderer->opengl.rbo_count);

		ImGui::Separator();
		ImGui::Text("Render World");
		
		ImGui::Text("Mesh Count: %d", renderer->render_world.render_mesh_count);
		ImGui::Text("Mesh Capacity: %d", renderer->render_world.render_mesh_capacity);

		
		ImGui::Text("Render Resoure Count: %d", renderer->render_world.resources_count);
		ImGui::Text("Material Count: %d", renderer->render_world.material_res_count);
		
		
			
	}
	ImGui::End();
}

static void do_viewport_rendering_and_logic(EditorInterface* editor, PanelViewports::ViewportType viewport_type) {
	RenderResource render_texture;
	RenderResource depth_texture;
	Entity camera;

	PanelViewports::Viewport* viewport = &editor->viewports.scene;

	switch (viewport_type) {
		case PanelViewports::ViewportType::Scene: {
			viewport = &editor->viewports.scene;
			break;
		}

		case PanelViewports::ViewportType::Top: {
			viewport = &editor->viewports.top;
			break;
		}

		case PanelViewports::ViewportType::Front: {
			viewport= &editor->viewports.front;
			break;
		}

		case PanelViewports::ViewportType::Side: {
			viewport = &editor->viewports.side;
			break;
		}
	}

	// The pos and size of the current dock/viewport
	ImVec2 start_group_pos = ImGui::GetCursorScreenPos();
	ImVec2 window_size = ImGui::GetCurrentWindow()->Size;
	ImVec2 rect = ImVec2(start_group_pos.x + window_size.x, start_group_pos.y + window_size.y);


	render_texture = viewport->render_texture;
	depth_texture = viewport->depth_texture;
	camera = viewport->camera;
	

	viewport->pos = Vec2i(start_group_pos.x, start_group_pos.y);
	viewport->size = Vec2i(window_size.x, window_size.y);

	void* color = render_resource_to_id(editor->api.renderer, render_texture);
	void* depth = render_resource_to_id(editor->api.renderer, depth_texture);

	

	// Need to update camera aspect ratio based on the dock's window size
	float aspect_ratio = (float)window_size.x / (float)window_size.y;
	set_camera_aspect_ratio(editor->api.entity_manager, camera, aspect_ratio);

	ImGui::GetWindowDrawList()->AddImage(color, start_group_pos, rect, ImVec2(0, 1), ImVec2(1, 0));

	if (editor->viewports.right_click_down) {
		// If right click is down then we should not try to capture anything
	} else {
		if (ImGui::IsWindowHovered()) {
			editor->viewports.current_viewport_capture = viewport_type;
		}
	}
}

static void draw_panel_viewports(EditorInterface* editor) {

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	// Reset capture to none if there is no right click down
	// the capture_viewport_if_possible calls will recapture the viewports for this frame
	if (!editor->viewports.right_click_down) {
		editor->viewports.current_viewport_capture = PanelViewports::ViewportType::None;
	}
	
	if (ImGui::Begin(ICON_FA_VIDEO " Scene")) {
		do_viewport_rendering_and_logic(editor, PanelViewports::ViewportType::Scene);
	}
	ImGui::End();

	if (ImGui::Begin(ICON_FA_GAMEPAD " Game")) {
		void* color = render_resource_to_id(editor->api.renderer, editor->viewports.scene.render_texture);
		void* depth = render_resource_to_id(editor->api.renderer, editor->viewports.scene.depth_texture);

		ImVec2 start_group_pos = ImGui::GetCursorScreenPos();
		ImVec2 viewport_size = ImGui::GetCurrentWindow()->Size;
		ImVec2 rect = ImVec2(start_group_pos.x + viewport_size.x, start_group_pos.y + viewport_size.y);

		
	
		ImGui::GetWindowDrawList()->AddImage(color, start_group_pos, rect, ImVec2(0, 1), ImVec2(1, 0));
	}
	ImGui::End();

	if (ImGui::Begin("Top - Y Axis")) {
		do_viewport_rendering_and_logic(editor, PanelViewports::ViewportType::Top);
	}
	ImGui::End();


	if (ImGui::Begin("Front - Z Axis")) {
		do_viewport_rendering_and_logic(editor, PanelViewports::ViewportType::Front);
	}
	ImGui::End();


	if (ImGui::Begin("Side - x Axis")) {
		do_viewport_rendering_and_logic(editor, PanelViewports::ViewportType::Side);
	}
	ImGui::End();
	ImGui::PopStyleVar(1);
}

static void draw_editor_command_undo_and_redo_stack(EditorInterface* editor) {
	if (ImGui::Begin(ICON_FA_HISTORY " Command History")) {
		
		float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
		if (ImGui::ArrowButton("##left", ImGuiDir_Left)) {
			perform_undo_operation(editor);
			perform_undo_operation(editor);
		}
		ImGui::SameLine(0.0f, spacing);
		if (ImGui::ArrowButton("##right", ImGuiDir_Right)) {
			perform_redo_operation(editor);
		}
		ImGui::SameLine(0.0f, spacing);
		ImGui::Text("Command Count %llu", editor->cmd_buffer.command_undo_stack_count);

		ImGui::Columns(2, "undo_redo_header", true);

		ImGui::Text("Undo");
		ImGui::NextColumn();

		ImGui::Text("Redo");
		ImGui::Separator();

		ImGui::Columns(1);

		ImGui::Columns(2, "undo_redo_stack", true);
		
		size_t undo_stack_count = editor->cmd_buffer.command_undo_stack_count;
		EditorCommand* undo_stack = editor->cmd_buffer.command_undo_stack;
		for (size_t i = 0; i < undo_stack_count; i++) {
			EditorCommand cmd = undo_stack[i];
			if (cmd.type == EditorCommandType::COMMAND_GROUP_START) {
				ImGui::Text("Cmd Group Start: ID %llu", cmd.cmd.group.stack_level);
			}
			else if (cmd.type == EditorCommandType::COMMAND_GROUP_END) {
				ImGui::Text("Cmd Group End: ID %llu", cmd.cmd.group.stack_level);
			} else {
				ImGui::Text("Cmd type %d", cmd.type);
			}
			
		}

		ImGui::NextColumn();
		
		
		
		size_t redo_stack_count = editor->cmd_buffer.command_redo_stack_count;
		EditorCommand* redo_stack = editor->cmd_buffer.command_redo_stack;
		for (size_t i = 0; i < redo_stack_count; i++) {
			EditorCommand cmd = redo_stack[i];
			if (cmd.type == EditorCommandType::COMMAND_GROUP_START) {
				ImGui::Text("Cmd Group Start: ID %llu", cmd.cmd.group.stack_level);
			}
			else if (cmd.type == EditorCommandType::COMMAND_GROUP_END) {
				ImGui::Text("Cmd Group End: ID %llu", cmd.cmd.group.stack_level);
			}
			else {
				ImGui::Text("Cmd type %d", cmd.type);
			}
		}
		ImGui::Columns(1);

	}
	ImGui::End();


}

static void draw_panel_asset_details(EditorInterface* editor) {
	if (!editor->panel_asset_details.panel_open) { return; }
	AssetBrowserFileNode* node = editor->panel_asset_details.current_asset;
	AssetID id = node->asset;
	
	if (ImGui::Begin("Asset Detail", &editor->panel_asset_details.panel_open)) {
		switch (id.type) {
			case AssetType::Texture: {
				InternalAsset internal_asset = get_internal_asset_by_id(editor->api.asset_manager, id);
				Texture2D* texture = internal_asset.texture;

				ImGui::Text("Width: %d", texture->width);
				ImGui::Text("Height : %d", texture->height);
				ImGui::Text("Channels: %d", texture->channels);

				ImGui::Text("Depth: %d", texture->depth);

				ImGui::Text("UV translation: %f,%f", texture->uv_translation.u, texture->uv_translation.v);
				ImGui::Text("UV rotation: %f", texture->uv_rotation);
				ImGui::Text("UV scaling: %f,%f", texture->uv_scaling.u, texture->uv_scaling.v);

				ImVec2 start_group_pos = ImGui::GetCursorScreenPos();
				// The size of the current dock/viewport
				ImVec2 window_size = ImGui::GetCurrentWindow()->Size;
				ImVec2 rect = ImVec2(start_group_pos.x + window_size.x, start_group_pos.y + window_size.y);

				RenderResource tex_resource = get_asset_browser_thumbnail_for_asset(editor, texture->id);
				void* tex_id = render_resource_to_id(editor->api.renderer, tex_resource);
				ImGui::Image(tex_id, ImVec2(100, 100));
				
				break;
			}

			case AssetType::Material: {
				InternalAsset internal_asset = get_internal_asset_by_id(editor->api.asset_manager, id);
				InternalMaterial* internal_mat = internal_asset.material;
				Material mat = internal_mat->material;

				ImGui::Text("Texture Maps");
				
				ImVec2 texture_thumbnail_size = ImVec2(50, 50);
				
				RenderResource tex_id;

				
				tex_id = get_asset_browser_thumbnail_for_asset(editor, AssetID(mat.albedo));
				ImGui::Image(render_resource_to_id(editor->api.renderer, tex_id), texture_thumbnail_size);
				ImGui::SameLine();
				
				
				ImGui::Text("Albedo");

				if (ImGui::ColorEdit3("Albedo Color", mat.albedo_color.data, ImGuiColorEditFlags_NoInputs)) {
				}

				tex_id = get_asset_browser_thumbnail_for_asset(editor, AssetID(mat.normal));
				ImGui::Image(render_resource_to_id(editor->api.renderer, tex_id), texture_thumbnail_size);
				ImGui::SameLine();
				ImGui::Text("Normal");


				tex_id = get_asset_browser_thumbnail_for_asset(editor, AssetID(mat.metal));
				ImGui::Image(render_resource_to_id(editor->api.renderer, tex_id), texture_thumbnail_size);
				ImGui::SameLine();
				ImGui::Text("Metal");

				if (ImGui::SliderFloat("Metallic factor", &mat.metallic_factor, 0, 1)) {

				}
				tex_id = get_asset_browser_thumbnail_for_asset(editor, AssetID(mat.roughness));
				ImGui::Image(render_resource_to_id(editor->api.renderer, tex_id), texture_thumbnail_size);
				ImGui::SameLine();
				ImGui::Text("Roughness");

				if (ImGui::SliderFloat("Roughness factor", &mat.roughness_factor, 0, 1)) {

				}

				tex_id = get_asset_browser_thumbnail_for_asset(editor, AssetID(mat.ao));
				ImGui::Image(render_resource_to_id(editor->api.renderer, tex_id), texture_thumbnail_size);
				ImGui::SameLine();
				ImGui::Text("AO");

				break;
			}

			case AssetType::StaticMesh: {
				ImGui::Text("StaticMesh Id %llu", id.mesh.id);
				break;
			}
			case AssetType::Scene: {
				ImGui::Text("Scene Id %llu", id.scene.id);
				break;
			}
		}
	}
	ImGui::End();
	

}

static void push_editor_undo_command(EditorInterface* editor, const EditorCommand& command) {

	
	editor->cmd_buffer.command_undo_stack[editor->cmd_buffer.command_undo_stack_count] = command;
	editor->cmd_buffer.command_undo_stack_count++;
	//editor->cmd_buffer.command_undo_stack_count = (editor->cmd_buffer.command_undo_stack_count + 1) % EDITOR_COMMAND_UNDO_BUFFER_CAPACITY;
	// Too many commands were sent this frame
	assert(editor->cmd_buffer.command_undo_stack_count < EDITOR_COMMAND_UNDO_REDO_BUFFER_CAPACITY && "UNDO STACK OVER FLOW");
}

static void push_editor_command(EditorInterface* editor, const EditorCommand& command) {
	
	
	//editor->cmd_buffer.current_group.command_count++;
	editor->cmd_buffer.commands[editor->cmd_buffer.command_count] = command;
	editor->cmd_buffer.command_count++;
	// Too many commands were sent this frame
	assert(editor->cmd_buffer.command_count < EDITOR_COMMAND_BUFFER_CAPACITY);

	push_editor_undo_command(editor, command);

	// Reset redo stack
	editor->cmd_buffer.command_redo_stack_count = 0;

}

static void push_editor_redo_command(EditorInterface* editor, const EditorCommand& command) {
	editor->cmd_buffer.command_redo_stack[editor->cmd_buffer.command_redo_stack_count] = command;
	editor->cmd_buffer.command_redo_stack_count++;
	// Too many commands were sent this frame
	assert(editor->cmd_buffer.command_redo_stack_count < EDITOR_COMMAND_UNDO_REDO_BUFFER_CAPACITY);
}

static void perform_undo_operation(EditorInterface* editor) {
	if (editor->cmd_buffer.command_undo_stack_count > 0) {
		LOG_INFO("Editor", "perform undo command");

		// Pop last command off the undo stack
		EditorCommand last_command = editor->cmd_buffer.command_undo_stack[editor->cmd_buffer.command_undo_stack_count - 1];
		editor->cmd_buffer.command_undo_stack_count--;
		
		// Push undo onto the redo stack
		push_editor_redo_command(editor, last_command);
		

		// If this is a group command
		// we need to pop the entire group off
		if (last_command.type == EditorCommandType::COMMAND_GROUP_END) {

			// The stack might look something like 
			// Start Group 0
			//  cmd 1
			//	Start Group 1
			//		cmd 2
			//		cmd 3
			//		cmd 4
			//	End Group 1
			//	cmd 5
			//	cmd 6
			// End Group 0

			
			u64 stack_level = last_command.cmd.group.stack_level;
			
			EditorCommand* sub_command;
			do {
				// Keep popping off sub commands till we reach the end command that is at the same level as the current stack level
				sub_command = &editor->cmd_buffer.command_undo_stack[editor->cmd_buffer.command_undo_stack_count - 1];
				editor->cmd_buffer.command_undo_stack_count--;

				perform_command(editor, *sub_command, true);

				// Push undo onto the redo stack
				push_editor_redo_command(editor, *sub_command);
			} while (!(sub_command->type == EditorCommandType::COMMAND_GROUP_START && sub_command->cmd.group.stack_level == stack_level));
			


		} else {
			// This is a non group command
			perform_command(editor, last_command, true);
			
		}


		if (last_command.type == EditorCommandType::COMMAND_FORCE_NO_MERGE) {
			// pop this command
			// And perform the next operation
			perform_undo_operation(editor);
				
		}

		

	
		assert(editor->cmd_buffer.command_undo_stack_count >= 0);

	}
	
}

static void perform_redo_operation(EditorInterface* editor) {
	LOG_INFO("Editor", "perform redo command");


	if (editor->cmd_buffer.command_redo_stack_count > 0) {
		EditorCommand last_command = editor->cmd_buffer.command_redo_stack[editor->cmd_buffer.command_redo_stack_count - 1];
		editor->cmd_buffer.command_redo_stack_count--;

		// Push undo onto the undo stack
		push_editor_undo_command(editor, last_command);

		


		if (last_command.type == EditorCommandType::COMMAND_GROUP_START) {
			u64 stack_level = last_command.cmd.group.stack_level;

			EditorCommand* sub_command;
			do {
				sub_command = &editor->cmd_buffer.command_redo_stack[editor->cmd_buffer.command_redo_stack_count - 1];
				editor->cmd_buffer.command_redo_stack_count--;
				perform_command(editor, *sub_command, false);
				// Push undo onto the redo stack
				push_editor_undo_command(editor, *sub_command);

			} while (!(sub_command->type == EditorCommandType::COMMAND_GROUP_END && sub_command->cmd.group.stack_level == stack_level));

		} else {
			perform_command(editor, last_command, false);

		}


		// We need to peek the redo stack for the no merge command
		if (editor->cmd_buffer.command_redo_stack_count > 0) { // Check if that there is commands on the redo stack. prevent buffer underflow
			EditorCommand peek_next_command = editor->cmd_buffer.command_redo_stack[editor->cmd_buffer.command_redo_stack_count - 1];
			if (peek_next_command.type == EditorCommandType::COMMAND_FORCE_NO_MERGE) {
				perform_redo_operation(editor);
			}
		}
		

		assert(editor->cmd_buffer.command_redo_stack_count >= 0);

	}

}

static void perform_command(EditorInterface* editor, EditorCommand command, bool undo) {
	
	switch (command.type) {

		case EditorCommandType::COMMAND_GROUP_START: {
			EditorCommandGroup group = command.cmd.group;
			break;
		}
		case EditorCommandType::COMMAND_GROUP_END: {
			EditorCommandGroup group = command.cmd.group;
			break;
		}
		case EditorCommandType::COMMAND_FORCE_NO_MERGE: {
			break;
		}
		case EditorCommandType::NEW_ENTITY: {

			EditorCommand_NewEntity new_entity_command = command.cmd.new_entity;
			if (undo) {
				

				destroy_entity(editor->api.entity_manager, new_entity_command.entity);
			} else {
				Entity e = create_entity(editor->api.entity_manager, new_entity_command.name);
				map_put(&editor->entity_selected, e.id, false);
				command.cmd.new_entity.entity = e;


				ComponentType comp_flags = new_entity_command.component_flags;

				for (int i = 1; i < (1 << (int)ComponentType::Count); i *= 2) {
					ComponentType component = ComponentType(i);
					bool comp_set = ((u64)comp_flags & (u64)component);
					if (comp_set) {
						add_component(editor->api.entity_manager, e, component);
					}
				}

				if (new_entity_command.staticmesh_id.id != 0) {
					set_static_mesh(editor->api.entity_manager, e, new_entity_command.staticmesh_id);
				}

				if (new_entity_command.material_id.id != 0) {
					set_render_material(editor->api.entity_manager, e, new_entity_command.material_id);
				}
			}
			break;
		}
		case EditorCommandType::DELETE_ENTITY: {
			destroy_entity(editor->api.entity_manager, command.cmd.delete_entity.entity_to_delete);
			break;
		}
		case EditorCommandType::SELECT_ENTITY: {

			
			EditorCommand_SelectEntity select_command = command.cmd.select_entity;
			Entity selected_entity = select_command.entity_to_select;
			bool selected = select_command.selected;



			if (undo) {
				bool already_selected = is_entity_selected(editor, selected_entity);
				if (!selected == already_selected) { return; }

				map_put(&editor->entity_selected, selected_entity.id, !selected);

				if (!selected) {
					assert(editor->entity_selected_count < ULLONG_MAX);
					editor->entity_selected_count++;
				} else {
					assert(editor->entity_selected_count > 0);
					editor->entity_selected_count--;
				}
			} else {
				bool already_selected = is_entity_selected(editor, selected_entity);
				if (selected == already_selected) { return; }

				map_put(&editor->entity_selected, selected_entity.id, selected);

				if (selected) {
					assert(editor->entity_selected_count < ULLONG_MAX);
					editor->entity_selected_count++;
				} else {
					assert(editor->entity_selected_count > 0);
					editor->entity_selected_count--;
				}
			}
			break;
		}
		case EditorCommandType::DUPLICATE_ENTITY: {
			break;
		}
		case EditorCommandType::SET_TRANSFORM_COMPONENT: {
			EditorCommand_SetTransformComponent set_transform = command.cmd.set_transform;
			EntityManager* entity_manager = editor->api.entity_manager;
			if (undo) {
				set_position(entity_manager, set_transform.entity, set_transform.old_position);
				set_rotation(entity_manager, set_transform.entity, set_transform.old_rotation);
				set_scale(entity_manager, set_transform.entity, set_transform.old_scale);
				
			} else {
				
				set_position(entity_manager, set_transform.entity, set_transform.position);
				set_rotation(entity_manager, set_transform.entity, set_transform.rotation);
				set_scale(entity_manager, set_transform.entity, set_transform.scale);
			}
			break;
		}
		case EditorCommandType::SET_STATICMESH_COMPONENT: {
			EditorCommand_SetStaticMeshComponent set_staticmesh = command.cmd.set_staticmesh;
			EntityManager* entity_manager = editor->api.entity_manager;
			if (undo) {
				set_static_mesh(entity_manager, set_staticmesh.entity, set_staticmesh.old_id);
			}
			else {
				set_static_mesh(entity_manager, set_staticmesh.entity, set_staticmesh.new_id);
			}
			break;
		}

		case EditorCommandType::SET_MATERIAL_COMPONENT: {
			EditorCommand_SetMaterialComponent set_material = command.cmd.set_material;
			EntityManager* entity_manager = editor->api.entity_manager;
			if (undo) {
				set_render_material(entity_manager, set_material.entity, set_material.old_id);
			}
			else {
				set_render_material(entity_manager, set_material.entity, set_material.new_id);
			}
			break;
		}
		case EditorCommandType::SET_LIGHT_COMPONENT: {
			EditorCommand_SetLightComponent set_light_data = command.cmd.set_light;
			EntityManager* entity_manager = editor->api.entity_manager;
			if (undo) {
				set_light(entity_manager, set_light_data.entity, set_light_data.old_light);
			}
			else {
				set_light(entity_manager, set_light_data.entity, set_light_data.new_light);
			}
			break;
		}
		case EditorCommandType::SET_CAMERA_COMPONENT: {
			EditorCommand_SetCameraComponent set_camera_data = command.cmd.set_camera;
			EntityManager* entity_manager = editor->api.entity_manager;
			if (undo) {
				set_camera(entity_manager, set_camera_data.entity, set_camera_data.old_camera);
			}
			else {
				set_camera(entity_manager, set_camera_data.entity, set_camera_data.new_camera);
			}
			break;
		}

		case EditorCommandType::REPARENT_ENTITY: {
			EditorCommand_ReparentEntity reparent_entity_data = command.cmd.reparent_entity;
			EntityManager* entity_manager = editor->api.entity_manager;
			if (undo) {
				attach_child_entity(entity_manager, reparent_entity_data.old_parent, reparent_entity_data.entity);
			}
			else {
				attach_child_entity(entity_manager, reparent_entity_data.new_parent, reparent_entity_data.entity);
			}
			break;
		}

		default: {
			// Unknown command was sent
			assert(false && "Unknown Editor command. Did you forget to implement an editor command?");
		}
	}



}

static void process_editor_command_buffer(EditorInterface* editor) {
	EditorCommandBuffer* cmd_buffer = &editor->cmd_buffer;
	const size_t command_count = cmd_buffer->command_count;
	for (size_t i = 0; i < command_count; i++) {
		const EditorCommand& command = cmd_buffer->commands[i];
		perform_command(editor, command, false);
	}

	
	cmd_buffer->command_count = 0;
}

static void cmd_editor_group_begin(EditorInterface* editor) {
	size_t group_count = editor->cmd_buffer.command_group_stack_count;


	EditorCommand command;
	command.type = EditorCommandType::COMMAND_GROUP_START;
	command.cmd.group.stack_level = group_count;
	push_editor_command(editor, command);

	assert(editor->cmd_buffer.group_stack[group_count].in_group_transaction == false && "Did you forget to call cmd_editor_group_end?");

	

	editor->cmd_buffer.group_stack[group_count].in_group_transaction = true;
	editor->cmd_buffer.command_group_stack_count++;

	//editor->cmd_buffer.current_group.in_group_transaction = true;
	//editor->cmd_buffer.current_group.command_count = 0;

}

static void cmd_editor_group_end(EditorInterface* editor) {
	
	size_t group_count = editor->cmd_buffer.command_group_stack_count;

	// Crash if this is not a group transaction
	assert(editor->cmd_buffer.group_stack[group_count - 1].in_group_transaction == true && "Did you forget to call cmd_editor_group_begin?");


	EditorCommand command;
	command.type = EditorCommandType::COMMAND_GROUP_END;
	command.cmd.group = editor->cmd_buffer.group_stack[group_count - 1];
	command.cmd.group.stack_level = group_count - 1;
	push_editor_command(editor, command);

	
	
	// Reset command transaction
	// Even though this will be guaranteed to happen when you call group begin
	// this prevents weird lingering issues when group begin is not called
	editor->cmd_buffer.group_stack[group_count - 1].in_group_transaction = false;
	editor->cmd_buffer.command_group_stack_count--;
	//editor->cmd_buffer.current_group.in_group_transaction = false;

	
}

static void cmd_editor_force_no_merge(EditorInterface* editor) {
	EditorCommand command;
	command.type = EditorCommandType::COMMAND_FORCE_NO_MERGE;
	push_editor_command(editor, command);
}

static void cmd_editor_create_emtpy_entity(EditorInterface* editor) {

	EditorCommand command;
	command.type = EditorCommandType::NEW_ENTITY;
	command.cmd.new_entity.name = String("Entity");

	ComponentType comp_flags = ComponentType::None;
	command.cmd.new_entity.component_flags = comp_flags;

	command.cmd.new_entity.staticmesh_id.id = 0;
	command.cmd.new_entity.material_id.id = 0;

	push_editor_command(editor, command);

}

static void cmd_editor_create_plane(EditorInterface* editor) {
	EntityManager* entity_manager = editor->api.entity_manager;

	EditorCommand command;
	command.type = EditorCommandType::NEW_ENTITY;
	command.cmd.new_entity.name = String("Plane");

	ComponentType comp_flags = ComponentType::None;
	comp_flags = comp_flags | ComponentType::StaticMesh;
	comp_flags = comp_flags | ComponentType::Render;

	command.cmd.new_entity.component_flags = comp_flags;
	command.cmd.new_entity.staticmesh_id = editor->api.asset_manager->plane_mesh.mesh;
	command.cmd.new_entity.material_id = editor->default_mat.material;

	push_editor_command(editor, command);

}

static void cmd_editor_create_sphere(EditorInterface* editor) {
	EntityManager* entity_manager = editor->api.entity_manager;
	EditorCommand command;
	command.type = EditorCommandType::NEW_ENTITY;
	command.cmd.new_entity.name = String("Sphere");



	ComponentType comp_flags = ComponentType::None;
	comp_flags = comp_flags | ComponentType::StaticMesh;
	comp_flags = comp_flags | ComponentType::Render;

	command.cmd.new_entity.component_flags = comp_flags;
	command.cmd.new_entity.staticmesh_id = editor->api.asset_manager->sphere_mesh.mesh;
	command.cmd.new_entity.material_id = editor->default_mat.material;


	push_editor_command(editor, command);

}

static void cmd_editor_create_cube(EditorInterface* editor) {
	EntityManager* entity_manager = editor->api.entity_manager;
	EditorCommand command;
	command.type = EditorCommandType::NEW_ENTITY;
	command.cmd.new_entity.name = String("Cube");

	ComponentType comp_flags = ComponentType::None;
	comp_flags = comp_flags | ComponentType::StaticMesh;
	comp_flags = comp_flags | ComponentType::Render;

	command.cmd.new_entity.component_flags = comp_flags;
	command.cmd.new_entity.staticmesh_id = editor->api.asset_manager->cube_mesh.mesh;
	command.cmd.new_entity.material_id = editor->default_mat.material;


	push_editor_command(editor, command);

}

static void cmd_editor_create_light(EditorInterface* editor) {
	EntityManager* entity_manager = editor->api.entity_manager;
	EditorCommand command;
	command.type = EditorCommandType::NEW_ENTITY;
	command.cmd.new_entity.name = String("Light");

	ComponentType comp_flags = ComponentType::None;
	comp_flags = comp_flags | ComponentType::Light;
	command.cmd.new_entity.component_flags = comp_flags;

	command.cmd.new_entity.staticmesh_id.id = 0;
	command.cmd.new_entity.material_id.id = 0;

	push_editor_command(editor, command);

}

static void cmd_editor_create_camera(EditorInterface* editor) {
	EntityManager* entity_manager = editor->api.entity_manager;
	EditorCommand command;
	command.type = EditorCommandType::NEW_ENTITY;
	command.cmd.new_entity.name = String("Camera");
	ComponentType comp_flags = ComponentType::None;

	comp_flags = comp_flags | ComponentType::Camera;
	command.cmd.new_entity.component_flags = comp_flags;

	command.cmd.new_entity.staticmesh_id.id = 0;
	command.cmd.new_entity.material_id.id = 0;

	push_editor_command(editor, command);
}

static void cmd_editor_select_entity(EditorInterface* editor, Entity entity, bool selected) {
	EditorCommand command;
	command.type = EditorCommandType::SELECT_ENTITY;
	command.cmd.select_entity.entity_to_select = entity;
	command.cmd.select_entity.selected = selected;
	

	push_editor_command(editor, command);
}

static void cmd_editor_deselect_all_entitys(EditorInterface* editor) {
	EntityManager* em = editor->api.entity_manager;

	if (editor->entity_selected_count > 0) {
		cmd_editor_group_begin(editor);
		for (int i = 0; i < em->entity_count; i++) {
			Entity e = em->entity_list[i];
			// Only deselect the ones that are selected
			if (is_entity_selected(editor, e)) {
				cmd_editor_select_entity(editor, e, false);
			}
		}
		cmd_editor_group_end(editor);
	}
	

	
	
}

static void cmd_edtior_set_transform_component(EditorInterface* editor, Entity e, Vec3f old_pos, Quat old_rot, Vec3f old_scale, Vec3f pos, Quat rot, Vec3f scale, bool merge_if_top) {
	EditorCommand command;
	command.type = EditorCommandType::SET_TRANSFORM_COMPONENT;
	command.cmd.set_transform.entity = e;


	command.cmd.set_transform.position = pos;
	command.cmd.set_transform.old_position = old_pos;

	command.cmd.set_transform.scale = scale;
	command.cmd.set_transform.old_scale = old_scale;

	command.cmd.set_transform.rotation = rot;
	command.cmd.set_transform.old_rotation = old_rot;
	
	// We only merge commands that are on the undo stack
	// This is usually used for when we starts a command on one frame, but need to end a command on another frame
	// For example if you start dragging a transform one frame 1, it might not also end on frame 1. So we need to merge multiple frames for one command
	// Therefore a simple way to solve this problem is to merge two commands
	if (merge_if_top) {
		size_t top = editor->cmd_buffer.command_undo_stack_count;
		if (top > 0) {
			EditorCommand* top_command = &editor->cmd_buffer.command_undo_stack[top - 1];
			if (top_command->type == EditorCommandType::SET_TRANSFORM_COMPONENT) {
				top_command->cmd.set_transform.position = pos;
				top_command->cmd.set_transform.scale = scale;
				top_command->cmd.set_transform.rotation = rot;
				return;
			}
		}
	}
	push_editor_command(editor, command);
	
}

static void cmd_editor_set_staticmesh_component(EditorInterface* editor, Entity e, StaticMeshID old_id, StaticMeshID new_id) {
	EditorCommand command;
	command.type = EditorCommandType::SET_STATICMESH_COMPONENT;
	command.cmd.set_staticmesh.entity = e;
	command.cmd.set_staticmesh.old_id = old_id;
	command.cmd.set_staticmesh.new_id = new_id;

	push_editor_command(editor, command);
	
}

static void cmd_editor_set_material_component(EditorInterface* editor, Entity e, MaterialID old_id, MaterialID new_id) {
	EditorCommand command;
	command.type = EditorCommandType::SET_MATERIAL_COMPONENT;
	command.cmd.set_material.entity = e;
	command.cmd.set_material.old_id = old_id;
	command.cmd.set_material.new_id = new_id;

	push_editor_command(editor, command);
}

static void cmd_editor_set_light_component(EditorInterface* editor, Entity e, Light old_light, Light new_light, bool merge_if_top) {
	EditorCommand command;
	command.type = EditorCommandType::SET_LIGHT_COMPONENT;
	command.cmd.set_light.entity = e;
	command.cmd.set_light.old_light = old_light;
	command.cmd.set_light.new_light = new_light;

	if (merge_if_top) {
		size_t top = editor->cmd_buffer.command_undo_stack_count;
		if (top > 0) {
			EditorCommand* top_command = &editor->cmd_buffer.command_undo_stack[top - 1];
			if (top_command->type == EditorCommandType::SET_LIGHT_COMPONENT) {
				top_command->cmd.set_light.new_light = new_light;
				return;
			}
		}
	}

	push_editor_command(editor, command);
}

static void cmd_editor_set_camera_component(EditorInterface* editor, Entity e, Camera old_camera, Camera new_camera, bool merge_if_top) {
	EditorCommand command;
	command.type = EditorCommandType::SET_CAMERA_COMPONENT;

	
	
	command.cmd.set_camera.entity = e;
	command.cmd.set_camera.old_camera = old_camera;
	command.cmd.set_camera.new_camera = new_camera;

	if (merge_if_top) {
		size_t top = editor->cmd_buffer.command_undo_stack_count;
		if (top > 0) {
			EditorCommand* top_command = &editor->cmd_buffer.command_undo_stack[top - 1];
			if (top_command->type == EditorCommandType::SET_CAMERA_COMPONENT) {
				top_command->cmd.set_camera.new_camera = new_camera;
				return;
			}
		}
	}

	push_editor_command(editor, command);
}

static void cmd_editor_reparent_entity(EditorInterface* editor, Entity e, Entity old_parent, Entity new_parent) {
	EditorCommand command;
	command.type = EditorCommandType::REPARENT_ENTITY;



	command.cmd.reparent_entity.entity = e;
	command.cmd.reparent_entity.old_parent = old_parent;
	command.cmd.reparent_entity.new_parent = new_parent;


	push_editor_command(editor, command);
}