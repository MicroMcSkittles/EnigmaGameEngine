#pragma once
#include <Enigma/Core/Types.h>
#include <Enigma/Renderer/Texture.h>

#include <string>
#include <vector>
#include <array>

#include <imgui.h>

#include <glm/glm.hpp>

#include "Scene/Scene.h"
#include "Scene/Entity.h"

namespace Enigma::Editor {

	enum EditorFont {
		EditorFont_Regular = 0,
		EditorFont_Italic,
		EditorFont_Bold,
		EditorFont_Last
	};
	enum EditorIcon {
		EditorIcon_Menu = 0,
		EditorIcon_Settings,
		EditorIcon_Translate,
		EditorIcon_Rotate,
		EditorIcon_Scale,
		EditorIcon_Play,
		EditorIcon_Pause,
		EditorIcon_Last
	};
	struct EditorStyle {
		glm::vec4 windowBackground         = glm::vec4(0.1f, 0.105f, 0.11f, 1.0f);

		// Header Colors
		glm::vec4 header                   = glm::vec4(0.2f,  0.205f,  0.21f,  1.0f);
		glm::vec4 headerHovered            = glm::vec4(0.3f,  0.305f,  0.31f,  1.0f);
		glm::vec4 headerActive             = glm::vec4(0.15f, 0.1505f, 0.151f, 1.0f);

		// Button Colors
		glm::vec4 button                   = glm::vec4(0.2f,  0.205f,  0.21f,  1.0f);
		glm::vec4 buttonHovered            = glm::vec4(0.3f,  0.305f,  0.31f,  1.0f);
		glm::vec4 buttonActive             = glm::vec4(0.15f, 0.1505f, 0.151f, 1.0f);

		// Frame Background Colors
		glm::vec4 frameBackground          = glm::vec4(0.2f,  0.205f,  0.21f,  1.0f);
		glm::vec4 frameBackgroundHovered   = glm::vec4(0.3f,  0.305f,  0.31f,  1.0f);
		glm::vec4 frameBackgroundActive    = glm::vec4(0.15f, 0.1505f, 0.151f, 1.0f);

		// Tab Colors
		glm::vec4 tab                      = glm::vec4(0.15f, 0.1505f, 0.151f, 1.0f);
		glm::vec4 tabHovered               = glm::vec4(0.38f, 0.3805f, 0.381f, 1.0f);
		glm::vec4 tabActive                = glm::vec4(0.28f, 0.2805f, 0.281f, 1.0f);
		glm::vec4 tabUnfocused             = glm::vec4(0.15f, 0.1505f, 0.151f, 1.0f);
		glm::vec4 tabUnfocusedActive       = glm::vec4(0.2f,  0.205f,  0.21f,  1.0f);

		// Title Background Colors
		glm::vec4 titleBackground          = glm::vec4(0.15f, 0.1505f, 0.151f, 1.0f);
		glm::vec4 titleBackgroundActive    = glm::vec4(0.15f, 0.1505f, 0.151f, 1.0f);
		glm::vec4 titleBackgroundCollapsed = glm::vec4(0.95f, 0.1505f, 0.951f, 1.0f);

		// Float Input Colors
		glm::vec4 colorX                   = glm::vec4(0.8f, 0.1f, 0.15f, 1.0f);
		glm::vec4 colorY                   = glm::vec4(0.2f, 0.7f, 0.2f, 1.0f);
		glm::vec4 colorZ                   = glm::vec4(0.1f, 0.25f, 0.8f, 1.0f);
		glm::vec4 colorW                   = glm::vec4(0.8f, 0.15f, 0.8f, 1.0f);
									            
		glm::vec4 pressedColorX            = glm::vec4(0.9f, 0.2f, 0.2f, 1.0f);
		glm::vec4 pressedColorY            = glm::vec4(0.3f, 0.8f, 0.3f, 1.0f);
		glm::vec4 pressedColorZ            = glm::vec4(0.2f, 0.35f, 0.9f, 1.0f);
		glm::vec4 pressedColorW            = glm::vec4(0.9f, 0.25f, 0.8f, 1.0f);

