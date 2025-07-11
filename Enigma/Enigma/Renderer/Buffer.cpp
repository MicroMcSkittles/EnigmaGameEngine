#include "Renderer/Buffer.h"
#include "Renderer/RenderAPI.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Enigma {
	namespace Renderer {
		VertexBuffer* VertexBuffer::Create(const std::vector<Renderer::DataType>& layout, Renderer::Usage usage)
		{
			switch (RenderAPI::GetAPI())
			{
			case API::OpenGL: return new Platform::OpenGL::OpenGLVertexBuffer(layout, usage); break;
			default:
				LOG_ERROR("Can't create vertex buffer, API not supported");
			}
			return nullptr;
		}

		IndexBuffer* IndexBuffer::Create(DataType type, Renderer::Usage usage)
		{
			switch (RenderAPI::GetAPI())
			{
			case API::OpenGL: return new Platform::OpenGL::OpenGLIndexBuffer(type, usage); break;
			default:
				LOG_ERROR("Can't create index buffer, API not supported");
			}
			return nullptr;
		}
		FrameBuffer* FrameBuffer::Create(const FrameBufferConfig& config)
		{
			switch (RenderAPI::GetAPI())
			{
			case API::OpenGL: return new Platform::OpenGL::OpenGLFrameBuffer(config); break;
			default:
				LOG_ERROR("Can't create frame buffer, API not supported");
			}
			return nullptr;
		}
	}
}