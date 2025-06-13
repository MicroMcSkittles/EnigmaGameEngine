#pragma once
#include "Core/Window.h"

namespace Enigma {
	namespace Platform {

		class WindowsWindow : public Core::Window {
		public:
			WindowsWindow(const Core::WindowConfig& config);
			~WindowsWindow();

			virtual int GetWidth() override;
			virtual int GetHeight() override;

			virtual bool ShouldClose() override;
			virtual void ShouldClose(bool shouldClose) override;

			virtual void Update() override;

			virtual void SetVSync(bool vSync) override;

		private:
			Core::WindowConfig m_Data;
			void* m_Handle;
		};

	}
}