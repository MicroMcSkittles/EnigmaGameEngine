include "scripts/build/Dependencies.lua"

workspace "EnigmaGameEngine"
    architecture "x64"
    startproject "EnigmaEditor"

    configurations {
        "Debug",
        "Release"
    }

    flags {
        "MultiProcessorCompile"
    }

    filter "system:linux"
      toolset "clang"

-- Directory final files will be placed into
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Libraries
group "Dependencies"
    include "Enigma/vendor/GLFW"
    include "Enigma/vendor/Glad"

    include "EnigmaEditor/vendor/yaml-cpp"
    include "EnigmaEditor/vendor/ImGuizmo"

    include "EnigmaSerialization/vendor/stb"

    include "vendor/glm"
    include "vendor/ImGui"
group ""

include "Enigma"
include "EnigmaSerialization"

include "EnigmaEditor" -- Enigma Editor Project
include "TestBed" -- A project used for testing new features in the engine
