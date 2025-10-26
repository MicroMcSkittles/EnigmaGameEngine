#include "Editor.h"
#include <Enigma/Core/Process/Application.h>
#include <Enigma/Core/Event/WindowEvent.h>

#include <imgui.h>

#include "EditorImGui.h"

namespace Enigma::Editor {
	void EditorProcess::StartUp()
	{
		Core::WindowConfig windowConfig;
		windowConfig.title     = "Enigma Editor";
		windowConfig.renderAPI = Renderer::API::OpenGL;

		Core::ImGuiConfig imguiConfig;
		imguiConfig.renderAPI = windowConfig.renderAPI;

		m_WindowID = Core::Application::CreateWindow(windowConfig, imguiConfig);

		Core::Application::GetWindow(m_WindowID)->AddEventCallback([&](Core::Event& e) { return OnEvent(e); });
		Core::Application::BindSubProcToWindow(Core::Application::GetSubProcess(m_ProcessID), m_WindowID);

		Core::Application::UseRenderAPI(windowConfig.renderAPI);
		Renderer::RenderAPI::SetClearMask(Renderer::ColorBufferBit);
		Renderer::RenderAPI::SetClearColor({ 0,0,0,1 });

		EditorStyle style;
		EditorGui::SetStyle(style);
	}
	void EditorProcess::ShutDown()
	{
	}

	bool EditorProcess::OnEvent(Core::Event& e)
	{
		Core::EventHandler handler(e);

		// Stop the application when the window closes
		handler.Dispatch<Core::WindowClose>([&](Core::WindowClose& windowEvent) {
			Core::Application::Close();
			return false;
		});

		return false;
	}

	void EditorProcess::Update(Engine::DeltaTime deltaTime)
	{
	}
	void EditorProcess::Render()
	{
		Renderer::RenderAPI::Clear();
	}

	f32 testFloat = 0.0f;
	glm::vec2 testVec2 = { 1,0 };
	glm::vec3 testVec3 = { 1,0,1 };
	glm::vec4 testVec4 = { 1,0,1, 2 };

	std::string testText = "Hello";

	i32 listBoxSelection = 0;
	i32 comboBoxSelection = 0;
	std::vector<std::string> listBoxItems = {
		"NONE",
		"item 1",
		"item 2",
		"item 3"
	};

	bool check = false;
	i32 inputInt = 24;
	glm::ivec2 inputInt2 = { 2,3 };
	glm::ivec3 inputInt3 = { 2,3,4 };

	void EditorProcess::ImGui()
	{
		ImGui::Begin("Test window");
		ImGui::TextWrapped("Creating the basic input widgets i'll need for the editor, as well as styling imgui");
		//ImGui::ShowDemoWindow();

		ImGuiIO& io = ImGui::GetIO();

		ImGui::Text("Some test text");

		ImGui::PushFont(io.Fonts->Fonts[static_cast<i32>(EditorFont::Bold)]);
		ImGui::Text("Some Bold test text");
		ImGui::PopFont();
		ImGui::PushFont(io.Fonts->Fonts[static_cast<i32>(EditorFont::Italic)]);
		ImGui::Text("Some Italic test text");
		ImGui::PopFont();

		EditorGui::InputFloat("Test Float", testFloat, 0.0f, 125.0f);
		EditorGui::InputVec2("Test Vector 2", testVec2, 0.0f, 125.0f);
		EditorGui::InputVec3("Test Vector 3", testVec3, 0.0f, 125.0f);
		EditorGui::InputVec4("Test Vector 4", testVec4, 0.0f, 125.0f);

		ImGui::Text("More text");
		EditorGui::InputText("Test Text", testText, 125.0f);
		ImGui::Spacing();
		EditorGui::ComboBox("Test Combo Box", comboBoxSelection, listBoxItems, 125.0f);
		ImGui::Spacing();
		EditorGui::ListBox("Test List Box", listBoxSelection, listBoxItems, 125.0f);

		EditorGui::CheckBox("Test Check Box", check, 125.0f);
		EditorGui::InputInt("Input int", inputInt, 0, 125.0f);
		EditorGui::InputInt2("Input int2", inputInt2, 0, 125.0f);
		EditorGui::InputInt3("Input int3", inputInt3, 0, 125.0f);

		if (ImGui::TreeNode("Example Transform")) {

			EditorGui::CheckBox("Relative", check);
			EditorGui::InputVec3("Position", testVec3);
			EditorGui::InputVec3("Rotation", testVec3);
			EditorGui::InputVec3("Scale", testVec3);

			ImGui::TreePop();
		}

		ImGui::End();
	}
}