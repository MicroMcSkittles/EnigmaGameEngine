#include "Enigma/EnigmaPCH.h"
#include "Enigma/Platform/OpenGL/OpenGLBuffer.h"
#include "Enigma/Platform/OpenGL/OpenGLRenderAPI.h"
#include "Enigma/Platform/OpenGL/OpenGLTexture.h"

#include "Enigma/Renderer/RenderAPI.h"

#include <glad/glad.h>

namespace Enigma::Platform::OpenGL {

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
	void OpenGLVertexBuffer::SetData(void* vertices, i32 size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_Handle);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, m_Usage);
	}
	void OpenGLVertexBuffer::SetSubData(void* vertices, i32 size, i32 offset)
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
	void OpenGLIndexBuffer::SetData(void* indices, i32 size)
	{
		m_IndicesCount = size / Conversions::DataTypeSize(m_Type);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Handle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, m_Usage);
	}
	void OpenGLIndexBuffer::SetSubData(void* indices, i32 size, i32 offset)
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


	OpenGLFrameBuffer::OpenGLFrameBuffer(const Renderer::FrameBufferConfig& config)
	{
		m_Config = config;
		m_Width = m_Config.width;
		m_Height = m_Config.height;

		m_ColorAttachmentCount = 0;
		m_DepthAttachmentID = 0;

		// used to make see if we need to make a RBO
		bool depthFlag = false;

		glGenFramebuffers(1, &m_Handle);
		glBindFramebuffer(GL_FRAMEBUFFER, m_Handle);

		for (u64 i = 0; i < m_Config.attachments.size(); ++i) {
			auto& attachment = m_Config.attachments[i];

			Renderer::TextureConfig textureConfig;
			textureConfig.data = (uint8_t*)NULL;
			textureConfig.width = m_Width;
			textureConfig.height = m_Height;
			textureConfig.format = attachment.format;
			textureConfig.internalFormat = attachment.internalFormat;
			textureConfig.dataType = attachment.dataType;

			if (attachment.type == Renderer::AttachmentType::DepthAttachment) {
				depthFlag = true;

				textureConfig.dataType = Renderer::DataType::UnsignedInt_24_8;
				ref<Renderer::Texture> depthTexture;
				if (attachment.output != nullptr) depthTexture = attachment.output;
				else depthTexture = Renderer::Texture::Create(textureConfig);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, *(uint32_t*)depthTexture->GetNativeTexture(), 0);
				m_DepthAttachmentID = i;
				m_Attachments.push_back(depthTexture);
				break;
			}

			if (attachment.type == Renderer::AttachmentType::ColorAttachment) {
				ref<Renderer::Texture> colorTexture;
				if (attachment.output != nullptr) colorTexture = attachment.output;
				else colorTexture = Renderer::Texture::Create(textureConfig);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_ColorAttachmentCount, GL_TEXTURE_2D, *(uint32_t*)colorTexture->GetNativeTexture(), 0);
				m_Attachments.push_back(colorTexture);
				m_ColorAttachmentCount += 1;
			}
		}
		m_Buffers = new u32[m_ColorAttachmentCount];
		for (u32 i = 0; i < m_ColorAttachmentCount; ++i) {
			m_Buffers[i] = GL_COLOR_ATTACHMENT0 + i;
		}

		if (!depthFlag) {
			glGenRenderbuffers(1, &m_RBO);
			glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);

			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);
		}

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			LOG_SOFT_ERROR("Failed to create Frame Buffer");
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1, &m_Handle);
	}
	void OpenGLFrameBuffer::Resize(i32 width, i32 height)
	{
		m_Width = width;
		m_Height = height;

		glBindFramebuffer(GL_FRAMEBUFFER, m_Handle);

		Renderer::RenderAPI::SetViewport(width, height);

		for (auto t : m_Attachments) {
			t->Resize(width, height);
		}

		if (m_DepthAttachmentID == 0) {
			glDeleteRenderbuffers(1, &m_RBO);

			glGenRenderbuffers(1, &m_RBO);
			glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void OpenGLFrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_Handle);
		glDrawBuffers(m_Attachments.size(), m_Buffers);
	}
	void OpenGLFrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		GLenum bufs[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, bufs);
	}
	ref<Renderer::Texture> OpenGLFrameBuffer::GetColorAttachment(i32 index)
	{
		if (index >= m_ColorAttachmentCount) {
			LOG_WARNING("Color attachment out of bounds");
			return nullptr;
		}
		return m_Attachments[index];
	}
	ref<Renderer::Texture> OpenGLFrameBuffer::SeverColorAttachment(i32 index)
	{
		if (index >= m_ColorAttachmentCount) {
			LOG_WARNING("Color attachment out of bounds");
			return nullptr;
		}
		ref<Renderer::Texture> out = m_Attachments[index];

		auto& attachment = m_Config.attachments[index];

		Renderer::TextureConfig textureConfig;
		textureConfig.data = (uint8_t*)NULL;
		textureConfig.width = m_Width;
		textureConfig.height = m_Height;
		textureConfig.format = attachment.format;
		textureConfig.internalFormat = attachment.internalFormat;
		textureConfig.dataType = attachment.dataType;

		m_Attachments[index] = Renderer::Texture::Create(textureConfig);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_ColorAttachmentCount, GL_TEXTURE_2D, *(uint32_t*)m_Attachments[index]->GetNativeTexture(), 0);

		return out;
	}
	ref<Renderer::Texture> OpenGLFrameBuffer::GetDepthAttachment()
	{
		if (m_DepthAttachmentID == 0) {
			LOG_WARNING("Frame buffer doesn't have a depth buffer");
			return nullptr;
		}
		return m_Attachments[m_DepthAttachmentID];
	}

	void OpenGLFrameBuffer::GetPixel(i32 x, i32 y, i32 attachment, void* data)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_Handle);

		Renderer::TextureConfig attachmentConfig = m_Attachments[attachment]->GetConfig();
		u32 format = Conversions::TexFormat(attachmentConfig.format);
		u32 dataType = Conversions::DataType(attachmentConfig.dataType);
		
		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment);
		glReadPixels(x, y, 1, 1, format, dataType, data);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}