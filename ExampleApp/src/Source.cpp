#include <Enigma/Enigma.h>
#include <Enigma/Core/EntryPoint.h>
#include <Enigma/Renderer/Render2D.h>

#include <Enigma/Renderer/RenderAPI.h>

#include <Enigma/Engine/ECS/Component.h>
#include <Enigma/Engine/ECS/EntityComponentSystem.h>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <iostream>

using namespace Enigma;

class Editor : public Core::SubProcess {
public:
	virtual void StartUp() override {
		
		Core::WindowConfig windowConfig;
		windowConfig.width = 800;
		windowConfig.height = 600;
		windowConfig.renderAPI = Renderer::API::OpenGL;
		windowConfig.title = "Enigma Editor";

		Core::ImGuiConfig imguiConfig;
		imguiConfig.docking = true;

		m_WindowID = Core::Application::CreateWindow(windowConfig, imguiConfig);

		Core::Application::BindSubProcToWindow(this, m_WindowID);

		Core::Application::UseRenderAPI(Renderer::API::OpenGL);
		Renderer::RenderAPI::SetClearColor({ 0,0,0,1 });
		Renderer::RenderAPI::SetClearMask(Renderer::ColorBufferBit);

		using namespace Engine::ECS;
		ECS ECS;
		{
			Core::ID entity = ECS.CreateEntity();
			ECS.AddComponent<Tag>(entity);
			Tag& tag = ECS.GetComponent<Tag>(entity);
			tag.tag = "Entity 1";
		}
		{
			Core::ID entity = ECS.CreateEntity();
			ECS.AddComponent<Tag>(entity);
			Tag& tag = ECS.GetComponent<Tag>(entity);
			tag.tag = "Entity 2";
		}
		
		for (auto& tag : ECS.GetPool<Engine::ECS::Tag>()->GetData()) {
			LOG_WARNING(tag.tag);
		}

		LOG_MESSAGE("YIPPY", 4);
	}
	
	virtual void Update(Engine::DeltaTime) override {
		
	}

	virtual void ImGui() override {
		Core::Application::UseRenderAPI(Renderer::API::OpenGL);
		Renderer::RenderAPI::Clear();

		ImGui::Begin("Test Window");
		ImGui::Text("Ah");
		ImGui::End();
	}

private:
	Core::ID m_WindowID;
};

void Enigma::Core::ApplicationMain(Enigma::Core::Application* app) {
 	app->CreateSubProc<Editor>();
}