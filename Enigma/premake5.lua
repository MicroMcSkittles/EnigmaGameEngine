-- Start Enigma Project =============
project "Enigma"
    kind "StaticLib"
    staticruntime "On"
    systemversion "latest"

-- Configure C++
    language "C++"
    cppdialect "C++17"

-- Output Directories ===============
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

-- Setup precompiled headers
    pchheader "Enigma/EnigmaPCH.h"
    pchsource "Enigma/EnigmaPCH.cpp"

-- Include all c/c++ files in project
    files {
        "Enigma/Enigma.h",
        "Enigma/EnigmaPCH.h",
        "Enigma/EnigmaPCH.cpp",

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
        "./",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
    }

    defines {
        "GLM_ENABLE_EXPERIMENTAL",
    }

-- Link libraries ===================

    links {
        "Glad",
        "ImGui"
    }

-- Windows ==========================
    filter "system:windows"

        files {
            "Enigma/Platform/Windows/**.c",
            "Enigma/Platform/Windows/**.h",
            "Enigma/Platform/Windows/**.cpp",
            "Enigma/Platform/Windows/**.hpp"
        }

        defines {
            "PLATFORM_WINDOWS"
        }

        links {
            "Opengl32.lib",
            "Opengl32.dll"
        }

-- Linux ============================
    filter "system:linux"

        files {
            "Enigma/Platform/Linux/**.c",
            "Enigma/Platform/Linux/**.h",
            "Enigma/Platform/Linux/**.cpp",
            "Enigma/Platform/Linux/**.hpp"
        }

        includedirs {
            "%{IncludeDir.GLFW}"
        }
        links {
            "GLFW"
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