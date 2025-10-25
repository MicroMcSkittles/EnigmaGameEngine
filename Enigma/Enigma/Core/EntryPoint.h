#pragma once

#ifdef PLATFORM_LINUX
	#include<unistd.h>
	#include<string>
#endif

extern void Enigma::Core::ApplicationMain();

int main(int argc, char** argv) {

	// This is very hacky, I will try to find a better solution
#ifdef PLATFORM_LINUX
	std::string dir = argv[0];
	dir = std::string(dir.begin() + dir.find_last_of('/') + 1, dir.end());
	chdir(dir.c_str());
#endif

	Enigma::unique<Enigma::Core::Application>& app = Enigma::Core::Application::Create(argc, argv);

	Enigma::Core::ApplicationMain();

	app->Run();

	return 0;
}
