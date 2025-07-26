project "Enigma"
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
        "Enigma/Enigma.h",
        "Enigma/Core/**.c",
        "Enigma/Core/**.h",
        "Enigma/Core/**.cpp",
        "Enigma/Core/**.hpp",

        "Enigma/ImGui/**.c",
        "Enigma/ImGui/**.h",
        "Enigma/ImGui/**.cpp",
        "Enigma/ImGui/**.hpp",

        "Enigma/Engine/**.c",
        "Enigma/Engine/**.h",
        "Enigma/Engine/**.cpp",
        "Enigma/Engine/**.hpp",

        "Enigma/Renderer/**.c",
        "Enigma/Renderer/**.h",
        "Enigma/Renderer/**.cpp",
        "Enigma/Renderer/**.hpp",
        
        "Enigma/Platform/OpenGL/**.c",
        "Enigma/Platform/OpenGL/**.h",
        "Enigma/Platform/OpenGL/**.cpp",
        "Enigma/Platform/OpenGL/**.hpp",
    }
    includedirs {
        "Enigma",
        rootdir .. "Vendor/GLFW/GLFW/include",
        rootdir .. "Vendor/GLAD/GLAD/include",
        rootdir .. "Vendor/GLM/GLM",
        rootdir .. "Vendor/ImGui/ImGui",
    }

    defines {
        "GLM_ENABLE_EXPERIMENTAL"
    }

-- Link libraries ===================

    links {
        "GLAD",
        "GLFW",
        "ImGui"
    }

-- Windows ==========================
    filter "system:windows"
        --location "../"
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

-- Linux ============================
    filter "system:linux"

        files
        {
            "Enigma/Platform/Linux/**.c",
            "Enigma/Platform/Linux/**.h",
            "Enigma/Platform/Linux/**.cpp",
            "Enigma/Platform/Linux/**.hpp"
        }

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