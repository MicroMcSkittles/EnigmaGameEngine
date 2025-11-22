project "yaml-cpp"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

-- Output Directories ===============
    rootdir = "../../"
    objdir (rootdir .. "bin-int/" .. outputdir .. "/%{prj.name}")
    targetdir (rootdir .. "bin/" .. outputdir .. "/%{prj.name}")

-- Include all c/c++ files in project
    files
	{
		"yaml-cpp/src/*.h",
		"yaml-cpp/src/*.cpp",
        "contrib/**.h",
        "contrib/**.cpp",

		"yaml-cpp/include/**.h",
	}

    includedirs {
        "yaml-cpp/include",
        "./",
    }

    defines {
        "YAML_CPP_STATIC_DEFINE"
    }

-- Windows ==========================
    filter "system:windows"
		systemversion "latest"
		staticruntime "On"

-- Linux ============================
	filter "system:linux"
		pic "On"

		systemversion "latest"
		staticruntime "On"

-- Configurations ===================
    filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter { "system:windows", "configurations:Debug-AS" }	
		runtime "Debug"
		symbols "on"
		sanitize { "Address" }
		flags { "NoRuntimeChecks", "NoIncrementalLink" }

	filter "configurations:Release"
		runtime "Release"
		optimize "speed"