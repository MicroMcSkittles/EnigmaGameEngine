#pragma once
#include <glm/glm.hpp>
#include <string>

#include "Enigma/Core/Window.h"
#include "Enigma/Engine/Surface.h"

namespace Enigma {
	namespace Engine {

		struct InputConfig {
			Core::Window* window;
			Surface* surface;
		};

		class Input {
		public:
			static Input* Create(const InputConfig& config);
			static void MakeCurrent(Input* context) { s_CurrentContext = context; }

			static bool IsKeyPressed(int key) { return s_CurrentContext->IsKeyPressedImpl(key); }
			static bool IsMouseButtonPressed(int button) { return s_CurrentContext->IsMouseButtonPressedImpl(button); }

			static glm::vec2 GetMousePosition() { return s_CurrentContext->GetMousePositionImpl(); }
			static float GetMouseX() { return s_CurrentContext->GetMouseXImpl(); }
			static float GetMouseY() { return s_CurrentContext->GetMouseYImpl(); }

		protected:
			virtual bool IsKeyPressedImpl(int key) = 0;
			virtual bool IsMouseButtonPressedImpl(int button) = 0;
			
			virtual glm::vec2 GetMousePositionImpl() = 0;
			virtual float GetMouseXImpl() = 0;
			virtual float GetMouseYImpl() = 0;

		private:
			inline static Input* s_CurrentContext;
		};

	}
}