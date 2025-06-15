#include "Core/System.h"
#include "Core/Core.h"

#include <fstream>
#include <sstream>

namespace Enigma {
    namespace Core {
        std::string System::ReadFileStr(const std::string& filename) {
            std::ifstream file;
            file.open(filename);
            if(!file.is_open()) {
                LOG_WARNING("Failed to open text file ( " + filename + " )");
                return "";
            }

            std::stringstream ss;
            ss << file.rdbuf();
            return ss.str();
        }
    }
}
