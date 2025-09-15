#include "Enigma/Core/System.h"
#include "Enigma/Platform/Windows/WindowsWindow.h"
#include "Enigma/Core/Process/Application.h"
#include "Enigma/Core/Core.h"
#include "Enigma/Engine/InputCodes.h"

#include <chrono>
#include <Windows.h>
#include <commdlg.h>
//#include <GLFW/glfw3.h>
//#define GLFW_EXPOSE_NATIVE_WIN32
//#include <GLFW/glfw3native.h>

#include <ctime>
#include <sstream>

namespace Enigma {

    namespace Core {
        static std::chrono::time_point<std::chrono::steady_clock> s_TimeAtStartUp;

        void System::Init()
        {
            s_TimeAtStartUp = std::chrono::high_resolution_clock::now();
        }
        void System::Shutdown()
        {
            //glfwTerminate();
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
            //float ms = glfwGetTime();
            //if (ms == 0) LOG_ERROR("GLFW failed to get the time from start of application");
            //return glfwGetTime();
            std::chrono::time_point<std::chrono::steady_clock> endPoint = std::chrono::high_resolution_clock::now();

            long long start = std::chrono::time_point_cast<std::chrono::microseconds>(s_TimeAtStartUp).time_since_epoch().count();
            if (start == 0) return 0.0;

            long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endPoint).time_since_epoch().count();
            
            float ms = (end - start) * 1e-6;
            return ms;
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
            //ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)window->GetHandle());
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
            //std::string name = "Unknown Key";
            //switch (key)
            //{
            //case GLFW_KEY_LEFT_CONTROL: name = "Left Control"; break;
            //case GLFW_KEY_RIGHT_CONTROL: name = "Right Control"; break;
            //case GLFW_KEY_LEFT_SHIFT: name = "Left Shift"; break;
            //case GLFW_KEY_RIGHT_SHIFT: name = "Right Shift"; break;
            //case GLFW_KEY_LEFT_ALT: name = "Left Alt"; break;
            //case GLFW_KEY_RIGHT_ALT: name = "Right Alt"; break;
            //default:
                //int scancode = glfwGetKeyScancode(key);
                //if (scancode == -1) return name;
                //name = glfwGetKeyName(key, scancode);
            //}
            switch (key)
            {
            case Engine::KeyCode::KeyLeftShift:    return "Left Shift";
            case Engine::KeyCode::KeyRightShift:   return "Right Shift";
            case Engine::KeyCode::KeyLeftControl:  return "Left Control";
            case Engine::KeyCode::KeyRightControl: return "Right Control";
            case Engine::KeyCode::KeyEnter:        return "Enter";
            case Engine::KeyCode::KeyTab:          return "Tab";
            case Engine::KeyCode::KeyBackspace:    return "Backspace";

            case Engine::KeyCode::Key0: return "0";
            case Engine::KeyCode::Key1: return "1";
            case Engine::KeyCode::Key2: return "2";
            case Engine::KeyCode::Key3: return "3";
            case Engine::KeyCode::Key4: return "4";
            case Engine::KeyCode::Key5: return "5";
            case Engine::KeyCode::Key6: return "6";
            case Engine::KeyCode::Key7: return "7";
            case Engine::KeyCode::Key8: return "8";
            case Engine::KeyCode::Key9: return "9";

            case Engine::KeyCode::KeyA: return "A";
            case Engine::KeyCode::KeyB: return "B";
            case Engine::KeyCode::KeyC: return "C";
            case Engine::KeyCode::KeyD: return "D";
            case Engine::KeyCode::KeyE: return "E";
            case Engine::KeyCode::KeyF: return "F";
            case Engine::KeyCode::KeyG: return "G";
            case Engine::KeyCode::KeyH: return "H";
            case Engine::KeyCode::KeyI: return "I";
            case Engine::KeyCode::KeyJ: return "J";
            case Engine::KeyCode::KeyK: return "K";
            case Engine::KeyCode::KeyL: return "L";
            case Engine::KeyCode::KeyM: return "M";
            case Engine::KeyCode::KeyN: return "N";
            case Engine::KeyCode::KeyO: return "O";
            case Engine::KeyCode::KeyP: return "P";
            case Engine::KeyCode::KeyQ: return "Q";
            case Engine::KeyCode::KeyR: return "R";
            case Engine::KeyCode::KeyS: return "S";
            case Engine::KeyCode::KeyT: return "T";
            case Engine::KeyCode::KeyU: return "U";
            case Engine::KeyCode::KeyV: return "V";
            case Engine::KeyCode::KeyW: return "W";
            case Engine::KeyCode::KeyX: return "X";
            case Engine::KeyCode::KeyY: return "Y";
            case Engine::KeyCode::KeyZ: return "Z";
            }
            return "Unknown Key";
        }
        
    }
}