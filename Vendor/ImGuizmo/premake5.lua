project "ImGuizmo"
    kind "StaticLib"
    language "C++"

-- Output Directories ===============
    rootdir = "../../"
    objdir (rootdir .. "bin-int/" .. outputdir .. "/%{prj.name}")
    targetdir (rootdir .. "bin/" .. outputdir .. "/%{prj.name}")

-- Include all c/c++ files in project
    files
	{
		"ImGuizmo/*.h",
		"ImGuizmo/*.cpp",
	}

    includedirs {
        "ImGuizmo",
        rootdir .. "Vendor/ImGui/ImGui",
    }
    links {
        "ImGui"
    }

-- Windows ==========================
    filter "system:windows"
		systemversion "latest"
		staticruntime "On"

    -- Windows specific defines =====
		defines 
		{ 
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}

-- Linux ============================
	filter "system:linux"
		pic "On"

		systemversion "latest"
		staticruntime "On"

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