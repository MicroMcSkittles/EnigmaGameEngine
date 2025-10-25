#include "Enigma/Platform/OpenGL/OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Enigma {
	namespace Platform {
		namespace OpenGL {
			OpenGLVertexArray::OpenGLVertexArray()
			{
				glCreateVertexArrays(1, &m_Handle);
			}
			OpenGLVertexArray::~OpenGLVertexArray()
			{
				glDeleteVertexArrays(1, &m_Handle);
			}
			ref<Renderer::VertexBuffer> OpenGLVertexArray::GetVertexBuffer()
			{
				return m_VertexBuffer;
			}
			ref<Renderer::IndexBuffer> OpenGLVertexArray::GetIndexBuffer()
			{
				return m_IndexBuffer;
			}
			void OpenGLVertexArray::AttachBuffer(ref<Renderer::VertexBuffer> vertexBuffer)
			{
				m_VertexBuffer = vertexBuffer;
			}
			void OpenGLVertexArray::AttachBuffer(ref<Renderer::IndexBuffer> indexBuffer)
			{
				m_IndexBuffer = indexBuffer;
			}
			void OpenGLVertexArray::Bind()
			{
				glBindVertexArray(m_Handle);
			}
			void OpenGLVertexArray::Unbind()
			{
				glBindVertexArray(0);
			}
		}
	}
}