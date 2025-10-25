#pragma once
#include <string>
#include "Enigma/Core/Types.h"
#include "Enigma/Core/IdHandler.h"

namespace Enigma {
    namespace Core {

        struct Time {
            i32 hours; // Hours in range 1 to 24
            i32 minutes; // Minutes in range 0 to 59
            i32 seconds; // Seconds in range 0 to 59

            f32 miliseconds; // The time from the start of the app in miliseconds

            i32 month; // The month in range 1 to 12
            std::string monthName; // The month name
            i32 day; // The day in range 
            std::string dayName; // The name of the day
            i32 year; // The year represented as the number of years sence 1900
        
            // returns the time as a string in the format
            // HH : MM : SS AM/PM, ex. 9 : 59 : 29 AM
            std::string ToString(bool includeSeconds = false);
        };

        class System {
        public:
            static void Init();
            static void Shutdown();

            // Reads a file to a string
            static std::string ReadFileStr(const std::string& filename);

            // Gets the current time
            static Time GetTime();
            // returns the number of milliseconds sense the start of the app
            static f32 GetTimeMS();

            // Returns the name of your os
            static std::string GetOSName();

            // Opens a open file dialog, returns a empty string if cancled
            static std::string OpenFileDialog(const char* filter, Core::ID windowID);
            // Opens a save file dialog, returns a empty string if cancled
            static std::string SaveFileDialog(const char* filter, Core::ID windowID);

            // Returns the names of the codes in InputCodes.h
            
            // Returns the name of a key code
            static std::string GetKeyName(i32 key);
            // Returns the name of a mouse button code
            static std::string GetButtonName(i32 button);
            // Returns the name of a key action code
            static std::string GetActionName(i32 action);
            // Returns the name of key/button mods
            static std::string GetModsName(i32 mods);
        };

    }
}