#pragma once
#include "Enigma/Engine/DeltaTime.h"
#include "Enigma/Core/IdHandler.h"
#include "Enigma/Core/Window.h"
#include "Enigma/Core/Event/Event.h"
#include "Enigma/ImGui/ImGuiHandler.h"
#include "Enigma/Core/Process/SubProcess.h"
#include "Enigma/Renderer/RenderAPI.h"

#include <vector>
#include <map>
#include <string>

namespace Enigma {
	namespace Core {

		class Application {
		public:
			Application(int argc, char** argv);
			~Application();

			static Application* Get() { return s_Instance; }
			// Argument 0 should always be a path to the exe file
			static std::vector<std::string>& GetArguments();
			// Argument 0 should always be a path to the exe file
			static const std::string& GetArgument(int id);

			// Finishes the rest of the current frame then closes the application
			static void Close();

			static void EventCallback(Event& e);

			// Creates a sub process and stores it to the sub process stack
			// Returns a pointer to the process
			// T must be a inherited class of the SubProcess class
			template<typename T>
			static T* CreateSubProc() {
				T* proc = new T;
				ID id = s_Data->subProcStack.PushProcBack(proc);
				return proc;
			}
			static void BindSubProcToWindow(SubProcess* proc, ID windowID);

			/*static ID CreateEngineInstance(const Engine::EngineConfig& config);
			static void DeleteEngineInstance(Engine::Engine* instance);
			static Engine::Engine* GetEngineInstance(ID id);
			static ID GetEngineInstanceID(Engine::Engine* instance);*/

			static ID CreateWindow(const WindowConfig& config);
			static ID CreateWindow(const WindowConfig& windowConfig, const ImGuiConfig& imguiConfig);
			static Window* GetWindow(ID id);
			static ID GetWindowID(Window* window);
			static ImGuiHandler* GetImGui(ID id);

			static void UseRenderAPI(Renderer::API api);
			static Renderer::RenderAPI* GetRenderAPI(Renderer::API api);

			static void Run();

		private:
			struct WindowHandler {
				Window* window;
				ImGuiHandler* imgui;
				std::vector<ID> engineInstances;
				std::vector<ID> subProcesses;
			};

			struct Data {
				// All command line arguments the program received
				std::vector<std::string> arguments;
				bool isRunning;

				Engine::DeltaTime deltaTime;

				SubProcStack subProcStack;

				IDHandler<WindowHandler*> windows;
				//IDHandler<Engine::Engine*> engineInstances;
				std::map<Renderer::API, Renderer::RenderAPI*> renderAPIs;
			};

		private:
			inline static Application* s_Instance;
			inline static Data* s_Data;
		};

		void ApplicationMain(Application* app);

	};
};