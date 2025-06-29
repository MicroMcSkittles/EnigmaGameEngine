#pragma once
#include "Engine/Input.h"

#include "Platform/Windows/WindowsWindow.h"

namespace Enigma {
	namespace Platform {

		class WindowsInput : public Engine::Input {
		public:
			WindowsInput(const Engine::InputConfig& config);

		protected:
			virtual bool IsKeyPressedImpl(int key) override;
			virtual bool IsMouseButtonPressedImpl(int button) override;

			virtual glm::vec2 GetMousePositionImpl() override;
			virtual float GetMouseXImpl() override;
			virtual float GetMouseYImpl() override;

		private:
			WindowsWindow* m_Window;
			Engine::Surface* m_Surface;
		};

	}
}