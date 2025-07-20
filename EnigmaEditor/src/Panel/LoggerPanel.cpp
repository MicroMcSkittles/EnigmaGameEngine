#include "Panel/LoggerPanel.h"

#include <imgui.h>

namespace Enigma {
	namespace Editor {

		void LoggerPanel::Log(const std::string& msg, const Core::LogInfo& info)
		{
			if (info.type == Core::LogType::Error) {
				std::cout << "\033[31m" << msg << "\033[0m" << std::endl;
				exit(-1);
			}

			m_Logs.push_back({ msg, info });
			if (m_Logs.size() > m_LogCap) {
				m_Logs.erase(m_Logs.begin() + (m_Logs.size() - m_LogCap - 1));
			}
		}

		void LoggerPanel::ImGui()
		{
			ImGui::ShowDemoWindow();
			ImGui::Begin("Logger");

			ImGui::Checkbox("Messages", &m_ShowMessageLogs);
			ImGui::SameLine();
			ImGui::Checkbox("Warnings", &m_ShowWarningLogs);
			ImGui::SameLine();
			ImGui::Checkbox("Errors", &m_ShowErrorLogs);
			ImGui::SameLine();
			if (ImGui::Button("Clear Logs")) m_Logs.clear();
			ImGui::Separator();

			ImGui::BeginChild("LOGGER_CHILD_WINDOW", { 0,0 }, ImGuiChildFlags_Border);
			for (int i = m_Logs.size() - 1; i >= 0; --i) {
				LogData& log = m_Logs[i];

				if (log.info.type == Core::LogType::Message && !m_ShowMessageLogs) continue;
				if (log.info.type == Core::LogType::Warning && !m_ShowWarningLogs) continue;
				if (log.info.type == Core::LogType::SoftError && !m_ShowErrorLogs) continue;

				ImU32 textColor = IM_COL32(255, 255, 255, 255);
				if (log.info.type == Core::LogType::Message) textColor = IM_COL32(0, 200, 0, 255);
				else if (log.info.type == Core::LogType::Warning) textColor = IM_COL32(255, 155, 0, 255);
				else if (log.info.type == Core::LogType::SoftError) textColor = IM_COL32(255, 0, 0, 255);
				ImGui::PushStyleColor(ImGuiCol_Text, textColor);
				ImGui::TextWrapped(log.message.c_str());
				ImGui::PopStyleColor();
			}
			ImGui::EndChild();

			ImGui::End();
		}
	}
}