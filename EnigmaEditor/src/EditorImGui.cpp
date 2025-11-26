#include "EditorImGui.h"

#include <misc/cpp/imgui_stdlib.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>

namespace Enigma::Editor {

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
		io.Fonts->AddFontFromFileTTF(style.regularFont.c_str(), style.fontSize);
		io.Fonts->AddFontFromFileTTF(style.italicFont.c_str(), style.fontSize);
		io.Fonts->AddFontFromFileTTF(style.boldFont.c_str(), style.fontSize);

		io.FontDefault = io.Fonts->Fonts[static_cast<i32>(style.defaultFont)];

		// ImGuizmo Style Config
		ImGuizmo::Style gizmoStyle = ImGuizmo::GetStyle();
		gizmoStyle.Colors[ImGuizmo::COLOR::DIRECTION_X] = ToImVec(style.colorX);
		gizmoStyle.Colors[ImGuizmo::COLOR::DIRECTION_Y] = ToImVec(style.colorY);
		gizmoStyle.Colors[ImGuizmo::COLOR::DIRECTION_Z] = ToImVec(style.colorZ);

		gizmoStyle.Colors[ImGuizmo::COLOR::PLANE_X]     = ToImVec(style.colorX);
		gizmoStyle.Colors[ImGuizmo::COLOR::PLANE_Y]     = ToImVec(style.colorY);
		gizmoStyle.Colors[ImGuizmo::COLOR::PLANE_Z]     = ToImVec(style.colorZ);
	}

	// Int Inputs =======================
	bool EditorGui::InputInt(const std::string& lable, i32& value, i32 resetValue, f32 columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImFont* boldFont = io.Fonts->Fonts[static_cast<i32>(EditorFont::Bold)];

		ImGui::PushID(lable.c_str());

		bool edited = false;

		ImGui::Columns(2);
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
		ImFont* boldFont = io.Fonts->Fonts[static_cast<i32>(EditorFont::Bold)];

		bool edited = false;

		ImGui::PushID(lable.c_str());

		ImGui::Columns(2);
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
		ImFont* boldFont = io.Fonts->Fonts[static_cast<i32>(EditorFont::Bold)];

		bool edited = false;

		ImGui::PushID(lable.c_str());

		ImGui::Columns(2);
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
		ImFont* boldFont = io.Fonts->Fonts[static_cast<i32>(EditorFont::Bold)];

		std::string floatPrecisionStr = "%." + std::to_string(floatPrecision) + "f";
		std::string tooltipTextStr = "Reset the %s field to " + floatPrecisionStr;
		const char* floatPrecisionCStr = floatPrecisionStr.c_str();
		const char* tooltipTextCStr = tooltipTextStr.c_str();

		ImGui::PushID(lable.c_str());

		bool edited = false;

		ImGui::Columns(2);
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
		ImFont* boldFont = io.Fonts->Fonts[static_cast<i32>(EditorFont::Bold)];

		std::string floatPrecisionStr = "%." + std::to_string(floatPrecision) + "f";
		std::string tooltipTextStr = "Reset the %s field to " + floatPrecisionStr;
		const char* floatPrecisionCStr = floatPrecisionStr.c_str();
		const char* tooltipTextCStr = tooltipTextStr.c_str();

		bool edited = false;

		ImGui::PushID(lable.c_str());

		ImGui::Columns(2);
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
		ImFont* boldFont = io.Fonts->Fonts[static_cast<i32>(EditorFont::Bold)];

		std::string floatPrecisionStr = "%." + std::to_string(floatPrecision) + "f";
		std::string tooltipTextStr = "Reset the %s field to " + floatPrecisionStr;
		const char* floatPrecisionCStr = floatPrecisionStr.c_str();
		const char* tooltipTextCStr = tooltipTextStr.c_str();

		bool edited = false;

		ImGui::PushID(lable.c_str());

		ImGui::Columns(2);
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
		ImFont* boldFont = io.Fonts->Fonts[static_cast<i32>(EditorFont::Bold)];

		std::string floatPrecisionStr = "%." + std::to_string(floatPrecision) + "f";
		std::string tooltipTextStr = "Reset the %s field to " + floatPrecisionStr;
		const char* floatPrecisionCStr = floatPrecisionStr.c_str();
		const char* tooltipTextCStr = tooltipTextStr.c_str();

		bool edited = false;

		ImGui::PushID(lable.c_str());

		ImGui::Columns(2);
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

		ImGui::Columns(2);
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

		ImGui::Columns(2);
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
	bool EditorGui::ListBox(const std::string& lable, i32& selection, std::vector<std::string>& items, f32 columnWidth)
	{
		bool edited = false;

		ImGui::PushID(lable.c_str());

		ImGui::Columns(2);
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

		ImGui::Columns(2);
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
		ImFont* boldFont = io.Fonts->Fonts[static_cast<i32>(EditorFont::Bold)];

		glm::vec3 rgbValue = value * 255.0f;
		float rgbResetValue = resetValue * 255.0f;

		bool edited = false;

		ImGui::PushID(lable.c_str());

		ImGui::Columns(2);
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