#pragma once
#include <string>
#include <functional>
#include "Core/Event/Event.h"

namespace Enigma {
	namespace Core {

		struct WindowConfig {
			std::string title;

			int width = 600;
			int height = 800;

			bool resizable = true;
			bool vSync = true;

			WindowConfig(const std::string& title = "Window", 
				int width = 800, int height = 600, 
				bool resizable = true, bool vSync = true)
				: title(title), width(width), height(height), 
				 resizable(resizable), vSync(vSync) { }
		};

		class Window {
		public:
			static Window* Create(const WindowConfig& config, std::function<void(Event&)> eventCallback);
			static Window* Get() { return s_Instance; }

			virtual int GetWidth() = 0;
			virtual int GetHeight() = 0;

			virtual bool ShouldClose() = 0;
			virtual void ShouldClose(bool shouldClose) = 0;

			virtual void Update() = 0;

			virtual void SetVSync(bool vSync) = 0;

		protected:
			inline static Window* s_Instance;
		};

	}
}