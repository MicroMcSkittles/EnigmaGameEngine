#pragma once
#include "Enigma/Core/Window.h"

#include <vector>

namespace Enigma {
	namespace Platform {

		class LinuxWindow : public Core::Window {
		public:
			LinuxWindow(const Core::WindowConfig& config);
			~LinuxWindow();

			virtual void AddEventCallback(std::function<void(Core::Event&)> callback) override;
			virtual void MakeCurrent() override;

			virtual int GetWidth() override;
			virtual int GetHeight() override;

			virtual Renderer::API GetAPI() override;

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
