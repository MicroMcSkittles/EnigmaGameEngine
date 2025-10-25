#include "Enigma/Core/Process/SubProcess.h"
#include "Enigma/Core/Process/Application.h"

namespace Enigma {
	namespace Core {
		ref<SubProcess> SubProcStack::Create()
		{
			return CreateRef<SubProcess>();
		}

		ID SubProcStack::PushProcBack(ref<SubProcess> subProc)
		{
			ID id = m_SubProcesses.Create(subProc);
			subProc->m_ProcessID = id;
			return id;
		}

		void SubProcStack::RemoveProc(ID id)
		{
			m_SubProcesses.Delete(id);
		}

		ref<SubProcess> SubProcStack::GetProcess(ID id)
		{
			return m_SubProcesses.Get(id);
		}

		ID SubProcStack::GetProcessID(ref<SubProcess> proc)
		{
			return m_SubProcesses.Get(proc);
		}

		std::vector<ref<SubProcess>>& SubProcStack::GetData()
		{
			return m_SubProcesses.GetData();
		}

		void SubProcStack::OnEvent(Core::Event& e)
		{
			if (e.Handled()) return;
			// Loop through each sub process from front to back and
			std::vector<ref<SubProcess>>& processes = m_SubProcesses.GetData();
			for (u64 i = processes.size(); i > 0; i--) {
				auto proc = processes[i - 1];
				e.Handled(proc->OnEvent(e));
				if (e.Handled()) break;
			}
		}

		void SubProcStack::Update(Engine::DeltaTime deltaTime)
		{
			for (u64 i = 0; i < m_SubProcesses.GetData().size(); ++i) {
				ref<SubProcess> proc = m_SubProcesses.GetData()[i];
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
