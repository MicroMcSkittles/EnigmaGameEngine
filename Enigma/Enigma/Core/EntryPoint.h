#pragma once

extern void Enigma::Core::ApplicationMain(Enigma::Core::Application* app);

int main(int argc, char** argv) {

	Enigma::Core::Application* app = new Enigma::Core::Application(argc, argv);

	ApplicationMain(app);

	app->Run();

	delete app;

	return 0;
}