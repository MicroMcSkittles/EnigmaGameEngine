#include "Platform/Linux/LinuxInput.h"
#include "Platform/Linux/LinuxWindow.h"

#include <GLFW/glfw3.h>

namespace Enigma {

	Core::Input* Core::Input::s_Instance = new Platform::LinuxInput();

	namespace Platform {
		bool LinuxInput::IsKeyPressedImpl(int key)
		{
			GLFWwindow* window = (GLFWwindow*)((LinuxWindow*)Core::Window::Get())->GetHandle();
			int rslt = glfwGetKey(window, key);
			return rslt == GLFW_PRESS || rslt == GLFW_REPEAT;
		}
		bool LinuxInput::IsMouseButtonPressedImpl(int button)
		{
			GLFWwindow* window = (GLFWwindow*)((LinuxWindow*)Core::Window::Get())->GetHandle();
			int rslt = glfwGetMouseButton(window, button);
			return rslt == GLFW_PRESS || rslt == GLFW_REPEAT;
		}

		glm::vec2 LinuxInput::GetMousePositionImpl()
		{
			GLFWwindow* window = (GLFWwindow*)((LinuxWindow*)Core::Window::Get())->GetHandle();
			double x;
			double y;
			glfwGetCursorPos(window, &x, &y);
			return { (float)x, (float)y };
		}
		float LinuxInput::GetMouseXImpl()
		{
			return GetMousePositionImpl().x;
		}
		float LinuxInput::GetMouseYImpl()
		{
			return GetMousePositionImpl().y;
		}

		std::string LinuxInput::GetKeyNameImpl(int key)
		{
			std::string name = glfwGetKeyName(key, glfwGetKeyScancode(key));
			return name;
		}
		std::string LinuxInput::GetButtonNameImpl(int button)
		{
			std::string name = std::to_string(button);
			if (button == GLFW_MOUSE_BUTTON_LEFT) name = "Left";
			else if (button == GLFW_MOUSE_BUTTON_MIDDLE) name = "Middle";
			else if (button == GLFW_MOUSE_BUTTON_RIGHT) name = "right";
			return name;
		}
		std::string LinuxInput::GetActionNameImpl(int action)
		{
			std::string name = std::to_string(action);
			if (action == GLFW_PRESS) name = "Press";
			else if (action == GLFW_REPEAT) name = "Repeat";
			else if (action == GLFW_RELEASE) name = "Release";
			return name;
		}
		std::string LinuxInput::GetModsNameImpl(int action)
		{
			return std::string();
		}
	}
}