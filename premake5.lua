workspace "EnigmaGameEngine"
    architecture "x64"
    
    configurations {
        "Debug",
        "Release"
    }


    filter "system:linux"
      toolset "clang"

-- Directory final files will be placed into
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Libraries
group "Dependencies"
include "Vendor/GLFW" -- Temp
include "Vendor/stb" -- Temp
include "Vendor/yaml-cpp" -- Temp

include "Vendor/GLAD"
include "Vendor/GLM"

include "Vendor/ImGui"
include "Vendor/ImGuizmo"

group ""
include "Enigma"
include "EnigmaSerialization"

include "EnigmaEditor" -- Enigma Editor Project
include "TestBed" -- A project used for testing new features in the engine
