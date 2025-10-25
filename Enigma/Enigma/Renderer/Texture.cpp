#include "Enigma/Renderer/Texture.h"
#include "Enigma/Renderer/RenderAPI.h"
#include "Enigma/Renderer/VertexArray.h"

#include "Enigma/Platform/OpenGL/OpenGLTexture.h"

namespace Enigma {
	namespace Renderer {
		ref<Texture> Texture::Create(const TextureConfig& config)
		{
			switch (RenderAPI::GetAPI())
			{
			case API::OpenGL: return CreateRef<Platform::OpenGL::OpenGLTexture>(config); break;
			default:
				LOG_ERROR("Can't create shader, API not supported");
				return nullptr;
			}
		}
	}
}