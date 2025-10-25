#pragma once
#include "Enigma/Core/Types.h"
#include "Enigma/Core/Event/Event.h"
#include "Enigma/Renderer/RenderEnum.h"

#include <string>
#include <functional>

namespace Enigma {
	namespace Core {

		struct WindowConfig {
			std::string title;

			i32 width = 600;
			i32 height = 800;

			bool resizable = true;
			bool vSync = true;

			Renderer::API renderAPI = Renderer::API::OpenGL;

			WindowConfig(
				const std::string& title = "Window", 
				i32 width = 800, i32 height = 600,
				bool resizable = true, 
				bool vSync = true,
				Renderer::API renderAPI = Renderer::API::OpenGL
			): title(title), width(width), height(height), resizable(resizable), vSync(vSync), renderAPI(renderAPI) { }
		};

		// TODO: add remove event callback function
		class Window {
		public:
			static ref<Window> Create(const WindowConfig& config);
			virtual ~Window() { }

			virtual void AddEventCallback(std::function<void(Event&)> callback) = 0;

			virtual void MakeCurrent() = 0;

			virtual i32 GetWidth() = 0;
			virtual i32 GetHeight() = 0;

			virtual Renderer::API GetAPI() = 0;

			virtual void* GetNativeWindow() = 0;

			// Returns if the window should close
			virtual bool ShouldClose() = 0;
			// Tells the window if it should close
			virtual void ShouldClose(bool shouldClose) = 0;

			virtual void Update() = 0;

			virtual void SetVSync(bool vSync) = 0;

			// Returns the address of a OpenGL function
			static void* GetGLProcAddress(const char* procName);

		};

	}
}