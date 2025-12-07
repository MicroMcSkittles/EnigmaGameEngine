#pragma once
#include "Enigma/Renderer/Buffer.h"

namespace Enigma::Platform::OpenGL {

	class OpenGLVertexBuffer : public Renderer::VertexBuffer {
	public:
		OpenGLVertexBuffer(const std::vector<Renderer::DataType>& layout, Renderer::Usage usage);
		~OpenGLVertexBuffer();

		virtual void InitAttribs() override;

		virtual void SetData(void* vertices, u64 size) override;
		virtual void SetSubData(void* vertices, u64 size, u32 offset) override;

		virtual void Bind() override;
		virtual void Unbind() override;

	private:
		u32 m_Handle;
		u32 m_Usage;
		std::vector<Renderer::DataType> m_Layout;
	};

	class OpenGLIndexBuffer : public Renderer::IndexBuffer {
	public:
		OpenGLIndexBuffer(Renderer::DataType type, Renderer::Usage usage);
		~OpenGLIndexBuffer();

		virtual u32 GetIndexCount() override;
		virtual Renderer::DataType GetIndexType() override;

		virtual void SetData(void* indices, u64 size) override;
		virtual void SetSubData(void* indices, u64 size, u32 offset) override;

		virtual void Bind() override;
		virtual void Unbind() override;

	private:
		u32 m_Handle;
		u32 m_IndicesCount;
		u32 m_Usage;
		Renderer::DataType m_Type;
	};
	
	class OpenGLUniformBuffer : public Renderer::UniformBuffer {
	public:
		OpenGLUniformBuffer(u32 size, u32 binding, Renderer::Usage usage);
		~OpenGLUniformBuffer();

		virtual void SetData(void* data, u32 size, u32 offset) override;

		virtual void Bind() override;
		virtual void Unbind() override;

	private:
		u32 m_Handle;
		u32 m_Binding;
		u32 m_Usage;
		u32 m_Size;
	};

	class OpenGLFrameBuffer : public Renderer::FrameBuffer {
	public:
		OpenGLFrameBuffer(const Renderer::FrameBufferConfig& config);
		~OpenGLFrameBuffer();

		virtual void Resize(u32 width, u32 height) override;

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual std::vector<ref<Renderer::Texture>> GetAttachments() override { return m_Attachments; }
		virtual ref<Renderer::Texture> GetColorAttachment(u32 index) override;
		virtual ref<Renderer::Texture> SeverColorAttachment(u32 index) override;
		virtual ref<Renderer::Texture> GetDepthAttachment() override;

		virtual void GetPixel(u32 x, u32 y, u32 attachment, void* data) override;

	private:
		Renderer::FrameBufferConfig m_Config;

		u32 m_Handle;
		u32 m_RBO;
		
		u32 m_Width;
		u32 m_Height;
		
		i32 m_AttachmentCount;
		u32 m_ColorAttachmentCount;
		u32 m_DepthAttachmentID;
		u32* m_Buffers;
		std::vector<ref<Renderer::Texture>> m_Attachments;
	};
}