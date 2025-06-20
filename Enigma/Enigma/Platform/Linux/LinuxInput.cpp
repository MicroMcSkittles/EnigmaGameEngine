#include "Platform/Linux/LinuxInput.h"


#include <GLFW/glfw3.h>

namespace Enigma {

	Engine::Input* Engine::Input::Create(const Engine::InputConfig& config) {
		return new Platform::LinuxInput(config);
	}

	namespace Platform {

		LinuxInput::LinuxInput(const Engine::InputConfig& config)
		{
			m_Window = (LinuxWindow*)config.window;
			m_Surface = config.surface;
		}

		bool LinuxInput::IsKeyPressedImpl(int key)
		{
			GLFWwindow* window = (GLFWwindow*)m_Window->GetHandle();
			int rslt = glfwGetKey(window, key);
			return rslt == GLFW_PRESS || rslt == GLFW_REPEAT;
			return false;
		}
		bool LinuxInput::IsMouseButtonPressedImpl(int button)
		{
			GLFWwindow* window = (GLFWwindow*)m_Window->GetHandle();
			int rslt = glfwGetMouseButton(window, button);
			return rslt == GLFW_PRESS || rslt == GLFW_REPEAT;
			return false;
		}

		glm::vec2 LinuxInput::GetMousePositionImpl()
		{
			GLFWwindow* window = (GLFWwindow*)m_Window->GetHandle();
			double x;
			double y;
			glfwGetCursorPos(window, &x, &y);
			glm::vec2 rslt = { (float)x, (float)y };
			rslt -= m_Surface->position;
			return rslt;
		}
		float LinuxInput::GetMouseXImpl()
		{
			return GetMousePositionImpl().x;
		}
		float LinuxInput::GetMouseYImpl()
		{
			return GetMousePositionImpl().y;
		}

		
	}
}