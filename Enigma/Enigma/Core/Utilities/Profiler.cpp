#include "Enigma/EnigmaPCH.h"
#include "Enigma/Core/Utilities/Profiler.h"
#include "Enigma/Core/Utilities/Utilities.h"
#include "Enigma/Core/Core.h"

#include <imgui.h>

// TODO: fix this time point stuff

namespace Enigma::Core {

	ProfilingTimer::ProfilingTimer(const std::string& function, const std::string& file, bool additive, const std::string& description)
	{
		m_Function    = function;
		m_File        = file;
		m_Description = description;
		m_Additive    = additive;
		//m_StartPoint  = std::chrono::high_resolution_clock::now();
	}
	ProfilingTimer::~ProfilingTimer()
	{
		//auto endPoint = std::chrono::high_resolution_clock::now();

		//long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartPoint).time_since_epoch().count();
		//long long end   = std::chrono::time_point_cast<std::chrono::microseconds>(endPoint).time_since_epoch().count();

		//float duration = (end - start) * 0.001f;
		Profiler::Submit(m_Function, m_File, m_Additive, m_Description, 0.0f);
	}

	void Profiler::Init(u8 profileDepth)
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
			s_Data->profileDepth = inputProfileDepth % std::numeric_limits<uint8_t>::max();
			if (s_Data->profileDepth == 0) s_Data->profileDepth = 1;
			UpdateProfileDepth();
		}
		if (ImGui::Button("Clear")) {
			s_Data->files.clear();
			for (auto& [profileHash, profile] : s_Data->profiles) {
				// Make sure memory was allocated properly
				if (profile.durations == nullptr) {
					LOG_ERROR("Failed to allocate memory for profile duration ( %s, %s )", profile.function, profile.file);
					return;
				}
				else {
					free(profile.durations);
				}
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
	void Profiler::FileNodeImGui(const std::vector<u64>& profiles)
	{
		// Loop through each profile in file
		for (const u64& profileHash : profiles) {
			ImGui::PushID(profileHash);
			Profile& profile = s_Data->profiles[profileHash];

			if (ImGui::TreeNode((profile.description.empty() ? profile.function : profile.description).c_str())) {
				if (!profile.description.empty()) ImGui::Text("%s", profile.function);
				// Display duration info
				f32 lastMS = profile.durations[s_Data->profileDepth - 1];
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
		std::map<u64, Profile> profiles = s_Data->profiles;
		for (auto& [profileHash, profile] : profiles) {
			// Make sure memory was allocated properly
			if (profile.durations == nullptr) {
				LOG_ERROR("Failed to allocate memory for profile duration ( %s, %s )", profile.function, profile.file);
				return;
			}
			else {
				free(profile.durations);
			}
			s_Data->profiles.erase(profileHash);
			CreateProfileEntry(profile.function, profile.file, profile.description, 0.0);
		}
	}
	
	void Profiler::Submit(const std::string& function, const std::string& file, bool additive, const std::string& description, f32 duration)
	{
		u64 profileHash = Hash((description.empty() ? function : description).c_str(), file.c_str());

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
		if (!additive || (additive && profile.SOF)) {
			memmove(profile.durations, profile.durations + 1, sizeof(f32) * (s_Data->profileDepth - 1));
			profile.durations[s_Data->profileDepth - 1] = duration;
			profile.SOF = false;
		}
		else if (additive && !profile.SOF) {
			profile.durations[s_Data->profileDepth - 1] += duration;
		}
	}
	void Profiler::EndFrame()
	{
		for (auto& [profileHash, profile] : s_Data->profiles) {
			profile.SOF = true;
		}
	}

	void Profiler::CreateProfileEntry(const std::string& function, const std::string& file, const std::string& description, f32 duration)
	{
		// Create profile
		Profile profile;
		profile.file        = file;
		profile.function    = function;
		profile.description = description;
		profile.SOF         = true;

		// Allocate memory for profile durations
		profile.durations = (f32*)malloc(sizeof(f32) * s_Data->profileDepth);

		// Make sure memory was allocated properly
		if (!profile.durations) {
			LOG_ERROR("Failed to allocate memory for profile duration ( %s, %s )", function, file);
			return;
		}
		memset(profile.durations, 0, sizeof(f32) * s_Data->profileDepth);

		// set the last element to the new duration
		profile.durations[s_Data->profileDepth - 1] = duration;

		// Store profile in table
		u64 profileHash = Hash(((description.size() == 0) ? function : description).c_str(), file.c_str());
		s_Data->profiles.insert({ profileHash, profile });

		// Store profile hash in the file table
		if (!s_Data->files.count(file)) {
			s_Data->files.insert({ std::string(file), { profileHash } });
			return;
		}
		s_Data->files[file].push_back(profileHash);
	}
	
}
