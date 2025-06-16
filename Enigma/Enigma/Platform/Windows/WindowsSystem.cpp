#include "Platform/Windows/WindowsSystem.h"
#include "Core/Process/Application.h"
#include "Core/Core.h"

#include <GLFW/glfw3.h>
#include <ctime>

namespace Enigma {

    Core::System* Core::System::s_Instance = new Platform::WindowsSystem();

    namespace Platform {

        Core::Time WindowsSystem::GetTimeImpl() {
            time_t timestamp;
            time(&timestamp);
            tm timestruct;
            timestruct = *localtime(&timestamp);

            Core::Time rslt;
            rslt.miliseconds = GetTimeMSImpl();
            rslt.seconds = timestruct.tm_sec;
            rslt.minutes = timestruct.tm_min;
            rslt.hours = timestruct.tm_hour;

            rslt.day = timestruct.tm_mday + 1;

            if (timestruct.tm_wday == 0)      rslt.dayName = "Sunday";
            else if (timestruct.tm_wday == 1) rslt.dayName = "Monday";
            else if (timestruct.tm_wday == 2) rslt.dayName = "Tuesday";
            else if (timestruct.tm_wday == 3) rslt.dayName = "Wednesday";
            else if (timestruct.tm_wday == 4) rslt.dayName = "Thursday";
            else if (timestruct.tm_wday == 5) rslt.dayName = "friday";
            else if (timestruct.tm_wday == 6) rslt.dayName = "Saturday";

            rslt.month = timestruct.tm_mon + 1;
            rslt.year = timestruct.tm_year;

            return rslt;
        }
        float WindowsSystem::GetTimeMSImpl() {
            float ms = glfwGetTime();
            if (ms == 0) LOG_ERROR("GLFW failed to get the time from start of application");
            return glfwGetTime();
        }

        std::string WindowsSystem::GetOSNameImpl() {
            return "Windows";
        }
    }
}