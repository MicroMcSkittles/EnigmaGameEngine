#include "Enigma/Platform/Windows/WindowsWindow.h"
#include "Enigma/Platform/Windows/WindowsInput.h"
#include "Enigma/Core/Event/WindowEvent.h"
#include "Enigma/Core/Event/InputEvent.h"
#include "Enigma/Core/Core.h"
#include "Enigma/Engine/InputCodes.h"

#include <glad/glad.h>

#include <cwchar>
#include <Windows.h>
#include <windowsx.h>

#include <backends/imgui_impl_win32.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Steve Jobs is the number 1 op
// or who ever wrote the windows "API"
// its more of a hodge podge of random functions
// that have a chance to work

namespace Enigma {

	ref<Core::Window> Core::Window::Create(const Core::WindowConfig& config) {
		return CreateRef<Platform::WindowsWindow>(config);
	}
	void* Core::Window::GetGLProcAddress(const char* procName) {
		void* proc = (void*)wglGetProcAddress(procName);
		// Manually load proc address from the opengl dll if wglGetProcAddress fails
		if (proc == 0 || proc == (void*)0x1 || proc == (void*)0x2 || proc == (void*)0x3 || proc == (void*)-1) {
			HMODULE hModule = LoadLibraryA("opengl32.dll");
			if (hModule == NULL) {
				LOG_ERROR("Failed to open ( opengl32.dll )");
			}
			proc = (void*)GetProcAddress(hModule, procName);
		}
		return proc;
	}

	namespace Platform {

		i32 VKToEnigma(i32 vk) {
			using namespace Engine::KeyCode;
			if (vk >= 'A' && vk <= 'Z') return vk;
			if (vk >= '0' && vk <= '9') return vk;
			return 0;
		}
		std::string GetErrorMessage() {
			DWORD errorCode = GetLastError();
			LPWSTR errorMessage = nullptr;
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				errorCode,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPWSTR)&errorMessage,
				0,
				NULL
			);

