#pragma once
#include <string>

namespace Enigma {
    namespace Core {

        struct Time {
            int hours;
            int minutes;
            int seconds;
            float miliseconds;

            int month;
            std::string monthName;
            int day;
            std::string dayName;
            int year;
        };

        class System {
        public:
            // Reads a file to a string
            static std::string ReadFileStr(const std::string& filename);
        
            static Time GetTime() { return s_Instance->GetTimeImpl(); }
            // returns the number of milliseconds sense the start of the app
            static float GetTimeMS() { return s_Instance->GetTimeMSImpl(); }

            // Returns the name of your os
            static std::string GetOSName() { return s_Instance->GetOSNameImpl(); }

        protected:
            virtual Time GetTimeImpl() = 0;
            virtual float GetTimeMSImpl() = 0;

            virtual std::string GetOSNameImpl() = 0;

        private:
            static System* s_Instance;
        };

    }
}