#pragma once
#include "Enigma/Renderer/VertexArray.h"

namespace Enigma {
	namespace Platform {
		namespace OpenGL {

			class OpenGLVertexArray : public Renderer::VertexArray {
			public:
				OpenGLVertexArray();
				~OpenGLVertexArray();

				virtual Renderer::VertexBuffer* GetVertexBuffer() override;
				virtual Renderer::IndexBuffer* GetIndexBuffer() override;

				virtual void AttachBuffer(Renderer::VertexBuffer* vertexBuffer) override;
				virtual void AttachBuffer(Renderer::IndexBuffer* indexBuffer) override;

				virtual void Bind() override;
				virtual void Unbind() override;

			private:
				uint32_t m_Handle;
				Renderer::VertexBuffer* m_VertexBuffer;
				Renderer::IndexBuffer* m_IndexBuffer;
			};

		}
	}
}