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
        "Enigma/**.c",
        "Enigma/**.h",
        "Enigma/**.cpp",
        "Enigma/**.hpp"
    }
    includedirs {
        "Enigma"
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
        defines "DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
        defines "RELEASE"
		runtime "Release"
		optimize "speed"