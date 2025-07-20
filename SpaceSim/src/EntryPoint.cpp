#include <Enigma/Enigma.h>
#include <Enigma/Core/EntryPoint.h>

#include "SpaceSim.h"

void Enigma::Core::ApplicationMain(Enigma::Core::Application* app) {
	app->CreateSubProc<SpaceSim>();
}