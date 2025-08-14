#include "Enigma/Core/System.h"
#include "Enigma/Platform/Windows/WindowsWindow.h"
#include "Enigma/Core/Process/Application.h"
#include "Enigma/Core/Core.h"
#include "Enigma/Engine/InputCodes.h"

#include <Windows.h>
#include <commdlg.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <ctime>
#include <sstream>

namespace Enigma {

    namespace Core {
        void System::Init()
        {
            if (!glfwInit()) {
                LOG_ERROR("Failed to initialize GLFW");
            }
        }
        void System::Shutdown()
        {
            glfwTerminate();
        }

        Core::Time System::GetTime() {
            time_t timestamp;
            time(&timestamp);
            tm timestruct;
            timestruct = *localtime(&timestamp);

            Core::Time rslt;
            rslt.miliseconds = GetTimeMS();
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
        float System::GetTimeMS() {
            float ms = glfwGetTime();
            //if (ms == 0) LOG_ERROR("GLFW failed to get the time from start of application");
            return glfwGetTime();
        }

        std::string System::GetOSName() {
            return "Windows";
        }

        std::string System::OpenFileDialog(const char* filter, Core::ID windowID)
        {
            Window* window = Application::GetWindow(windowID);

            OPENFILENAMEA ofn;
            CHAR szFile[260] = { 0 };
            ZeroMemory(&ofn, sizeof(OPENFILENAME));
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner = (HWND)window->GetNativeWindow();
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = filter;
            ofn.nFilterIndex = 1;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
            if (GetOpenFileNameA(&ofn) == TRUE) {
                return ofn.lpstrFile;
            }

            return std::string();
        }

        std::string System::SaveFileDialog(const char* filter, Core::ID windowID)
        {
            Platform::WindowsWindow* window = (Platform::WindowsWindow*)Application::GetWindow(windowID);

            OPENFILENAMEA ofn;
            CHAR szFile[260] = { 0 };
            ZeroMemory(&ofn, sizeof(OPENFILENAME));
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)window->GetHandle());
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = filter;
            ofn.nFilterIndex = 1;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
            if (GetSaveFileNameA(&ofn) == TRUE) {
                return ofn.lpstrFile;
            }

            return std::string();
        }

        std::string System::GetKeyName(int key)
        {
            std::string name = "Unknown Key";
            switch (key)
            {
            case GLFW_KEY_LEFT_CONTROL: name = "Left Control"; break;
            case GLFW_KEY_RIGHT_CONTROL: name = "Right Control"; break;
            case GLFW_KEY_LEFT_SHIFT: name = "Left Shift"; break;
            case GLFW_KEY_RIGHT_SHIFT: name = "Right Shift"; break;
            case GLFW_KEY_LEFT_ALT: name = "Left Alt"; break;
            case GLFW_KEY_RIGHT_ALT: name = "Right Alt"; break;
            default:
                int scancode = glfwGetKeyScancode(key);
                if (scancode == -1) return name;
                name = glfwGetKeyName(key, scancode);
            }
            return name;
        }
        
    }
}