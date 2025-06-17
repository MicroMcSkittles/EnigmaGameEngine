#pragma once
#include "Renderer/Buffer.h"

namespace Enigma {
	namespace Platform {
		namespace OpenGL {

			class OpenGLVertexBuffer : public Renderer::VertexBuffer {
			public:
				OpenGLVertexBuffer(const std::vector<Renderer::DataType>& layout, Renderer::Usage usage);
				~OpenGLVertexBuffer();

				virtual void InitAttribs() override;

				virtual void SetData(void* vertices, int size) override;
				virtual void SetSubData(void* vertices, int size, int offset) override;

				virtual void Bind() override;
				virtual void Unbind() override;

			private:
				uint32_t m_Handle;
				int m_Usage;
				std::vector<Renderer::DataType> m_Layout;
			};

			class OpenGLIndexBuffer : public Renderer::IndexBuffer {
			public:
				OpenGLIndexBuffer(Renderer::DataType type, Renderer::Usage usage);
				~OpenGLIndexBuffer();

				virtual int GetIndexCount() override;
				virtual Renderer::DataType GetIndexType() override;

				virtual void SetData(void* indices, int size) override;
				virtual void SetSubData(void* indices, int size, int offset) override;

				virtual void Bind() override;
				virtual void Unbind() override;

			private:
				uint32_t m_Handle;
				Renderer::DataType m_Type;
				int m_IndicesCount;
				int m_Usage;
			};

		}
	}
}