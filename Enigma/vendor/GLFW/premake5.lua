-- Start GLFW Project ===============
project "GLFW"
    kind "StaticLib"
	staticruntime "On"
	systemversion "latest"

-- Configure C ======================
    language "C"

-- Output Directories ===============
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")

-- Include all c/c++ files in project
    files {
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
		"GLFW/src/null_platform.h",
		"GLFW/src/null_joystick.h",

		"GLFW/src/platform.c",
		"GLFW/src/vulkan.c",
		"GLFW/src/window.c",

		"GLFW/src/internal.h",
		"GLFW/src/platform.h",
		"GLFW/src/mappings.h",
	}

-- Windows ==========================
    filter "system:windows"

    -- Include windows specific files
        files {
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
		defines { 
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}

-- Linux ============================
	filter "system:linux"
		pic "On"

		files {
			"GLFW/src/x11_init.c",
			"GLFW/src/x11_monitor.c",
			"GLFW/src/x11_window.c",
			"GLFW/src/x11_platform.h",
			"GLFW/src/xkb_unicode.c",
			"GLFW/src/posix_time.h",
			"GLFW/src/posix_time.c",
			"GLFW/src/posix_thread.h",
			"GLFW/src/posix_thread.c",
			"GLFW/src/posix_module.c",
			"GLFW/src/posix_poll.c",
			"GLFW/src/posix_poll.h",
			"GLFW/src/glx_context.c",
			"GLFW/src/egl_context.c",
			"GLFW/src/osmesa_context.c",
			"GLFW/src/linux_joystick.c",
		}

		includedirs {
			"GLFW/src"
		}

		defines {
			"_GLFW_X11",
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