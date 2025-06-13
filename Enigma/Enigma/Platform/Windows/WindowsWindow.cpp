#include "Platform/Windows/WindowsWindow.h"
#include "Core/Core.h"

#include <GLFW/glfw3.h>

namespace Enigma {

	Core::Window* Core::Window::Create(const Core::WindowConfig& config) {
		return new Platform::WindowsWindow(config);
	}

	namespace Platform {

		WindowsWindow::WindowsWindow(const Core::WindowConfig& config)
		{
			m_Data = config;
			
			if (!glfwInit()) {
				LOG_ERROR("Failed to initialize GLFW");
			}

			// Configure GLFW for OpenGL
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

			// Configure window
			glfwWindowHint(GLFW_RESIZABLE, m_Data.resizable);

			// Create window handle
			GLFWwindow* window = glfwCreateWindow(m_Data.width, m_Data.height, m_Data.title.c_str(), NULL, NULL);
			if (window == NULL) {
				glfwTerminate();
				LOG_ERROR("Failed to initalize window ( " + m_Data.title + " )");
			}
			glfwMakeContextCurrent(window);
			m_Handle = (void*)window;

			SetVSync(m_Data.vSync);

			// TODO: Set callback functions
			glfwSetWindowUserPointer(window, (void*)&m_Data);
		}
		WindowsWindow::~WindowsWindow()
		{
			glfwTerminate();
		}

		int WindowsWindow::GetWidth()
		{
			return m_Data.width;
		}
		int WindowsWindow::GetHeight()
		{
			return m_Data.height;
		}

		bool WindowsWindow::ShouldClose()
		{
			return glfwWindowShouldClose((GLFWwindow*)m_Handle);
		}
		void WindowsWindow::ShouldClose(bool shouldClose)
		{
			glfwSetWindowShouldClose((GLFWwindow*)m_Handle, shouldClose);
		}

		void WindowsWindow::Update()
		{
			glfwPollEvents();
			glfwSwapBuffers((GLFWwindow*)m_Handle);
		}
		void WindowsWindow::SetVSync(bool vSync)
		{
			m_Data.vSync = vSync;
			if (vSync) glfwSwapInterval(1);
			else	   glfwSwapInterval(0);
		}
	}
}