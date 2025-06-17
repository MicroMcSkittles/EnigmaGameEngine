#include "Platform/OpenGL/OpenGLBuffer.h"
#include "Platform/OpenGL/OpenGLRenderAPI.h"

#include <glad/glad.h>

namespace Enigma {
	namespace Platform {
		namespace OpenGL {
			OpenGLVertexBuffer::OpenGLVertexBuffer(const std::vector<Renderer::DataType>& layout, Renderer::Usage usage)
			{
				m_Layout = layout;
				m_Usage = Conversions::Usage(usage);
				glGenBuffers(1, &m_Handle);

				
			}
			OpenGLVertexBuffer::~OpenGLVertexBuffer()
			{
				glDeleteBuffers(1, &m_Handle);
			}
			void OpenGLVertexBuffer::InitAttribs()
			{
				int stride = 0;
				for (auto type : m_Layout) {
					stride += Conversions::DataTypeSize(type);
				}

				int offset = 0;
				for (int i = 0; i < m_Layout.size(); ++i) {
					auto type = m_Layout[i];
					glVertexAttribPointer(i, Conversions::DataTypeCount(type), Conversions::DataType(type), GL_FALSE, stride, (void*)offset);
					glEnableVertexAttribArray(i);
					offset += Conversions::DataTypeSize(type);
				}
			}
			void OpenGLVertexBuffer::SetData(void* vertices, int size)
			{
				glBindBuffer(GL_ARRAY_BUFFER, m_Handle);
				glBufferData(GL_ARRAY_BUFFER, size, vertices, m_Usage);
			}
			void OpenGLVertexBuffer::SetSubData(void* vertices, int size, int offset)
			{
				glBindBuffer(GL_ARRAY_BUFFER, m_Handle);
				glBufferSubData(GL_ARRAY_BUFFER, offset, size, vertices);
			}
			void OpenGLVertexBuffer::Bind()
			{
				glBindBuffer(GL_ARRAY_BUFFER, m_Handle);
			}
			void OpenGLVertexBuffer::Unbind()
			{
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}


			OpenGLIndexBuffer::OpenGLIndexBuffer(Renderer::DataType type, Renderer::Usage usage)
			{
				m_IndicesCount = 0;
				m_Usage = Conversions::Usage(usage);
				m_Type = type;
				glGenBuffers(1, &m_Handle);
			}
			OpenGLIndexBuffer::~OpenGLIndexBuffer()
			{
				glDeleteBuffers(1, &m_Handle);
			}
			int OpenGLIndexBuffer::GetIndexCount()
			{
				return m_IndicesCount;
			}
			Renderer::DataType OpenGLIndexBuffer::GetIndexType()
			{
				return m_Type;
			}
			void OpenGLIndexBuffer::SetData(void* indices, int size)
			{
				m_IndicesCount = size / Conversions::DataTypeSize(m_Type);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Handle);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, m_Usage);
			}
			void OpenGLIndexBuffer::SetSubData(void* indices, int size, int offset)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Handle);
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, indices);
			}
			void OpenGLIndexBuffer::Bind()
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Handle);
			}
			void OpenGLIndexBuffer::Unbind()
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}

		}
	}
}