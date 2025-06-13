project "GLM"
    kind "StaticLib"
    language "C++"

-- Output Directories ===============
    rootdir = "../../"
    targetdir (rootdir .. "bin/" .. outputdir .. "/%{prj.name}")
    objdir (rootdir .. "bin-int/" .. outputdir .. "/%{prj.name}")

-- Include all c/c++ files in project
    files
	{
        "glm/**.hpp",
        "glm/**.h"
    }
    includedirs {
        "glm"
    }

    defines  {
        "GLM_ENABLE_EXPERIMENTAL"
    }

-- Windows ==========================
    filter "system:windows"
		systemversion "latest"
		staticruntime "On"

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