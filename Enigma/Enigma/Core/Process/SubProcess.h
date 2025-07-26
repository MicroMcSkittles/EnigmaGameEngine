#pragma once
#include "Enigma/Core/Event/Event.h"
#include "Enigma/Core/IdHandler.h"
#include "Enigma/Engine/DeltaTime.h"
#include <vector>

namespace Enigma {
	namespace Core {
		class SubProcStack;

		class SubProcess {
		public:
			SubProcess() : m_Started(false) { }

			// Called when sub process is created
			virtual void StartUp() {}
			// Called when sub process is destroyed
			virtual void ShutDown() {}
			// Called every time a event occures
			// Events trical down to sub processes from front to back
			virtual bool OnEvent(Event& e) { return false; }
			// Called every frame
			virtual void Update(Engine::DeltaTime deltaTime) {}
			// Called when the engine is rendering every thing
			virtual void Render() {}
			// Called when the engine is showing ImGui
			virtual void ImGui() {}

		private:
			bool m_Started;

		private:
			friend SubProcStack;
		};

		class SubProcStack {
		public:
			SubProcStack();
			~SubProcStack();

			ID PushProcBack(SubProcess* subProc);

			void RemoveProc(ID id);

			SubProcess* GetProcess(ID id);
			ID GetProcessID(SubProcess* proc);
			std::vector<SubProcess*>& GetData();

			// Calls the OnEvent function for every sub process, goes through list form front to back
			void OnEvent(Event& e);
			// Calls the Update function for every sub process
			void Update(Engine::DeltaTime deltaTime);
			// Calls the Render function for every sub process
			void Render();
			// Calls the ImGui function for every sub process
			void ImGui();
			// Calls the Shutdown function for every sub process, then deletes every sub process
			void ShutDown();

		private:
			IDHandler<SubProcess*> m_SubProcesses;
		};
	}
}