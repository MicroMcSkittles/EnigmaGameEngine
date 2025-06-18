#include <Enigma/Enigma.h>
#include <Enigma/Core/Input.h>
#include <Enigma/Core/EntryPoint.h>

#include <Enigma/Renderer/Renderer2D.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <filesystem>

using namespace Enigma;

class MainProc : public Core::SubProcess {
public:
	virtual void StartUp()override {
		Renderer::ViewBox viewBox;
		viewBox = Renderer::ViewBox::ScreenViewBox();
		m_Camera = new Renderer::OrthographicCamera(viewBox);

		Renderer::TextureConfig textureConfig;
		m_Texture = Renderer::Texture::Create("ExampleApp/assets/test.jpg", textureConfig);

		Renderer::Render2D::Init({});

		m_Rotation = 0;

		LOG_MESSAGE("Using " + Core::System::GetOSName(), 2);
	}
	virtual void ShutDown() {
		delete m_Camera;
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
		m_Rotation += deltaTime * 50;
	}
	virtual void Render() override {
		Renderer::Render2D::StartFrame(m_Camera);

		Renderer::Render2D::DrawQuad({ 0,0 }, { 0.5,0.5 }, glm::radians(m_Rotation), m_Texture);

		Renderer::Render2D::EndFrame();
	}

private:
	Renderer::Camera* m_Camera;
	Renderer::Texture* m_Texture;

	float m_Rotation;
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
	return new App(config, argc, argv);
}