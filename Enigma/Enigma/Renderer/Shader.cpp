#include "Enigma/EnigmaPCH.h"
#include "Enigma/Renderer/Shader.h"
#include "Enigma/Renderer/RenderAPI.h"

#include "Enigma/Platform/OpenGL/OpenGLShader.h"

namespace Enigma::Renderer {
	ref<Shader> Shader::Create(const ShaderConfig& config)
	{
		switch (RenderAPI::GetAPI())
		{
		case API::OpenGL: return CreateRef<Platform::OpenGL::OpenGLShader>(config); break;
		default:
			LOG_ERROR("Can't create shader, API not supported");
		}
		return nullptr;
	}
}


