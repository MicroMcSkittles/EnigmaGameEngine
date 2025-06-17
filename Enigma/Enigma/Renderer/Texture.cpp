#include "Renderer/Texture.h"
#include "Renderer/RenderAPI.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace Enigma {
	namespace Renderer {
		Texture* Texture::Create(const std::string& path, const TextureConfig& config)
		{
			Texture* texture = nullptr;
			switch (RenderAPI::GetAPI())
			{
			case API::OpenGL: texture = new Platform::OpenGL::OpenGLTexture(path, config); break;
			default:
				LOG_ERROR("Can't create shader, API not supported");
				return nullptr;
			}
			texture->m_ID = s_IdHandler.Create(texture);
			return texture;
		}
		Texture* Texture::Create(const TextureConfig& config)
		{
			Texture* texture = nullptr;
			switch (RenderAPI::GetAPI())
			{
			case API::OpenGL: texture = new Platform::OpenGL::OpenGLTexture(config); break;
			default:
				LOG_ERROR("Can't create shader, API not supported");
				return nullptr;
			}
			texture->m_ID = s_IdHandler.Create(texture);
			return texture;
		}
	}
}