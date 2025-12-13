#include "EditorImGui.h"

#include <Enigma/Core/Core.h>
#include <Enigma/Core/Utilities/Utilities.h>

#include <EnigmaSerialization/Image.h>

#include <misc/cpp/imgui_stdlib.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>

namespace Enigma::Editor {

	std::string EntityDragSourceName = "DRAG_ENTITY_SOURCE";

	// Create static EditorGui Data struct
	unique<EditorGui::Data> EditorGui::s_Data = CreateUnique<EditorGui::Data>();

	void EditorGui::SetStyle(const EditorStyle& style)
	{
		// ImGui Style Config
		s_Data->style = style;

		ImVec4 (&colors)[60] = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg]           = ToImVec(style.windowBackground);
									        
		colors[ImGuiCol_Header]             = ToImVec(style.header);
		colors[ImGuiCol_HeaderHovered]      = ToImVec(style.headerHovered);
		colors[ImGuiCol_HeaderActive]       = ToImVec(style.headerActive);
									        
		colors[ImGuiCol_Button]             = ToImVec(style.button);
		colors[ImGuiCol_ButtonHovered]      = ToImVec(style.buttonHovered);
		colors[ImGuiCol_ButtonActive]       = ToImVec(style.buttonActive);
										    
		colors[ImGuiCol_FrameBg]            = ToImVec(style.frameBackground);
		colors[ImGuiCol_FrameBgHovered]     = ToImVec(style.frameBackgroundHovered);
		colors[ImGuiCol_FrameBgActive]      = ToImVec(style.frameBackgroundActive);

		colors[ImGuiCol_Tab]                = ToImVec(style.tab);
		colors[ImGuiCol_TabHovered]         = ToImVec(style.tabHovered);
		colors[ImGuiCol_TabActive]          = ToImVec(style.tabActive);
		colors[ImGuiCol_TabUnfocused]       = ToImVec(style.tabUnfocused);
		colors[ImGuiCol_TabUnfocusedActive] = ToImVec(style.tabUnfocusedActive);

		colors[ImGuiCol_TitleBg]            = ToImVec(style.titleBackground);
		colors[ImGuiCol_TitleBgActive]      = ToImVec(style.titleBackground);
		colors[ImGuiCol_TitleBgCollapsed]   = ToImVec(style.titleBackground);

		// ImGui Font Config
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF(style.fontPaths[EditorFont_Regular].c_str(), style.fontSize);
		io.Fonts->AddFontFromFileTTF(style.fontPaths[EditorFont_Italic].c_str(), style.fontSize);
		io.Fonts->AddFontFromFileTTF(style.fontPaths[EditorFont_Bold].c_str(), style.fontSize);

		io.FontDefault = io.Fonts->Fonts[static_cast<i32>(style.defaultFont)];

		// ImGuizmo Style Config
		ImGuizmo::Style gizmoStyle = ImGuizmo::GetStyle();
		gizmoStyle.Colors[ImGuizmo::COLOR::DIRECTION_X] = ToImVec(style.colorX);
		gizmoStyle.Colors[ImGuizmo::COLOR::DIRECTION_Y] = ToImVec(style.colorY);
		gizmoStyle.Colors[ImGuizmo::COLOR::DIRECTION_Z] = ToImVec(style.colorZ);

		gizmoStyle.Colors[ImGuizmo::COLOR::PLANE_X]     = ToImVec(style.colorX);
		gizmoStyle.Colors[ImGuizmo::COLOR::PLANE_Y]     = ToImVec(style.colorY);
		gizmoStyle.Colors[ImGuizmo::COLOR::PLANE_Z]     = ToImVec(style.colorZ);

