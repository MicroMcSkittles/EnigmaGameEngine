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
        "Enigma/Enigma.h",
        "Enigma/Core/**.c",
        "Enigma/Core/**.h",
        "Enigma/Core/**.cpp",
        "Enigma/Core/**.hpp",

        "Enigma/Renderer/**.c",
        "Enigma/Renderer/**.h",
        "Enigma/Renderer/**.cpp",
        "Enigma/Renderer/**.hpp"
    }
    includedirs {
        "Enigma",
        rootdir .. "Vendor/GLFW/GLFW/include",
    }

-- Link libraries ===================

    links {
        "GLFW"
    }

-- Windows ==========================
    filter "system:windows"
        cppdialect "C++17"
		systemversion "latest"
		staticruntime "On"

        files
        {
            "Enigma/Platform/Windows/**.c",
            "Enigma/Platform/Windows/**.h",
            "Enigma/Platform/Windows/**.cpp",
            "Enigma/Platform/Windows/**.hpp"
        }

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