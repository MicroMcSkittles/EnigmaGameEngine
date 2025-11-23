#pragma once
#include "Enigma/Renderer/VertexArray.h"

namespace Enigma::Platform::OpenGL {

	class OpenGLVertexArray : public Renderer::VertexArray {
	public:
		OpenGLVertexArray();
		~OpenGLVertexArray();

		virtual ref<Renderer::VertexBuffer> GetVertexBuffer() override;
		virtual ref<Renderer::IndexBuffer> GetIndexBuffer() override;

		virtual void AttachBuffer(ref<Renderer::VertexBuffer> vertexBuffer) override;
		virtual void AttachBuffer(ref<Renderer::IndexBuffer> indexBuffer) override;

		virtual void Bind() override;
		virtual void Unbind() override;

	private:
		u32 m_Handle;
		ref<Renderer::VertexBuffer> m_VertexBuffer;
		ref<Renderer::IndexBuffer> m_IndexBuffer;
	};

}