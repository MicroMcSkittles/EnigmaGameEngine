#include "Core/Application.h"
#include <cstdlib>
#include <iostream>

Enigma::Application::Application()
{
	if (s_Instance) {
		// TODO: and error handling
		exit(-1);
	}
	s_Instance = this;

	std::cout << "STARTING" << std::endl;
}
Enigma::Application::~Application()
{
	std::cout << "CLOSING" << std::endl;
}

void Enigma::Application::run()
{
	std::cout << "RUNNING" << std::endl;
}
