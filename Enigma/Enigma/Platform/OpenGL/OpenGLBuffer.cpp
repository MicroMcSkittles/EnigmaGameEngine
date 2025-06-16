#include "Platform/OpenGL/OpenGLBuffer.h"
#include "Platform/OpenGL/OpenGLRenderAPI.h"

#include <glad/glad.h>

namespace Enigma {
	namespace Platform {
		namespace OpenGL {
			OpenGLVertexBuffer::OpenGLVertexBuffer(const std::vector<Renderer::DataType>& layout, Renderer::Usage usage)
			{
				m_Layout = layout;
				m_Vertices = (void*)NULL;
				m_VerticesSize = 0;
				m_Usage = Conversions::Usage(usage);
				glGenBuffers(1, &m_Handle);
			}
			OpenGLVertexBuffer::~OpenGLVertexBuffer()
			{
				m_Vertices = nullptr;
				glDeleteBuffers(1, &m_Handle);
			}
			void OpenGLVertexBuffer::SetData(void* vertices, int size)
			{
				m_Vertices = vertices;
				m_VerticesSize = size;
			}
			void OpenGLVertexBuffer::Initialize()
			{
				glBindBuffer(GL_ARRAY_BUFFER, m_Handle);
				glBufferData(GL_ARRAY_BUFFER, m_VerticesSize, m_Vertices, m_Usage);

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
				m_Indices = (void*)NULL;
				m_IndicesSize = 0;
				m_Usage = Conversions::Usage(usage);
				m_Type = type;
				glGenBuffers(1, &m_Handle);
			}
			OpenGLIndexBuffer::~OpenGLIndexBuffer()
			{
				m_Indices = nullptr;
				glDeleteBuffers(1, &m_Handle);
			}
			int OpenGLIndexBuffer::GetIndexCount()
			{
				return m_IndicesSize / Conversions::DataTypeSize(m_Type);
			}
			Renderer::DataType OpenGLIndexBuffer::GetIndexType()
			{
				return m_Type;
			}
			void OpenGLIndexBuffer::SetData(unsigned int* indices, int size)
			{
				m_Indices = (void*)indices;
				m_IndicesSize = size;
			}
			void OpenGLIndexBuffer::Initialize()
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Handle);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndicesSize, m_Indices, m_Usage);
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