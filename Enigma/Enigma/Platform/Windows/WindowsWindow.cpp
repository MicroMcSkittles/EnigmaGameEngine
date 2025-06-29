#include "Platform/Windows/WindowsWindow.h"
#include "Core/Event/WindowEvent.h"
#include "Core/Event/InputEvent.h"
#include "Core/Core.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// TODO: terminat GLFW someware

namespace Enigma {

	Core::Window* Core::Window::Create(const Core::WindowConfig& config) {
		return new Platform::WindowsWindow(config);
	}

	namespace Platform {

		WindowsWindow::WindowsWindow(const Core::WindowConfig& config)
		{
			m_Config = config;
			m_Data.width = m_Config.width;
			m_Data.height = m_Config.height;

			// Configure GLFW for OpenGL
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

			// Configure window
			glfwWindowHint(GLFW_RESIZABLE, m_Config.resizable);

			// Create window handle
			GLFWwindow* window = glfwCreateWindow(m_Data.width, m_Data.height, m_Config.title.c_str(), NULL, NULL);
			if (window == NULL) {
				glfwTerminate();
				LOG_ERROR("Failed to initalize window ( " + m_Config.title + " )");
			}
			glfwMakeContextCurrent(window);
			m_Handle = (void*)window;

			SetVSync(m_Config.vSync);

			// Set callback functions
			glfwSetWindowUserPointer(window, (void*)&m_Data);

			glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
				auto data = (WindowData*)glfwGetWindowUserPointer(window);
				Core::WindowClose e;
				for (auto& callback : data->callbacks) {
					callback(e);
				}
			});
			glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
				auto data = (WindowData*)glfwGetWindowUserPointer(window);
				data->width = width;
				data->height = height;
				Core::WindowResize e(width, height);
				for (auto& callback : data->callbacks) {
					callback(e);
				}
			});

			// Set input callback events
			glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x, double y) {
				auto data = (WindowData*)glfwGetWindowUserPointer(window);
				Core::MouseMoved e((float)x, (float)y);
				for (auto& callback : data->callbacks) {
					callback(e);
				}
			});
			glfwSetScrollCallback(window, [](GLFWwindow* window, double x, double y) {
				auto data = (WindowData*)glfwGetWindowUserPointer(window);
				Core::MouseScroll e((float)x, (float)y);
				for (auto& callback : data->callbacks) {
					callback(e);
				}
			});
			glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
				auto data = (WindowData*)glfwGetWindowUserPointer(window);
				Core::MouseButton e(button, action, mods);
				for (auto& callback : data->callbacks) {
					callback(e);
				}
			});
			glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
				auto data = (WindowData*)glfwGetWindowUserPointer(window);
				Core::Keyboard e(key, scancode, action, mods);
				for (auto& callback : data->callbacks) {
					callback(e);
				}
			});
		}
		WindowsWindow::~WindowsWindow()
		{
			glfwDestroyWindow((GLFWwindow*)m_Handle);
		}

		void WindowsWindow::AddEventCallback(std::function<void(Core::Event&)> callback)
		{
			m_Data.callbacks.push_back(callback);
		}

		void WindowsWindow::MakeCurrent()
		{
			glfwMakeContextCurrent((GLFWwindow*)m_Handle);
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
			glfwSwapBuffers((GLFWwindow*)m_Handle);
			glfwPollEvents();
		}
		void WindowsWindow::SetVSync(bool vSync)
		{
			m_Config.vSync = vSync;
			if (vSync) glfwSwapInterval(1);
			else	   glfwSwapInterval(0);
		}
	}
}