			i32 messageSize = WideCharToMultiByte(CP_UTF8, 0, errorMessage, -1, NULL, 0, NULL, NULL);
			std::string message(messageSize, '\0');
			WideCharToMultiByte(CP_UTF8, 0, errorMessage, -1, &message[0], messageSize, NULL, NULL);
			return message;
		}
		LRESULT CALLBACK WindowProcess(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			if (msg == WM_CREATE) {
				CREATESTRUCT* create = reinterpret_cast<CREATESTRUCT*>(lParam);
				SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)create->lpCreateParams);
				return 0;
			}

			// Get the window object
			WindowsWindow* window = nullptr;
			LONG_PTR userData = GetWindowLongPtr(handle, GWLP_USERDATA);
			window = reinterpret_cast<WindowsWindow*>(userData);

			switch (msg) {
			case WM_DESTROY: {
				if (!window->ShouldClose())window->CloseEvent();
			} break;
			case WM_SIZE: {
				i32 width  = LOWORD(lParam);
				i32 height = HIWORD(lParam);
				window->ResizeEvent(width, height);
			} return 0;
			case WM_MOUSEMOVE: {
				i32 x = GET_X_LPARAM(lParam);
				i32 y = GET_Y_LPARAM(lParam);
				window->MouseMovedEvent(x, y);
			} break;
			case WM_MOUSEWHEEL: {
				i32 delta = GET_WHEEL_DELTA_WPARAM(wParam) / 120;
				window->MouseScrollEvent(0, delta); // TODO: figure out how to do touch pads
			} break;

			// Left mouse button
			case WM_LBUTTONDOWN: {
				i32 mods = 0;
				mods |= (wParam & MK_CONTROL) ? Engine::KeyCode::CtrlMask  : 0;
				mods |= (wParam & MK_SHIFT)   ? Engine::KeyCode::ShiftMask : 0;
				mods |= (wParam & MK_ALT)     ? Engine::KeyCode::CtrlMask  : 0;
				window->MouseClickEvent(Engine::KeyCode::MouseButtonLeft, mods, Engine::KeyCode::KeyPress);
			} break;
			case WM_LBUTTONUP: {
				i32 mods = 0;
				mods |= (wParam & MK_CONTROL) ? Engine::KeyCode::CtrlMask : 0;
				mods |= (wParam & MK_SHIFT) ? Engine::KeyCode::ShiftMask : 0;
				mods |= (wParam & MK_ALT) ? Engine::KeyCode::CtrlMask : 0;
				window->MouseClickEvent(Engine::KeyCode::MouseButtonLeft, mods, Engine::KeyCode::KeyRelease);
			} break;

			// Middle mouse button
			case WM_MBUTTONDOWN: {
				i32 mods = 0;
				mods |= (wParam & MK_CONTROL) ? Engine::KeyCode::CtrlMask : 0;
				mods |= (wParam & MK_SHIFT) ? Engine::KeyCode::ShiftMask : 0;
				mods |= (wParam & MK_ALT) ? Engine::KeyCode::CtrlMask : 0;
				window->MouseClickEvent(Engine::KeyCode::MouseButtonMiddle, mods, Engine::KeyCode::KeyPress);
			} break;
			case WM_MBUTTONUP: {
				i32 mods = 0;
				mods |= (wParam & MK_CONTROL) ? Engine::KeyCode::CtrlMask : 0;
				mods |= (wParam & MK_SHIFT) ? Engine::KeyCode::ShiftMask : 0;
				mods |= (wParam & MK_ALT) ? Engine::KeyCode::CtrlMask : 0;
				window->MouseClickEvent(Engine::KeyCode::MouseButtonMiddle, mods, Engine::KeyCode::KeyRelease);
			} break;

			// Right mouse button
			case WM_RBUTTONDOWN: {
				i32 mods = 0;
				mods |= (wParam & MK_CONTROL) ? Engine::KeyCode::CtrlMask : 0;
				mods |= (wParam & MK_SHIFT) ? Engine::KeyCode::ShiftMask : 0;
				mods |= (wParam & MK_ALT) ? Engine::KeyCode::CtrlMask : 0;
				window->MouseClickEvent(Engine::KeyCode::MouseButtonRight, mods, Engine::KeyCode::KeyPress);
			} break;
			case WM_RBUTTONUP: {
				i32 mods = 0;
				mods |= (wParam & MK_CONTROL) ? Engine::KeyCode::CtrlMask : 0;
				mods |= (wParam & MK_SHIFT) ? Engine::KeyCode::ShiftMask : 0;
				mods |= (wParam & MK_ALT) ? Engine::KeyCode::CtrlMask : 0;
				window->MouseClickEvent(Engine::KeyCode::MouseButtonRight, mods, Engine::KeyCode::KeyRelease);
			} break;
			// Keyboard
			case WM_KEYDOWN: {
				u32 action = (lParam & (1 << 30) ? Engine::KeyCode::KeyPress : Engine::KeyCode::KeyRepeat);
				u32 scancode = lParam & 0xFF0000;
				window->KeyboardEvent(WindowsInput::ConvertWindowsKeyCode(wParam), scancode, 0, action);
			} break;
			case WM_KEYUP: {
				u32 action = Engine::KeyCode::KeyRelease;
				u32 scancode = lParam & 0xFF0000;
				window->KeyboardEvent(WindowsInput::ConvertWindowsKeyCode(wParam), scancode, 0, action);
			} break;
			}
			if(ImGui_ImplWin32_WndProcHandler(handle, msg, wParam, lParam)) return 0;
			return DefWindowProc(handle, msg, wParam, lParam);
		}

		WindowsWindow::WindowsWindow(const Core::WindowConfig& config)
		{
			m_Config = config;
			m_Width = m_Config.width;
			m_Height = m_Config.height;

			// Gets the hInstance?
			HINSTANCE hInstance = GetModuleHandle(NULL);

			// Get window class title
			u64 windowClassTitleSize = 0;
			mbstowcs_s(&windowClassTitleSize, nullptr, 0, (config.title + "_WINDOWS_WINDOW_CLASS").c_str(), _TRUNCATE);
			std::vector<wchar_t> windowClassTitle(windowClassTitleSize);
			mbstowcs_s(&windowClassTitleSize, windowClassTitle.data(), windowClassTitleSize, (config.title + "_WINDOWS_WINDOW_CLASS").c_str(), _TRUNCATE);

			// Create window class
			WNDCLASS windowClass = { };
			windowClass.lpfnWndProc   = WindowProcess;
			windowClass.hInstance     = hInstance;
			windowClass.hCursor       = LoadCursor(NULL, IDI_APPLICATION);
			windowClass.lpszClassName = windowClassTitle.data();
			RegisterClass(&windowClass);

			// Get window title
			u64 windowTitleSize = 0;
			mbstowcs_s(&windowTitleSize, nullptr, 0, config.title.c_str(), _TRUNCATE);
			std::vector<wchar_t> windowTitle(windowTitleSize);
			mbstowcs_s(&windowTitleSize, windowTitle.data(), windowTitleSize, config.title.c_str(), _TRUNCATE);

			// Create window
			HWND window = CreateWindowEx(
				0,
				windowClassTitle.data(),
				windowTitle.data(),
				WS_OVERLAPPEDWINDOW,

				CW_USEDEFAULT, CW_USEDEFAULT,
				config.width, config.height,

				NULL,
				NULL,
				hInstance,
				this
			);

			// Varify window
			if (!IsWindow(window)) {
				LOG_ERROR("Failed to create window ( %s ): %s", m_Config.title.c_str(), GetErrorMessage().c_str());
			}
			m_Handle = (void*)window;

			HDC deviceContext = GetDC(window);
			m_DeviceContext = (void*)deviceContext;

			// Set pixel format
			PIXELFORMATDESCRIPTOR pixelFormatSpec = {
				sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd  
				1,                     // version number  
				PFD_DRAW_TO_WINDOW |   // support window  
				PFD_SUPPORT_OPENGL |   // support OpenGL  
				PFD_DOUBLEBUFFER,      // double buffered  
				PFD_TYPE_RGBA,         // RGBA type  
				24,                    // 24-bit color depth  
				0, 0, 0, 0, 0, 0,      // color bits ignored  
				0,                     // no alpha buffer  
				0,                     // shift bit ignored  
				0,                     // no accumulation buffer  
				0, 0, 0, 0,            // accum bits ignored  
				32,                    // 32-bit z-buffer  
				0,                     // no stencil buffer  
				0,                     // no auxiliary buffer  
				PFD_MAIN_PLANE,        // main layer  
				0,                     // reserved  
				0, 0, 0                // layer masks ignored  
			};
			// Find best pixel format from device context
			int pixelFormat = ChoosePixelFormat(deviceContext, &pixelFormatSpec);
			SetPixelFormat(deviceContext, pixelFormat, &pixelFormatSpec);

			// Create rendering context
			HGLRC openGLContext = wglCreateContext(deviceContext);
			if (openGLContext == NULL) {
				LOG_ERROR("Failed to create OpenGL context for window ( %s ): %s", m_Config.title.c_str(), GetErrorMessage().c_str());
			}
			m_RenderingContext = (void*)openGLContext;
			
			// Make rendering context current
			if(!wglMakeCurrent(deviceContext, openGLContext)) {
				LOG_ERROR("Failed to create OpenGL context for window ( %s ): %s", m_Config.title.c_str(), GetErrorMessage().c_str());
			}

			SetVSync(m_Config.vSync);

			ShowWindow(window, SW_NORMAL);
			m_ShouldClose = false;

			// Allocate memory for all mouse button states
			m_InputData.mouseButtonStates = (i32*)malloc(sizeof(i32) * Engine::KeyCode::MouseButtonLast);
			if (!m_InputData.mouseButtonStates) {
				LOG_ERROR("Failed to allocate memory for the mouse button states buffer ( %s )", m_Config.title.c_str());
			}
			memset(m_InputData.mouseButtonStates, 0, sizeof(i32) * Engine::KeyCode::MouseButtonLast);

			// Allocate memory for all key states
			m_InputData.keyboardStates = (i32*)malloc(sizeof(i32) * Engine::KeyCode::KeyLast);
			if (!m_InputData.keyboardStates) {
				LOG_ERROR("Failed to allocate memory for the key states buffer ( %s )", m_Config.title.c_str());
			}
			memset(m_InputData.keyboardStates, 0, sizeof(i32) * Engine::KeyCode::KeyLast);
		}
		WindowsWindow::~WindowsWindow()
		{
			if (DestroyWindow((HWND)m_Handle)) {
				LOG_SOFT_ERROR("Failed to close window ( %s ): %s", m_Config.title.c_str(), GetErrorMessage().c_str());
				return;
			}
			if (m_InputData.mouseButtonStates != nullptr) free(m_InputData.mouseButtonStates);

			// TODO: delete the renderer context
		}

		void WindowsWindow::AddEventCallback(std::function<void(Core::Event&)> callback)
		{
			m_Callbacks.push_back(callback);
		}

		void WindowsWindow::MakeCurrent()
		{
			switch (m_Config.renderAPI) {
			case Renderer::API::OpenGL: {
				if (!wglMakeCurrent((HDC)m_DeviceContext, (HGLRC)m_RenderingContext)) {
					LOG_ERROR("Failed to create OpenGL context for window ( %s ): %s", m_Config.title.c_str(), GetErrorMessage().c_str());
				}
			} break;
			default: {
				LOG_ERROR("Renderer context not supported");
			}
			}
		}

		int WindowsWindow::GetWidth()
		{
			return m_Width;
		}
		int WindowsWindow::GetHeight()
		{
			return m_Height;
		}

		Renderer::API WindowsWindow::GetAPI()
		{
			return m_Config.renderAPI;
		}

		void* WindowsWindow::GetNativeWindow()
		{
			return m_Handle;
		}

		bool WindowsWindow::ShouldClose()
		{
			return m_ShouldClose;
		}
		void WindowsWindow::ShouldClose(bool shouldClose)
		{
			m_ShouldClose = shouldClose;
		}

		void WindowsWindow::Update()
		{
			if (m_ShouldClose) return;

			if (!SwapBuffers((HDC)m_DeviceContext)) {
				LOG_ERROR("Failed to swap buffers for window ( %s ): %s", m_Config.title.c_str(), GetErrorMessage().c_str());
			}

			MSG message = { };
			if (!PeekMessage(&message, (HWND)m_Handle, 0, 0, PM_REMOVE)) return;
			if (message.message == WM_QUIT) {
				m_ShouldClose = true;
				return;
			}

			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		void WindowsWindow::SetVSync(bool vSync)
		{
			m_Config.vSync = vSync;
			//if (vSync) glfwSwapInterval(1);
			//else	   glfwSwapInterval(0);
		}

		void WindowsWindow::CloseEvent()
		{
			m_ShouldClose = true;
			Core::WindowClose e;
			for (auto& callback : m_Callbacks) {
				callback(e);
			}
			DestroyWindow((HWND)m_Handle);
		}

		void WindowsWindow::ResizeEvent(i32 width, i32 height)
		{
			m_Width = width;
			m_Height = height;
			Core::WindowResize e(width, height);
			for (auto& callback : m_Callbacks) {
				callback(e);
			}
		}

		void WindowsWindow::MouseMovedEvent(i32 x, i32 y)
		{
			m_InputData.mousePosition = { static_cast<f32>(x), static_cast<f32>(y) };
			Core::MouseMoved e(static_cast<f32>(x), static_cast<f32>(y));
			for (auto& callback : m_Callbacks) {
				callback(e);
			}
		}

		void WindowsWindow::MouseScrollEvent(i32 x, i32 y)
		{
			Core::MouseScroll e(static_cast<f32>(x), static_cast<f32>(y));
			for (auto& callback : m_Callbacks) {
				callback(e);
			}
		}

		void WindowsWindow::MouseClickEvent(i32 button, i32 mods, i32 action)
		{
			if (button < 0 || button >= Engine::KeyCode::MouseButtonLast) {
				LOG_SOFT_ERROR("Invalid mouse button code ( %d )", button);
			}
			m_InputData.mouseButtonStates[button] = action;

			Core::MouseButton e(button, action, mods);
			for (auto& callback : m_Callbacks) {
				callback(e);
			}
		}

		void WindowsWindow::KeyboardEvent(i32 keycode, i32 scancode, i32 mods, i32 action)
		{
			if (keycode < 0 || keycode >= Engine::KeyCode::KeyLast) {
				LOG_SOFT_ERROR("Invalid key code ( %d )", keycode);
			}
			m_InputData.keyboardStates[keycode] = action;

			Core::Keyboard e(keycode, scancode, action, mods);
			for (auto& callback : m_Callbacks) {
				callback(e);
			}
		}
		
	}
}