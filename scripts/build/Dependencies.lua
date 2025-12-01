
-- see https://github.com/TheCherno/Hazel/blob/master/Dependencies.lua 
-- I basicly copied this file from there

-- Enigma Dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")
--VULKAN_SDK = "C:\\VulkanSDK\\1.3.268.0"

IncludeDir = {}
IncludeDir["GLFW"]      = "%{wks.location}/Enigma/vendor/GLFW/GLFW/include"
IncludeDir["Glad"]      = "%{wks.location}/Enigma/vendor/Glad/Glad/include"
IncludeDir["glm"]       = "%{wks.location}/vendor/glm/glm"
IncludeDir["ImGui"]     = "%{wks.location}/vendor/ImGui/ImGui"
IncludeDir["ImGuizmo"]  = "%{wks.location}/EnigmaEditor/vendor/ImGuizmo/ImGuizmo"
IncludeDir["yaml_cpp"]  = "%{wks.location}/EnigmaEditor/vendor/yaml-cpp/yaml-cpp/include"
IncludeDir["stb"]       = "%{wks.location}/EnigmaSerialization/vendor/stb"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["Vulkan"]      = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"]          = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"]      = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"]      = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"]          = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"]      = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"