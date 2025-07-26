#pragma once
#include <string>
#include <functional>
#include "Enigma/Core/Event/Event.h"
#include "Enigma/Renderer/RenderEnum.h"

namespace Enigma {
	namespace Core {

		struct WindowConfig {
			std::string title;

			int width = 600;
			int height = 800;

			bool resizable = true;
			bool vSync = true;

			Renderer::API renderAPI = Renderer::API::OpenGL;

			WindowConfig(const std::string& title = "Window", 
				int width = 800, int height = 600, 
				bool resizable = true, bool vSync = true,
				Renderer::API renderAPI = Renderer::API::OpenGL)
				: title(title), width(width), height(height), 
				 resizable(resizable), vSync(vSync),
				renderAPI(renderAPI) { }
		};

		// TODO: add remove event callback function
		class Window {
		public:
			static Window* Create(const WindowConfig& config);
			virtual ~Window() { }

			virtual void AddEventCallback(std::function<void(Event&)> callback) = 0;

			virtual void MakeCurrent() = 0;

			virtual int GetWidth() = 0;
			virtual int GetHeight() = 0;

			virtual Renderer::API GetAPI() = 0;

			virtual bool ShouldClose() = 0;
			virtual void ShouldClose(bool shouldClose) = 0;

			virtual void Update() = 0;

			virtual void SetVSync(bool vSync) = 0;
		};

	}
}