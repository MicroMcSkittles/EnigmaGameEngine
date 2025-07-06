#include "Core/System.h"
#include "Core/Core.h"

#include <fstream>
#include <sstream>
#include <iomanip>

namespace Enigma {
    namespace Core {
        std::string Time::ToString(bool includeSeconds)
        {
            std::stringstream ss;
            ss << (hours - ((hours > 12) ? 12 : 0) + ((hours == 0) ? 12 : 0));
            ss << " : ";
            ss << std::setw(2) << std::setfill('0') << std::right << minutes;
            
            if(includeSeconds) {
                ss << " : ";
                ss << std::setw(2) << std::setfill('0') << std::right << seconds;
            }

            ss << " ";
            ss << ((hours > 12) ? "PM" : "AM");
            return ss.str();
        }

        std::string System::ReadFileStr(const std::string& filename) {
            std::ifstream file;
            file.open(filename);
            if(!file.is_open()) {
                LOG_WARNING("Failed to open text file ( %s )", filename.c_str());
                return "";
            }

            std::stringstream ss;
            ss << file.rdbuf();
            return ss.str();
        }
    }
}
