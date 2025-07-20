project "SpaceSim"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

-- Output Directories ===============
    rootdir = "../"
    targetdir (rootdir .. "bin/" .. outputdir .. "/%{prj.name}")
    objdir (rootdir .. "bin-int/" .. outputdir .. "/%{prj.name}")

    -- Include all c/c++ files in project
    files {
        "src/**.c",
        "src/**.h",
        "src/**.cpp",
        "src/**.hpp",
        rootdir .. "Vendor/stb/stb/stb_image.h",
    }

    includedirs {
        "src",
        rootdir .. "Enigma",
        rootdir .. "Enigma/Enigma",
        rootdir .. "Vendor/GLM/GLM",
        rootdir .. "Vendor/ImGui/ImGui",
        rootdir .. "Vendor/stb",
    }

    defines {
        "STB_IMAGE_IMPLEMENTATION",
        "GLM_ENABLE_EXPERIMENTAL"
    }

-- Link libraries ===================
    links {
        "Enigma",
        "ImGui"
    }

-- Windows ==========================
    filter "system:windows"
        -- Use debugdir!
        --location "../"
        staticruntime "On"
        systemversion "latest"

        defines {
            "PLATFORM_WINDOWS"
        }

-- Linux ==========================
    filter "system:linux"

        links {
            "GLAD",
            "GLFW"
        }

        defines {
            "PLATFORM_LINUX"
        }

-- Configuations ====================
    filter "configurations:Debug"
        defines "DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "RELEASE"
        optimize "On"