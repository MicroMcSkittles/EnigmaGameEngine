#pragma once
#include "Core/Window.h"

namespace Enigma {
	namespace Platform {

		class WindowsWindow : public Core::Window {
		public:
			WindowsWindow(const Core::WindowConfig& config, std::function<void(Core::Event&)> eventCallback);
			~WindowsWindow();

			virtual int GetWidth() override;
			virtual int GetHeight() override;

			virtual bool ShouldClose() override;
			virtual void ShouldClose(bool shouldClose) override;

			virtual void Update() override;

			virtual void SetVSync(bool vSync) override;

			void* GetHandle() { return m_Handle; }

		private:
			struct WindowData {
				int width;
				int height;

				std::function<void(Core::Event&)> callback;
			};
			WindowData m_Data;
			Core::WindowConfig m_Config;
			void* m_Handle;
		};

	}
}