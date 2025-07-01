#include <Enigma/Enigma.h>
#include <Enigma/Core/EntryPoint.h>
#include <Enigma/Renderer/Render2D.h>

#include <Enigma/Renderer/RenderAPI.h>

#include <Enigma/Engine/ECS/Component.h>
#include <Enigma/Engine/ECS/RenderComponent.h>
#include <Enigma/Engine/ECS/EntityComponentSystem.h>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <iostream>

using namespace Enigma;

class EngineProc : public Core::SubProcess {
public:
	virtual void StartUp() override {
		using namespace Engine::ECS;
		m_Entity = ECS::CreateEntity();
		Transform& transform = ECS::GetComponent<Transform>(m_Entity);
		transform.scale = { 0.5,0.5,1 };
		Render2D& renderComp = ECS::AddComponent<Render2D>(m_Entity);
		renderComp.tint = { 0,1,1,1 };

		m_Camera = ECS::CreateEntity();
		ECS::AddComponent<Camera>(m_Camera);

		LOG_MESSAGE("Engine started", 5);
	}
	virtual void Update(Engine::DeltaTime deltaTime) {
		using namespace Engine::ECS;
		Transform& transform = ECS::GetComponent<Transform>(m_Entity);
		transform.rotation += deltaTime;
	}

private:
	Core::ID m_Entity;
	Core::ID m_Camera;
};
class App : public Core::SubProcess {
public:
	void CreateEngine() {
		Renderer::Render2DConfig renderConfig;
		renderConfig.surface.frame = nullptr;
		renderConfig.renderAPI = Renderer::API::OpenGL;
		
		Renderer::ShaderConfig shaderConfig;

		shaderConfig.vertexPath = "ExampleApp/assets/shader2D.vert";
		shaderConfig.pixelPath = "ExampleApp/assets/shader2D.frag";
		renderConfig.mainShader = Renderer::Shader::Create(shaderConfig);

		shaderConfig.vertexPath = "ExampleApp/assets/postProc.vert";
		shaderConfig.pixelPath = "ExampleApp/assets/postProc.frag";
		renderConfig.postProcShader = Renderer::Shader::Create(shaderConfig);

		Engine::EngineConfig config;
		config.rendererConfig = &renderConfig;
		config.windowID = m_WindowID;

		Core::ID id = Core::Application::CreateEngineInstance(config);
		m_EngineProc = Core::Application::GetEngineInstance(id)->CreateSubProc<EngineProc>();
	}
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

		CreateEngine();
		
		LOG_MESSAGE("YIPPY", 4);
	}
	
	virtual void Update(Engine::DeltaTime) override {
		
	}

	virtual void ImGui() override {
		//Core::Application::UseRenderAPI(Renderer::API::OpenGL);
		//Renderer::RenderAPI::Clear();

		ImGui::Begin("Test Window");
		ImGui::Text("Ah");
		ImGui::End();
	}

private:
	Core::ID m_WindowID;
	EngineProc* m_EngineProc;
};

void Enigma::Core::ApplicationMain(Enigma::Core::Application* app) {
 	app->CreateSubProc<App>();
}