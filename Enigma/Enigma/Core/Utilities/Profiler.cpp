#include "Enigma/Core/Utilities/Profiler.h"
#include "Enigma/Core/Utilities/Utilities.h"
#include "Enigma/Core/Core.h"

#include <cstring>
#include <sstream>
#include <filesystem>
#include <imgui.h>

namespace Enigma {
	namespace Core {

		ProfilingTimer::ProfilingTimer(const char* function, const char* file, const char* description)
		{
			m_Function    = function;
			m_File        = file;
			m_Description = description;
			m_StartPoint  = std::chrono::high_resolution_clock::now();
		}
		ProfilingTimer::~ProfilingTimer()
		{
			std::chrono::time_point<std::chrono::steady_clock> endPoint = std::chrono::high_resolution_clock::now();

			long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartPoint).time_since_epoch().count();
			long long end   = std::chrono::time_point_cast<std::chrono::microseconds>(endPoint).time_since_epoch().count();

			float duration = (end - start) * 0.001f;
			Profiler::Submit(m_Function, m_File, m_Description, duration);
		}

		void Profiler::Init(uint8_t profileDepth)
		{
			s_Data = new Data;
			s_Data->profileDepth = profileDepth;
			LOG_WARNING("Using profiler, This is a debug tool and could harm performance");
		}

		void Profiler::ImGui()
		{
			ImGui::Begin("Enigma Profiler");

			int inputProfileDepth = s_Data->profileDepth;
			if (ImGui::InputInt("Profile Depth", &inputProfileDepth, 1, 5)) {
				s_Data->profileDepth = inputProfileDepth % ((uint8_t)-1);
				if (s_Data->profileDepth == 0) s_Data->profileDepth = 1;
				UpdateProfileDepth();
			}
			if (ImGui::Button("Clear")) {
				s_Data->files.clear();
				for (auto& [profileHash, profile] : s_Data->profiles) {
					// Make sure memory was allocated properly
					if (!profile.durations) {
						LOG_ERROR("Failed to allocate memory for profile duration ( %s, %s )", profile.function, profile.file);
						return;
					}
					else free(profile.durations);
				}
				s_Data->profiles.clear();
			}
			
			// Loop through each file
			for (auto& [file, profiles] : s_Data->files) {
				ImGui::PushID(file.c_str());
				std::string filename = std::filesystem::path(file).filename().string();

				// Start file tree node
				if (ImGui::TreeNode(filename.c_str())) {
					FileNodeImGui(profiles);
					ImGui::TreePop();
				}

				ImGui::PopID();
			}

			ImGui::End();
		}
		void Profiler::FileNodeImGui(const std::vector<uint64_t>& profiles)
		{
			// Loop through each profile in file
			for (const uint64_t& profileHash : profiles) {
				ImGui::PushID(profileHash);
				Profile& profile = s_Data->profiles[profileHash];

				if (ImGui::TreeNode((profile.description == "") ? profile.function : profile.description)) {
					if (profile.description == "") ImGui::Text(profile.function);
					// Display duration info
					float lastMS = profile.durations[s_Data->profileDepth - 1];
					ImGui::Text("Last MS: %.2f", lastMS);
					ImGui::PlotLines("Durations", profile.durations, s_Data->profileDepth);
					ImGui::TreePop();
				}

				ImGui::PopID();
			}
		}

		void Profiler::UpdateProfileDepth()
		{
			s_Data->files.clear();
			std::map<uint64_t, Profile> profiles = s_Data->profiles;
			for (auto& [profileHash, profile] : profiles) {
				// Make sure memory was allocated properly
				if (!profile.durations) {
					LOG_ERROR("Failed to allocate memory for profile duration ( %s, %s )", profile.function, profile.file);
					return;
				} else free(profile.durations);
				s_Data->profiles.erase(profileHash);
				CreateProfileEntry(profile.function, profile.file, profile.description, 0.0);
			}
		}
		
		void Profiler::Submit(const char* function, const char* file, const char* description, float duration)
		{
			uint64_t profileHash = Hash(function, file);

			// If this is a new profile, then create a new entry in the profiles table
			if (!s_Data->profiles.count(profileHash)) {
				CreateProfileEntry(function, file, description, duration);
				return;
			}

			Profile& profile = s_Data->profiles[profileHash];

			// Make sure memory was allocated properly
			if (!profile.durations) {
				LOG_ERROR("Failed to allocate memory for profile duration ( %s, %s )", function, file);
				return;
			}

			// Shift new duration into the profiles durations
			memmove(profile.durations, profile.durations + 1, sizeof(float) * (s_Data->profileDepth - 1));
			profile.durations[s_Data->profileDepth - 1] = duration;
		}
		void Profiler::CreateProfileEntry(const char* function, const char* file, const char* description, float duration)
		{
			// Create profile
			Profile profile;
			profile.file        = file;
			profile.function    = function;
			profile.description = description;

			// Allocate memory for profile durations
			profile.durations = (float*)malloc(sizeof(float) * s_Data->profileDepth);

			// Make sure memory was allocated properly
			if (!profile.durations) {
				LOG_ERROR("Failed to allocate memory for profile duration ( %s, %s )", function, file);
				return;
			}
			memset(profile.durations, 0, sizeof(float) * s_Data->profileDepth);

			// set the last element to the new duration
			profile.durations[s_Data->profileDepth - 1] = duration;

			// Store profile in table
			uint64_t profileHash = Hash(function, file);
			s_Data->profiles.insert({ profileHash, profile });

			// Store profile hash in the file table
			if (!s_Data->files.count(file)) {
				s_Data->files.insert({ std::string(file), { profileHash } });
				return;
			}
			s_Data->files[file].push_back(profileHash);
		}
	}
}