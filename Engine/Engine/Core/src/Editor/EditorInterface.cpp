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
	
	InternalAsset asset = get_asset_by_id(editor->api.asset_manager, scene_id);

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

		ImGui::DockBuilderDockWindow("Scene", dock_main_id);
		ImGui::DockBuilderDockWindow("Game", dock_main_id);

		ImGui::DockBuilderDockWindow("Entity Scene Tree", dock_id_left);
		ImGui::DockBuilderDockWindow("Entity Components", dock_id_right);
		
		

		ImGui::DockBuilderDockWindow("Game Loop", dock_id_bottom);
		ImGui::DockBuilderDockWindow("Asset Browser", dock_id_bottom);
		ImGui::DockBuilderDockWindow("Render Stats", dock_id_bottom);
		ImGui::DockBuilderDockWindow("Log", dock_id_bottom);
		ImGui::DockBuilderDockWindow("Command History", dock_id_left_bottom);
		
		

		ImGui::DockBuilderFinish(editor->dockspace_id);
	}

	ImGui::DockSpace(editor->dockspace_id, ImVec2(0,0), opt_flags);

	ImGui::End();
}



static void set_editor_style(EditorInterface* editor) {

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
	style.FrameBorderSize = 0;
	
	style.WindowRounding = 0;
	style.ChildRounding = 0;
	style.FrameRounding = 2;
	style.PopupRounding = 2;
	style.ScrollbarRounding = 2;
	style.GrabRounding = 2;
	style.TabRounding = 2;
	
	style.WindowTitleAlign = ImVec2(0, 0.5f);
	style.ButtonTextAlign= ImVec2(0.5f, 0.5f);
	
	ImVec4* colors = ImGui::GetStyle().Colors; 

	
	

	ImVec4 dark_accent = ImVec4(0.12f, 0.29f, 0.62f, 1.00f);
	ImVec4 light_accent = ImVec4(0.29f, 0.53f, 1.00f, 1.00f);
	

	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.28f, 0.28f, 0.28f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
	colors[ImGuiCol_Border] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = light_accent;
	colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.53f, 0.53f, 0.53f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.30f, 0.30f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = dark_accent;
	colors[ImGuiCol_CheckMark] = light_accent;
	colors[ImGuiCol_SliderGrab] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = dark_accent;
	colors[ImGuiCol_Button] = dark_accent;
	colors[ImGuiCol_ButtonHovered] = light_accent;
	colors[ImGuiCol_ButtonActive] = dark_accent;
	colors[ImGuiCol_Header] = dark_accent;
	colors[ImGuiCol_HeaderHovered] = light_accent;
	colors[ImGuiCol_HeaderActive] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_SeparatorActive] = light_accent;
	colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = light_accent;
	colors[ImGuiCol_Tab] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_TabHovered] = dark_accent;
	colors[ImGuiCol_TabActive] = dark_accent;
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
	colors[ImGuiCol_DockingPreview] = dark_accent;
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
	colors[ImGuiCol_PlotLines] = light_accent;
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.99f, 0.08f, 0.08f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = light_accent;
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = light_accent;
	colors[ImGuiCol_DragDropTarget] = light_accent;
	colors[ImGuiCol_NavHighlight] = light_accent;
	colors[ImGuiCol_NavWindowingHighlight] = light_accent;
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


