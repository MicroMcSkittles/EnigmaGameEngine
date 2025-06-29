#include "Platform/Windows/WindowsInput.h"


#include <GLFW/glfw3.h>

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

		bool WindowsInput::IsKeyPressedImpl(int key)
		{
			GLFWwindow* window = (GLFWwindow*)m_Window->GetHandle();
			int rslt = glfwGetKey(window, key);
			return rslt == GLFW_PRESS || rslt == GLFW_REPEAT;
			return false;
		}
		bool WindowsInput::IsMouseButtonPressedImpl(int button)
		{
			GLFWwindow* window = (GLFWwindow*)m_Window->GetHandle();
			int rslt = glfwGetMouseButton(window, button);
			return rslt == GLFW_PRESS || rslt == GLFW_REPEAT;
			return false;
		}

		glm::vec2 WindowsInput::GetMousePositionImpl()
		{
			GLFWwindow* window = (GLFWwindow*)m_Window->GetHandle();
			double x;
			double y;
			glfwGetCursorPos(window, &x, &y);
			glm::vec2 rslt = { (float)x, (float)y };
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