		// Load Icons
		using namespace Serialization;
		ImageConfig iconConfig;
		iconConfig.minFilter = Renderer::TexFilterMode::Linear;
		s_Data->icons[EditorIcon_Menu]      = ImageLoader::Load(style.iconPaths[EditorIcon_Menu],      iconConfig);
		s_Data->icons[EditorIcon_Settings]  = ImageLoader::Load(style.iconPaths[EditorIcon_Settings],  iconConfig);
		s_Data->icons[EditorIcon_Translate] = ImageLoader::Load(style.iconPaths[EditorIcon_Translate], iconConfig);
		s_Data->icons[EditorIcon_Rotate]    = ImageLoader::Load(style.iconPaths[EditorIcon_Rotate],    iconConfig);
		s_Data->icons[EditorIcon_Scale]     = ImageLoader::Load(style.iconPaths[EditorIcon_Scale],     iconConfig);
		s_Data->icons[EditorIcon_Play]      = ImageLoader::Load(style.iconPaths[EditorIcon_Play],      iconConfig);
		s_Data->icons[EditorIcon_Pause]     = ImageLoader::Load(style.iconPaths[EditorIcon_Pause],     iconConfig);

	}

	EditorStyle& EditorGui::GetStyle() {
		return s_Data->style;
	}

	ref<Renderer::Texture> EditorGui::GetIcon(EditorIcon icon) {
		return s_Data->icons[icon];
	}

	void EditorGui::TintStyle(const f32& tint)
	{
		s_Data->style.windowBackground *= tint;

		s_Data->style.header        *= tint;
		s_Data->style.headerHovered *= tint;
		s_Data->style.headerActive  *= tint;
								    
		s_Data->style.button        *= tint;
		s_Data->style.buttonHovered *= tint;
		s_Data->style.buttonActive  *= tint;

		// Frame Background Colors
		s_Data->style.frameBackground        *= tint;
		s_Data->style.frameBackgroundHovered *= tint;
		s_Data->style.frameBackgroundActive  *= tint;

		// Tab Colors
		s_Data->style.tab                *= tint;
		s_Data->style.tabHovered         *= tint;
		s_Data->style.tabActive          *= tint;
		s_Data->style.tabUnfocused       *= tint;
		s_Data->style.tabUnfocusedActive *= tint;

		// Title Background Colors
		s_Data->style.titleBackground          *= tint;
		s_Data->style.titleBackgroundActive    *= tint;
		s_Data->style.titleBackgroundCollapsed *= tint;

		// Float Input Colors
		s_Data->style.colorX *= tint;
		s_Data->style.colorY *= tint;
		s_Data->style.colorZ *= tint;
		s_Data->style.colorW *= tint;

		s_Data->style.pressedColorX *= tint;
		s_Data->style.pressedColorY *= tint;
		s_Data->style.pressedColorZ *= tint;
		s_Data->style.pressedColorW *= tint;

		s_Data->tint = tint;
		SetStyle(s_Data->style);
	}
	void EditorGui::RemoveStyleTint()
	{
		s_Data->style.windowBackground /= s_Data->tint;

		s_Data->style.header        /= s_Data->tint;
		s_Data->style.headerHovered /= s_Data->tint;
		s_Data->style.headerActive  /= s_Data->tint;

		s_Data->style.button        /= s_Data->tint;
		s_Data->style.buttonHovered /= s_Data->tint;
		s_Data->style.buttonActive  /= s_Data->tint;

		// Frame Background Colors
		s_Data->style.frameBackground        /= s_Data->tint;
		s_Data->style.frameBackgroundHovered /= s_Data->tint;
		s_Data->style.frameBackgroundActive  /= s_Data->tint;

		// Tab Colors
		s_Data->style.tab                /= s_Data->tint;
		s_Data->style.tabHovered         /= s_Data->tint;
		s_Data->style.tabActive          /= s_Data->tint;
		s_Data->style.tabUnfocused       /= s_Data->tint;
		s_Data->style.tabUnfocusedActive /= s_Data->tint;

		// Title Background Colors
		s_Data->style.titleBackground          /= s_Data->tint;
		s_Data->style.titleBackgroundActive    /= s_Data->tint;
		s_Data->style.titleBackgroundCollapsed /= s_Data->tint;

		// Float Input Colors
		s_Data->style.colorX /= s_Data->tint;
		s_Data->style.colorY /= s_Data->tint;
		s_Data->style.colorZ /= s_Data->tint;
		s_Data->style.colorW /= s_Data->tint;

		s_Data->style.pressedColorX /= s_Data->tint;
		s_Data->style.pressedColorY /= s_Data->tint;
		s_Data->style.pressedColorZ /= s_Data->tint;
		s_Data->style.pressedColorW /= s_Data->tint;

		s_Data->tint = 1.0f;
		SetStyle(s_Data->style);
	}

	// Int Inputs =======================
	bool EditorGui::InputInt(const std::string& lable, i32& value, i32 resetValue, f32 columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImFont* boldFont = io.Fonts->Fonts[EditorFont_Bold];

		ImGui::PushID(lable.c_str());

		bool edited = false;

		ImGui::BeginColumns(("##Columns_" + lable).c_str(), 2, ImGuiOldColumnFlags_NoResize);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text("%s", lable.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		f32 lineHeight = s_Data->style.fontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button,        ToImVec(s_Data->style.colorX));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ToImVec(s_Data->style.pressedColorX));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ToImVec(s_Data->style.colorX));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize)) {
			value = resetValue;
			edited = true;
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text("Reset the X field to %i", resetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::InputInt("##X", &value, 1, 100)) {
			edited = true;
		}
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::PopID();

		ImGui::EndColumns();

		return edited;
	}
	bool EditorGui::InputInt2(const std::string& lable, glm::ivec2& value, i32 resetValue, f32 columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImFont* boldFont = io.Fonts->Fonts[EditorFont_Bold];

		bool edited = false;

		ImGui::PushID(lable.c_str());

		ImGui::BeginColumns(("##Columns_" + lable).c_str(), 2, ImGuiOldColumnFlags_NoResize);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text("%s", lable.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		f32 lineHeight = s_Data->style.fontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button,        ToImVec(s_Data->style.colorX));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ToImVec(s_Data->style.pressedColorX));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ToImVec(s_Data->style.colorX));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize)) {
			value.x = resetValue;
			edited = true;
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text("Reset the X field to %i", resetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::InputInt("##X", &value.x, 1, 100)) {
			edited = true;
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button,        ToImVec(s_Data->style.colorY));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ToImVec(s_Data->style.pressedColorY));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ToImVec(s_Data->style.colorY));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize)) {
			value.y = resetValue;
			edited = true;
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text("Reset the Y field to %i", resetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::InputInt("##Y", &value.y, 1, 100)) {
			edited = true;
		}
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::PopID();

		ImGui::EndColumns();

		return edited;
	}
	bool EditorGui::InputInt3(const std::string& lable, glm::ivec3& value, i32 resetValue, f32 columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImFont* boldFont = io.Fonts->Fonts[EditorFont_Bold];

		bool edited = false;

		ImGui::PushID(lable.c_str());

		ImGui::BeginColumns(("##Columns_" + lable).c_str(), 2, ImGuiOldColumnFlags_NoResize);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text("%s", lable.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		f32 lineHeight = s_Data->style.fontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button,        ToImVec(s_Data->style.colorX));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ToImVec(s_Data->style.pressedColorX));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ToImVec(s_Data->style.colorX));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize)) {
			value.x = resetValue;
			edited = true;
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text("Reset the X field to %i", resetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::InputInt("##X", &value.x, 1, 100)) {
			edited = true;
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button,        ToImVec(s_Data->style.colorY));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ToImVec(s_Data->style.pressedColorY));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ToImVec(s_Data->style.colorY));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize)) {
			value.y = resetValue;
			edited = true;
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text("Reset the Y field to %i", resetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::InputInt("##Y", &value.y, 1, 100)) {
			edited = true;
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button,        ToImVec(s_Data->style.colorZ));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ToImVec(s_Data->style.pressedColorZ));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ToImVec(s_Data->style.colorZ));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize)) {
			value.z = resetValue;
			edited = true;
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text("Reset the Z field to %i", resetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::InputInt("##Z", &value.z, 1, 100)) {
			edited = true;
		}
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::PopID();

		ImGui::EndColumns();

		return edited;
	}

	// Float Inputs =====================
	bool EditorGui::InputFloat(const std::string& lable, f32& value, f32 resetValue, f32 columnWidth, u32 floatPrecision)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImFont* boldFont = io.Fonts->Fonts[EditorFont_Bold];

		std::string floatPrecisionStr = "%." + std::to_string(floatPrecision) + "f";
		std::string tooltipTextStr = "Reset the %s field to " + floatPrecisionStr;
		const char* floatPrecisionCStr = floatPrecisionStr.c_str();
		const char* tooltipTextCStr = tooltipTextStr.c_str();

		ImGui::PushID(lable.c_str());

		bool edited = false;

		ImGui::BeginColumns(("##Columns_" + lable).c_str(), 2, ImGuiOldColumnFlags_NoResize);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text("%s", lable.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		f32 lineHeight = s_Data->style.fontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button,        ToImVec(s_Data->style.colorX));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ToImVec(s_Data->style.pressedColorX));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ToImVec(s_Data->style.colorX));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize) && resetValue != value) {
			value = resetValue;
			edited = true;
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text(tooltipTextCStr, "X", resetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &value, 0.1f, 0.0f, 0.0f, floatPrecisionCStr);
		if (ImGui::IsItemDeactivatedAfterEdit()) edited = true;

		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::PopID();

		ImGui::EndColumns();

		return edited;
	}
	bool EditorGui::InputVec2(const std::string& lable, glm::vec2& value, f32 resetValue, f32 columnWidth, u32 floatPrecision)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImFont* boldFont = io.Fonts->Fonts[EditorFont_Bold];

		std::string floatPrecisionStr = "%." + std::to_string(floatPrecision) + "f";
		std::string tooltipTextStr = "Reset the %s field to " + floatPrecisionStr;
		const char* floatPrecisionCStr = floatPrecisionStr.c_str();
		const char* tooltipTextCStr = tooltipTextStr.c_str();

		bool edited = false;

		ImGui::PushID(lable.c_str());

		ImGui::BeginColumns(("##Columns_" + lable).c_str(), 2, ImGuiOldColumnFlags_NoResize);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text("%s", lable.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		f32 lineHeight = s_Data->style.fontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button,        ToImVec(s_Data->style.colorX));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ToImVec(s_Data->style.pressedColorX));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ToImVec(s_Data->style.colorX));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize) && resetValue != value.x) {
			value.x = resetValue;
			edited = true;
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text(tooltipTextCStr, "X", resetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &value.x, 0.1f, 0.0f, 0.0f, floatPrecisionCStr);
		if (ImGui::IsItemDeactivatedAfterEdit()) edited = true;

		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button,        ToImVec(s_Data->style.colorY));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ToImVec(s_Data->style.pressedColorY));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ToImVec(s_Data->style.colorY));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize) && resetValue != value.y) {
			value.y = resetValue;
			edited = true;
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text(tooltipTextCStr, "Y", resetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &value.y, 0.1f, 0.0f, 0.0f, floatPrecisionCStr);
		if (ImGui::IsItemDeactivatedAfterEdit()) edited = true;

		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::PopID();

		ImGui::EndColumns();

		return edited;
	}
	bool EditorGui::InputVec3(const std::string& lable, glm::vec3& value, f32 resetValue, f32 columnWidth, u32 floatPrecision)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImFont* boldFont = io.Fonts->Fonts[EditorFont_Bold];

		std::string floatPrecisionStr = "%." + std::to_string(floatPrecision) + "f";
		std::string tooltipTextStr = "Reset the %s field to " + floatPrecisionStr;
		const char* floatPrecisionCStr = floatPrecisionStr.c_str();
		const char* tooltipTextCStr = tooltipTextStr.c_str();

		bool edited = false;

		ImGui::PushID(lable.c_str());

		ImGui::BeginColumns(("##Columns_" + lable).c_str(), 2, ImGuiOldColumnFlags_NoResize);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text("%s", lable.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		f32 lineHeight = s_Data->style.fontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button,        ToImVec(s_Data->style.colorX));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ToImVec(s_Data->style.pressedColorX));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ToImVec(s_Data->style.colorX));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize) && resetValue != value.x) {
			value.x = resetValue;
			edited = true;
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text(tooltipTextCStr, "X", resetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &value.x, 0.1f, 0.0f, 0.0f, floatPrecisionCStr);
		if (ImGui::IsItemDeactivatedAfterEdit()) edited = true;
		
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button,        ToImVec(s_Data->style.colorY));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ToImVec(s_Data->style.pressedColorY));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ToImVec(s_Data->style.colorY));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize) && resetValue != value.y) {
			value.y = resetValue;
			edited = true;
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text(tooltipTextCStr, "Y", resetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &value.y, 0.1f, 0.0f, 0.0f, floatPrecisionCStr);
		if (ImGui::IsItemDeactivatedAfterEdit()) edited = true;

		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button,        ToImVec(s_Data->style.colorZ));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ToImVec(s_Data->style.pressedColorZ));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ToImVec(s_Data->style.colorZ));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize) && resetValue != value.z) {
			value.z = resetValue;
			edited = true;
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text(tooltipTextCStr, "Z", resetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &value.z, 0.1f, 0.0f, 0.0f, floatPrecisionCStr);
		if (ImGui::IsItemDeactivatedAfterEdit()) edited = true;

		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::PopID();

		ImGui::EndColumns();

		return edited;
	}
	bool EditorGui::InputVec4(const std::string& lable, glm::vec4& value, f32 resetValue, f32 columnWidth, u32 floatPrecision)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImFont* boldFont = io.Fonts->Fonts[EditorFont_Bold];

		std::string floatPrecisionStr = "%." + std::to_string(floatPrecision) + "f";
		std::string tooltipTextStr = "Reset the %s field to " + floatPrecisionStr;
		const char* floatPrecisionCStr = floatPrecisionStr.c_str();
		const char* tooltipTextCStr = tooltipTextStr.c_str();

		bool edited = false;

		ImGui::PushID(lable.c_str());

		ImGui::BeginColumns(("##Columns_" + lable).c_str(), 2, ImGuiOldColumnFlags_NoResize);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text("%s", lable.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		f32 lineHeight = s_Data->style.fontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button,        ToImVec(s_Data->style.colorX));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ToImVec(s_Data->style.pressedColorX));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ToImVec(s_Data->style.colorX));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize) && resetValue != value.x) {
			value.x = resetValue;
			edited = true;
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text(tooltipTextCStr, "X", resetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &value.x, 0.1f, 0.0f, 0.0f, floatPrecisionCStr);
		if (ImGui::IsItemDeactivatedAfterEdit()) edited = true;

		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button,        ToImVec(s_Data->style.colorY));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ToImVec(s_Data->style.pressedColorY));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ToImVec(s_Data->style.colorY));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize) && resetValue != value.y) {
			value.y = resetValue;
			edited = true;
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text(tooltipTextCStr, "Y", resetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &value.y, 0.1f, 0.0f, 0.0f, floatPrecisionCStr);
		if (ImGui::IsItemDeactivatedAfterEdit()) edited = true;

		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button,        ToImVec(s_Data->style.colorZ));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ToImVec(s_Data->style.pressedColorZ));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ToImVec(s_Data->style.colorZ));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize) && resetValue != value.z) {
			value.z = resetValue;
			edited = true;
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text(tooltipTextCStr, "Z", resetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &value.z, 0.1f, 0.0f, 0.0f, floatPrecisionCStr);
		if (ImGui::IsItemDeactivatedAfterEdit()) edited = true;

		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button,        ToImVec(s_Data->style.colorW));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ToImVec(s_Data->style.pressedColorW));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ToImVec(s_Data->style.colorW));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("W", buttonSize) && resetValue != value.w) {
			value.w = resetValue;
			edited = true;
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text(tooltipTextCStr, "W", resetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##W", &value.w, 0.1f, 0.0f, 0.0f, floatPrecisionCStr);
		if (ImGui::IsItemDeactivatedAfterEdit()) edited = true;

		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::PopID();

		ImGui::EndColumns();

		return edited;
	}

	// Misc Inputs ======================
	bool EditorGui::CheckBox(const std::string& lable, bool& value, f32 columnWidth)
	{
		bool edited = false;

		ImGui::PushID(lable.c_str());

		ImGui::BeginColumns(("##Columns_" + lable).c_str(), 2, ImGuiOldColumnFlags_NoResize);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text("%s", lable.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		if (ImGui::Checkbox("##CheckBox", &value)) {
			edited = true;
		}

		ImGui::PopStyleVar();
		ImGui::PopID();

		ImGui::EndColumns();

		return edited;
	}
	bool EditorGui::InputText(const std::string& lable, std::string& value, f32 columnWidth)
	{
		bool edited = false;

		ImGui::PushID(lable.c_str());

		ImGui::BeginColumns(("##Columns_" + lable).c_str(), 2, ImGuiOldColumnFlags_NoResize);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text("%s", lable.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		if (ImGui::InputText("##InputText", &value)) {
			edited = true;
		}
		
		ImGui::PopStyleVar();
		ImGui::PopID();

		ImGui::EndColumns();

		return edited;
	}
	void EditorGui::Text(const std::string& lable, const std::string& value, f32 columnWidth)
	{
		ImGui::PushID(lable.c_str());

		ImGui::BeginColumns(("##Columns_" + lable).c_str(), 2, ImGuiOldColumnFlags_NoResize);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text("%s", lable.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::Text("%s", value.c_str());
		ImGui::PopStyleVar();

		ImGui::EndColumns();
		ImGui::PopID();
	}
	bool EditorGui::RenamableText(std::string& text, const std::string& textID, bool* renaming, std::string* original)
	{
		// Calculate the ID
		u64 id = ImGui::GetCurrentWindow()->ID;
		id ^= Core::Hash(textID);

		ImGui::PushID(textID.c_str());

		// Init data
		if (!s_Data->itemData.count(id)) {
			RenamableTextData* data = new RenamableTextData;
			data->started = false;
			data->ended = true;
			s_Data->itemData.insert({ id, data });
		}
		RenamableTextData* data = static_cast<RenamableTextData*>(s_Data->itemData[id]);

		bool edited = false;

		if (renaming != nullptr) *renaming = !data->ended;

		// Show text
		if (data->ended) {
			ImGui::Text("%s", text.c_str());
			// start renaming if double clicked
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				data->started = true;
				data->ended = false;
				data->original = text;
			}
		}
		// Show text box for renaming
		else {
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ToImVec(s_Data->style.windowBackground));

			// Set Keyboard focus on the text box
			if (data->started) {
				ImGui::SetKeyboardFocusHere();
				data->started = false;
			}

			// Show text box
			ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;
			std::string buffer = text;
			if (ImGui::InputText(("##" + textID).c_str(), &buffer, inputFlags)) {
				text = buffer;
				data->ended = true;
			}
			if (ImGui::IsItemDeactivated()) {
				text = buffer;
				data->ended = true;
			}
			if (data->ended) {
				if (text != data->original) edited = true;
				if (original != nullptr) *original = data->original;
				data->original = "";
			}

			ImGui::PopStyleColor();
			ImGui::PopStyleVar();
		}

		ImGui::PopID();

		return edited;
	}

	bool EditorGui::ListBox(const std::string& lable, i32& selection, std::vector<std::string>& items, f32 columnWidth)
	{
		bool edited = false;

		ImGui::PushID(lable.c_str());

		ImGui::BeginColumns(("##Columns_" + lable).c_str(), 2, ImGuiOldColumnFlags_NoResize);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text("%s", lable.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		
		if (ImGui::ListBox("##ListBox", &selection, [](void* user_data, int idx) {
			std::vector<std::string>* items = static_cast<std::vector<std::string>*>(user_data);
			return items->at(idx).c_str();
		}, static_cast<void*>(&items), items.size())) {
			edited = true;
		}

		ImGui::PopStyleVar();
		ImGui::PopID();

		ImGui::EndColumns();

		return edited;
	}
	bool EditorGui::ComboBox(const std::string& lable, i32& selection, std::vector<std::string>& items, f32 columnWidth)
	{
		bool edited = false;

		ImGui::PushID(lable.c_str());

		ImGui::BeginColumns(("##Columns_" + lable).c_str(), 2, ImGuiOldColumnFlags_NoResize);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text("%s", lable.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		if (ImGui::Combo("##ComboBox", &selection, [](void* user_data, int idx) {
			std::vector<std::string>* items = static_cast<std::vector<std::string>*>(user_data);
			return items->at(idx).c_str();
			}, static_cast<void*>(&items), items.size())) {
			edited = true;
		}

		ImGui::PopStyleVar();
		ImGui::PopID();

		ImGui::EndColumns();

		return edited;
	}
	
	bool EditorGui::InputColor(const std::string& lable, glm::vec3& value, f32 resetValue, f32 columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImFont* boldFont = io.Fonts->Fonts[EditorFont_Bold];

		glm::vec3 rgbValue = value * 255.0f;
		float rgbResetValue = resetValue * 255.0f;

		bool edited = false;

		ImGui::PushID(lable.c_str());

		ImGui::BeginColumns(("##Columns_" + lable).c_str(), 2, ImGuiOldColumnFlags_NoResize);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text("%s", lable.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		f32 lineHeight = s_Data->style.fontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ToImVec(s_Data->style.colorX));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ToImVec(s_Data->style.pressedColorX));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ToImVec(s_Data->style.colorX));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("R", buttonSize)) {
			rgbValue.x = rgbResetValue;
			edited = true;
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text("Reset the Red field to %.2f", rgbResetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##R", &rgbValue.r, 0.1f, 0.0f, 0.0f, "%.2f");
		if (ImGui::IsItemDeactivatedAfterEdit()) edited = true;

		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ToImVec(s_Data->style.colorY));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ToImVec(s_Data->style.pressedColorY));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ToImVec(s_Data->style.colorY));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("G", buttonSize)) {
			rgbValue.y = rgbResetValue;
			edited = true;
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text("Reset the Green field to %.2f", rgbResetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##G", &rgbValue.g, 0.1f, 0.0f, 0.0f, "%.2f");
		if (ImGui::IsItemDeactivatedAfterEdit()) edited = true;

		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ToImVec(s_Data->style.colorZ));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ToImVec(s_Data->style.pressedColorZ));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ToImVec(s_Data->style.colorZ));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("B", buttonSize)) {
			rgbValue.z = rgbResetValue;
			edited = true;
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text("Reset the Blue field to %.2f", rgbResetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##B", &rgbValue.b, 0.1f, 0.0f, 0.0f, "%.2f"));
		if (ImGui::IsItemDeactivatedAfterEdit()) edited = true;

		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::PopID();

		ImGui::SameLine();
		ImGui::ColorEdit3("##ColorPicker", &value.r, ImGuiColorEditFlags_NoInputs);
		if (ImGui::IsItemDeactivatedAfterEdit()) edited = true;

		ImGui::EndColumns();

		if (edited) {
			value = rgbValue / 255.0f;
		}

		return edited;
	}
	
}