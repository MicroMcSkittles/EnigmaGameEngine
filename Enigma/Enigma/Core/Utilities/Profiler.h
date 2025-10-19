#pragma once
#include <chrono>
#include <string>
#include <vector>
#include <map>

namespace Enigma {
	namespace Core {

		struct Profile {
			const char* function;    // The name of the function being profiled
			const char* file;        // The name of the file the function is in
			const char* description; // An optional value
			float*      durations;   // Each recorded duration, its length is specifed by the profileDepth variable
			bool        SOF;         // Start of frame
		};

		// When it gets created it records the current time, 
		// when it gets destroyed it finds the duration from start to finish and sends it to the profiling
		class ProfilingTimer {
		public:
			ProfilingTimer(const char* function, const char* file, bool additive, const char* description = "");
			~ProfilingTimer();
		
		private:
			const char* m_Function;
			const char* m_File;
			const char* m_Description;
			bool m_Additive;
			std::chrono::time_point<std::chrono::steady_clock> m_StartPoint;
		};

		class Profiler {
		public:
			static void Init(uint8_t profileDepth);
			// Shows a imgui window that displays all profiles
			static void ImGui(); 

			// If additive is true, then each duration submited will be added together untill the end of the frame
			static void Submit(const char* function, const char* file, bool additive, const char* description, float duration);

			static void EndFrame();

		private:
			static void CreateProfileEntry(const char* function, const char* file, const char* description, float duration);

			static void FileNodeImGui(const std::vector<uint64_t>& profiles);

			static void UpdateProfileDepth();

		private:
			struct Data {
				std::map<uint64_t, Profile> profiles;
				std::map<std::string, std::vector<uint64_t>> files; // Maps each file to the its functions
				uint8_t profileDepth = 25;
			};
			inline static Data* s_Data;
		};

	}
}
