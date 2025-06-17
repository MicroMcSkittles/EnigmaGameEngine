#include "Renderer/VertexArray.h"
#include "Renderer/RenderAPI.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Enigma {
	namespace Renderer {
		VertexArray* VertexArray::Create()
		{
			VertexArray* vertexArray = nullptr;
			switch (RenderAPI::GetAPI())
			{
			case API::OpenGL: vertexArray = new Platform::OpenGL::OpenGLVertexArray(); break;
			default:
				LOG_ERROR("Can't create vertex array, API not supported");
				return nullptr;
			}
			vertexArray->m_ID = s_IDHandle.Create(vertexArray);
			return vertexArray;
		}
	}
}