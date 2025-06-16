#include "Core/Process/Application.h"
#include "Core/Core.h"
#include "Core/System.h"
#include "Renderer/RenderAPI.h"

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

			Renderer::RenderAPI::Init(config.rendererAPI);

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

		const std::string& Application::GetArgument(int id) const
		{
			if (id < 0 || id >= m_Arguments.size()) {
				LOG_WARNING("Argument at index ( " + std::to_string(id) + " ) doesn't exist");
				return "";
			}
			return m_Arguments[0];
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
			m_LastMS = System::GetTimeMS();

			while (m_IsRunning) {

				float currentMS = System::GetTimeMS();
				float deltaTime = currentMS - m_LastMS;
				m_LastMS = currentMS;

				m_SubProcStack.Update(deltaTime);

				m_SubProcStack.Render();

				m_SubProcStack.ImGui();

				m_Window->Update();
			}
		}
	};
};