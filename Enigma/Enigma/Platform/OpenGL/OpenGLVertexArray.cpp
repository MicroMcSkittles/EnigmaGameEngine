#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Enigma {
	namespace Platform {
		namespace OpenGL {
			OpenGLVertexArray::OpenGLVertexArray()
			{
				glCreateVertexArrays(1, &m_Handle);
				m_VertexBuffer = nullptr;
				m_IndexBuffer = nullptr;
			}
			OpenGLVertexArray::~OpenGLVertexArray()
			{
				delete m_VertexBuffer;
				m_VertexBuffer = nullptr;

				delete m_IndexBuffer;
				m_IndexBuffer = nullptr;
				
				glDeleteVertexArrays(1, &m_Handle);
			}
			Renderer::VertexBuffer* OpenGLVertexArray::GetVertexBuffer()
			{
				return m_VertexBuffer;
			}
			Renderer::IndexBuffer* OpenGLVertexArray::GetIndexBuffer()
			{
				return m_IndexBuffer;
			}
			void OpenGLVertexArray::AttachBuffer(Renderer::VertexBuffer* vertexBuffer)
			{
				m_VertexBuffer = vertexBuffer;
			}
			void OpenGLVertexArray::AttachBuffer(Renderer::IndexBuffer* indexBuffer)
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