bool init_editor_interface(EditorInterface* editor, EngineAPI api) {

	set_editor_style(editor);

	map_init(&editor->entity_selected);
	editor->show_editor = true;

	editor->show_info = true;
	editor->show_warning = true;
	editor->show_fatal = true;

	editor->log_start_offset = 0;

	editor->show_time = true;
	editor->show_tag = true;
	editor->show_thread_id = true;
	editor->show_filename = true;
	editor->show_function = true;
	editor->show_line = true;
	editor->show_message = true;


	editor->window_scene_tree_open = true;
	editor->window_log_open = true;
	editor->window_asset_browser_open = true;
	editor->window_entity_components_open = true;
	editor->window_engine_timers_open = true;
	editor->window_render_stats = true;
	

	editor->api = api;
	
	

	
	
	
	

	

	
	
	

	arena_init(&editor->arena);
	size_t mem_size = EDITOR_MEMORY;
	void* mem_block = arena_alloc(&editor->arena, mem_size);
	mem_size = editor->arena.end - cast(char*) mem_block;
	stack_alloc_init(&editor->stack, mem_block, mem_size);

	
	
	init_asset_importer(&editor->importer, &editor->api.asset_manager->asset_tracker);

	editor->asset_browser.root = editor->api.asset_manager->asset_tracker.dir_root;
	editor->asset_browser.current_directory = editor->api.asset_manager->asset_tracker.dir_root;
	

	map_put(&editor->entity_selected, editor->api.entity_manager->root.id, false);

	editor->editor_camera = create_entity(api.entity_manager, "Editor Camera");
	//add_component(api.entity_manager, editor->editor_camera , ComponentType::Transform);
	add_component(api.entity_manager, editor->editor_camera , ComponentType::Camera);

	
	set_camera_params(api.entity_manager, editor->editor_camera, 0.0001f, 100.0f, 90.0f, editor->render_texture_size.x / cast(float) editor->render_texture_size.y);
	set_position(api.entity_manager, editor->editor_camera, Vec3f(0, 0, 0));

	
	load_hdr_skymap(&editor->hdr_skymap, &editor->stack, "InternalAssets/skyboxes/hdr/Alexs_Apartment/Alexs_Apt_2k.hdr");
	//load_hdr_skymap(&editor->hdr_skymap, &editor->stack, "InternalAssets/skyboxes/hdr/Mono_Lake_B/Mono_Lake_B_Ref.hdr");
	//load_hdr_skymap(&editor->hdr_skymap, &editor->stack, "InternalAssets/skyboxes/hdr/Newport_Loft/Newport_Loft_Ref.hdr");

	create_skymap(api.renderer, &editor->hdr_skymap);
	create_shadowmap(api.renderer);

	
	load_texture("editor_resources/thumbnails/Folder-Icon.png", &editor->folder_icon_texture, &editor->arena, false);
	load_texture("editor_resources/thumbnails/Asset-Icon.png", &editor->asset_icon_texture, &editor->arena, false);

	editor->res_folder_icon_texture = create_texture_resource(editor->api.renderer, &editor->folder_icon_texture, false);
	editor->res_asset_icon_texture = create_texture_resource(editor->api.renderer, &editor->asset_icon_texture, false);
	

	

	editor->render_texture_size = Vec2i(1920, 1080);
	u32 width = editor->render_texture_size.x;
	u32 height = editor->render_texture_size.y;


	Texture2D color_texture = Texture2D();
	color_texture.data = NULL;
	color_texture.width = width;
	color_texture.height = height;

	Texture2D depth_texture = Texture2D(); 
	depth_texture.data = NULL;
	depth_texture.width = width;
	depth_texture.height = height;



	editor->render_texture = create_texture_resource(editor->api.renderer, &color_texture, false);
	editor->depth_texture = create_texture_resource(editor->api.renderer, &depth_texture, false, true);

	FrameBufferAttachement color_texture_attachement = FrameBufferAttachement(FrameBufferAttachementType::COLOR, editor->render_texture, 0);
	FrameBufferAttachement depth_texture_attachement = FrameBufferAttachement(FrameBufferAttachementType::DEPTH, editor->depth_texture, 0);
	FrameBufferAttachement attachments[2] = { color_texture_attachement, depth_texture_attachement };

	editor->render_framebuffer = create_frame_buffer(editor->api.renderer, (u32)2, attachments);
	editor->api.renderer->render_world.render_framebuffer = editor->render_framebuffer;
	
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
	//editor->test_mat = load_asset_by_name(editor->api.asset_manager, "Assets/textures/bamboo-wood/bamboo-wood_mat.easset");
	//editor->test_mat = load_asset_by_name(editor->api.asset_manager, "Assets/textures/plastic/plastic_mat.easset");
	//editor->test_mat = load_asset_by_name(editor->api.asset_manager, "Assets/textures/plastic/plastic_green_mat.easset");
	//editor->test_mat = load_asset_by_name(editor->api.asset_manager, "Assets/textures/wet_stone/slipperystonework_mat.easset");
	editor->test_mat = load_asset_by_name(editor->api.asset_manager, "Assets/textures/gold/gold_mat.easset");

	//editor->test_mat = load_asset_by_name(editor->api.asset_manager, "Assets/textures/rust_iron/rust_iron_mat.easset");
	//editor->test_mat = load_asset_by_name(editor->api.asset_manager, "Assets/textures/paint_cement/paint_cement_mat_mat.easset");
	
	


	EntityManager* entity_manager = editor->api.entity_manager;
	AssetManager* asset_manager = editor->api.asset_manager;

	Entity e3 = create_entity(entity_manager, "Entity 3");
	Entity e4 = create_entity(entity_manager, "Entity 4");
	Entity e5 = create_entity(entity_manager, "Entity 5");
	Entity e6 = create_entity(entity_manager, "Entity 6");
	Entity e7 = create_entity(entity_manager, "Entity 7");
	Entity e8 = create_entity(entity_manager, "Entity 8");
	editor->test_mesh = create_entity(entity_manager, "Test mesh");
	editor->entity_test_light = create_entity(entity_manager, "Test Light");

	add_component(entity_manager, editor->entity_test_light, ComponentType::Light);


	attach_child_entity(entity_manager, e3, e4);
	attach_child_entity(entity_manager, e3, e8);
	attach_child_entity(entity_manager, e4, e5);
	attach_child_entity(entity_manager, e3, e6);
	attach_child_entity(entity_manager, e3, editor->test_mesh);


	

	map_put(&editor->entity_selected, editor->editor_camera.id, false);
	map_put(&editor->entity_selected, e3.id, false);
	map_put(&editor->entity_selected, e4.id, false);
	map_put(&editor->entity_selected, e5.id, false);
	map_put(&editor->entity_selected, e6.id, false);
	map_put(&editor->entity_selected, e7.id, false);
	map_put(&editor->entity_selected, e8.id, false);
	map_put(&editor->entity_selected, editor->test_mesh.id, false);
	map_put(&editor->entity_selected, editor->entity_test_light.id, false);

	



	
	

	add_component(entity_manager, editor->test_mesh, ComponentType::StaticMesh);
	add_component(entity_manager, editor->test_mesh, ComponentType::Render);
	set_render_material(entity_manager, editor->test_mesh, editor->test_mat.material);
	
	set_static_mesh(entity_manager, editor->test_mesh, editor->api.asset_manager->monkey_mesh.mesh);


	add_component(entity_manager, e3, ComponentType::StaticMesh);
	add_component(entity_manager, e3, ComponentType::Render);
	set_render_material(entity_manager, e3, editor->test_mat.material);
	set_static_mesh(entity_manager, e3, editor->api.asset_manager->sphere_mesh.mesh);

	set_position(entity_manager, editor->test_mesh, Vec3f(5, 0, 0));

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
	destroy_asset_importer(&editor->importer);
	arena_free(&editor->arena);
}




