#pragma once

#include <Enigma/Renderer/Camera.h>
#include <Enigma/Engine/DeltaTime.h>

using namespace Enigma;

class Player {
public:

	Player(Renderer::Camera* camera);

	void Update(Engine::DeltaTime deltaTime);

private:
	Renderer::Camera* m_Camera;

	glm::vec3 m_Position;
	glm::vec3 m_Direction;

	bool m_FocusLastFrame;
	glm::vec2 m_LastMousePosition;

	float m_Speed;
	float m_MouseSensitivity;
};