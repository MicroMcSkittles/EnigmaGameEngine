#include "Renderer/Buffer.h"
#include "Renderer/RenderAPI.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Enigma {
	namespace Renderer {
		VertexBuffer* VertexBuffer::Create(const std::vector<Renderer::DataType>& layout, Renderer::Usage usage)
		{
			switch (RenderAPI::GetAPI())
			{
			case API::OpenGL: return new Platform::OpenGL::OpenGLVertexBuffer(layout, usage);
			default:
				LOG_ERROR("Can't create vertex buffer, API not supported");
			}
			return nullptr;
		}

		IndexBuffer* IndexBuffer::Create(DataType type, Renderer::Usage usage)
		{
			switch (RenderAPI::GetAPI())
			{
			case API::OpenGL: return new Platform::OpenGL::OpenGLIndexBuffer(type, usage);
			default:
				LOG_ERROR("Can't create index buffer, API not supported");
			}
			return nullptr;
		}
	}
}