void editor_update(EditorInterface* editor) {

	
	Input* input = editor->api.input;
	GameTimer* timer = editor->api.game_loop;
	EntityManager* entity_manager = editor->api.entity_manager;

	Renderer* renderer = editor->api.renderer;
	Window* window = editor->api.window;

	Camera* camera = get_camera(entity_manager, editor->editor_camera);
	EditorControlsData* editor_control_data = &editor->editor_control_data;
	
	
	//  Used to style the editor
	ImGuiStyle* style = &ImGui::GetStyle();
	ShowStyleEditor(style);
	
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

	if (is_key_pressed(input, KEYCODE_BACKQUOTE)) {
		editor->show_editor = !editor->show_editor;
	}

	
	if (is_mouse_pressed(input, MouseButton::Left)) {
		Vec2i mouse_pos = get_mouse_pos(input);

		// Do a raycast to check which object is pressed
		//LOG_INFO("MOUSE", "Mouse pos %d, %d\n", mouse_pos.x, mouse_pos.y);
	}
	

	Vec2i scroll = get_scroll_delta(input);
	
	// Capture scolling to move camera forward and back
	if (scroll.y != 0) {
		Vec3f new_cam_direction = (delta_time * -forward(entity_manager, editor->editor_camera));

		// TODO: make this configurable
		float scroll_scale = 10.0f;

		float cam_move_scale = scroll_scale * scroll.y;
		Vec3f cam_pos = get_position(entity_manager, editor->editor_camera);
		set_position(entity_manager, editor->editor_camera, cam_pos + (cam_move_scale * new_cam_direction));
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
		//SDL_ShowCursor(SDL_DISABLE);
		//
		SDL_SetRelativeMouseMode(SDL_TRUE);
		//SDL_CaptureMouse(SDL_TRUE);

		Vec2i rel_pos = Vec2i(0, 0);


		SDL_GetRelativeMouseState(&rel_pos.x, &rel_pos.y);


		// TODO: remove need for sdl specific scan codes. convert to our own input api

		Vec3f new_cam_direction;


		// Since the camera always looks down -z
		if (is_key_down(input, KEYCODE_W)) { new_cam_direction += (delta_time * -forward(entity_manager, editor->editor_camera)); }
		if (is_key_down(input, KEYCODE_S)) { new_cam_direction += (delta_time * forward(entity_manager, editor->editor_camera)); }
		if (is_key_down(input, KEYCODE_D)) { new_cam_direction += (delta_time * right(entity_manager, editor->editor_camera)); }
		if (is_key_down(input, KEYCODE_A)) { new_cam_direction += (delta_time * -right(entity_manager, editor->editor_camera)); }
		if (is_key_down(input, KEYCODE_LSHIFT)) { new_cam_direction += (delta_time * up(entity_manager, editor->editor_camera)); }
		if (is_key_down(input, KEYCODE_LCTRL)) { new_cam_direction += (delta_time * -up(entity_manager, editor->editor_camera)); }

		float cam_move_scale = 10;
		Vec3f cam_pos = get_position(entity_manager, editor->editor_camera);
		set_position(entity_manager, editor->editor_camera, cam_pos + (cam_move_scale * new_cam_direction));


		// Prevent camera rotation jump, when the delta between the last time the right mouse was down and now
		if (editor->was_last_frame_using_right_click) {
			// TODO: this will be exposed to the user, we still need to implement proper control handling in engine
			float sensitivity = 0.25f;
			Quat old_cam_rot = get_rotation(entity_manager, editor->editor_camera);
			Quat new_cam_rot = quat_from_axis_angle(Vec3f_Up, -rel_pos.x * sensitivity) * old_cam_rot;
			new_cam_rot = new_cam_rot * quat_from_axis_angle(Vec3f_Right, -rel_pos.y * sensitivity);

			set_rotation(entity_manager, editor->editor_camera, new_cam_rot);
		}
		


		editor->was_last_frame_using_right_click = true;
	} else {
		editor->was_last_frame_using_right_click = false;
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}
		

	if (editor->show_editor) {

		set_editor_layout(editor);
		draw_main_menu_bar(editor);
		draw_toolbar(editor);
		draw_window_entity_components(editor);
		draw_window_scene_hierarchy(editor);
		draw_window_engine_timer(editor);
		draw_window_log(editor);
		draw_window_assets(editor);
		draw_window_scene_viewports(editor);
		draw_window_renderer_stats(editor);
		draw_editor_command_undo_and_redo_stack(editor);
	} // END OF SHOW EDITOR


	process_editor_command_buffer(editor);
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

			
			
			ImGui::MenuItem("Scene Tree", NULL, &editor->window_scene_tree_open);
			ImGui::MenuItem("Entity Components", NULL, &editor->window_entity_components_open);
			ImGui::MenuItem("Game Loop", NULL, &editor->window_engine_timers_open);
			ImGui::MenuItem("Log", NULL, &editor->window_log_open);
			ImGui::MenuItem("Asset Browser", NULL, &editor->window_asset_browser_open);
			ImGui::MenuItem("Render Stats", NULL, &editor->window_render_stats);
			
			
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

}

