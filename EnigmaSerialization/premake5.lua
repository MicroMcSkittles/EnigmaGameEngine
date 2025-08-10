project "EnigmaSerialization"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

-- Output Directories ===============
    rootdir = "../"
    targetdir (rootdir .. "bin/" .. outputdir .. "/%{prj.name}")
    objdir (rootdir .. "bin-int/" .. outputdir .. "/%{prj.name}")

-- Include all c/c++ files in project
    files
	{
        "EnigmaSerialization/**.hpp",
        "EnigmaSerialization/**.h",
        "EnigmaSerialization/**.cpp",
        "EnigmaSerialization/**.c",

        rootdir .. "Vendor/stb/stb/stb_image.h",
    }
    includedirs {
        "./",
        rootdir .. "Enigma",
        --rootdir .. "Enigma/Enigma",
        rootdir .. "Vendor/GLM/GLM",
        rootdir .. "Vendor/stb",
    }

    defines {
        "STB_IMAGE_IMPLEMENTATION",
        "GLM_ENABLE_EXPERIMENTAL"
    }

-- Link libraries ===================

    links {
        "Enigma"
    }

-- Windows ==========================
    filter "system:windows"
        --location "../"
		systemversion "latest"
		staticruntime "On"

        defines {
            "PLATFORM_WINDOWS"
        }

-- Linux ============================
    filter "system:linux"

        defines {
            "PLATFORM_LINUX"
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