#include "Player.h"

#include <Enigma/Engine/Input.h>
#include <Enigma/Engine/InputCodes.h>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

Player::Player(Renderer::Camera* camera) {
	m_Camera = camera;
	m_Position = m_Camera->GetPosition();
	m_Direction = m_Camera->GetDirection();

	m_LastMousePosition = { 0,0 };

	m_Speed = 15;
	m_MouseSensitivity = 2;
}

void Player::Update(Engine::DeltaTime deltaTime) {

	if (!Engine::Input::IsMouseButtonPressed(Engine::KeyCode::MouseButtonRight)) {
		m_FocusLastFrame = false;
		return;
	}

	// Find direction vectors
	constexpr glm::vec3 upDirection(0.0f, 1.0f, 0.0f);
	glm::vec3 forwardDirection = glm::normalize(glm::vec3(m_Direction.x, 0, m_Direction.z));
	glm::vec3 rightDirection = glm::cross(forwardDirection, upDirection);

	// Process position inputs 
	bool moved = false;
	glm::vec3 newPosition = m_Position;

	glm::vec3 movementDirection = { 0,0,0 };
	glm::vec3 collitionDirection = { 0,0,0 };

	if (Engine::Input::IsKeyPressed(Engine::KeyCode::KeyW)) {
		movementDirection += forwardDirection;
		moved = true;
	}
	if (Engine::Input::IsKeyPressed(Engine::KeyCode::KeyS)) {
		movementDirection -= forwardDirection;
		moved = true;
	}
	if (Engine::Input::IsKeyPressed(Engine::KeyCode::KeyD)) {
		movementDirection += rightDirection;
		moved = true;
	}
	if (Engine::Input::IsKeyPressed(Engine::KeyCode::KeyA)) {
		movementDirection -= rightDirection;
		moved = true;
	}
	if (Engine::Input::IsKeyPressed(Engine::KeyCode::KeyE)) {
		movementDirection += upDirection;
		moved = true;
	}
	if (Engine::Input::IsKeyPressed(Engine::KeyCode::KeyQ)) {
		movementDirection -= upDirection;
		moved = true;
	}
	movementDirection = glm::normalize(movementDirection);

	if (moved) {
		m_Position = m_Position + movementDirection * (m_Speed * deltaTime);
		m_Camera->SetPosition(m_Position);
	}
	else {
		moved = false;
	}

	if (!m_FocusLastFrame) {
		m_LastMousePosition = Engine::Input::GetMousePosition();
		m_FocusLastFrame = true;
	}
	glm::vec2 mousePos = Engine::Input::GetMousePosition();
	glm::vec2 delta = (mousePos - m_LastMousePosition) * 0.002f;
	m_LastMousePosition = mousePos;

	// Process direction input
	if (delta.x != 0 || delta.y != 0) {
		float pitchDelta = delta.y * m_MouseSensitivity;
		float yawDelta = delta.x * m_MouseSensitivity;

		glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, rightDirection), glm::angleAxis(-yawDelta, upDirection)));
		m_Direction = glm::rotate(q, m_Direction);

		m_Camera->SetDirection(m_Direction);
	}
}