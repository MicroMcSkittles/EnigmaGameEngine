#include <Enigma.h>
#include <Core/EntryPoint.h>

Enigma::Application* Enigma::CreateApplication(int argc, char** argv) {
	Application* app = new Application();
	return app;
}