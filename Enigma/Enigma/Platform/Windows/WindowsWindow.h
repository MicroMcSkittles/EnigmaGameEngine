#pragma once
#include "Enigma/Core/Window.h"

namespace Enigma {
	namespace Platform {

		class WindowsWindow : public Core::Window {
		public:
			WindowsWindow(const Core::WindowConfig& config);
			~WindowsWindow();

			virtual void AddEventCallback(std::function<void(Core::Event&)> callback) override;
			virtual void MakeCurrent() override;

			virtual int GetWidth() override;
			virtual int GetHeight() override;

			virtual Renderer::API GetAPI() override;

			virtual void* GetNativeWindow() override;

			virtual bool ShouldClose() override;
			virtual void ShouldClose(bool shouldClose) override;

			virtual void Update() override;

			virtual void SetVSync(bool vSync) override;

			void* GetHandle() { return m_Handle; }

		private:
			struct WindowData {
				int width = NULL;
				int height = NULL;

				std::vector<std::function<void(Core::Event&)>> callbacks;
			};
			WindowData m_Data;
			Core::WindowConfig m_Config;
			void* m_Handle;
		};

	}
}