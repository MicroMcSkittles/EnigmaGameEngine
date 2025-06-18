#include <Enigma/Enigma.h>
#include <Enigma/Core/Input.h>
#include <Enigma/Core/EntryPoint.h>

#include <Enigma/Renderer/Renderer2D.h>

#include <Enigma/ECS/Entity.h>
#include <Enigma/ECS/RenderComponment.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <filesystem>

using namespace Enigma;

class MainProc : public Core::SubProcess {
public:
	virtual void StartUp()override {
		Renderer::TextureConfig textureConfig;
		m_Texture = Renderer::Texture::Create("ExampleApp/assets/test.jpg", textureConfig);

		Renderer::Render2D::Init({});

		// Init entity 1
		{
			m_Entity1 = ECS::Entity::Create();
			ECS::Transform* transform = m_Entity1->GetComponent<ECS::Transform>();
			transform->GetScale() = { 0.5,0.5,0.5 };
			transform->GetPosition() = { 0.5,0,0 };
			m_Entity1->AddComponent<ECS::Render2D>(new ECS::Render2D({ 1,1,1 }, m_Texture));
		}

		// Init entity 2
		{
			m_Entity2 = ECS::Entity::Create();
			ECS::Transform* transform = m_Entity2->GetComponent<ECS::Transform>();
			transform->GetScale() = { 0.15, 0.75, 1 };
			transform->GetPosition() = { -1,0,0 };
			m_Entity2->AddComponent<ECS::Render2D>(new ECS::Render2D({ 1,0,0 }));
		}

		m_Camera = ECS::Entity::Create();
		m_Camera->AddComponent<ECS::Camera>(new ECS::Camera(true));

		LOG_MESSAGE("Using " + Core::System::GetOSName(), 2);
	}
	virtual void ShutDown() {
		LOG_MESSAGE("Main process shutting down :(", 2);
	}

	bool OnResize(Core::WindowResize& e) {
		Renderer::Render2D::Resize(e.GetWidth(), e.GetHeight());
		return false;
	}
	virtual bool OnEvent(Core::Event& e) {
		Core::EventHandler handler(e);
		handler.Dispatch<Core::WindowResize>(CLASS_BIND_ARGS_1(MainProc::OnResize));
		return e.Handled();
	}

	virtual void Update(float deltaTime) override {
		auto transform = m_Entity1->GetComponent<ECS::Transform>();
		transform->GetRotation().z += deltaTime * 50;
	}

private:
	Renderer::Texture* m_Texture;

	ECS::Entity* m_Entity1;
	ECS::Entity* m_Entity2;
	ECS::Entity* m_Camera;
};

class App : public Core::Application {
public:
	App(Core::ApplicationConfig config, int argc, char** argv) : Application(config, argc, argv)
	{
		CreateSubProc<MainProc>();
	}
};

Enigma::Core::Application* Enigma::Core::CreateApplication(int argc, char** argv) {
	Core::WindowConfig windowConfig;
	windowConfig.title = "Example Application";
	windowConfig.width = 800;
	windowConfig.height = 600;
	windowConfig.resizable = true;
	windowConfig.vSync = true;

	Core::LoggerConfig loggerConfig;
	loggerConfig.flags = Core::LoggerFile | Core::LoggerFunction | Core::LoggerPriority | Core::LoggerTime;
	loggerConfig.priorityLevel = 5;
	
	ApplicationConfig config;
	config.rendererAPI = Renderer::API::OpenGL;
	config.windowConfig = windowConfig;
	config.loggerConfig = loggerConfig;

	config.useRenderProc = true;

	return new App(config, argc, argv);
}