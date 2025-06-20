#pragma once
#include "Engine/Input.h"

#include "Platform/Linux/LinuxWindow.h"

namespace Enigma {
	namespace Platform {

		class LinuxInput : public Engine::Input {
		public:
			LinuxInput(const Engine::InputConfig& config);

		protected:
			virtual bool IsKeyPressedImpl(int key) override;
			virtual bool IsMouseButtonPressedImpl(int button) override;

			virtual glm::vec2 GetMousePositionImpl() override;
			virtual float GetMouseXImpl() override;
			virtual float GetMouseYImpl() override;

		private:
			LinuxWindow* m_Window;
			Engine::Surface* m_Surface;
		};

	}
}