#include "Enigma/EnigmaPCH.h"
#include "Enigma/Platform/OpenGL/OpenGLShader.h"
#include "Enigma/Platform/OpenGL/OpenGLRenderAPI.h"
#include "Enigma/Platform/OpenGL/OpenGLTexture.h"
#include "Enigma/Core/System.h"
#include "Enigma/Core/Utilities/Utilities.h"

#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace Enigma::Platform::OpenGL {

	shaderc_shader_kind StageType(Renderer::ShaderStageType type) {
		switch (type)
		{
		case Enigma::Renderer::ShaderStageType::Vertex:   return shaderc_shader_kind::shaderc_vertex_shader;
		case Enigma::Renderer::ShaderStageType::Geometry: return shaderc_shader_kind::shaderc_geometry_shader;
		case Enigma::Renderer::ShaderStageType::Fragment: return shaderc_shader_kind::shaderc_fragment_shader;
		case Enigma::Renderer::ShaderStageType::Compute:  return shaderc_shader_kind::shaderc_compute_shader;
		default: LOG_ERROR("Unknown shader stage type \"%u\"", static_cast<u32>(type));
		}
	}
	u32 GLStageType(Renderer::ShaderStageType type) {
		switch (type)
		{
		case Enigma::Renderer::ShaderStageType::Vertex: return GL_VERTEX_SHADER;
		case Enigma::Renderer::ShaderStageType::Geometry: return GL_GEOMETRY_SHADER;
		case Enigma::Renderer::ShaderStageType::Fragment: return GL_FRAGMENT_SHADER;
		case Enigma::Renderer::ShaderStageType::Compute: return GL_COMPUTE_SHADER;
		}
		LOG_ERROR("Unknown shader stage type");
		return 0;
	}
	std::string ToString(spirv_cross::SPIRType::BaseType type) {
		switch (type)
		{
		case spirv_cross::SPIRType::Unknown:               return "Unknown";
		case spirv_cross::SPIRType::Void:                  return "Void";
		case spirv_cross::SPIRType::Boolean:               return "Boolean";
		case spirv_cross::SPIRType::SByte:                 return "SByte";
		case spirv_cross::SPIRType::UByte:                 return "UByte";
		case spirv_cross::SPIRType::Short:                 return "Short";
		case spirv_cross::SPIRType::UShort:                return "UShort";
		case spirv_cross::SPIRType::Int:                   return "Int";
		case spirv_cross::SPIRType::UInt:                  return "UInt";
		case spirv_cross::SPIRType::Int64:                 return "Int64";
		case spirv_cross::SPIRType::UInt64:                return "UInt64";
		case spirv_cross::SPIRType::AtomicCounter:         return "AtomicCounter";
		case spirv_cross::SPIRType::Half:                  return "Half";
		case spirv_cross::SPIRType::Float:                 return "Float";
		case spirv_cross::SPIRType::Double:                return "Double";
		case spirv_cross::SPIRType::Struct:                return "Struct";
		case spirv_cross::SPIRType::Image:                 return "Image";
		case spirv_cross::SPIRType::SampledImage:          return "SampledImage";
		case spirv_cross::SPIRType::Sampler:               return "Sampler";
		case spirv_cross::SPIRType::AccelerationStructure: return "AccelerationStructure";
		case spirv_cross::SPIRType::RayQuery:              return "RayQuery";
		case spirv_cross::SPIRType::ControlPointArray:     return "ControlPointArray";
		case spirv_cross::SPIRType::Interpolant:           return "Interpolant";
		case spirv_cross::SPIRType::Char:                  return "Char";
		case spirv_cross::SPIRType::MeshGridProperties:    return "MeshGridProperties";
		case spirv_cross::SPIRType::BFloat16:              return "BFloat16";
		}
		return "Unknown";
	}

	// Makes sure shader compiled correctly, logs a warning if it didn't
	bool ValidateShader(u32 handle) {
		i32 success = 0;
		i8 info[512];
		glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
		if (!success) {
#ifdef ENABLE_LOGGER
			glGetShaderInfoLog(handle, 512, NULL, info);
			i32 type;
			glGetShaderiv(handle, GL_SHADER_TYPE, &type);
			std::string infoLog = info;
			std::string shaderType;
			switch (type) {
			case GL_VERTEX_SHADER: shaderType = "Vertex"; break;
			case GL_FRAGMENT_SHADER: shaderType = "Fragment"; break;
			case GL_GEOMETRY_SHADER: shaderType = "Geometry"; break;
			case GL_COMPUTE_SHADER: shaderType = "Compute"; break;
			}
			LOG_WARNING("Failed to compile %s shader.\n%s", shaderType.c_str(), infoLog.c_str());
			return false;
#endif
		}
		return true;
	}

	OpenGLShader::OpenGLShader(const Renderer::ShaderConfig& config) : m_PushConstantCount(0)
	{
		m_Config = config;

		for (const Renderer::ShaderStage& stageConfig : m_Config.stages) {
			OpenGLShaderStage stage;
			stage.type = stageConfig.type;
			stage.filename = stageConfig.filename;
			stage.checksum = Core::Hash(Core::System::ReadFileStr(stage.filename));

			if (m_Config.flags & Renderer::ShaderFlagForceCompile) {
				// Compile shaders
				CompileStageToVKSpirv(stage);
				CompileStageToGLSpirv(stage);
			}
			else {
				// Check if opengl cache exists
				if (IsStageGLSpirvCached(stage)) LoadStageGLSpirvCache(stage);
				else {
					// Check if vulkan cache exists
					if (IsStageVKSpirvCached(stage)) LoadStageVKSpirvCache(stage);
					else CompileStageToVKSpirv(stage);

					// Compile shaders
					CompileStageToGLSpirv(stage);
				}
			}
			
			ReflectStage(stage);
			m_Stages.push_back(stage);
		}
		
		CreateProgram();

		m_Stages.clear();
	}
	OpenGLShader::~OpenGLShader()
	{
		//if (m_Config.flags & Renderer::VertexShader) glDeleteShader(m_VertexHandle);
		//if (m_Config.flags & Renderer::PixelShader) glDeleteShader(m_PixelHandle);
		//if (m_Config.flags & Renderer::GeometryShader) glDeleteShader(m_GeometryHandle);
		glDeleteProgram(m_Handle);
	}

	const std::vector<Renderer::Uniform>& OpenGLShader::GetActiveUniforms()
	{
		return m_Uniforms;
	}
	Renderer::Uniform& OpenGLShader::GetUniform(const std::string& name)
	{
		return m_Uniforms[m_UniformLookup[name]];
	}
	bool OpenGLShader::IsUniformActive(const std::string& name)
	{
		return m_UniformLookup.find(name) != m_UniformLookup.end();
	}
	void OpenGLShader::SetUniform(const std::string& name, void* data)
	{
		/*if (!IsUniformActive(name)) return;

		glUseProgram(m_Handle);

		Renderer::Uniform& uniform = m_Uniforms[m_UniformLookup[name]];
		u32 location = m_UniformLocations[m_UniformLookup[name]];
		uniform.data = data;

		switch (uniform.type)
		{
		case Renderer::DataType::Int: {
			i32 value = *static_cast<i32*>(data);
			glUniform1i(location, value);
			break;
		}
		case Renderer::DataType::Int2: {
			glm::ivec2 value = *static_cast<glm::ivec2*>(data);
			glUniform2i(location, value.x, value.y);
			break;
		}
		case Renderer::DataType::Int3: {
			glm::ivec3 value = *static_cast<glm::ivec3*>(data);
			glUniform3i(location, value.x, value.y, value.z);
			break;
		}
		case Renderer::DataType::Int4: {
			glm::ivec4 value = *static_cast<glm::ivec4*>(data);
			glUniform4i(location, value.x, value.y, value.z, value.w);
			break;
		}
		case Renderer::DataType::Float: {
			f32 value = *static_cast<f32*>(data);
			glUniform1f(location, value);
			break;
		}
		case Renderer::DataType::Float2: {
			glm::vec2 value = *static_cast<glm::vec2*>(data);
			glUniform2f(location, value.x, value.y);
			break;
		}
		case Renderer::DataType::Float3: {
			glm::vec3 value = *static_cast<glm::vec3*>(data);
			glUniform3f(location, value.x, value.y, value.z);
			break;
		}
		case Renderer::DataType::Float4: {
			glm::vec4 value = *static_cast<glm::vec4*>(data);
			glUniform4f(location, value.x, value.y, value.z, value.w);
			break;
		}
		case Renderer::DataType::Matrix2: {
			glm::mat2 value = *static_cast<glm::mat2*>(data);
			glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value));
			break;
		}
		case Renderer::DataType::Matrix3: {
			glm::mat3 value = *static_cast<glm::mat3*>(data);
			glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
			break;
		}
		case Renderer::DataType::Matrix4: {
			glm::mat4 value = *static_cast<glm::mat4*>(data);
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
			break;
		}
		case Renderer::DataType::Texture2D: {
			ref<OpenGLTexture> texture = *static_cast<ref<OpenGLTexture>*>(data);
			glUniform1i(location, texture->GetSlot());
			break;
		}

		default:
			LOG_WARNING("Uniform type isn't supported");
		}*/
	}
	void OpenGLShader::SetPushConstant(void* data, u32 offset, u32 size)
	{

	}
	//void OpenGLShader::FindUniforms()
	//{
	//	/*glUseProgram(m_Handle);
	//
	//	i32 uniformCount = 0;
	//	glGetProgramiv(m_Handle, GL_ACTIVE_UNIFORMS, &uniformCount);
	//
	//	for (i32 i = 0; i < uniformCount; ++i) {
	//		i32 maxNameLength = 0;
	//		i32 nameLength = 0;
	//		i32 size = 0;
	//		u32 type = GL_NONE;
	//		glGetProgramiv(m_Handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);
	//
	//		std::vector<i8> name(maxNameLength);
	//		glGetActiveUniform(m_Handle, i, maxNameLength, &nameLength, &size, &type, name.data());
	//
	//		Renderer::Uniform uniform;
	//		uniform.name = std::string(name.begin(), name.begin() + nameLength);
	//		uniform.type = Conversions::DataType(type);
	//		uniform.data = (void*)NULL;
	//
	//		m_UniformLookup.insert({ uniform.name, m_Uniforms.size() });
	//		m_Uniforms.push_back(uniform);
	//		m_UniformLocations.push_back(glGetUniformLocation(m_Handle, uniform.name.c_str()));
	//	}
	//
	//	glUseProgram(0);*/
	//}

	void OpenGLShader::Bind() {
		glUseProgram(m_Handle);
	}
	void OpenGLShader::Unbind() {
		glUseProgram(0);
	}

	bool OpenGLShader::IsStageVKSpirvCached(const OpenGLShaderStage& stage)
	{
		// Check if cache directory exists
		std::string name = std::filesystem::path(stage.filename).filename().string();
		std::filesystem::path cacheDir = (c_CacheDirectory + name + c_VKSpirvCacheExt);
		if (!std::filesystem::exists(cacheDir)) return false;

		// Skip checksum if stage doesn't have one
		if (stage.checksum == -1) return true;

		// Read the checksum from the file
		std::ifstream file;
		file.open(cacheDir.string(), std::ios::binary);
		if (!file.is_open()) return false;

		u64 checksum = 0;
		file.read(reinterpret_cast<char*>(&checksum), sizeof(u64));
		file.close();

		if (checksum != stage.checksum) return false;
		return true;
	}
	void OpenGLShader::LoadStageVKSpirvCache(OpenGLShaderStage& stage)
	{
		// Get cache directory
		std::string name = std::filesystem::path(stage.filename).filename().string();
		std::filesystem::path cacheDir = (c_CacheDirectory + name + c_VKSpirvCacheExt);

		LOG_MESSAGE("Loading Vulkan SPIR-V \"%s\"", 5, cacheDir.string().c_str());

		// Open cache
		std::ifstream file;
		file.open(cacheDir.string(), std::ios::binary);
		if (!file.is_open()) {
			LOG_ERROR("Failed to open shader spir-v cache for \"%s\"", stage.filename.c_str());
		}

		// Read byte code
		u64 fileSize = std::filesystem::file_size(cacheDir) - c_CacheChecksumSize;
		stage.vulkanSpirv.resize(fileSize / sizeof(u32)); // divid by 4 because byteCode is a list of u32 and not u8
		file.seekg(c_CacheChecksumSize, std::ios::beg);
		file.read(reinterpret_cast<char*>(stage.vulkanSpirv.data()), fileSize);
		file.close();
	}
	void OpenGLShader::CompileStageToVKSpirv(OpenGLShaderStage& stage)
	{
		std::string typeStr = Renderer::ToString(stage.type);
		LOG_MESSAGE("Compiling shader stage \"%s\" from file \"%s\" to Vulkan SPIR-V", 5, typeStr.c_str(), stage.filename.c_str());

		// Open shader source file
		std::ifstream file;
		file.open(stage.filename);
		if (!file.is_open()) {
			LOG_ERROR("Failed to open shader source file \"%s\"", stage.filename.c_str());
		}

		// Read source code
		std::stringstream sourceBuffer;
		sourceBuffer << file.rdbuf();
		std::string source = sourceBuffer.str();

		// Close shader source file
		file.close();
		
		// Configure shaderc
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		options.SetOptimizationLevel(shaderc_optimization_level_performance);

		// Compile to spir-v
		shaderc::Compiler compiler;
		shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(source, StageType(stage.type), stage.filename.c_str(), options);
		if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
			// Show error message
			std::string message = result.GetErrorMessage();
			LOG_SOFT_ERROR(
				"Failed to compile shader Vulkan SPIR-V \"%s\" with %u errors and %u warnings:\n%s",
				Renderer::ToString(stage.type).c_str(), 
				result.GetNumErrors(),
				result.GetNumWarnings(),
				message.c_str()
			);
			return;
		}
		
		// Store result
		stage.vulkanSpirv = std::vector<u32>(result.begin(), result.end());

		// Get cache directory
		std::string name = std::filesystem::path(stage.filename).filename().string();
		std::filesystem::path cacheDir = (c_CacheDirectory + name + c_VKSpirvCacheExt);
		VarifyCacheDirectory();

		// Open cache
		std::ofstream cache;
		cache.open(cacheDir.string(), std::ios::binary);
		if (!cache.is_open()) {
			LOG_WARNING("Failed to open shader Vulkan SPIR-V cache \"%s\"", cacheDir.string().c_str());
			return;
		}

		// Write data and close
		cache.write(reinterpret_cast<char*>(&stage.checksum), c_CacheChecksumSize);
		cache.write(reinterpret_cast<char*>(stage.vulkanSpirv.data()), stage.vulkanSpirv.size() * sizeof(u32));
		cache.close();
	}

	bool OpenGLShader::IsStageGLSpirvCached(const OpenGLShaderStage& stage)
	{
		// Check if cache directory exists
		std::string name = std::filesystem::path(stage.filename).filename().string();
		std::filesystem::path cacheDir = (c_CacheDirectory + name + c_GLSpirvCacheExt);
		if (!std::filesystem::exists(cacheDir)) return false;

		// Skip checksum if stage doesn't have one
		if (stage.checksum == -1) return true;

		// Read the checksum from the file
		std::ifstream file;
		file.open(cacheDir.string(), std::ios::binary);
		if (!file.is_open()) return false;

		u64 checksum = 0;
		file.read(reinterpret_cast<char*>(&checksum), sizeof(u64));
		file.close();

		if (checksum != stage.checksum) return false;
		return true;
	}
	void OpenGLShader::LoadStageGLSpirvCache(OpenGLShaderStage& stage)
	{
		// Get cache directory
		std::string name = std::filesystem::path(stage.filename).filename().string();
		std::filesystem::path cacheDir = (c_CacheDirectory + name + c_GLSpirvCacheExt);

		LOG_MESSAGE("Loading OpenGL SPIR-V \"%s\"", 5, cacheDir.string().c_str());

		// Open cache
		std::ifstream file;
		file.open(cacheDir.string(), std::ios::binary);
		if (!file.is_open()) {
			LOG_ERROR("Failed to open shader OpenGL SPIR-V cache for \"%s\"", stage.filename.c_str());
		}

		// Read byte code
		u64 fileSize = std::filesystem::file_size(cacheDir) - c_CacheChecksumSize;
		stage.openglSpirv.resize(fileSize / sizeof(u32)); // divid by 4 because byteCode is a list of u32 and not u8
		file.seekg(c_CacheChecksumSize, std::ios::beg);
		file.read(reinterpret_cast<char*>(stage.openglSpirv.data()), fileSize);
		file.close();
	}
	void OpenGLShader::CompileStageToGLSpirv(OpenGLShaderStage& stage)
	{
		std::string typeStr = Renderer::ToString(stage.type);
		LOG_MESSAGE("Compiling shader stage \"%s\" from file \"%s\" to OpenGL SPIR-V", 5, typeStr.c_str(), stage.filename.c_str());

		// Convert spirv to opengl glsl
		spirv_cross::CompilerGLSL glslCompiler(stage.vulkanSpirv);
		for (const spirv_cross::Resource& pushConstant : glslCompiler.get_shader_resources().push_constant_buffers) {
			const spirv_cross::SPIRType& type = glslCompiler.get_type(pushConstant.type_id);
			glslCompiler.set_decoration(pushConstant.id, spv::DecorationLocation, m_PushConstantCount);
			m_PushConstantCount += type.member_types.size();
		}
		stage.glslSource = glslCompiler.compile();

		//LOG_MESSAGE("Generated OpenGL GLSL for stage \"%s\" from file \"%s\":\n%s", 5, Renderer::ToString(stage.type).c_str(), stage.filename.c_str(), stage.glslSource.c_str());

		// Configure compiler
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		options.SetOptimizationLevel(shaderc_optimization_level_performance);
		
		// Compile opengl glsl
		shaderc::Compiler compiler;
		shaderc::CompilationResult result = compiler.CompileGlslToSpv(stage.glslSource, StageType(stage.type), stage.filename.c_str(), options);
		if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
			// Show error message
			std::string message = result.GetErrorMessage();
			LOG_SOFT_ERROR(
				"Failed to compile shader GLSL \"%s\" with %u errors and %u warnings:\n%s",
				Renderer::ToString(stage.type).c_str(),
				result.GetNumErrors(),
				result.GetNumWarnings(),
				message.c_str()
			);
			return;
		}
		
		stage.openglSpirv = std::vector<u32>(result.begin(), result.end());

		// Get cache directory
		std::string name = std::filesystem::path(stage.filename).filename().string();
		std::filesystem::path cacheDir = (c_CacheDirectory + name + c_GLSpirvCacheExt);
		VarifyCacheDirectory();

		// Open cache
		std::ofstream cache;
		cache.open(cacheDir.string(), std::ios::binary);
		if (!cache.is_open()) {
			LOG_WARNING("Failed to open shader OpenGL SPIR-V cache \"%s\"", cacheDir.string().c_str());
			return;
		}

		// Write data and close
		cache.write(reinterpret_cast<char*>(&stage.checksum), c_CacheChecksumSize);
		cache.write(reinterpret_cast<char*>(stage.openglSpirv.data()), stage.openglSpirv.size() * sizeof(u32));
		cache.close();
	}

	void OpenGLShader::CreateProgram()
	{
		m_Handle = glCreateProgram();
		for (OpenGLShaderStage& stage : m_Stages) {
			stage.handle = glCreateShader(GLStageType(stage.type));
			glShaderBinary(1, &stage.handle, GL_SHADER_BINARY_FORMAT_SPIR_V, stage.openglSpirv.data(), stage.openglSpirv.size() * sizeof(u32));
			glSpecializeShader(stage.handle, "main", 0, nullptr, nullptr);
			glAttachShader(m_Handle, stage.handle);
			glDeleteShader(stage.handle);
		}
		glLinkProgram(m_Handle);

		// Validate shader program
		i32 success = 0;
		i8 info[512];
		glGetProgramiv(m_Handle, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(m_Handle, 512, NULL, info);
			std::string infoLog = info;
			LOG_SOFT_ERROR("Failed to link shader program.\n%s", infoLog.c_str());
		}
	}

	void OpenGLShader::ReflectStage(const OpenGLShaderStage& stage)
	{
		/*spirv_cross::Compiler compiler(byteCode);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		LOG_MESSAGE("%s Stage Reflection \"%s\"", 5, Renderer::ToString(stage.type).c_str(), stage.filename.c_str());
		LOG_MESSAGE("    Input Count: %u", 5, resources.stage_inputs.size());
		LOG_MESSAGE("    Output Count: %u", 5, resources.stage_outputs.size());
		LOG_MESSAGE("    Uniform Buffer Count: %u", 5, resources.uniform_buffers.size());
		LOG_MESSAGE("    Push Constant Buffer Count: %u", 5, resources.push_constant_buffers.size());
		LOG_MESSAGE("    Sampled Image Count: %u", 5, resources.sampled_images.size());
		LOG_MESSAGE("", 5);

		for (const spirv_cross::Resource& resource : resources.stage_inputs) {
			const spirv_cross::SPIRType& type = compiler.get_type(resource.base_type_id);
			u32 binding = compiler.get_decoration(resource.id, spv::DecorationLocation);
			LOG_MESSAGE("    Input \"%s\":", 5, resource.name.c_str());
			LOG_MESSAGE("         Location: %u", 5, binding);
		}
		for (const spirv_cross::Resource& resource : resources.stage_outputs) {
			u32 location = compiler.get_decoration(resource.id, spv::DecorationLocation);
			LOG_MESSAGE("    Output \"%s\":", 5, resource.name.c_str());
			LOG_MESSAGE("         Location: %u", 5, location);
		}

		for (const spirv_cross::Resource& resource : resources.uniform_buffers) {
			const spirv_cross::SPIRType& type = compiler.get_type(resource.base_type_id);
			u32 size = compiler.get_declared_struct_size(type);
			u32 binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			u64 memberCount = type.member_types.size();
			LOG_MESSAGE("    Uniform Buffer \"%s\":", 5, resource.name.c_str());
			LOG_MESSAGE("         Size: %u", 5, size);
			LOG_MESSAGE("         Binding: %u", 5, binding);
			LOG_MESSAGE("         Member Count: %u", 5, memberCount);
		}
		for (const spirv_cross::Resource& resource : resources.push_constant_buffers) {
			const spirv_cross::SPIRType& type = compiler.get_type(resource.base_type_id);
			u32 size = compiler.get_declared_struct_size(type);
			u32 binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			u64 memberCount = type.member_types.size();
			LOG_MESSAGE("    Push Constant Buffer \"%s\":", 5, resource.name.c_str());
			LOG_MESSAGE("         Size: %u", 5, size);
			LOG_MESSAGE("         Binding: %u", 5, binding);
			LOG_MESSAGE("         Member Count: %u", 5, memberCount);
		}*/
	}
	
	void OpenGLShader::VarifyCacheDirectory()
	{
		// Create cache directory if it doesn't exist
		if (!std::filesystem::is_directory(c_CacheDirectory)) {
			if (!std::filesystem::create_directories(c_CacheDirectory)) {
				LOG_WARNING("Failed to create shader cache directory");
				return;
			}
		}
	}
}