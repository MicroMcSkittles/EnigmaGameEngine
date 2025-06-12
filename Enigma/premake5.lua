project "Enigma"
    kind "StaticLib"
    language "C++"

-- Output Directories ===============
    rootdir = "../"
    targetdir (rootdir .. "bin/" .. outputdir .. "/%{prj.name}")
    objdir (rootdir .. "bin-int/" .. outputdir .. "/%{prj.name}")

-- Include all c/c++ files in project
    files
	{
        "src/**.c",
        "src/**.h",
        "src/**.cpp",
        "src/**.hpp"
    }
    includedirs {
        "src"
    }


-- Windows ==========================
    filter "system:windows"
        cppdialect "C++17"
		systemversion "latest"
		staticruntime "On"

        defines {
            "PLATFORM_WINDOWS"
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