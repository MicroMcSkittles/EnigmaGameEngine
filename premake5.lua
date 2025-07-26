workspace "EnigmaGameEngine"
    architecture "x64"

    configurations {
        "Debug",
        "Release"
    }

    startproject "SpaceSim"

-- Directory final files will be placed into
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Libraries
include "Vendor/GLAD"
include "Vendor/GLFW"
include "Vendor/GLM"
include "Vendor/stb"
include "Vendor/ImGui"
include "Enigma"
include "EnigmaSerialization"

include "EnigmaEditor" -- Enigma Editor Project
include "SpaceSim" -- The space simulation project
include "TestBed" -- A project used for testing new features in the engine