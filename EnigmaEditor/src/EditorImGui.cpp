#include "EditorImGui.h"

#include <misc/cpp/imgui_stdlib.h>
#include <imgui_internal.h>

namespace Enigma::Editor {

	// Create static EditorGui Data struct
	unique<EditorGui::Data> EditorGui::s_Data = CreateUnique<EditorGui::Data>();

	void EditorGui::SetStyle(const EditorStyle& style)
	{
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

		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF(style.regularFont.c_str(), style.fontSize);
		io.Fonts->AddFontFromFileTTF(style.italicFont.c_str(), style.fontSize);
		io.Fonts->AddFontFromFileTTF(style.boldFont.c_str(), style.fontSize);

		io.FontDefault = io.Fonts->Fonts[static_cast<i32>(style.defaultFont)];
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

		ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
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

		ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
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

		ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
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
	bool EditorGui::InputFloat(const std::string& lable, f32& value, f32 resetValue, f32 columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImFont* boldFont = io.Fonts->Fonts[static_cast<i32>(EditorFont::Bold)];

		ImGui::PushID(lable.c_str());

		bool edited = false;

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
		if (ImGui::Button("X", buttonSize)) {
			value = resetValue;
			edited = true;
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text("Reset the X field to %.2f", resetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##X", &value, 0.1f, 0.0f, 0.0f, "%.2f")) {
			edited = true;
		}
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::PopID();

		ImGui::EndColumns();

		return edited;
	}
	bool EditorGui::InputVec2(const std::string& lable, glm::vec2& value, f32 resetValue, f32 columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImFont* boldFont = io.Fonts->Fonts[static_cast<i32>(EditorFont::Bold)];

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
		if (ImGui::Button("X", buttonSize)) {
			value.x = resetValue;
			edited = true;
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text("Reset the X field to %.2f", resetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##X", &value.x, 0.1f, 0.0f, 0.0f, "%.2f")) {
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
			ImGui::Text("Reset the Y field to %.2f", resetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##Y", &value.y, 0.1f, 0.0f, 0.0f, "%.2f")) {
			edited = true;
		}
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::PopID();

		ImGui::EndColumns();

		return edited;
	}
	bool EditorGui::InputVec3(const std::string& lable, glm::vec3& value, f32 resetValue, f32 columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImFont* boldFont = io.Fonts->Fonts[static_cast<i32>(EditorFont::Bold)];

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
		if (ImGui::Button("X", buttonSize)) {
			value.x = resetValue;
			edited = true;
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text("Reset the X field to %.2f", resetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##X", &value.x, 0.1f, 0.0f, 0.0f, "%.2f")) {
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
			ImGui::Text("Reset the Y field to %.2f", resetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##Y", &value.y, 0.1f, 0.0f, 0.0f, "%.2f")) {
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
			ImGui::Text("Reset the Z field to %.2f", resetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##Z", &value.z, 0.1f, 0.0f, 0.0f, "%.2f")) {
			edited = true;
		}
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::PopID();

		ImGui::EndColumns();

		return edited;
	}
	bool EditorGui::InputVec4(const std::string& lable, glm::vec4& value, f32 resetValue, f32 columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImFont* boldFont = io.Fonts->Fonts[static_cast<i32>(EditorFont::Bold)];

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
		if (ImGui::Button("X", buttonSize)) {
			value.x = resetValue;
			edited = true;
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text("Reset the X field to %.2f", resetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##X", &value.x, 0.1f, 0.0f, 0.0f, "%.2f")) {
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
			ImGui::Text("Reset the Y field to %.2f", resetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##Y", &value.y, 0.1f, 0.0f, 0.0f, "%.2f")) {
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
			ImGui::Text("Reset the Z field to %.2f", resetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##Z", &value.z, 0.1f, 0.0f, 0.0f, "%.2f")) {
			edited = true;
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button,        ToImVec(s_Data->style.colorW));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ToImVec(s_Data->style.pressedColorW));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ToImVec(s_Data->style.colorW));
		ImGui::PushFont(boldFont);
		if (ImGui::Button("W", buttonSize)) {
			value.w = resetValue;
			edited = true;
		}
		if (ImGui::BeginItemTooltip()) {
			ImGui::Text("Reset the W field to %.2f", resetValue);
			ImGui::EndTooltip();
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##W", &value.w, 0.1f, 0.0f, 0.0f, "%.2f")) {
			edited = true;
		}
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
}