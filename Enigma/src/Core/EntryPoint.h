#pragma once

extern Enigma::Application* Enigma::CreateApplication(int argc, char** argv);

int main(int argc, char** argv) {
	Enigma::Application* app = Enigma::CreateApplication(argc, argv);
	app->run();
	delete app;
	return 0;
}