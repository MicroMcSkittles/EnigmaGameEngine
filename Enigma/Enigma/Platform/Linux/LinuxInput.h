#pragma once
#include "Core/Input.h"

namespace Enigma {
	namespace Platform {

		class LinuxInput : public Core::Input {
		protected:
			virtual bool IsKeyPressedImpl(int key) override;
			virtual bool IsMouseButtonPressedImpl(int button) override;

			virtual glm::vec2 GetMousePositionImpl() override;
			virtual float GetMouseXImpl() override;
			virtual float GetMouseYImpl() override;

			virtual std::string GetKeyNameImpl(int key) override;
			virtual std::string GetButtonNameImpl(int button) override;
			virtual std::string GetActionNameImpl(int action) override;
			virtual std::string GetModsNameImpl(int action) override;
		};

	}
}