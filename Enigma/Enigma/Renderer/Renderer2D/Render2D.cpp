#include "Enigma/Renderer/Renderer2D/Render2D.h"
#include "Enigma/Core/Process/Application.h"

#include <glm/gtc/matrix_transform.hpp>
#include <cstring>

namespace Enigma {
	namespace Renderer {
		Render2D::Render2D(const Render2DConfig& config)
		{
			m_RenderAPI = config.renderAPI;
			m_Surface = config.surface;

			if (!config.mainShader) m_MainShader = LoadDefaultMainShader();
			else m_MainShader = config.mainShader;
			if (!config.postProcShader) m_PostProcShader = LoadDefaultPostProcShader();
			else m_PostProcShader = config.postProcShader;

			m_CurrentCamera = nullptr;

			// Init frame buffers
			FrameBufferConfig frameBufferConfig;
			frameBufferConfig.width = config.surface.scale.x;
			frameBufferConfig.height = config.surface.scale.y;
			frameBufferConfig.attachments = { { AttachmentType::ColorAttachment } };
			m_FrameBuffer = FrameBuffer::Create(frameBufferConfig);

			m_OutputBuffer = nullptr;
			if (config.surface.frame != nullptr) {
				frameBufferConfig.attachments = { { AttachmentType::ColorAttachment, config.surface.frame } };
				m_OutputBuffer = FrameBuffer::Create(frameBufferConfig);
			}

			// Create blank texture
			TextureConfig textureConfig;
			textureConfig.width = 1;
			textureConfig.height = 1;
			textureConfig.data = (uint8_t*)malloc(3);
			if (textureConfig.data) memset(textureConfig.data, 255, 3);
			m_BlankTexture = Texture::Create(textureConfig);
			free(textureConfig.data);
		
			// Create quad vertex array
			s_Quad = VertexArray::Create();
			s_Quad->Bind();

			VertexBuffer* vertexBuffer = VertexBuffer::Create(s_VertexLayout, Usage::StaticDraw);
			vertexBuffer->SetData(&s_QuadVertices[0], s_QuadVertices.size() * sizeof(s_QuadVertices[0]));
			vertexBuffer->InitAttribs();
			s_Quad->AttachBuffer(vertexBuffer);

			IndexBuffer* indexBuffer = IndexBuffer::Create(DataType::UnsignedInt, Usage::StaticDraw);
			indexBuffer->SetData(&s_QuadIndices[0], s_QuadIndices.size() * sizeof(s_QuadIndices[0]));
			s_Quad->AttachBuffer(indexBuffer);

			s_Quad->Unbind();

			// Config render api
			Core::Application::UseRenderAPI(m_RenderAPI);
			RenderAPI::SetClearColor({ 0,0,0,1 });
			RenderAPI::SetClearMask(ColorBufferBit | DepthBufferBit);
			RenderAPI::SetDrawMode(DrawMode::Triangles);
			RenderAPI::SetViewport(config.surface.scale.x, config.surface.scale.y);
		}

		void Render2D::Resize(int width, int height)
		{
			RenderAPI::SetViewport(width, height);
			if(m_CurrentCamera != nullptr)m_CurrentCamera->Resize(width, height);
			m_FrameBuffer->Resize(width, height);
			if (m_OutputBuffer != nullptr) m_OutputBuffer->Resize(width, height);
		}
		void Render2D::StartFrame(OrthographicCamera* camera)
		{
			m_CurrentCamera = camera;

			Core::Application::UseRenderAPI(m_RenderAPI);

			RenderAPI::Clear();

			m_FrameBuffer->Bind();

			RenderAPI::Clear();
		}
		void Render2D::EndFrame()
		{
			m_MainShader->Bind();

			for (auto& call : m_DrawCalls) {
				call.texture->Bind();
				m_MainShader->SetUniform("TextureMap", (void*)call.texture);
				m_MainShader->SetUniform("Tint", (void*)&call.tint);
				m_MainShader->SetUniform("ViewProjection", (void*)&call.camera->GetViewProjection());
				m_MainShader->SetUniform("Model", (void*)&call.model);

				s_Quad->Bind();
				RenderAPI::DrawIndexed(6, DataType::UnsignedInt, NULL);
				s_Quad->Unbind();

				call.texture->Unbind();
			}
			m_DrawCalls.clear();

			m_MainShader->Unbind();

			m_FrameBuffer->Unbind();

			if (m_OutputBuffer != nullptr) {
				m_OutputBuffer->Bind();
				RenderAPI::Clear();
			}

			m_PostProcShader->Bind();
			Texture* frame = m_FrameBuffer->GetColorAttachment(0);
			frame->Bind();
			m_PostProcShader->SetUniform("FrameTexture", (void*)frame);

			s_Quad->Bind();
			RenderAPI::DrawIndexed(6, DataType::UnsignedInt, NULL);
			s_Quad->Unbind();

			frame->Unbind();
			m_PostProcShader->Unbind();

			if (m_OutputBuffer != nullptr) m_OutputBuffer->Unbind();
		}

		void Render2D::DrawQuad(const glm::vec2& position, const glm::vec2& scale, float rotation, int depth, const glm::vec4& tint)
		{
			glm::mat4 transform = glm::mat4(1.0f);
			transform = glm::translate(transform, { position, depth -1.0f });
			transform = glm::rotate(transform, rotation, { 0.0f, 0.0f, 1.0f });
			transform = glm::scale(transform, { scale, 1.0f });

			m_DrawCalls.push_back({ m_CurrentCamera, m_BlankTexture, transform, tint });
		}
		void Render2D::DrawQuad(const glm::vec2& position, const glm::vec2& scale, float rotation, int depth, Texture* texture, const glm::vec4& tint)
		{
			glm::mat4 transform = glm::mat4(1.0f);
			transform = glm::translate(transform, { position, depth - 1.0f });
			transform = glm::rotate(transform, rotation, { 0.0f, 0.0f, 1.0f });
			transform = glm::scale(transform, { scale, 1.0f });

			m_DrawCalls.push_back({ m_CurrentCamera, texture, transform, tint });
		}
	}
}