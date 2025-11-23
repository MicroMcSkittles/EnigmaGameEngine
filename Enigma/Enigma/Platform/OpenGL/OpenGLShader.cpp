#include "Enigma/EnigmaPCH.h"
#include "Enigma/Platform/OpenGL/OpenGLShader.h"
#include "Enigma/Platform/OpenGL/OpenGLRenderAPI.h"
#include "Enigma/Platform/OpenGL/OpenGLTexture.h"
#include "Enigma/Core/System.h"

#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

namespace Enigma::Platform::OpenGL {
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

	OpenGLShader::OpenGLShader(const Renderer::ShaderConfig& config)
	{
		m_Config = config;

		if (m_Config.flags & Renderer::VertexShader) {
			m_VertexHandle = glCreateShader(GL_VERTEX_SHADER);

			std::string source;
			const char* cSource;
			if (m_Config.flags & Renderer::PathAsSource) source = m_Config.vertexPath;
			else source = Core::System::ReadFileStr(m_Config.vertexPath);
			cSource = source.c_str();

			glShaderSource(m_VertexHandle, 1, &cSource, NULL);
			glCompileShader(m_VertexHandle);

			ValidateShader(m_VertexHandle);
		}
		if (m_Config.flags & Renderer::PixelShader) {
			m_PixelHandle = glCreateShader(GL_FRAGMENT_SHADER);

			std::string source;
			const char* cSource;
			if (m_Config.flags & Renderer::PathAsSource) source = m_Config.pixelPath;
			else source = Core::System::ReadFileStr(m_Config.pixelPath);
			cSource = source.c_str();

			glShaderSource(m_PixelHandle, 1, &cSource, NULL);
			glCompileShader(m_PixelHandle);

			ValidateShader(m_PixelHandle);
		}
		if (m_Config.flags & Renderer::GeometryShader) {
			m_GeometryHandle = glCreateShader(GL_GEOMETRY_SHADER);

			std::string source;
			const char* cSource;
			if (m_Config.flags & Renderer::PathAsSource) source = m_Config.geomatryPath;
			else source = Core::System::ReadFileStr(m_Config.geomatryPath);
			cSource = source.c_str();

			glShaderSource(m_GeometryHandle, 1, &cSource, NULL);
			glCompileShader(m_GeometryHandle);

			ValidateShader(m_GeometryHandle);
		}

		m_Handle = glCreateProgram();;
		if (m_Config.flags & Renderer::VertexShader) glAttachShader(m_Handle, m_VertexHandle);
		if (m_Config.flags & Renderer::PixelShader) glAttachShader(m_Handle, m_PixelHandle);
		if (m_Config.flags & Renderer::GeometryShader) glAttachShader(m_Handle, m_GeometryHandle);
		glLinkProgram(m_Handle);

		i32 success = 0;
		i8 info[512];
		glGetProgramiv(m_Handle, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(m_Handle, 512, NULL, info);
			std::string infoLog = info;
			LOG_SOFT_ERROR("Failed to link shader program.\n%s", infoLog.c_str());
		}

		FindUniforms();
	}
	OpenGLShader::~OpenGLShader()
	{
		if (m_Config.flags & Renderer::VertexShader) glDeleteShader(m_VertexHandle);
		if (m_Config.flags & Renderer::PixelShader) glDeleteShader(m_PixelHandle);
		if (m_Config.flags & Renderer::GeometryShader) glDeleteShader(m_GeometryHandle);
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
		if (!IsUniformActive(name)) return;

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
		}
	}
	void OpenGLShader::FindUniforms()
	{
		glUseProgram(m_Handle);

		i32 uniformCount = 0;
		glGetProgramiv(m_Handle, GL_ACTIVE_UNIFORMS, &uniformCount);

		for (i32 i = 0; i < uniformCount; ++i) {
			i32 maxNameLength = 0;
			i32 nameLength = 0;
			i32 size = 0;
			u32 type = GL_NONE;
			glGetProgramiv(m_Handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);

			std::vector<i8> name(maxNameLength);
			glGetActiveUniform(m_Handle, i, maxNameLength, &nameLength, &size, &type, name.data());

			Renderer::Uniform uniform;
			uniform.name = std::string(name.begin(), name.begin() + nameLength);
			uniform.type = Conversions::DataType(type);
			uniform.data = (void*)NULL;

			m_UniformLookup.insert({ uniform.name, m_Uniforms.size() });
			m_Uniforms.push_back(uniform);
			m_UniformLocations.push_back(glGetUniformLocation(m_Handle, uniform.name.c_str()));
		}

		glUseProgram(0);
	}

	void OpenGLShader::Bind()
	{
		glUseProgram(m_Handle);
	}
	void OpenGLShader::Unbind()
	{
		glUseProgram(0);
	}
}