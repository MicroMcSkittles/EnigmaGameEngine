#include "Platform/Linux/LinuxSystem.h"
#include "Core/Core.h"

#include <GLFW/glfw3.h>
#include <ctime>

namespace Enigma {

    Core::System* Core::System::s_Instance = new Platform::LinuxSystem();

    namespace Platform {

        void LinuxSystem::InitImpl()
        {
            if (!glfwInit()) {
                LOG_ERROR("Failed to initialize GLFW");
            }
        }
        void LinuxSystem::ShutdownImpl()
        {
            glfwTerminate();
        }

        Core::Time LinuxSystem::GetTimeImpl() {
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
            
            if(timestruct.tm_wday == 0)      rslt.dayName = "Sunday";
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
            float ms = glfwGetTime();
            // if (ms == 0) LOG_ERROR("GLFW failed to get the time from start of application");
            return glfwGetTime();
        }

        std::string LinuxSystem::GetOSNameImpl() {
            return "Linux";
        }

        std::string LinuxSystem::GetKeyNameImpl(int key)
        {
            std::string name = glfwGetKeyName(key, glfwGetKeyScancode(key));
            return name;
        }
        std::string LinuxSystem::GetButtonNameImpl(int button)
        {
            std::string name = std::to_string(button);
            if (button == GLFW_MOUSE_BUTTON_LEFT) name = "Left";
            else if (button == GLFW_MOUSE_BUTTON_MIDDLE) name = "Middle";
            else if (button == GLFW_MOUSE_BUTTON_RIGHT) name = "right";
            return name;
        }
        std::string LinuxSystem::GetActionNameImpl(int action)
        {
            std::string name = std::to_string(action);
            if (action == GLFW_PRESS) name = "Press";
            else if (action == GLFW_REPEAT) name = "Repeat";
            else if (action == GLFW_RELEASE) name = "Release";
            return name;
        }
        std::string LinuxSystem::GetModsNameImpl(int action)
        {
            return std::string();
        }

    }
}