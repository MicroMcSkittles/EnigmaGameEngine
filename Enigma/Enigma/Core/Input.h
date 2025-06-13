#pragma once
#include <glm/glm.hpp>
#include <string>

namespace Enigma {
	namespace Core {

		class Input {
		public:
			static bool IsKeyPressed(int key) { return s_Instance->IsKeyPressedImpl(key); }
			static bool IsMouseButtonPressed(int button) { return s_Instance->IsMouseButtonPressedImpl(button); }

			static glm::vec2 GetMousePosition() { return s_Instance->GetMousePositionImpl(); }
			static float GetMouseX() { return s_Instance->GetMouseXImpl(); }
			static float GetMouseY() { return s_Instance->GetMouseYImpl(); }

			static std::string GetKeyName(int key) { return s_Instance->GetKeyNameImpl(key); }
			static std::string GetButtonName(int button) { return s_Instance->GetButtonNameImpl(button); }
			static std::string GetActionName(int action) { return s_Instance->GetActionNameImpl(action); }
			static std::string GetModsName(int mods) { return s_Instance->GetModsNameImpl(mods); }

		protected:
			virtual bool IsKeyPressedImpl(int key) = 0;
			virtual bool IsMouseButtonPressedImpl(int button) = 0;
			
			virtual glm::vec2 GetMousePositionImpl() = 0;
			virtual float GetMouseXImpl() = 0;
			virtual float GetMouseYImpl() = 0;

			virtual std::string GetKeyNameImpl(int key) = 0;
			virtual std::string GetButtonNameImpl(int button) = 0;
			virtual std::string GetActionNameImpl(int action) = 0;
			virtual std::string GetModsNameImpl(int action) = 0;

		private:
			static Input* s_Instance;
		};

	}
}