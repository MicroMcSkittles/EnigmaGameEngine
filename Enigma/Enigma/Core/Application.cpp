#include "Core/Application.h"
#include "Core/Core.h"

namespace Enigma {
	namespace Core {
		void Application::Initialize()
		{
			if (s_Instance) {
				LOG_SOFT_ERROR("Application Instance already exists");
				return;
			}
			s_Instance = this;

			// Setup logger if its enabled
#ifdef ENABLE_LOGGER
			LoggerConfig loggerConfig;
			loggerConfig.flags = File | Function | Priority;
			loggerConfig.priorityLevel = 5;
			Logger::Init(loggerConfig);
#endif

			// Set to false for testing reasons
			m_IsRunning = false;
		}

		Application::Application() {
			Initialize();
		}
		Application::Application(int argc, char** argv)
		{
			m_Arguments = std::vector<std::string>(argv, argv + argc);

			Initialize();
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
			}
		}
	};
};