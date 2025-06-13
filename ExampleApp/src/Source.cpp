#include <Enigma/Enigma.h>
#include <Enigma/Core/EntryPoint.h>

Enigma::Core::Application* Enigma::Core::CreateApplication(int argc, char** argv) {
	Application* app = new Application();
	return app;
}