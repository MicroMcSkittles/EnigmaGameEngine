#pragma once
#include "Enigma/Renderer/Buffer.h"

namespace Enigma {
	namespace Platform {
		namespace OpenGL {

			class OpenGLVertexBuffer : public Renderer::VertexBuffer {
			public:
				OpenGLVertexBuffer(const std::vector<Renderer::DataType>& layout, Renderer::Usage usage);
				~OpenGLVertexBuffer();

				virtual void InitAttribs() override;

				virtual void SetData(void* vertices, i32 size) override;
				virtual void SetSubData(void* vertices, i32 size, i32 offset) override;

				virtual void Bind() override;
				virtual void Unbind() override;

			private:
				u32 m_Handle;
				i32 m_Usage;
				std::vector<Renderer::DataType> m_Layout;
			};

			class OpenGLIndexBuffer : public Renderer::IndexBuffer {
			public:
				OpenGLIndexBuffer(Renderer::DataType type, Renderer::Usage usage);
				~OpenGLIndexBuffer();

				virtual i32 GetIndexCount() override;
				virtual Renderer::DataType GetIndexType() override;

				virtual void SetData(void* indices, i32 size) override;
				virtual void SetSubData(void* indices, i32 size, i32 offset) override;

				virtual void Bind() override;
				virtual void Unbind() override;

			private:
				u32 m_Handle;
				Renderer::DataType m_Type;
				i32 m_IndicesCount;
				i32 m_Usage;
			};
			
			class OpenGLFrameBuffer : public Renderer::FrameBuffer {
			public:
				OpenGLFrameBuffer(const Renderer::FrameBufferConfig& config);
				~OpenGLFrameBuffer();

				virtual void Resize(i32 width, i32 height) override;

				virtual void Bind() override;
				virtual void Unbind() override;

				virtual std::vector<ref<Renderer::Texture>> GetAttachments() override { return m_Attachments; }
				virtual ref<Renderer::Texture> GetColorAttachment(i32 index) override;
				virtual ref<Renderer::Texture> SeverColorAttachment(i32 index) override;
				virtual ref<Renderer::Texture> GetDepthAttachment() override;

			private:
				Renderer::FrameBufferConfig m_Config;

				u32 m_Handle;
				u32 m_RBO;
				
				u32 m_Width;
				u32 m_Height;
				
				u32 m_ColorAttachmentCount;
				u32 m_DepthAttachmentID;
				u32* m_Buffers;
				std::vector<ref<Renderer::Texture>> m_Attachments;
			};
		}
	}
}