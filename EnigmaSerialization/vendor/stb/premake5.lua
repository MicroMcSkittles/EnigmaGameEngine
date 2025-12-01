-- Start stb Project ================
project "stb"
    kind "StaticLib"
    staticruntime "off"
    systemversion "latest"

-- Configue C++ =====================
    language "C++"

-- Output Directories ===============
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

-- Include all c/c++ files in project
    files {
        "stb/stb_image.h"
    }
    includedirs {
        "stb"
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