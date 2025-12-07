#pragma once
#include "Enigma/Core/Window.h"

#include <glm/glm.hpp>

namespace Enigma::Platform {

	struct WindowsInputData {
		glm::vec2 mousePosition;
		i32* mouseButtonStates;
		i32* keyboardStates;
	};

	class WindowsWindow : public Core::Window {
	public:
		WindowsWindow(const Core::WindowConfig& config);
		~WindowsWindow();

		virtual void AddEventCallback(std::function<void(Core::Event&)> callback) override;
		virtual void MakeCurrent() override;

		virtual i32 GetWidth() override;
		virtual i32 GetHeight() override;

		virtual Renderer::API GetAPI() override;

		virtual void* GetNativeWindow() override;

		virtual bool ShouldClose() override;
		virtual void ShouldClose(bool shouldClose) override;

		virtual void Update() override;

		virtual void SetVSync(bool vSync) override;

		void* GetHandle() { return m_Handle; }

		const WindowsInputData& GetInputData() { return m_InputData; }

	public:
		void CloseEvent();
		void ResizeEvent(u32 width, u32 height);
		void MouseMovedEvent(i32 x, i32 y);
		void MouseScrollEvent(i32 x, i32 y);
		void MouseClickEvent(u64 button, u32 mods, u32 action);
		void KeyboardEvent(u64 keycode, u32 scancode, u32 mods, u32 action);

	private:
		Core::WindowConfig m_Config;
		WindowsInputData m_InputData;

		void* m_Handle;
		void* m_RenderingContext;
		void* m_DeviceContext;

		bool m_ShouldClose;
		u32 m_Width;
		u32 m_Height;
		std::vector<std::function<void(Core::Event&)>> m_Callbacks;
	};

}