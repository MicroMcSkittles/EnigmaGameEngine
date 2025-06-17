#include "Renderer/Shader.h"
#include "Renderer/RenderAPI.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Enigma {
	namespace Renderer {
		Shader* Shader::Create(const ShaderConfig& config)
		{
			Shader* shader;
			switch (RenderAPI::GetAPI())
			{
			case API::OpenGL: shader = new Platform::OpenGL::OpenGLShader(config); break;
			default:
				LOG_ERROR("Can't create shader, API not supported");
				return nullptr;
			}
			shader->m_ID = s_IDHandler.Create(shader);
			return shader;
		}
	}
}


