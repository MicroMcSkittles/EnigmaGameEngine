#include <Enigma/Enigma.h>
#include <Enigma/Core/EntryPoint.h>
#include <Enigma/Renderer/Render2D.h>

#include <Enigma/Renderer/RenderAPI.h>

#include <Enigma/Engine/ECS/EntityComponentSystem.h>
#include <Enigma/Engine/ECS/StanderedComponents.h>
#include <Enigma/Engine/ECS/RenderComponents.h>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <iostream>
using namespace Enigma;

struct Entity {
	std::string name;
	Engine::ECS::Entity* entity;
	std::vector<Entity*> children;
};

class SceneView : public Core::SubProcess {
public:
	virtual void StartUp() override {
		
		m_Frame = GetEngine()->GetSurface().frame;

		LOG_MESSAGE("Started Scene View", 4);
	}

	Renderer::Texture* GetFrame() { return m_Frame; }

private:
	Renderer::Texture* m_Frame;
	Engine::ECS::Entity* m_Entity;
};

class Editor : public Core::SubProcess {
public:
	void CreateSceneView() {
		Renderer::Render2DConfig renderConfig;
		renderConfig.renderAPI = Renderer::API::OpenGL;

		Renderer::TextureConfig frameConfig;
		frameConfig.width = 400;
		frameConfig.height = 400;
		renderConfig.surface.frame = Renderer::Texture::Create(frameConfig);
		renderConfig.surface.scale = { 400,400 };

		Renderer::ShaderConfig shaderConfig;

		shaderConfig.vertexPath = "ExampleApp/assets/shader2D.vert";
		shaderConfig.pixelPath = "ExampleApp/assets/shader2D.frag";
		renderConfig.mainShader = Renderer::Shader::Create(shaderConfig);

		shaderConfig.vertexPath = "ExampleApp/assets/postProc.vert";
		shaderConfig.pixelPath = "ExampleApp/assets/postProc.frag";
		renderConfig.postProcShader = Renderer::Shader::Create(shaderConfig);

		Engine::EngineConfig engineConfig;
		engineConfig.windowID = m_WindowID;
		engineConfig.rendererConfig = &renderConfig;

		Core::ID sceneView = Core::Application::CreateEngineInstance(engineConfig);
		m_SceneView = Core::Application::GetEngineInstance(sceneView)->CreateSubProc<SceneView>();
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

		CreateSceneView();
		
		Engine::ECS::ECS::MakeCurrent(m_SceneView->GetEngine()->GetECS());

		Engine::ECS::Entity* entity = Engine::ECS::ECS::CreateEntity();
		Engine::ECS::Transform* transform = entity->GetComponent<Engine::ECS::Transform>();
		transform->GetScale() = { 0.25,0.25,1 };
		transform->GetRotation().z = glm::radians(45.0f);
		entity->AddComponent<Engine::ECS::Render2D>(new Engine::ECS::Render2D());

		Entity* e = new Entity();
		e->entity = entity;
		e->name = "Default Square";
		m_Hierarchy.push_back(e);

		m_Selected = nullptr;

		LOG_MESSAGE("YIPPY", 4);
	}
	
	virtual void Update(Engine::DeltaTime) override {
		
	}

	void ShowSceneView() {
		ImGui::Begin("Scene View");

		ImVec2 viewport = ImGui::GetContentRegionAvail();
		ImVec2 viewportPos = ImGui::GetCursorScreenPos();

		m_SceneView->GetEngine()->GetSurface().Resize(viewport.x, viewport.y);
		m_SceneView->GetEngine()->GetSurface().position = { viewportPos.x, viewportPos.y };

		ImGui::Image(m_SceneView->GetFrame());

		ImGui::End();
	}
	void ShowTransform(Engine::ECS::Transform* transform) {
		ImGui::SeparatorText("Transform");
		ImGui::InputFloat3("Position", &transform->GetPosition().x);
		ImGui::InputFloat3("Rotation", &transform->GetRotation().x);
		ImGui::InputFloat3("Scale", &transform->GetScale().x);
	}
	void ShowRenderComp(Engine::ECS::Render2D* comp) {
		ImGui::SeparatorText("Render2D");
		ImGui::InputFloat3("Tint", &comp->GetTint().x);
		ImGui::InputInt("Depth", &comp->GetDepth());
	}
	void ShowInspector() {
		if (m_Selected == nullptr) return;

		ImGui::Begin("Inspector");

		ImGui::InputText("Name", &m_Selected->name);

		auto& components = m_Selected->entity->GetComponents();
		for (auto& [type, id] : components) {
			switch (type)
			{
			case Engine::ECS::ComponentType::Transform: ShowTransform(m_Selected->entity->GetComponent<Engine::ECS::Transform>()); break;
			case Engine::ECS::ComponentType::Render2D: ShowRenderComp(m_Selected->entity->GetComponent<Engine::ECS::Render2D>()); break;
			}
		}

		ImGui::End();
	}
	void ShowEntity(Entity* e) {
		std::string name;
		if (e->name.empty()) {
			name = "No Name";
		}
		else {
			name = e->name;
		}
		if (ImGui::TreeNode(name.c_str())) {
			if (ImGui::Button("Open")) {
				m_Selected = e;
			}
			for (auto& c : e->children) {
				ShowEntity(c);
			}
			ImGui::TreePop();
		}
	}
	void ShowSceneHierarchy() {
		Engine::ECS::ECS::MakeCurrent(m_SceneView->GetEngine()->GetECS());

		ImGui::Begin("Hierarchy View");

		if (ImGui::Button("Create Entity")) {
			Engine::ECS::Entity* entity = Engine::ECS::ECS::CreateEntity();
			entity->AddComponent<Engine::ECS::Render2D>(new Engine::ECS::Render2D);
			Entity* e = new Entity();
			e->entity = entity;
			/*if (m_Selected != -) {
				Engine::ECS::Transform* transform = entity->GetComponent<Engine::ECS::Transform>();
				transform->GetParentTransform() = m_Hierarchy[m_Selected]->entity->GetComponent<Engine::ECS::Transform>();
				e->name = "Child Entity " + std::to_string(m_Hierarchy[m_Selected]->children.size());
				m_Hierarchy[m_Selected]->children.push_back(e);
			}*/
			e->name = "Entity " + std::to_string(m_Hierarchy.size());
			m_Hierarchy.push_back(e);
		}

		for (auto& e : m_Hierarchy) {
			ShowEntity(e);
		}

		ImGui::End();
	}
	virtual void ImGui() override {
		Core::Application::UseRenderAPI(Renderer::API::OpenGL);
		Renderer::RenderAPI::Clear();

		ShowSceneView();
		ShowSceneHierarchy();
		ShowInspector();
	}

private:
	Core::ID m_WindowID;

	Entity* m_Selected;
	std::vector<Entity*> m_Hierarchy;

	SceneView* m_SceneView;
};

void Enigma::Core::ApplicationMain(Enigma::Core::Application* app) {
	app->CreateSubProc<Editor>();
}