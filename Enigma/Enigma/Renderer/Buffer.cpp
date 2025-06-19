#include "Renderer/Buffer.h"
#include "Renderer/RenderAPI.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Enigma {
	namespace Renderer {
		VertexBuffer* VertexBuffer::Create(const std::vector<Renderer::DataType>& layout, Renderer::Usage usage)
		{
			VertexBuffer* vertexBuffer;
			switch (RenderAPI::GetAPI())
			{
			case API::OpenGL: vertexBuffer = new Platform::OpenGL::OpenGLVertexBuffer(layout, usage); break;
			default:
				LOG_ERROR("Can't create vertex buffer, API not supported");
				return nullptr;
			}
			return vertexBuffer;
		}

		IndexBuffer* IndexBuffer::Create(DataType type, Renderer::Usage usage)
		{
			IndexBuffer* indexBuffer;
			switch (RenderAPI::GetAPI())
			{
			case API::OpenGL: indexBuffer = new Platform::OpenGL::OpenGLIndexBuffer(type, usage); break;
			default:
				LOG_ERROR("Can't create index buffer, API not supported");
				return nullptr;
			}
			return indexBuffer;
		}
		FrameBuffer* FrameBuffer::Create(const FrameBufferConfig& config)
		{
			FrameBuffer* frameBuffer;
			switch (RenderAPI::GetAPI())
			{
			case API::OpenGL: frameBuffer = new Platform::OpenGL::OpenGLFrameBuffer(config); break;
			default:
				LOG_ERROR("Can't create frame buffer, API not supported");
				return nullptr;
			}
			return frameBuffer;
		}
	}
}