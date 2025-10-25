#include "Enigma/Core/Process/Application.h"
#include "Enigma/Core/Core.h"
#include "Enigma/Core/System.h"
#include "Enigma/Core/Utilities/Logger.h"

namespace Enigma {
	namespace Core {
		unique<Application>& Application::Create(i32 argc, i8** argv)
		{
			if (s_Instance != nullptr) {
				LOG_SOFT_ERROR("Application Instance already exists");
				return s_Instance;
			}
			
			s_Instance = CreateUnique<Application>(argc, argv);

			return s_Instance;
		}
		Application::Application(i32 argc, i8** argv)
		{
			s_Data = CreateUnique<Data>();
      
			System::Init();

			// Store all commandline arguments
			s_Data->arguments = std::vector<std::string>(argv, argv + argc);

			// Initialize logger
			Core::LoggerConfig loggerConfig;
			loggerConfig.flags = Core::LoggerFunction | Core::LoggerTime | Core::LoggerShort;
			loggerConfig.priorityLevel = 5;
			INIT_LOGGER(loggerConfig);

			s_Data->isRunning = true;
		}
		Application::~Application()
		{
			System::Shutdown();
		}

		std::vector<std::string>& Application::GetArguments()
		{
			return s_Data->arguments;
		}
		const std::string& Application::GetArgument(size_t id)
		{
			// Check if id is valid
			if (id < 0 || id >= s_Data->arguments.size()) {
				LOG_WARNING("Argument at index ( %s ) doesn't exist", std::to_string(id).c_str());
				return s_Data->arguments[0];
			}
			// Return argument at id
			return s_Data->arguments[id];
		}

		void Application::Close()
		{
			s_Data->isRunning = false;
		}

		void Application::EventCallback(Event& e)
		{
			for (ref<WindowHandler>& handler : s_Data->windows.GetData()) {
				for (ID& id : handler->subProcesses) s_Data->subProcStack.GetProcess(id)->OnEvent(e);
			}
		}

		ref<SubProcess> Application::GetSubProcess(ID id)
		{
			return s_Data->subProcStack.GetProcess(id);
		}

		void Application::BindSubProcToWindow(ref<SubProcess> proc, ID windowID)
		{
			if (!s_Data->windows.Contains(windowID)) {
				LOG_ERROR("Failed to create sub process. Window does not exist");
			}
			ID procID = s_Data->subProcStack.GetProcessID(proc);
			s_Data->windows.Get(windowID)->subProcesses.push_back(procID);
		}

		ID Application::CreateWindow(const WindowConfig& config)
		{
			ref<WindowHandler> handler = CreateRef<WindowHandler>();
			handler->window = Window::Create(config);

			if (!s_Data->renderAPIs.count(config.renderAPI)) {
				ref<Renderer::RenderAPI> api = Renderer::RenderAPI::CreateContext(config.renderAPI);
				s_Data->renderAPIs.insert({ config.renderAPI, api });
			}

			handler->imgui = nullptr;
			return s_Data->windows.Create(handler);
		}
		ID Application::CreateWindow(const WindowConfig& windowConfig, const ImGuiConfig& imguiConfig)
		{
			ref<WindowHandler> handler = CreateRef<WindowHandler>();
			handler->window = Window::Create(windowConfig);

			if (!s_Data->renderAPIs.count(windowConfig.renderAPI)) {
				ref<Renderer::RenderAPI> api = Renderer::RenderAPI::CreateContext(windowConfig.renderAPI);
				s_Data->renderAPIs.insert({ windowConfig.renderAPI, api });
			}

			ImGuiConfig altImGuiConfig = imguiConfig;
			altImGuiConfig.window = handler->window;
			altImGuiConfig.renderAPI = windowConfig.renderAPI;
			handler->imgui = ImGuiHandler::Create(altImGuiConfig);

			return s_Data->windows.Create(handler);
		}
		ref<Window> Application::GetWindow(ID id)
		{
			if (!s_Data->windows.Contains(id)) {
				LOG_WARNING("Window with id %s doesn't exist", id.ToString().c_str());
				return nullptr;
			}

			return s_Data->windows.Get(id)->window;
		}
		ID Application::GetWindowID(ref<Window> window)
		{
			for (ref<WindowHandler>& handler : s_Data->windows.GetData()) {
				if (handler->window == window) {
					return s_Data->windows.Get(handler);
				}
			}
			LOG_WARNING("Window isn't registered");
			return Core::InvalidID;
		}
		ref<ImGuiHandler> Application::GetImGui(ID id)
		{
			if (!s_Data->windows.Contains(id)) {
				LOG_WARNING("ImGui context with id %s doesn't exist", id.ToString().c_str());
				return nullptr;
			}

			ref<ImGuiHandler> context = s_Data->windows.Get(id)->imgui;
			if (context == nullptr) {
				LOG_WARNING("ImGui context with id %s doesn't exist", id.ToString().c_str());
			}

			return context;
		}

		void Application::UseRenderAPI(Renderer::API api)
		{
			if (!s_Data->renderAPIs.count(api)) {
				LOG_WARNING("Render API doesn't exist ( %s )", Renderer::ToString(api).c_str());
				return;
			}
			Renderer::RenderAPI::MakeContextCurrent(s_Data->renderAPIs[api]);
		}
		ref<Renderer::RenderAPI> Application::GetRenderAPI(Renderer::API api)
		{
			if (!s_Data->renderAPIs.count(api)) {
				LOG_WARNING("Render API doesn't exist ( %s )", Renderer::ToString(api).c_str());
				return nullptr;
			}

			return s_Data->renderAPIs[api];
		}

		void Application::Run()
		{
			while (s_Data->isRunning) {

				s_Data->deltaTime.Calculate();

				s_Data->subProcStack.Update(s_Data->deltaTime);
				s_Data->subProcStack.Render();

				std::vector<ref<WindowHandler>>& windows = s_Data->windows.GetData();
				for (u64 i = 0; i < windows.size(); ++i) {
					ref<WindowHandler> windowHandler = windows[i];

					windowHandler->window->MakeCurrent();
					if (windowHandler->window->ShouldClose()) {
					  //windowHandler->window.;
					  //if (windowHandler->imgui != nullptr) delete windowHandler->imgui;
					  s_Data->windows.Delete(s_Data->windows.Get(i));
					  i -= 1;
					  continue;
					}

					if (windowHandler->imgui != nullptr && (!windowHandler->engineInstances.empty() || !windowHandler->subProcesses.empty())) {
						ImGuiHandler::MakeCurrent(windowHandler->imgui);
						ImGuiHandler::StartFrame();

						for (auto& processID : windowHandler->subProcesses) {
							s_Data->subProcStack.GetProcess(processID)->ImGui();
						}

						ImGuiHandler::EndFrame();
					}

					windowHandler->window->Update();
				}

				if (s_Data->windows.GetData().empty() && s_Data->subProcStack.GetData().empty()) s_Data->isRunning = false;
			}

			s_Data->subProcStack.ShutDown();
		}
	};
};
