#include "Renderer/Texture.h"
#include "Renderer/RenderAPI.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace Enigma {
	namespace Renderer {
		Texture* Texture::Create(const std::string& path, const TextureConfig& config)
		{
			switch (RenderAPI::GetAPI())
			{
			case API::OpenGL: return new Platform::OpenGL::OpenGLTexture(path, config);
			default:
				LOG_ERROR("Can't create shader, API not supported");
			}
			return nullptr;
		}
		Texture* Texture::Create(const TextureConfig& config)
		{
			switch (RenderAPI::GetAPI())
			{
			case API::OpenGL: return new Platform::OpenGL::OpenGLTexture(config);
			default:
				LOG_ERROR("Can't create shader, API not supported");
			}
			return nullptr;
		}
	}
}