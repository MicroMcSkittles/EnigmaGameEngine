#include <Enigma/Enigma.h>
#include <Enigma/Core/Input.h>
#include <Enigma/Core/EntryPoint.h>
#include <glm/glm.hpp>
#include <iostream>

using namespace Enigma;

class MainProc : public Core::SubProcess {
public:
	virtual void StartUp()override {
		LOG_MESSAGE("Main process!!", 2);
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
		glm::vec2 pos = Core::Input::GetMousePosition();
		std::cout << pos.x << ", " << pos.y << std::endl;
	}
};

class App : public Core::Application {
public:
	App() : Application({
		{ "Example Application", 800, 600, true, true },
		{ Core::File | Core::Function | Core::Priority, 5 }
	}) 
	{
		CreateSubProc<MainProc>();
	}
};

Enigma::Core::Application* Enigma::Core::CreateApplication(int argc, char** argv) {
	return new App();
}