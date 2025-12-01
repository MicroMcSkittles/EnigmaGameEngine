-- Start EnigmaEditor Project =======
project "EnigmaEditor"
    kind "ConsoleApp"
    staticruntime "off"
    systemversion "latest"

-- Configure C++ ====================
    language "C++"
    cppdialect "C++17"

-- Output Directories ===============
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

-- Include all c/c++ files in project
    files {
        "src/**.c",
        "src/**.h",
        "src/**.cpp",
        "src/**.hpp",
        "src/**.inl",
    }

    includedirs {
        "src",
        "%{wks.location}/Enigma",
        "%{wks.location}/EnigmaSerialization",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.ImGuizmo}",
        "%{IncludeDir.yaml_cpp}"
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

        defines {
            "PLATFORM_WINDOWS"
        }

-- Linux ============================
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
