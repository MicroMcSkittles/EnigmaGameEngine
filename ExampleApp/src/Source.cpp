#include <Enigma/Enigma.h>
#include <Enigma/Core/EntryPoint.h>

using namespace Enigma;

class MainProc : public Core::SubProcess {
public:
	virtual void StartUp()override {
		LOG_MESSAGE("Main process!!", 2);
	}
	virtual void ShutDown() {
		LOG_MESSAGE("Main process shutting down :(", 2);
	}
	virtual void Update(float deltaTime) {
		
	}
};

class App : public Core::Application {
public:
	App(): Application() { 
		CreateSubProc<MainProc>();
	}
};

Enigma::Core::Application* Enigma::Core::CreateApplication(int argc, char** argv) {
	return new App();
}