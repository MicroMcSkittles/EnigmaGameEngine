#include "Enigma/Renderer/VertexArray.h"
#include "Enigma/Renderer/RenderAPI.h"

#include "Enigma/Platform/OpenGL/OpenGLVertexArray.h"

namespace Enigma {
	namespace Renderer {
		VertexArray* VertexArray::Create()
		{
			switch (RenderAPI::GetAPI())
			{
			case API::OpenGL: return new Platform::OpenGL::OpenGLVertexArray(); break;
			default:
				LOG_ERROR("Can't create vertex array, API not supported");
			}
			return nullptr;
		}
	}
}