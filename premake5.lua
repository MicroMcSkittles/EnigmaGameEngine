workspace "EnigmaGameEngine"
    architecture "x64"

    configurations {
        "Debug",
        "Release"
    }

    startproject "EnigmaEditor"

-- Directory final files will be placed into
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Libraries
include "Vendor/GLAD"
include "Vendor/GLFW"
include "Vendor/GLM"
include "Vendor/stb"
include "Vendor/ImGui"
include "Enigma"

include "ExampleApp" -- Example Project
include "EnigmaEditor" -- Enigma Editor Project
include "SpaceSim" -- The space simulation project