project "GLFW"
    kind "StaticLib"
    language "C"

-- Output Directories ===============
    rootdir = "../../"
    targetdir (rootdir .. "bin/" .. outputdir .. "/%{prj.name}")
    objdir (rootdir .. "bin-int/" .. outputdir .. "/%{prj.name}")

-- Include all c/c++ files in project
    files
	{
		"GLFW/include/GLFW/glfw3.h",
		"GLFW/include/GLFW/glfw3native.h",
		"GLFW/src/glfw_config.h",
		"GLFW/src/context.c",
		"GLFW/src/init.c",
		"GLFW/src/input.c",
		"GLFW/src/monitor.c",

		"GLFW/src/null_init.c",
		"GLFW/src/null_joystick.c",
		"GLFW/src/null_monitor.c",
		"GLFW/src/null_window.c",

		"GLFW/src/platform.c",
		"GLFW/src/vulkan.c",
		"GLFW/src/window.c",
	}

-- Windows ==========================
    filter "system:windows"
		systemversion "latest"
		staticruntime "On"

    -- Include windows specific files
        files
		{
			"GLFW/src/win32_init.c",
			"GLFW/src/win32_joystick.c",
			"GLFW/src/win32_module.c",
			"GLFW/src/win32_monitor.c",
			"GLFW/src/win32_time.c",
			"GLFW/src/win32_thread.c",
			"GLFW/src/win32_window.c",
			"GLFW/src/wgl_context.c",
			"GLFW/src/egl_context.c",
			"GLFW/src/osmesa_context.c"
		}

    -- Windows specific defines =====
		defines 
		{ 
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
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