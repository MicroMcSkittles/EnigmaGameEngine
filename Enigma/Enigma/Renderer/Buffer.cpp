#include "Enigma/EnigmaPCH.h"
#include "Enigma/Renderer/Buffer.h"
#include "Enigma/Renderer/RenderAPI.h"

#include "Enigma/Platform/OpenGL/OpenGLBuffer.h"

namespace Enigma::Renderer {

	ref<VertexBuffer> VertexBuffer::Create(const std::vector<Renderer::DataType>& layout, Renderer::Usage usage)
	{
		switch (RenderAPI::GetAPI())
		{
		case API::OpenGL: return CreateRef<Platform::OpenGL::OpenGLVertexBuffer>(layout, usage); break;
		default:
			LOG_ERROR("Can't create vertex buffer, API not supported");
		}
		return nullptr;
	}

	ref<IndexBuffer> IndexBuffer::Create(DataType type, Renderer::Usage usage)
	{
		switch (RenderAPI::GetAPI())
		{
		case API::OpenGL: return CreateRef<Platform::OpenGL::OpenGLIndexBuffer>(type, usage); break;
		default:
			LOG_ERROR("Can't create index buffer, API not supported");
		}
		return nullptr;
	}
	ref<FrameBuffer> FrameBuffer::Create(const FrameBufferConfig& config)
	{
		switch (RenderAPI::GetAPI())
		{
		case API::OpenGL: return CreateRef<Platform::OpenGL::OpenGLFrameBuffer>(config); break;
		default:
			LOG_ERROR("Can't create frame buffer, API not supported");
		}
		return nullptr;
	}
	
}