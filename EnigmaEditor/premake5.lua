project "EnigmaEditor"
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
        "src/**.inl",
        rootdir .. "Vendor/stb/stb/stb_image.h",
    }

    includedirs {
        "src",
        rootdir .. "Enigma",
        rootdir .. "EnigmaSerialization",
        rootdir .. "Vendor/GLM/GLM",
        rootdir .. "Vendor/ImGui/ImGui",
        rootdir .. "Vendor/ImGuizmo/ImGuizmo",
        rootdir .. "Vendor/yaml-cpp/yaml-cpp/include",
    }

    defines {
        "GLM_ENABLE_EXPERIMENTAL",
        "YAML_CPP_STATIC_DEFINE",
    }

-- Link libraries ===================
    links {
        "Enigma",
        "EnigmaSerialization",
        "ImGui",
        "ImGuizmo",
        "yaml-cpp",
    }

-- Windows ==========================
    filter "system:windows"
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
