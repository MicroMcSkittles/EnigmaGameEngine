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
        
            // returns the time as a string in the format
            // HH : MM : SS AM/PM, ex. 9 : 59 : 29 AM
            std::string ToString(bool includeSeconds = false);
        };

        class System {
        public:
            static void Init() { s_Instance->InitImpl(); }
            static void Shutdown() { s_Instance->ShutdownImpl(); }

            // Reads a file to a string
            static std::string ReadFileStr(const std::string& filename);

            // Gets the current time
            static Time GetTime() { return s_Instance->GetTimeImpl(); }
            // returns the number of milliseconds sense the start of the app
            static float GetTimeMS() { return s_Instance->GetTimeMSImpl(); }

            // Returns the name of your os
            static std::string GetOSName() { return s_Instance->GetOSNameImpl(); }

            // Returns the names of the codes in InputCodes.h
            
            // Returns the name of a key code
            static std::string GetKeyName(int key) { return s_Instance->GetKeyNameImpl(key); }
            // Returns the name of a mouse button code
            static std::string GetButtonName(int button) { return s_Instance->GetButtonNameImpl(button); }
            // Returns the name of a key action code
            static std::string GetActionName(int action) { return s_Instance->GetActionNameImpl(action); }
            // Returns the name of key/button mods
            static std::string GetModsName(int mods) { return s_Instance->GetModsNameImpl(mods); }

            // Implementations
        protected:
            virtual void InitImpl() = 0;
            virtual void ShutdownImpl() = 0;

            virtual Time GetTimeImpl() = 0;
            virtual float GetTimeMSImpl() = 0;

            virtual std::string GetOSNameImpl() = 0;

            virtual std::string GetKeyNameImpl(int key) = 0;
            virtual std::string GetButtonNameImpl(int button) = 0;
            virtual std::string GetActionNameImpl(int action) = 0;
            virtual std::string GetModsNameImpl(int action) = 0;

        private:
            static System* s_Instance;
        };

    }
}