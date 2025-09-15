#pragma once
#include "Enigma/Core/Window.h"

#include <glm/glm.hpp>

namespace Enigma {
	namespace Platform {

		struct WindowsInputData {
			glm::vec2 mousePosition;
			int* mouseButtonStates;
			int* keyboardStates;
		};

		class WindowsWindow : public Core::Window {
		public:
			WindowsWindow(const Core::WindowConfig& config);
			~WindowsWindow();

			virtual void AddEventCallback(std::function<void(Core::Event&)> callback) override;
			virtual void MakeCurrent() override;

			virtual int GetWidth() override;
			virtual int GetHeight() override;

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
			void ResizeEvent(int width, int height);
			void MouseMovedEvent(int x, int y);
			void MouseScrollEvent(int x, int y);
			void MouseClickEvent(int button, int mods, int action);
			void KeyboardEvent(int keycode, int scancode, int mods, int action);

		private:
			Core::WindowConfig m_Config;
			WindowsInputData m_InputData;

			void* m_Handle;
			void* m_RenderingContext;
			void* m_DeviceContext;

			bool m_ShouldClose;
			int m_Width;
			int m_Height;
			std::vector<std::function<void(Core::Event&)>> m_Callbacks;
		};

	}
}