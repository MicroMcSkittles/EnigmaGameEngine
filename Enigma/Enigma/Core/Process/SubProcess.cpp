#include "SubProcess.h"
#include "Core/Process/Application.h"

namespace Enigma {
	namespace Core {
		Engine::Engine* SubProcess::GetEngine()
		{
			return Application::GetEngineInstance(*m_EngineID);
		}

		SubProcStack::SubProcStack()
		{
		}
		SubProcStack::~SubProcStack()
		{
		}

		ID SubProcStack::PushProcBack(SubProcess* subProc)
		{
			return m_SubProcesses.Create(subProc);
		}

		void SubProcStack::RemoveProc(ID id)
		{
			m_SubProcesses.Delete(id);
		}

		SubProcess* SubProcStack::GetProcess(ID id)
		{
			return m_SubProcesses.Get(id);
		}

		ID SubProcStack::GetProcessID(SubProcess* proc)
		{
			return m_SubProcesses.Get(proc);
		}

		std::vector<SubProcess*>& SubProcStack::GetData()
		{
			return m_SubProcesses.GetData();
		}

		void SubProcStack::OnEvent(Core::Event& e)
		{
			if (e.Handled()) return;
			// Loop through each sub process from front to back and
			std::vector<SubProcess*>& processes = m_SubProcesses.GetData();
			for (int i = processes.size() - 1; i >= 0; i--) {
				auto proc = processes[i];
				e.Handled(proc->OnEvent(e));
				if (e.Handled()) break;
			}
		}

		void SubProcStack::Update(Engine::DeltaTime deltaTime)
		{
			for (int i = 0; i < m_SubProcesses.GetData().size(); ++i) {
				SubProcess* proc = m_SubProcesses.GetData()[i];
				if (!proc->m_Started) {
					proc->StartUp();
					proc->m_Started = true;
				}
				proc->Update(deltaTime);
			}
		}

		void SubProcStack::Render()
		{
			for (auto& proc : m_SubProcesses.GetData()) {
				proc->Render();
			}
		}

		void SubProcStack::ImGui()
		{
			for (auto& proc : m_SubProcesses.GetData()) {
				proc->ImGui();
			}
		}

		void SubProcStack::ShutDown()
		{
			for (auto& proc : m_SubProcesses.GetData()) {
				proc->ShutDown();
			}

			m_SubProcesses.Clear();
		}
		
	}
}
