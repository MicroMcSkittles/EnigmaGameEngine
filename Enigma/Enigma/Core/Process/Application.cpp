#include "Core/Process/Application.h"
#include "Core/Core.h"

//TEMP delete later
#include "Core/Input.h"
#include <glad/glad.h>

namespace Enigma {
	namespace Core {
		void Application::Initialize(const ApplicationConfig& config)
		{
			if (s_Instance) {
				LOG_SOFT_ERROR("Application Instance already exists");
				return;
			}
			s_Instance = this;

			INIT_LOGGER(config.loggerConfig);

			m_Window = Window::Create(config.windowConfig, CLASS_BIND_ARGS_1(Application::OnEvent));

			m_IsRunning = true;
		}

		Application::Application(const ApplicationConfig& config) {
			Initialize(config);
		}
		Application::Application(const ApplicationConfig& config, int argc, char** argv)
		{
			m_Arguments = std::vector<std::string>(argv, argv + argc);

			Initialize(config);
		}
		Application::~Application()
		{
			// Destroy all processes
			for (auto& proc : m_SubProcStack.GetData()) {
				proc->ShutDown();
			}
		}

		void Application::OnEvent(Event& e)
		{
			EventHandler handler(e);
			handler.Dispatch<WindowClose>(CLASS_BIND_ARGS_1(Application::OnWindowClose));

			m_SubProcStack.OnEvent(e);
		}
		bool Application::OnWindowClose(WindowClose& e)
		{
			m_IsRunning = false;
			return false;
		}

		void Application::run()
		{
			while (m_IsRunning) {

				float deltaTime = 0;

				m_SubProcStack.Update(deltaTime);

				m_SubProcStack.Render();

				m_SubProcStack.ImGui();

				//TEMP delete later
				auto pos = Input::GetMousePosition();
				pos.x /= (float)m_Window->GetWidth();
				pos.y /= (float)m_Window->GetHeight();
				glClearColor(pos.x, pos.y, 1, 1);
				glClear(GL_COLOR_BUFFER_BIT);

				m_Window->Update();
			}
		}
	};
};