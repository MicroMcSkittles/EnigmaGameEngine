#include <Enigma/Enigma.h>
#include <Enigma/Core/Input.h>
#include <Enigma/Core/EntryPoint.h>

#include <glm/glm.hpp>
#include <iostream>

using namespace Enigma;

class MainProc : public Core::SubProcess {
public:
	virtual void StartUp()override {
		LOG_MESSAGE("Using " + Core::System::GetOSName(), 2);
	}
	virtual void ShutDown() {
		LOG_MESSAGE("Main process shutting down :(", 2);
	}
	virtual bool OnEvent(Core::Event& e) {
		if (e.GetType() == Core::EventType::MouseMoved) return false;
		std::cout << e.ToString() << std::endl;
		return false;
	}
	virtual void Update(float deltaTime) override {
		
	}
};

class App : public Core::Application {
public:
	App(Core::ApplicationConfig config) : Application(config)
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
	return new App(config);
}