static void draw_toolbar(EditorInterface* editor) {
	bool open = true;
	auto flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar;
	if (ImGui::Begin("Toolbar", &open, flags)) {
		ImGui::Button("Save"); ImGui::SameLine();
		if (ImGui::Button("Undo")) {
			perform_undo_operation(editor);
		}
		ImGui::SameLine();
		if (ImGui::Button("Redo")) {
			perform_redo_operation(editor);
		}
		ImGui::SameLine();
		ImGui::Button("Translate"); ImGui::SameLine();
		ImGui::Button("Rotate"); ImGui::SameLine();
		ImGui::Button("Scale"); ImGui::SameLine();

		

		ImGui::Button("Play"); ImGui::SameLine();
	}
	ImGui::End();
}

static void draw_component_transform(EditorInterface* editor, Entity e) {
	EntityManager* entity_manager = editor->api.entity_manager;
	
	
	
	
	ImGui::PushID("transform_component");
	if (ImGui::CollapsingHeader("Transform", 0, ImGuiTreeNodeFlags_DefaultOpen)) {
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

		ImGui::PopID();
		

		
		
		
	} else {
		
	}

	ImGui::PopID();
}


static void draw_component_camera(EditorInterface* editor, Entity e) {
	EntityManager* entity_manager = editor->api.entity_manager;

	ImGui::PushID("camera_component");
	bool open = true;
	if (ImGui::CollapsingHeader("Camera", &open, ImGuiTreeNodeFlags_DefaultOpen)) {
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


		if (old_cam->projection == CameraProjection::Orthographic) {

			//ImGui::Separator();
			//ImGui::Text("Orthographic Settings");

			ImGui::PushID("camera_left_default");
			if (ImGui::SmallButton("z")) {
				new_camera.left = 0.0f;
				cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);
			}
			ImGui::SameLine();
			if (ImGui::DragFloat("Left", &new_camera.left, 1.0f, 0.0f, FLT_MAX)) {
				cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);
			}
			if (ImGui::IsItemDeactivatedAfterEdit()) {
				// Send this command when we are finished dragging
				// Once we are done dragging we want to a force a barrier between future drags,
				// so that multiple drags dont merge into one command
				cmd_editor_force_no_merge(editor);
			}
			ImGui::PopID();

			ImGui::PushID("camera_right_default");
			if (ImGui::SmallButton("z")) {
				new_camera.right = 100.0f;
				cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);
			}
			ImGui::SameLine();
			if (ImGui::DragFloat("Right", &new_camera.right, 1.0f, 0.0f, FLT_MAX)) {
				cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);
			}

			if (ImGui::IsItemDeactivatedAfterEdit()) {
				// Send this command when we are finished dragging
				// Once we are done dragging we want to a force a barrier between future drags,
				// so that multiple drags dont merge into one command
				cmd_editor_force_no_merge(editor);
			}
			ImGui::PopID();

			ImGui::PushID("camera_top_default");
			if (ImGui::SmallButton("z")) {
				new_camera.top = 0.0f;
				cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);
			}
			ImGui::SameLine();
			if (ImGui::DragFloat("Top", &new_camera.top, 1.0f, 0.0f, FLT_MAX)) {
				cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);
			}
			if (ImGui::IsItemDeactivatedAfterEdit()) {
				// Send this command when we are finished dragging
				// Once we are done dragging we want to a force a barrier between future drags,
				// so that multiple drags dont merge into one command
				cmd_editor_force_no_merge(editor);
			}
			ImGui::PopID();

			ImGui::PushID("camera_bottom_default");
			if (ImGui::SmallButton("z")) {
				new_camera.bottom = 100.0f;
				cmd_editor_set_camera_component(editor, e, *old_cam, new_camera, true);
			}
			ImGui::SameLine();

			if (ImGui::DragFloat("Bottom", &new_camera.bottom, 1.0f, 0.0f, FLT_MAX)) {
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

		set_camera(entity_manager, e, new_camera);
	}
	if (!open) {
		remove_component(entity_manager, e, ComponentType::Camera);
	}
	ImGui::PopID();
}


