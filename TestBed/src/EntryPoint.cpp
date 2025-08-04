#include <Enigma/Enigma.h>
#include <Enigma/Core/EntryPoint.h>

#include "TestBed.h"

void Enigma::Core::ApplicationMain(Enigma::Core::Application* app) {
  //std::cout << "Hello, World" << std::endl;
  Enigma::Core::Application::CreateSubProc<TestBed>();
}
