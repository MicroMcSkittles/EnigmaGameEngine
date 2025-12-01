-- Start glm Project ================
project "glm"
    kind "StaticLib"
    staticruntime "off"
    systemversion "latest"

-- Configure C++ ====================
    language "C++"
    
-- Output Directories ===============
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

-- Include all c/c++ files in project
    files {
        "glm/**.hpp",
        "glm/**.h"
    }
    includedirs {
        "glm"
    }

    defines {
        "GLM_ENABLE_EXPERIMENTAL"
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