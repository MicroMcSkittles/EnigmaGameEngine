-- Start ImGuizmo Project ===========
project "ImGuizmo"
    kind "StaticLib"
	staticruntime "On"
	systemversion "latest"

-- Configure C++ ====================
    language "C++"
	cppdialect "C++17"

-- Output Directories ===============
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")

-- Include all c/c++ files in project
    files {
		"ImGuizmo/*.h",
		"ImGuizmo/*.cpp",
	}

    includedirs {
        "ImGuizmo",
        "%{IncludeDir.ImGui}"
    }
    links {
        "ImGui"
    }

-- Windows ==========================
    filter "system:windows"

		defines { 
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}

-- Linux ============================
	filter "system:linux"
		pic "On"

		defines {
			"_GLFW_X11",
			"_CRT_SECURE_NO_WARNINGS"
		}

-- Configurations ===================
    filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter { "system:windows", "configurations:Debug-AS" }	
		runtime "Debug"
		symbols "on"
		sanitize { "Address" }
		flags { "NoRuntimeChecks", "NoIncrementalLink" }

	filter "configurations:Release"
		runtime "Release"
		optimize "speed"