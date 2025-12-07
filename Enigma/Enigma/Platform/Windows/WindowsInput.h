#pragma once
#include "Enigma/Engine/Input.h"

#include "Enigma/Platform/Windows/WindowsWindow.h"

namespace Enigma::Platform {

	class WindowsInput : public Engine::Input {
	public:
		WindowsInput(const Engine::InputConfig& config);

		static u64 ConvertWindowsKeyCode(u64 keycode);

	protected:
		virtual bool IsKeyPressedImpl(i32 key) override;
		virtual bool IsMouseButtonPressedImpl(i32 button) override;

		virtual glm::vec2 GetMousePositionImpl() override;
		virtual f32 GetMouseXImpl() override;
		virtual f32 GetMouseYImpl() override;

	private:
		ref<WindowsWindow> m_Window;
		Engine::Surface* m_Surface;
	};

}