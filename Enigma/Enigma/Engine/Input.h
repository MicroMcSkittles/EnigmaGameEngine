#pragma once
#include <glm/glm.hpp>
#include <string>

#include "Enigma/Core/Types.h"
#include "Enigma/Core/Window.h"
#include "Enigma/Engine/Surface.h"

namespace Enigma {
	namespace Engine {

		struct InputConfig {
			ref<Core::Window> window;
			Surface* surface;
		};

		class Input {
		public:
			static ref<Input> Create(const InputConfig& config);
			static void MakeCurrent(ref<Input> context) { s_CurrentContext = context; }

			static bool IsKeyPressed(i32 key) { return s_CurrentContext->IsKeyPressedImpl(key); }
			static bool IsMouseButtonPressed(i32 button) { return s_CurrentContext->IsMouseButtonPressedImpl(button); }

			static glm::vec2 GetMousePosition() { return s_CurrentContext->GetMousePositionImpl(); }
			static f32 GetMouseX() { return s_CurrentContext->GetMouseXImpl(); }
			static f32 GetMouseY() { return s_CurrentContext->GetMouseYImpl(); }

		protected:
			virtual bool IsKeyPressedImpl(i32 key) = 0;
			virtual bool IsMouseButtonPressedImpl(i32 button) = 0;
			
			virtual glm::vec2 GetMousePositionImpl() = 0;
			virtual f32 GetMouseXImpl() = 0;
			virtual f32 GetMouseYImpl() = 0;

		private:
			inline static ref<Input> s_CurrentContext;
		};

	}
}