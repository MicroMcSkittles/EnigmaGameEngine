#include "SubProcess.h"

namespace Enigma {
	namespace Core {
		SubProcStack::SubProcStack()
		{
		}
		SubProcStack::~SubProcStack()
		{
		}

		void SubProcStack::PushProcBack(SubProcess* subProc)
		{
			m_SubProcesses.push_back(subProc);
		}
		void SubProcStack::PushProcFront(SubProcess* subProc)
		{
			m_SubProcesses.insert(m_SubProcesses.begin(), subProc);
		}

		void SubProcStack::RemoveProc(SubProcess* subProc)
		{
			for (int i = 0; i < m_SubProcesses.size(); ++i) {
				SubProcess* proc = m_SubProcesses[i];
				if (subProc != proc) continue;
				m_SubProcesses.erase(m_SubProcesses.begin() + i);
				break;
			}
		}

		std::vector<SubProcess*>& SubProcStack::GetData()
		{
			return m_SubProcesses;
		}

		void SubProcStack::Update(float deltaTime)
		{
			for (auto& proc : m_SubProcesses) {
				proc->Update(deltaTime);
			}
		}

		void SubProcStack::Render()
		{
			for (auto& proc : m_SubProcesses) {
				proc->Render();
			}
		}

		void SubProcStack::ImGui()
		{
			for (auto& proc : m_SubProcesses) {
				proc->ImGui();
			}
		}
	}
}
