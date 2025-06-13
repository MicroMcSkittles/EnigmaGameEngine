#include "Core/Process/Application.h"
#include "Core/Core.h"

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

			m_Window = Window::Create(config.windowConfig);

			// Set to false for testing reasons
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

		void Application::run()
		{
			while (m_IsRunning) {

				float deltaTime = 0;

				m_SubProcStack.Update(deltaTime);

				m_SubProcStack.Render();

				m_SubProcStack.ImGui();

				if (m_Window->ShouldClose()) m_IsRunning = false;
				m_Window->Update();
			}
		}
	};
};