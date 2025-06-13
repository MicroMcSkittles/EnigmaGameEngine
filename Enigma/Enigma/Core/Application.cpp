#include "Core/Application.h"
#include "Core/Core.h"

namespace Enigma {
	namespace Core {
		Application::Application()
		{
			if (s_Instance) {
				LOG_SOFT_ERROR("Application Instance already exists");
				return;
			}
			s_Instance = this;

			// Setup logger if its enabled
#ifdef ENABLE_LOGGER
			LoggerConfig loggerConfig;
			loggerConfig.flags = File | Function | Priority;
			loggerConfig.priorityLevel = 5;
			Logger::Init(loggerConfig);
#endif

			LOG_MESSAGE("Application created successfully", 2);
		}
		Application::~Application()
		{
			LOG_MESSAGE("Application closed successfully", 2);
		}

		void Application::run()
		{
			LOG_MESSAGE("Running application", 2);
		}
	};
};