		// Font Paths
		std::array<std::string, EditorFont_Last> fontPaths = {
			"resources/fonts/OpenSans-Regular.ttf",
			"resources/fonts/OpenSans-Italic.ttf",
			"resources/fonts/OpenSans-Bold.ttf"
		};
		f32 fontSize = 18.0f;
		EditorFont defaultFont = EditorFont_Regular;
		
		// Icon Paths
		std::array<std::string, EditorIcon_Last> iconPaths = {
			"resources/icons/MenuIcon.png",
			"resources/icons/SettingsIcon.png",
			"resources/icons/TranslateIcon.png",
			"resources/icons/RotateIcon.png",
			"resources/icons/ScaleIcon.png",
			"resources/icons/PlayIcon.png",
			"resources/icons/PauseIcon.png"
		};
	};

	inline glm::vec2 FromImVec(const ImVec2& vec) { return { vec.x, vec.y }; }
	inline ImVec2 ToImVec(const glm::vec2& vec)   { return { vec.x, vec.y }; }
	inline ImVec4 ToImVec(const glm::vec4& vec)   { return { vec.x, vec.y, vec.z, vec.w }; }

	extern std::string EntityDragSourceName;

	class EditorGui {
	public:
		static void SetStyle(const EditorStyle& style);
		static EditorStyle& GetStyle();
		static ref<Renderer::Texture> GetIcon(EditorIcon icon);
		static void TintStyle(const f32& tint);
		static void RemoveStyleTint();

		static bool InputInt(const std::string& lable, i32& value, i32 resetValue = 0, f32 columnWidth = 100.0f);
		static bool InputInt2(const std::string& lable, glm::ivec2& value, i32 resetValue = 0, f32 columnWidth = 100.0f);
		static bool InputInt3(const std::string& lable, glm::ivec3& value, i32 resetValue = 0, f32 columnWidth = 100.0f);

		static bool InputFloat(const std::string& lable, f32& value, f32 resetValue = 0.0f, f32 columnWidth = 100.0f, u32 floatPrecision = 2);
		static bool InputVec2(const std::string& lable, glm::vec2& value, f32 resetValue = 0.0f, f32 columnWidth = 100.0f, u32 floatPrecision = 2);
		static bool InputVec3(const std::string& lable, glm::vec3& value, f32 resetValue = 0.0f, f32 columnWidth = 100.0f, u32 floatPrecision = 2);
		static bool InputVec4(const std::string& lable, glm::vec4& value, f32 resetValue = 0.0f, f32 columnWidth = 100.0f, u32 floatPrecision = 2);

		static bool CheckBox(const std::string& lable, bool& value, f32 columnWidth = 100.0f);

		static bool InputText(const std::string& lable, std::string& value, f32 columnWidth = 100.0f);
		static void Text(const std::string& lable, const std::string& value, f32 columnWidth = 100.0f);
		static bool RenamableText(std::string& text, const std::string& textID = "", bool* renaming = nullptr, std::string* original = nullptr);

		static bool ListBox(const std::string& lable, i32& selection, std::vector<std::string>& items, f32 columnWidth = 100.0f);
		static bool ComboBox(const std::string& lable, i32& selection, std::vector<std::string>& items, f32 columnWidth = 100.0f);
	
		static bool InputColor(const std::string& lable, glm::vec3& value, f32 resetValue = 0.0f, f32 columnWidth = 100.0f);

		template <typename... Comps>
		static bool InputEntity(const std::string& lable, Entity& entity, const ref<Scene>& scene, f32 columnWidth = 100.0f);

	private:
		struct RenamableTextData {
			bool started;
			bool ended;
			std::string original;
		};
		struct Data {
			EditorStyle style;
			f32 tint;
			std::array<ref<Renderer::Texture>, EditorIcon_Last> icons;
			std::unordered_map<u64, void*> itemData;
		};
		static unique<Data> s_Data;
	};
	
}

#include "EditorImGui.inl"