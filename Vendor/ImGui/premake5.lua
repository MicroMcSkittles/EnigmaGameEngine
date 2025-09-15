project "ImGui"
    kind "StaticLib"
    language "C++"

-- Output Directories ===============
    rootdir = "../../"
    objdir (rootdir .. "bin-int/" .. outputdir .. "/%{prj.name}")
    targetdir (rootdir .. "bin/" .. outputdir .. "/%{prj.name}")

-- Include all c/c++ files in project
    files
	{
		"ImGui/imconfig.h",
		"ImGui/imgui_demo.cpp",
		"ImGui/imgui_draw.cpp",
		"ImGui/imgui_internal.h",
		"ImGui/imgui_tables.cpp",
		"ImGui/imgui_widgets.cpp",
		"ImGui/imgui.cpp",
		"ImGui/imgui.h",
		"ImGui/imstb_rectpack.h",
		"ImGui/imstb_textedit.h",
		"ImGui/imstb_truetype.h",

		"ImGui/misc/cpp/*.h",
		"ImGui/misc/cpp/*.cpp",
	}

    includedirs {
        "ImGui",
        rootdir .. "Vendor/GLFW/GLFW/include",
        rootdir .. "Vendor/GLAD/GLAD/include",
    }
    links {
        "GLAD",
        "GLFW"
    }

-- Windows ==========================
    filter "system:windows"
		systemversion "latest"
		staticruntime "On"

    -- Include windows specific files
        files
		{
			"ImGui/backends/imgui_impl_win32.cpp",
			"ImGui/backends/imgui_impl_win32.h",
			"ImGui/backends/imgui_impl_opengl3.cpp",
			"ImGui/backends/imgui_impl_opengl3.h",
			"ImGui/backends/imgui_impl_opengl3_loader.h",
		}

    -- Windows specific defines =====
		defines 
		{ 
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}

-- Linux ============================
	filter "system:linux"
		pic "On"

		systemversion "latest"
		staticruntime "On"

		files {
			"ImGui/backends/imgui_impl_glfw.cpp",
			"ImGui/backends/imgui_impl_glfw.h",
			"ImGui/backends/imgui_impl_opengl3.cpp",
			"ImGui/backends/imgui_impl_opengl3.h",
			"ImGui/backends/imgui_impl_opengl3_loader.h",
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