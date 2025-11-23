#pragma once
#include "Enigma/Engine/DeltaTime.h"
#include "Enigma/Core/SparseSet.h"
#include "Enigma/Core/Window.h"
#include "Enigma/Core/Event/Event.h"
#include "Enigma/ImGui/ImGuiHandler.h"
#include "Enigma/Core/Process/SubProcess.h"
#include "Enigma/Renderer/RenderAPI.h"

namespace Enigma::Core {

	class Application {
	public:
		static unique<Application>& Create(i32 argc, i8** argv);
		Application(i32 argc, i8** argv);
		~Application();

		static unique<Application>& Get() { return s_Instance; }
		// Argument 0 should always be a path to the exe file
		static std::vector<std::string>& GetArguments();
		// Argument 0 should always be a path to the exe file
		static const std::string& GetArgument(size_t id);

		// Finishes the rest of the current frame then closes the application
		static void Close();

		static void EventCallback(Event& e);

		// Creates a sub process and stores it to the sub process stack
		// Returns a pointer to the process
		// T must be a inherited class of the SubProcess class
		template<typename T>
		static ref<T> CreateSubProc() {
			ref<T> proc = CreateRef<T>();
			ID id = s_Data->subProcStack.PushProcBack(proc);
			return proc;
		}
		static ref<SubProcess> GetSubProcess(ID id);
		static void BindSubProcToWindow(ref<SubProcess> proc, ID windowID);

		static ID CreateWindow(const WindowConfig& config);
		static ID CreateWindow(const WindowConfig& windowConfig, const ImGuiConfig& imguiConfig);
		static ref<Window> GetWindow(ID id);
		static ID GetWindowID(ref<Window> window);
		static ref<ImGuiHandler> GetImGui(ID id);

		static void UseRenderAPI(Renderer::API api);
		static ref<Renderer::RenderAPI> GetRenderAPI(Renderer::API api);

		static void Run();

	private:
		
		struct WindowHandler {
			ref<Window> window;
			ref<ImGuiHandler> imgui;
			std::vector<ID> engineInstances;
			std::vector<ID> subProcesses;
		};

		struct Data {
			// All command line arguments the program received
			std::vector<std::string> arguments;
			bool isRunning;

			Engine::DeltaTime deltaTime;

			SubProcStack subProcStack;

			IDHandler<ref<WindowHandler>> windows;
			std::map<Renderer::API, ref<Renderer::RenderAPI>> renderAPIs;
		};

	private:
		inline static unique<Application> s_Instance;
		inline static unique<Data> s_Data;
	};

	void ApplicationMain();

};