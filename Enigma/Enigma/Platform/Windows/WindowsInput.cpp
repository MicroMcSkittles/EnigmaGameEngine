#include "Enigma/Platform/Windows/WindowsInput.h"
#include "Enigma/Engine/InputCodes.h"

namespace Enigma {

	Engine::Input* Engine::Input::Create(const Engine::InputConfig& config) {
		return new Platform::WindowsInput(config);
	}

	namespace Platform {

		WindowsInput::WindowsInput(const Engine::InputConfig& config)
		{
			m_Window = (WindowsWindow*)config.window;
			m_Surface = config.surface;
		}

		int WindowsInput::ConvertWindowsKeyCode(int keycode)
		{
			int result = 0;
			if (keycode >= Engine::KeyCode::KeySpace && keycode <= Engine::KeyCode::KeyGraveAccent) result = keycode;
			return result;
		}

		bool WindowsInput::IsKeyPressedImpl(int key)
		{
			int rslt = m_Window->GetInputData().keyboardStates[key];
			return rslt == Engine::KeyCode::KeyPress || rslt == Engine::KeyCode::KeyRepeat;
			return false;
		}
		bool WindowsInput::IsMouseButtonPressedImpl(int button)
		{
			int rslt = m_Window->GetInputData().mouseButtonStates[button];
			return rslt == Engine::KeyCode::KeyPress || rslt == Engine::KeyCode::KeyRepeat;
		}

		glm::vec2 WindowsInput::GetMousePositionImpl()
		{
			glm::vec2 rslt = m_Window->GetInputData().mousePosition;
			rslt -= m_Surface->position;
			return rslt;
		}
		float WindowsInput::GetMouseXImpl()
		{
			return GetMousePositionImpl().x;
		}
		float WindowsInput::GetMouseYImpl()
		{
			return GetMousePositionImpl().y;
		}

		
	}
}