#include <Enigma/Enigma.h>
#include <Enigma/Core/EntryPoint.h>

#include "Editor.h"

void Enigma::Core::ApplicationMain(Enigma::Core::Application* app) {
	app->CreateSubProc<Enigma::Editor::Editor>();
}