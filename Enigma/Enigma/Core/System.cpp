#include "Enigma/Core/System.h"
#include "Enigma/Core/Core.h"
#include "Enigma/Engine/InputCodes.h"

#include <fstream>
#include <sstream>
#include <iomanip>

namespace Enigma {
    namespace Core {
        std::string Time::ToString(bool includeSeconds)
        {
            std::stringstream ss;

            // Push hours and minutes
            ss << (hours - ((hours > 12) ? 12 : 0) + ((hours == 0) ? 12 : 0));
            ss << " : ";
            ss << std::setw(2) << std::setfill('0') << std::right << minutes;
            
            // Push seconds
            if(includeSeconds) {
                ss << " : ";
                ss << std::setw(2) << std::setfill('0') << std::right << seconds;
            }

            // Push AM/PM
            ss << " ";
            ss << ((hours > 12) ? "PM" : "AM");
            return ss.str();
        }

        std::string System::ReadFileStr(const std::string& filename) {
            // Open file
            std::ifstream file;
            file.open(filename);
            if(!file.is_open()) {
                LOG_WARNING("Failed to open text file ( %s )", filename.c_str());
                return "";
            }

            // Copy content to a string then return
            std::stringstream ss;
            ss << file.rdbuf();
            return ss.str();
        }

        std::string System::GetButtonName(i32 button)
        {
            std::string name = std::to_string(button);
            if (button == Engine::KeyCode::MouseButtonLeft) name = "Left";
            else if (button == Engine::KeyCode::MouseButtonMiddle) name = "Middle";
            else if (button == Engine::KeyCode::MouseButtonRight) name = "right";
            return name;
        }
        std::string System::GetActionName(i32 action)
        {
            std::string name = std::to_string(action);
            if (action == Engine::KeyCode::KeyPress) name = "Press";
            else if (action == Engine::KeyCode::KeyRepeat) name = "Repeat";
            else if (action == Engine::KeyCode::KeyRelease) name = "Release";
            return name;
        }
        std::string System::GetModsName(i32 mods)
        {
            std::stringstream ss;
            if (mods & Engine::KeyCode::ShiftMask) ss << "( Shift ) ";
            if (mods & Engine::KeyCode::CtrlMask) ss << "( Ctrl ) ";
            if (mods & Engine::KeyCode::AltMask) ss << "( Alt )";
            return ss.str();
        }

    }
}
