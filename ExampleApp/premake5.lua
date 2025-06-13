project "ExampleApp"
    kind "ConsoleApp"
    language "C++"

-- Output Directories ===============
    rootdir = "../"
    targetdir (rootdir .. "bin/" .. outputdir .. "/%{prj.name}")
    objdir (rootdir .. "bin-int/" .. outputdir .. "/%{prj.name}")

    -- Include all c/c++ files in project
    files {
        "src/**.c",
        "src/**.h",
        "src/**.cpp",
        "src/**.hpp"
    }

    includedirs {
        "src",
        rootdir .. "Enigma",
        rootdir .. "Vendor/GLM/GLM",
    }

-- Link libraries ===================
    links {
        "Enigma"
    }

-- Windows ==========================
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines {
            "PLATFORM_WINDOWS"
        }

-- Configuations ====================
    filter "configurations:Debug"
        defines "DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "RELEASE"
        optimize "On"