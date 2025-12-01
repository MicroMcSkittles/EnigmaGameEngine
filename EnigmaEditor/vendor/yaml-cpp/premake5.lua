-- Start yaml-cpp Project ===========
project "yaml-cpp"
    kind "StaticLib"
	staticruntime "off"
	systemversion "latest"

-- Configure C++ ====================
    language "C++"
    cppdialect "C++17"

-- Output Directories ===============
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")

-- Include all c/c++ files in project
    files {
		"yaml-cpp/src/**.h",
		"yaml-cpp/src/**.cpp",

		"yaml-cpp/include/**.h",
	}

    includedirs {
        "yaml-cpp/include",
    }

    defines {
        "YAML_CPP_STATIC_DEFINE"
    }

-- Windows ==========================
	filter "system:windows"

	-- Replace broken header file ===
		prebuildcommands {
			"{COPYFILE} ./dragonbox_FIX.h yaml-cpp/src/contrib/dragonbox.h"
		}

-- Linux ============================
	filter "system:linux"
		pic "On"

	-- Replace broken header file ===
		prebuildcommands {
			"cp -f ./dragonbox_FIX.h yaml-cpp/src/contrib/dragonbox.h"
		}

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