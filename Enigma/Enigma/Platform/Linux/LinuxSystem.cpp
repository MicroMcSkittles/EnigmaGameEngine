#include "Platform/Linux/LinuxSystem.h"

#include <GLFW/glfw3.h>
#include <ctime>
#include <iostream>

namespace Enigma {

    Core::System* Core::System::s_Instance = new Platform::LinuxSystem();

    namespace Platform {

        Core::Time LinuxSystem::GetTimeImpl() {
            time_t timestamp;
            time(&timestamp);
            tm timestruct;
            timestruct = *localtime(&timestamp);

            Core::Time rslt;
            rslt.miliseconds = glfwGetTime();
            rslt.seconds = timestruct.tm_sec;
            rslt.minutes = timestruct.tm_min;
            rslt.hours = timestruct.tm_hour;

            rslt.day = timestruct.tm_mday + 1;
            
            if(timestruct.tm_wday == 0) rslt.dayName = "Sunday";
            else if(timestruct.tm_wday == 1) rslt.dayName = "Monday";
            else if(timestruct.tm_wday == 2) rslt.dayName = "Tuesday";
            else if(timestruct.tm_wday == 3) rslt.dayName = "Wednesday";
            else if(timestruct.tm_wday == 4) rslt.dayName = "Thursday";
            else if(timestruct.tm_wday == 5) rslt.dayName = "friday";
            else if(timestruct.tm_wday == 6) rslt.dayName = "Saturday";

            rslt.month = timestruct.tm_mon + 1;
            rslt.year = timestruct.tm_year;

            return rslt;
        }
        float LinuxSystem::GetTimeMSImpl() {
            return glfwGetTime();
        }

        std::string LinuxSystem::GetOSNameImpl() {
            return "Linux";
        }

    }
}