#include "Enigma/Renderer/Shader.h"
#include "Enigma/Renderer/RenderAPI.h"

#include "Enigma/Platform/OpenGL/OpenGLShader.h"

namespace Enigma {
	namespace Renderer {
		Shader* Shader::Create(const ShaderConfig& config)
		{
			switch (RenderAPI::GetAPI())
			{
			case API::OpenGL: return new Platform::OpenGL::OpenGLShader(config); break;
			default:
				LOG_ERROR("Can't create shader, API not supported");
			}
			return nullptr;
		}
	}
}