static void draw_component_light(EditorInterface* editor, Entity e) {
	EntityManager* entity_manager = editor->api.entity_manager;
	ImGui::PushID("light_component");
	bool open = true;
	if (ImGui::CollapsingHeader("Light", &open, ImGuiTreeNodeFlags_DefaultOpen)) {
		
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
		
		
	}

	if (!open) {
		remove_component(entity_manager, e, ComponentType::Light);
	}
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
	if (ImGui::CollapsingHeader("Static Mesh", &open, ImGuiTreeNodeFlags_DefaultOpen)) {
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
	}
	if (!open) {
		remove_component(entity_manager, e, ComponentType::StaticMesh);
	}
	ImGui::PopID();
}




static void draw_component_render(EditorInterface* editor, Entity e) {
	EntityManager* entity_manager = editor->api.entity_manager;
	ImGui::PushID("render_component");
	bool open = true;
	if (ImGui::CollapsingHeader("Render", &open, ImGuiTreeNodeFlags_DefaultOpen)) {
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
	}

	if (!open) {
		remove_component(entity_manager, e, ComponentType::Render);
	}
	ImGui::PopID();
}




static void draw_window_entity_components(EditorInterface* editor) {
	EntityManager* entity_manager = editor->api.entity_manager;

	if (!editor->window_entity_components_open) {
		return;
	}

	if (ImGui::Begin("Entity Components", &editor->window_entity_components_open)) {
		
		

		for (int i = 0; i < entity_manager->entity_count; i++) {
			Entity e = entity_manager->entity_list[i];
			MapResult<bool> result = map_get(&editor->entity_selected, e.id);
			if (!result.found) continue;
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

				if (has_component(entity_manager, e, ComponentType::Transform)) {
					draw_component_transform(editor, e);
				}

				if (has_component(entity_manager, e, ComponentType::Camera)) {
					draw_component_camera(editor, e);
				}

				if (has_component(entity_manager, e, ComponentType::Light)) {
					draw_component_light(editor, e);
				}

				if (has_component(entity_manager, e, ComponentType::StaticMesh)) {
					draw_component_static_mesh(editor, e);
				}
				if (has_component(entity_manager, e, ComponentType::Render)) {
					draw_component_render(editor, e);
				}
				ImGui::PopID();




				ImGui::Separator();
				ImGui::Spacing();

			}





		}
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

static void draw_window_scene_hierarchy(EditorInterface* editor) {
	if (!editor->window_scene_tree_open) {
		return;
	}
	EntityManager* entity_manager = editor->api.entity_manager;
	if (ImGui::Begin("Entity Scene Tree", &editor->window_scene_tree_open)) {
		
		
		
		ImGui::Text("Total Entitys Created %llu", entity_manager->entitys_created);
		ImGui::Text("Current Entity Count %llu", entity_manager->entity_count);
		ImGui::Text("Entity Selected Count %llu", editor->entity_selected_count);
		ImGui::Separator();


		//static char str0[128] = "";
		//ImGui::Text("Filter Entitys");
		//ImGui::InputText("", str0, IM_ARRAYSIZE(str0));
		

		editor->scene_tree_entity_filter.Draw("Filter");

		ImGui::Separator();

		if (editor->scene_tree_entity_filter.IsActive()) {
			// Draw as linear list when filtered
			u64 count = entity_manager->entity_count;
			for (int i = 0; i < count; i++) {
				Entity e = entity_manager->entity_list[i];
				String name = get_name(&editor->api.entity_manager->meta_manager, e);

				if (editor->scene_tree_entity_filter.PassFilter(name.buffer, name.buffer + name.length)) {
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


static void draw_window_engine_timer(EditorInterface* editor) {
	GameTimer* timer = editor->api.game_loop;
	if (!editor->window_engine_timers_open) {
		return;
	}
	
	if (ImGui::Begin("Game Loop", &editor->window_engine_timers_open)) {

		ImGui::Checkbox("Cap framerate", &timer->cap_framerate);
		if (timer->cap_framerate) {
			ImGui::SliderInt("Target Framerate", &timer->target_fps, 0, 240);
		}
		editor->fps_history[editor->fps_history_index] = timer->fps;
		editor->fps_history_index = ((editor->fps_history_index + 1) % FPS_HISTORY_COUNT);
		static char fps_text[32];
		snprintf(fps_text, 32, "FPS %d - dt %f", timer->fps, timer->delta_time);
		ImGui::PlotLines("Frame Times", editor->fps_history, IM_ARRAYSIZE(editor->fps_history), 0, fps_text, 0.0f, 120.0f, ImVec2(0, 140));


		ImGui::Text("Time %f", timer->seconds);
		ImGui::Text("Physics Time %f", timer->physics_time);
		ImGui::Text("Physics Time Step %f", timer->time_step);

		ImGui::Text("Frame Count %d", timer->frame_count);
		ImGui::Text("Ticks %llu", timer->ticks);
		ImGui::Text("Delta Ticks %llu", timer->delta_ticks);
		ImGui::Text("Ticks per sec %llu", timer->ticks_per_sec);

	}
	ImGui::End();

}

static void draw_window_log(EditorInterface* editor) {

	if (!editor->window_log_open) {
		return;
	}

	if (ImGui::Begin("Log", &editor->window_log_open)) {

		LogList logs = g_get_loglist();

		editor->log_filter.Draw("Filter");
		ImGui::SameLine();
		

		const float ItemSpacing = ImGui::GetStyle().ItemSpacing.x;
		static float HostButtonWidth = 100.0f; //The 100.0f is just a guess size for the first frame.
		float pos = HostButtonWidth + ItemSpacing;

		ImGui::SameLine(ImGui::GetWindowWidth() - pos);

		if (ImGui::Button("Clear")) {
			editor->log_start_offset = logs.log_count;
		}
		HostButtonWidth = ImGui::GetItemRectSize().x; //Get the actual width for next frame.

		ImGui::Separator();


		ImGui::Checkbox("Info", &editor->show_info);
		ImGui::SameLine();
		ImGui::Checkbox("Warning", &editor->show_warning);
		ImGui::SameLine();
		ImGui::Checkbox("Fatal", &editor->show_fatal);
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

		if (ImGui::Button("Options")) {
			ImGui::OpenPopup("log_options_popup");
		}
		HostButtonWidth2 = ImGui::GetItemRectSize().x; //Get the actual width for next frame.

		

		
		
		

		if (ImGui::BeginPopup("log_options_popup"))
		{
			ImGui::Checkbox("Auto Scroll", &editor->auto_scroll);
			ImGui::Separator();
			ImGui::Text("Column Filters");
			ImGui::Checkbox(str_time, &editor->show_time); ImGui::SameLine();
			ImGui::Checkbox(str_tag, &editor->show_tag); ImGui::SameLine();
			ImGui::Checkbox(str_thread_id, &editor->show_thread_id); ImGui::SameLine();
			ImGui::Checkbox(str_filename, &editor->show_filename); ImGui::SameLine();
			ImGui::Checkbox(str_function, &editor->show_function); ImGui::SameLine();
			ImGui::Checkbox(str_line, &editor->show_line); ImGui::SameLine();
			//ImGui::Checkbox(str_msg, &editor->show_message); ImGui::SameLine();
			ImGui::EndPopup();
		}

		ImGui::Spacing();
		ImGui::Separator();
		int col_count =
			editor->show_time
			+ editor->show_tag
			+ editor->show_thread_id
			+ editor->show_filename
			+ editor->show_function
			+ editor->show_line
			+ editor->show_message;

		ImGui::BeginGroup();
		ImGui::BeginChild(1);
		if (col_count != 0) {
			ImGui::Columns(col_count, "log_columns");
			if (editor->show_time) {
				ImGui::Text(str_time); ImGui::NextColumn();
			}
			if (editor->show_tag) {
				ImGui::Text(str_tag); ImGui::NextColumn();
			}
			if (editor->show_thread_id) {
				ImGui::Text(str_thread_id); ImGui::NextColumn();
			}
			if (editor->show_filename) {
				ImGui::Text(str_filename); ImGui::NextColumn();
			}
			if (editor->show_function) {
				ImGui::Text(str_function); ImGui::NextColumn();
			}
			if (editor->show_line) {
				ImGui::Text(str_line); ImGui::NextColumn();
			}

			if (editor->show_message) {
				ImGui::Text(str_msg); ImGui::NextColumn();
			}
			ImGui::Separator();

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 5));

			
				for (int i = editor->log_start_offset; i < logs.log_count; i++) {
					LogItem* log_item = &logs.logs[i];
					
					bool found = false;

					if (editor->log_filter.PassFilter(log_item->msg, log_item->msg + log_item->msg_length)) {
						found = true;
					}

					if (editor->log_filter.PassFilter(log_item->tag, log_item->tag + log_item->tag_length)) {
						found = true;
					}

					if (editor->log_filter.PassFilter(log_item->function, log_item->function + log_item->function_length)) {
						found = true;
					}

					if (editor->log_filter.PassFilter(log_item->filename, log_item->filename + log_item->filename_length)) {
						found = true;
					}
					if (!found) {
						continue;
					}
					
					
					bool pop_color = false;

					const char* log_type_str;
					switch (log_item->verbosity) {
						case LoggerVerbosity::INFO: {
							if (!editor->show_info) { continue; }
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); pop_color = true;
							log_type_str = "[INFO]"; break;
						}
						case LoggerVerbosity::WARN: {
							if (!editor->show_warning) { continue; }
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.90f, 0.25f, 1.0f)); pop_color = true;
							log_type_str = "[WARN]"; break;
						}
						case LoggerVerbosity::FATAL: {
							if (!editor->show_fatal) { continue; }
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

					if (editor->show_time) {
						ImGui::Text("%llu", log_item->time);
						ImGui::NextColumn();
					}
					if (editor->show_tag) {
						ImGui::Text(log_item->tag);
						ImGui::NextColumn();
					}
					if (editor->show_thread_id) {
						ImGui::Text("%d", log_item->thread_id);
						ImGui::NextColumn();
					}
					if (editor->show_filename) {
						ImGui::TextUnformatted(log_item->filename, log_item->filename + log_item->filename_length - 1);
						ImGui::NextColumn();
					}
					if (editor->show_function) {
						ImGui::TextUnformatted(log_item->function, log_item->function + log_item->function_length - 1);
						ImGui::NextColumn();
					}
					if (editor->show_line) {
						ImGui::Text("%d", log_item->line);
						ImGui::NextColumn();
					}

					if (editor->show_message) {
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

			if (editor->auto_scroll) {
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
static void push_assetbrowserfilenode_selected(AssetBrowserData* asset_browser, AssetBrowserFileNode* node, bool additive_select, bool selected) {
	if (!additive_select) {
		// if we are not a additive/multi selection, then we need to clear all the previously selected nodes
		recursive_deselect(asset_browser->root);
	}
	node->selected = selected;
}

static void push_asset_directory_change(AssetBrowserData* asset_browser, AssetBrowserFileNode* node) {
	asset_browser->current_directory = node;
}

static void draw_path_reverse(EditorInterface* editor, AssetTracker* tracker, AssetBrowserFileNode* leaf_node) {
	AssetBrowserData* asset_browser = &editor->asset_browser;
	if (leaf_node == NULL) {
		return;
	} 
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
		if (ImGui::BeginPopup("asset_path_popup")) {
			AssetBrowserFileNode* children_directories = leaf_node->first_child;
			while (children_directories != NULL) {
				if (children_directories->node_type == AssetBrowserFileNodeType::Directory) {
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
	AssetBrowserData* asset_browser = &editor->asset_browser;

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

static void draw_asset_tree(EditorInterface* editor, AssetTracker* tracker, AssetBrowserFileNode* node) {
	
	if (!should_draw_asset_tree(editor, node)) {
		return;
	}

	AssetBrowserData* asset_browser = &editor->asset_browser;
	
	

	if (node->node_type == AssetBrowserFileNodeType::Directory ) {

		if (node->has_child_directorys) {
			bool selected = asset_browser->current_directory == node;
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ((selected) ? ImGuiTreeNodeFlags_Selected : 0);
			bool open = ImGui::TreeNodeEx(node->name.buffer, node_flags);
			
			
			
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
					draw_asset_tree(editor, tracker, child);
					child = child->next_sibling;
				}

				ImGui::TreePop();
			}
		}
		else {
			ImGui::Indent();
			bool selected = asset_browser->current_directory == node;
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
	AssetBrowserData* asset_browser = &editor->asset_browser;

	AssetBrowserFileNode* node = asset_browser->current_directory->first_child;

	
	
	asset_browser->asset_browser_filter.Draw("Search");

	ImGui::Checkbox("Scene", &asset_browser->asset_scene_filter); ImGui::SameLine();
	ImGui::Checkbox("Static Mesh", &asset_browser->asset_mesh_filter); ImGui::SameLine();
	ImGui::Checkbox("Material", &asset_browser->asset_material_filter); ImGui::SameLine();
	ImGui::Checkbox("Texture", &asset_browser->asset_texture_filter); ImGui::SameLine();
	
	

	static int scale_count = 0;


	const float ItemSpacing3 = ImGui::GetStyle().ItemSpacing.x;
	static float HostButtonWidth3 = 100.0f; //The 100.0f is just a guess size for the first frame.
	float pos3 = HostButtonWidth3 + ItemSpacing3;
	
	ImGui::SameLine(ImGui::GetColumnWidth() - pos3);
	

	if (ImGui::Button("Options")) {
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

			
			if (node->node_type == AssetBrowserFileNodeType::Directory) {
				bool filter_pass = false;
				if (asset_browser->asset_browser_filter.PassFilter(node->name.buffer, node->name.buffer + node->name.length)) {
					filter_pass = true;
				}
				if (filter_pass) {
					ImGui::BeginGroup();
					bool selected = node->selected;


					
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
			else if (node->node_type == AssetBrowserFileNodeType::File) {


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
					if (ImGui::Selectable(node->name.buffer, &selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap | ImGuiSelectableFlags_AllowDoubleClick)) {
						
						if (ImGui::IsMouseDoubleClicked(0)) {
							// open asset viewer for that specific asset type
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
			
			if (node->node_type == AssetBrowserFileNodeType::File) {
				dragdrop_source_asset(editor, node->asset, node->name);
			}
			ImGui::Columns(1);
			node = node->next_sibling;
		}
	} else {
		
		// Tile View
		int buttons_count = 10;
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec2 button_sz(scale_count * 40, scale_count * 40);
		int n = 0;
		
		
		float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
		while (node != NULL) {

			bool filter_pass = false;
			void* icon_id = NULL;

			if (node->node_type == AssetBrowserFileNodeType::Directory) {
				if (asset_browser->asset_browser_filter.PassFilter(node->name.buffer, node->name.buffer + node->name.length)) {
					filter_pass = true;
					icon_id = render_resource_to_id(editor->api.renderer, editor->res_folder_icon_texture);
				}

			}
			else if (node->node_type == AssetBrowserFileNodeType::File) {

				if (asset_browser->asset_browser_filter.PassFilter(node->name.buffer, node->name.buffer + node->name.length)) {
					if (asset_browser->asset_scene_filter && node->asset.type == AssetType::Scene) { filter_pass = true; }
					else if (asset_browser->asset_mesh_filter && node->asset.type == AssetType::StaticMesh) { filter_pass = true; }
					else if (asset_browser->asset_material_filter && node->asset.type == AssetType::Material) { filter_pass = true; }
					else if (asset_browser->asset_texture_filter && node->asset.type == AssetType::Texture) { filter_pass = true; }

					icon_id = render_resource_to_id(editor->api.renderer, editor->res_asset_icon_texture);
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
				if (node->node_type == AssetBrowserFileNodeType::File) {
					dragdrop_source_asset(editor, node->asset, node->name);
				}

				if (ImGui::IsItemHovered()) {
					if (ImGui::IsMouseDoubleClicked(0)) {
						if (node->node_type == AssetBrowserFileNodeType::Directory) {
							// If this node is a directory, we want to go inside that directory
							push_asset_directory_change(asset_browser, node);
						}
						else if (node->node_type == AssetBrowserFileNodeType::File) {
							// If this node is a file, we want to open the file with a viewer
							
							
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

static void draw_window_assets(EditorInterface* editor) {
	AssetBrowserData* asset_browser = &editor->asset_browser;

	if (!editor->window_asset_browser_open) {
		return;
	}

	if (ImGui::Begin("Asset Browser", &editor->window_asset_browser_open)) {
		
		

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
		draw_path_reverse(editor, tracker, asset_browser->current_directory);
		ImGui::NewLine();
		ImGui::Separator();
		ImGui::PopID();

		

		ImGui::Columns(2, "asset_tree_and_browser", true);
		static float initial_spacing = 350.f; if (initial_spacing) ImGui::SetColumnWidth(0, initial_spacing), initial_spacing = 0;
		asset_browser->asset_tree_filter.Draw("Search Folders");
		ImGui::BeginChild("Asset Tree View");
		draw_asset_tree(editor, tracker, tracker->dir_root);
		ImGui::EndChild();
		ImGui::NextColumn();

		draw_asset_browser(editor, tracker);
		ImGui::Columns(1);


		


		
		






		
	
	}
	ImGui::End();
}

static void draw_window_renderer_stats(EditorInterface* editor) {

	
	if (!editor->window_render_stats) {
		return;
	}

	if (ImGui::Begin("Render Stats", &editor->window_render_stats)) {
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
		//ImGui::Text("Mesh Capacity: %d", renderer->render_world.render_mesh_capacity);
		
		
			
	}
	ImGui::End();
}

static void draw_window_scene_viewports(EditorInterface* editor) {


	void* color = render_resource_to_id(editor->api.renderer, editor->render_texture);
	void* depth = render_resource_to_id(editor->api.renderer, editor->depth_texture);
	
	
	
	
	if (ImGui::Begin("Scene")) {
		
		ImVec2 start_group_pos = ImGui::GetCursorScreenPos();
		// The size of the current dock/viewport
		ImVec2 window_size = ImGui::GetCurrentWindow()->Size;
		ImVec2 rect = ImVec2(start_group_pos.x + window_size.x, start_group_pos.y + window_size.y);

		// Need to update camera aspect ratio based on the dock's window size
		float aspect_ratio = (float)window_size.x / (float)window_size.y;
		set_camera_aspect_ratio(editor->api.entity_manager, editor->editor_camera, aspect_ratio);

		ImGui::GetWindowDrawList()->AddImage(color, start_group_pos, rect, ImVec2(0, 1), ImVec2(1, 0));
	}
	ImGui::End();

	if (ImGui::Begin("Game")) {		
		
		ImVec2 start_group_pos = ImGui::GetCursorScreenPos();
		ImVec2 viewport_size = ImGui::GetCurrentWindow()->Size;
		ImVec2 rect = ImVec2(start_group_pos.x + viewport_size.x, start_group_pos.y + viewport_size.y);

		ImGui::GetWindowDrawList()->AddImage(color, start_group_pos, rect, ImVec2(0, 1), ImVec2(1, 0));

		
	}
	ImGui::End();
	
	//if (ImGui::Begin("Multi Windows")) {
	//}
	//ImGui::End();

}

static void draw_editor_command_undo_and_redo_stack(EditorInterface* editor) {
	if (ImGui::Begin("Command History")) {
		
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


static void push_editor_undo_command(EditorInterface* editor, const EditorCommand& command) {

	
	editor->cmd_buffer.command_undo_stack[editor->cmd_buffer.command_undo_stack_count] = command;
	editor->cmd_buffer.command_undo_stack_count++;
	//editor->cmd_buffer.command_undo_stack_count = (editor->cmd_buffer.command_undo_stack_count + 1) % EDITOR_COMMAND_UNDO_BUFFER_CAPACITY;
	// Too many commands were sent this frame
	assert(editor->cmd_buffer.command_undo_stack_count < EDITOR_COMMAND_UNDO_REDO_BUFFER_CAPACITY);
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
	command.cmd.new_entity.material_id = editor->test_mat.material;

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
	command.cmd.new_entity.material_id = editor->test_mat.material;


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
	command.cmd.new_entity.material_id = editor->test_mat.material;


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