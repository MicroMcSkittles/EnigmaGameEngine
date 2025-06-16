#pragma once
#include <string>

namespace Enigma {
    namespace Core {

        struct Time {
            int hours; // Hours in range 1 to 24
            int minutes; // Minutes in range 0 to 59
            int seconds; // Seconds in range 0 to 59

            float miliseconds; // The time from the start of the app in miliseconds

            int month; // The month in range 1 to 12
            std::string monthName; // The month name
            int day; // The day in range 
            std::string dayName; // The name of the day
            int year; // The year represented as the number of years sence 1900
        };

        class System {
        public:
            // Reads a file to a string
            static std::string ReadFileStr(const std::string& filename);

            // Gets the current time
            static Time GetTime() { return s_Instance->GetTimeImpl(); }
            // returns the number of milliseconds sense the start of the app
            static float GetTimeMS() { return s_Instance->GetTimeMSImpl(); }

            // Returns the name of your os
            static std::string GetOSName() { return s_Instance->GetOSNameImpl(); }

            // Implementations
        protected:
            virtual Time GetTimeImpl() = 0;
            virtual float GetTimeMSImpl() = 0;

            virtual std::string GetOSNameImpl() = 0;

        private:
            static System* s_Instance;
        };

    }
}