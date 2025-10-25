#pragma once
#include "Enigma/Engine/Input.h"

#include "Enigma/Platform/Windows/WindowsWindow.h"

namespace Enigma {
	namespace Platform {

		class WindowsInput : public Engine::Input {
		public:
			WindowsInput(const Engine::InputConfig& config);

			static i32 ConvertWindowsKeyCode(i32 keycode);

		protected:
			virtual bool IsKeyPressedImpl(i32 key) override;
			virtual bool IsMouseButtonPressedImpl(i32 button) override;

			virtual glm::vec2 GetMousePositionImpl() override;
			virtual f32 GetMouseXImpl() override;
			virtual f32 GetMouseYImpl() override;

		private:
			ref<WindowsWindow> m_Window;
			ref<Engine::Surface> m_Surface;
		};

	}
}