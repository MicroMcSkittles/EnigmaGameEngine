#pragma once

extern Enigma::Core::Application * Enigma::Core::CreateApplication(int argc, char** argv);

int main(int argc, char** argv) {
	Enigma::Core::Application* app = Enigma::Core::CreateApplication(argc, argv);
	app->run();
	delete app;
	return 0;
}