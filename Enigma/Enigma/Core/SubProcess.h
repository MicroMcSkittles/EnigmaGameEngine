#pragma once
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