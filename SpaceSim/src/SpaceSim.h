#pragma once
#include <Enigma/Core/Process/SubProcess.h>
#include <Enigma/Core/Event/InputEvent.h>
#include <Enigma/Core/Event/WindowEvent.h>
#include <Enigma/Core/Window.h>
#include <Enigma/Engine/Input.h>
#include <Enigma/Engine/Surface.h>
#include <Enigma/Renderer/Camera.h>

#include "Player.h"
#include "Simulation.h"

using namespace Enigma;

class SpaceSim : public Core::SubProcess {
public:
	virtual void StartUp() override;

	virtual bool OnEvent(Core::Event& e) override;

	virtual void Update(Engine::DeltaTime deltaTime) override;
	virtual void Render() override;
	virtual void ImGui() override;

private:
	void OnResize(Core::WindowResize& e);
	void OnKeyboard(Core::Keyboard& e);
	void OnClose();

private:
	float m_FPS;
	float m_MS;

	Core::ID m_WindowID;
	Engine::Surface m_Surface;
	Engine::Input* m_InputContext;

	Simulation* m_Simulation;
	Core::ID m_SelectedPlanet;

	Renderer::Camera* m_Camera;
	Player* m_Player;
};