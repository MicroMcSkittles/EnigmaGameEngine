#include "Platform/Windows/WindowsInput.h"
#include "Platform/Windows/WindowsWindow.h"

#include <GLFW/glfw3.h>

namespace Enigma {

	Core::Input* Core::Input::s_Instance = new Platform::WindowsInput();

	namespace Platform {
		bool WindowsInput::IsKeyPressedImpl(int key)
		{
			GLFWwindow* window = (GLFWwindow*)((WindowsWindow*)Core::Window::Get())->GetHandle();
			int rslt = glfwGetKey(window, key);
			return rslt == GLFW_PRESS || rslt == GLFW_REPEAT;
		}
		bool WindowsInput::IsMouseButtonPressedImpl(int button)
		{
			GLFWwindow* window = (GLFWwindow*)((WindowsWindow*)Core::Window::Get())->GetHandle();
			int rslt = glfwGetMouseButton(window, button);
			return rslt == GLFW_PRESS || rslt == GLFW_REPEAT;
		}

		glm::vec2 WindowsInput::GetMousePositionImpl()
		{
			GLFWwindow* window = (GLFWwindow*)((WindowsWindow*)Core::Window::Get())->GetHandle();
			double x;
			double y;
			glfwGetCursorPos(window, &x, &y);
			return { (float)x, (float)y };
		}
		float WindowsInput::GetMouseXImpl()
		{
			return GetMousePositionImpl().x;
		}
		float WindowsInput::GetMouseYImpl()
		{
			return GetMousePositionImpl().y;
		}

		std::string WindowsInput::GetKeyNameImpl(int key)
		{
			std::string name = glfwGetKeyName(key, glfwGetKeyScancode(key));
			return name;
		}
		std::string WindowsInput::GetButtonNameImpl(int button)
		{
			std::string name = std::to_string(button);
			if (button == GLFW_MOUSE_BUTTON_LEFT) name = "Left";
			else if (button == GLFW_MOUSE_BUTTON_MIDDLE) name = "Middle";
			else if (button == GLFW_MOUSE_BUTTON_RIGHT) name = "right";
			return name;
		}
		std::string WindowsInput::GetActionNameImpl(int action)
		{
			std::string name = std::to_string(action);
			if (action == GLFW_PRESS) name = "Press";
			else if (action == GLFW_REPEAT) name = "Repeat";
			else if (action == GLFW_RELEASE) name = "Release";
			return name;
		}
		std::string WindowsInput::GetModsNameImpl(int action)
		{
			return std::string();
		}
	}
}