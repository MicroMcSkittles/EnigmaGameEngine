#include "Enigma/Platform/Windows/WindowsInput.h"
#include "Enigma/Engine/InputCodes.h"

namespace Enigma {

	ref<Engine::Input> Engine::Input::Create(const Engine::InputConfig& config) {
		return CreateRef<Platform::WindowsInput>(config);
	}

	namespace Platform {

		WindowsInput::WindowsInput(const Engine::InputConfig& config)
		{
			m_Window = CastRef<WindowsWindow>(config.window);
			m_Surface = config.surface;
		}

		i32 WindowsInput::ConvertWindowsKeyCode(i32 keycode)
		{
			i32 result = 0;
			if (keycode >= Engine::KeyCode::KeySpace && keycode <= Engine::KeyCode::KeyGraveAccent) result = keycode;
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
}