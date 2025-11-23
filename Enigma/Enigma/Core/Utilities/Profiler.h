#pragma once
#include <chrono>
#include <map>

namespace Enigma::Core {

	struct Profile {
		std::string function;            // The name of the function being profiled
		std::string file;                // The name of the file the function is in
		std::string description;         // An optional value
		f32*        durations = nullptr; // Each recorded duration, its length is specifed by the profileDepth variable
		bool        SOF       = false;   // Start of frame
	};

	// When it gets created it records the current time, 
	// when it gets destroyed it finds the duration from start to finish and sends it to the profiling
	class ProfilingTimer {
	public:
		ProfilingTimer(const std::string& function, const std::string& file, bool additive, const std::string& description = "");
		~ProfilingTimer();
	
	private:
		std::string m_Function;
		std::string m_File;
		std::string m_Description;
		bool m_Additive;
		std::chrono::time_point<std::chrono::steady_clock> m_StartPoint;
	};

	class Profiler {
	public:
		static void Init(u8 profileDepth);
		// Shows a imgui window that displays all profiles
		static void ImGui(); 

		// If additive is true, then each duration submited will be added together untill the end of the frame
		static void Submit(const std::string& function, const std::string& file, bool additive, const std::string& description, f32 duration);

		static void EndFrame();

	private:
		static void CreateProfileEntry(const std::string& function, const std::string& file, const std::string& description, f32 duration);

		static void FileNodeImGui(const std::vector<uint64_t>& profiles);

		static void UpdateProfileDepth();

	private:
		struct Data {
			std::map<u64, Profile> profiles;
			std::map<std::string, std::vector<u64>> files; // Maps each file to the its functions
			u8 profileDepth = 25;
		};
		inline static Data* s_Data;
	};

}
