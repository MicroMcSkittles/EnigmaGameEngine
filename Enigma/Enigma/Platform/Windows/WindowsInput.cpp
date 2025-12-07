#include "Enigma/EnigmaPCH.h"
#include "Enigma/Platform/Windows/WindowsInput.h"
#include "Enigma/Engine/InputCodes.h"

namespace Enigma::Engine {

	ref<Input> Input::Create(const InputConfig& config) {
		return CreateRef<Platform::WindowsInput>(config);
	}

}

namespace Enigma::Platform {

	WindowsInput::WindowsInput(const Engine::InputConfig& config)
	{
		m_Window = CastRef<WindowsWindow>(config.window);
		m_Surface = config.surface;
	}

	u64 WindowsInput::ConvertWindowsKeyCode(u64 keycode)
	{
		// TODO: fill in ALL keys
		u64 result = 0;
		if (keycode >= static_cast<u64>(Engine::KeyCode::KeySpace) && keycode <= static_cast<u64>(Engine::KeyCode::KeyGraveAccent)) result = keycode;
		return result;
	}

	bool WindowsInput::IsKeyPressedImpl(i32 key)
	{
		i32 rslt = m_Window->GetInputData().keyboardStates[key];
		return rslt == Engine::KeyCode::KeyPress || rslt == Engine::KeyCode::KeyRepeat;
		return false;
	}
	bool WindowsInput::IsMouseButtonPressedImpl(i32 button)
	{
		i32 rslt = m_Window->GetInputData().mouseButtonStates[button];
		return rslt == Engine::KeyCode::KeyPress || rslt == Engine::KeyCode::KeyRepeat;
	}

	glm::vec2 WindowsInput::GetMousePositionImpl()
	{
		glm::vec2 rslt = m_Window->GetInputData().mousePosition;
		rslt -= m_Surface->position;
		return rslt;
	}
	f32 WindowsInput::GetMouseXImpl()
	{
		return GetMousePositionImpl().x;
	}
	f32 WindowsInput::GetMouseYImpl()
	{
		return GetMousePositionImpl().y;
	}

	
}