#include <Enigma/Enigma.h>
#include <Enigma/Core/EntryPoint.h>

#include "TestBed.h"

void Enigma::Core::ApplicationMain(Enigma::Core::Application* app) {
  Enigma::Core::Application::CreateSubProc<TestBed>();
}
