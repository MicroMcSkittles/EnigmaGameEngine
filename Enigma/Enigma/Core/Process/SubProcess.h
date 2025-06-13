#pragma once
#include "Core/Event/Event.h"
#include <vector>

namespace Enigma {
	namespace Core {
		class SubProcess {
		public:
			SubProcess() { }

			// Called when sub process is created
			virtual void StartUp() {}
			// Called when sub process is destroyed
			virtual void ShutDown() {}
			// Called every time a event occures
			// Events trical down to sub processes from front to back
			virtual bool OnEvent(Event& e) { return false; }
			// Called every frame
			virtual void Update(float deltaTime) {}
			// Called when the engine is rendering every thing
			virtual void Render() {}
			// Called when the engine is showing ImGui
			virtual void ImGui() {}
		};

		class SubProcStack {
		public:
			SubProcStack();
			~SubProcStack();

			void PushProcBack(SubProcess* subProc);
			void PushProcFront(SubProcess* subProc);

			void RemoveProc(SubProcess* subProc);

			std::vector<SubProcess*>& GetData();

			// Calls the OnEvent function for every sub process, goes through list form front to back
			void OnEvent(Event& e);
			// Calls the Update function for every sub process
			void Update(float deltaTime);
			// Calls the Render function for every sub process
			void Render();
			// Calls the ImGui function for every sub process
			void ImGui();

		private:
			std::vector<SubProcess*> m_SubProcesses;
		};
	}
}