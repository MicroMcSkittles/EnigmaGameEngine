#pragma once
#define ENABLE_LOGGER
#include "Core/Core.h"

#include <vector>

namespace Enigma {
	namespace Editor {

		struct LogData {
			std::string message;
			Core::LogInfo info;
		};

		class LoggerPanel {
		public:
			LoggerPanel() :
				m_LogCap(100),
				m_ShowMessageLogs(true),
				m_ShowWarningLogs(true),
				m_ShowErrorLogs(true) { }

			void Log(const std::string& msg, const Core::LogInfo& info);

			void ImGui();

		private:
			int m_LogCap;
			bool m_ShowMessageLogs;
			bool m_ShowWarningLogs;
			bool m_ShowErrorLogs;
			std::vector<LogData> m_Logs;
		};

	}
}