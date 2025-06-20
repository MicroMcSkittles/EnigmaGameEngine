#pragma once
#include "Core/Process/SubProcess.h"
#include "Core/Process/RenderProc.h"
#include "Core/Process/ScriptProc.h"
#include "Physics/CollisionProc.h"
#include "Core/Window.h"
#include "Core/Event/Event.h"
#include "Core/Event/WindowEvent.h"
#include "Core/Logger.h"
#include "Renderer/RenderEnum.h"
#include <vector>
#include <string>

namespace Enigma {
	namespace Core {

		struct ApplicationConfig {
			Renderer::API rendererAPI;
			WindowConfig windowConfig;
			LoggerConfig loggerConfig; // only applies if ENABLE_LOGGER is defined
			bool useRenderProc;
		};

		class Application {
		public:
			Application(const ApplicationConfig& config);
			Application(const ApplicationConfig& config, int argc, char** argv);
			~Application();

			static Application* Get() { return s_Instance; }
			// Argument 0 should always be a path to the exe file
			std::vector<std::string>& GetArguments() { return m_Arguments; }
			// Argument 0 should always be a path to the exe file
			const std::string& GetArgument(int id) const;

			// Events trical down to sub processes from front to back
			void OnEvent(Event& e);
			bool OnWindowClose(WindowClose& e);

			// Creates a sub process and stores it to the sub process stack
			// Returns a pointer to the process
			// T must be a inherited class of the SubProcess class
			template<typename T>
			T* CreateSubProc() {
				T* proc = new T;
				proc->StartUp();
				m_SubProcStack.PushProcBack(proc);
				return proc;
			}

			void run();

		private:
			void Initialize(const ApplicationConfig& config);

		private:
			// All command line arguments the program received
			std::vector<std::string> m_Arguments;
			bool m_IsRunning;

			// Used to find delta time
			float m_LastMS;

			SubProcStack m_SubProcStack;

			RenderProc* m_RenderProc;
			ScriptProc* m_ScriptProc;
			Physics::CollisionProc* m_CollisionProc;

			Window* m_Window;

		private:
			inline static Application* s_Instance;
		};

		Application* CreateApplication(int argc, char** argv);

	};
};