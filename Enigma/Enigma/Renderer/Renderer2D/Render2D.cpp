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

			Core::Application::UseRenderAPI(m_RenderAPI);

			if (!config.mainShader) m_MainShader = LoadDefaultMainShader();
			else m_MainShader = config.mainShader;
			if (!config.postProcShader) m_PostProcShader = LoadDefaultPostProcShader();
			else m_PostProcShader = config.postProcShader;

			m_CircleStencilShader = LoadCircleStencilShader();
			m_LineCircleStencilShader = LoadLineCircleStencilShader();
			m_LineQuadStencilShader = LoadLineQuadStencilShader();

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

			// Create Stencil Buffer and Texture
			TextureConfig stencilTextureConfig;
			stencilTextureConfig.width = config.surface.scale.x;
			stencilTextureConfig.height = config.surface.scale.y;
			stencilTextureConfig.minFilter = TexFilterMode::Nearest;
			stencilTextureConfig.magFilter = TexFilterMode::Nearest;
			stencilTextureConfig.dataType = DataType::UnsignedInt;
			stencilTextureConfig.format = TexFormat::RED_INT;
			stencilTextureConfig.internalFormat = TexFormat::RED32UI;
			m_StencilTexture = Texture::Create(stencilTextureConfig);
			frameBufferConfig.attachments = { { AttachmentType::ColorAttachment, m_StencilTexture } };
			m_StencilBuffer = FrameBuffer::Create(frameBufferConfig);

			// Create blank texture
			TextureConfig blankTextureConfig;
			blankTextureConfig.width = 1;
			blankTextureConfig.height = 1;
			blankTextureConfig.data = (uint8_t*)malloc(3);
			if (blankTextureConfig.data) memset(blankTextureConfig.data, 255, 3);
			m_BlankTexture = Texture::Create(blankTextureConfig);
			free(blankTextureConfig.data);

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
			m_StencilBuffer->Resize(width, height);
			if (m_OutputBuffer != nullptr) m_OutputBuffer->Resize(width, height);
		}
		void Render2D::StartFrame(OrthographicCamera* camera)
		{
			m_CurrentCamera = camera;
			m_CurrentStencilID = 0;

			Core::Application::UseRenderAPI(m_RenderAPI);

			RenderAPI::Clear();

			m_StencilBuffer->Bind();
			RenderAPI::Clear();
			m_StencilBuffer->Unbind();

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

				int stencilID = -1;
				if (call.useStencil) {
					m_StencilTexture->Bind();
					m_MainShader->SetUniform("StencilMap", (void*)m_StencilTexture);
					stencilID = call.stencilID;
				}
				m_MainShader->SetUniform("StencilID", (void*)&stencilID);

				s_Quad->Bind();
				RenderAPI::DrawIndexed(6, DataType::UnsignedInt, NULL);
				s_Quad->Unbind();

				if (call.useStencil) m_StencilTexture->Unbind();
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
			//Texture* frame = m_StencilTexture;

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

			m_DrawCalls.push_back({ m_CurrentCamera, m_BlankTexture, transform, tint, false });
		}
		void Render2D::DrawQuad(const glm::vec2& position, const glm::vec2& scale, float rotation, int depth, Texture* texture, const glm::vec4& tint)
		{
			glm::mat4 transform = glm::mat4(1.0f);
			transform = glm::translate(transform, { position, depth - 1.0f });
			transform = glm::rotate(transform, rotation, { 0.0f, 0.0f, 1.0f });
			transform = glm::scale(transform, { scale, 1.0f });

			m_DrawCalls.push_back({ m_CurrentCamera, texture, transform, tint, false });
		}
		void Render2D::DrawLineQuad(const glm::vec2& position, const glm::vec2& scale, float rotation, float thickness, int depth, const glm::vec4& tint)
		{
			glm::mat4 transform = glm::mat4(1.0f);
			transform = glm::translate(transform, { position, depth - 1.0f });
			transform = glm::rotate(transform, rotation, { 0.0f, 0.0f, 1.0f });
			transform = glm::scale(transform, { scale, 1.0f });

			glm::vec2 bounds;
			bounds.x = (scale.x / scale.y) / 2;
			bounds.y = 0.5;
			m_LineQuadStencilShader->SetUniform("Thickness", (void*)&thickness);
			m_LineQuadStencilShader->SetUniform("Bounds", (void*)&scale);
			DrawStencil(m_LineQuadStencilShader, transform);

			m_DrawCalls.push_back({ m_CurrentCamera, m_BlankTexture, transform, tint, true, m_CurrentStencilID - 1 });
		}
		void Render2D::DrawLineQuad(const glm::vec2& position, const glm::vec2& scale, float rotation, float thickness, int depth, Texture* texture, const glm::vec4& tint)
		{
			glm::mat4 transform = glm::mat4(1.0f);
			transform = glm::translate(transform, { position, depth - 1.0f });
			transform = glm::rotate(transform, rotation, { 0.0f, 0.0f, 1.0f });
			transform = glm::scale(transform, { scale, 1.0f });

			glm::vec2 bounds = scale;
			m_LineQuadStencilShader->SetUniform("Thickness", (void*)&thickness);
			m_LineQuadStencilShader->SetUniform("Bounds", (void*)&bounds);
			DrawStencil(m_LineQuadStencilShader, transform);

			m_DrawCalls.push_back({ m_CurrentCamera, texture, transform, tint, true, m_CurrentStencilID - 1 });
		}

		void Render2D::DrawCircle(const glm::vec2& position, float radius, int depth, const glm::vec4& tint)
		{
			glm::mat4 transform = glm::mat4(1.0f);
			transform = glm::translate(transform, { position, depth - 1.0f });
			transform = glm::scale(transform, { radius, radius, 1.0f });

			DrawStencil(m_CircleStencilShader, transform);

			m_DrawCalls.push_back({ m_CurrentCamera, m_BlankTexture, transform, tint, true, m_CurrentStencilID - 1 });
		}
		void Render2D::DrawCircle(const glm::vec2& position, float radius, int depth, Texture* texture, const glm::vec4& tint)
		{
			glm::mat4 transform = glm::mat4(1.0f);
			transform = glm::translate(transform, { position, depth - 1.0f });
			transform = glm::scale(transform, { radius, radius, 1.0f });

			DrawStencil(m_CircleStencilShader, transform);
			
			m_DrawCalls.push_back({ m_CurrentCamera, texture, transform, tint, true, m_CurrentStencilID - 1 });
		}
		void Render2D::DrawLineCircle(const glm::vec2& position, float radius, float thickness, int depth, const glm::vec4& tint)
		{
			glm::mat4 transform = glm::mat4(1.0f);
			transform = glm::translate(transform, { position, depth - 1.0f });
			transform = glm::scale(transform, { radius, radius, 1.0f });

			m_LineCircleStencilShader->SetUniform("Thickness", (void*)&thickness);
			DrawStencil(m_LineCircleStencilShader, transform);

			m_DrawCalls.push_back({ m_CurrentCamera, m_BlankTexture, transform, tint, true, m_CurrentStencilID - 1 });
		}
		void Render2D::DrawLineCircle(const glm::vec2& position, float radius, float thickness, int depth, Texture* texture, const glm::vec4& tint)
		{
			glm::mat4 transform = glm::mat4(1.0f);
			transform = glm::translate(transform, { position, depth - 1.0f });
			transform = glm::scale(transform, { radius, radius, 1.0f });
			
			m_LineCircleStencilShader->SetUniform("Thickness", (void*)&thickness);
			DrawStencil(m_LineCircleStencilShader, transform);

			m_DrawCalls.push_back({ m_CurrentCamera, texture, transform, tint, true, m_CurrentStencilID - 1 });
		}

		void Render2D::DrawStencil(Shader* stencilShader, const glm::mat4& transform)
		{
			m_FrameBuffer->Unbind();
			m_StencilBuffer->Bind();
			stencilShader->Bind();
			stencilShader->SetUniform("ViewProjection", (void*)&m_CurrentCamera->GetViewProjection());
			stencilShader->SetUniform("Model", (void*)&transform);
			stencilShader->SetUniform("StencilID", (void*)&m_CurrentStencilID);

			s_Quad->Bind();
			RenderAPI::DrawIndexed(6, DataType::UnsignedInt, NULL);
			s_Quad->Unbind();

			stencilShader->Unbind();
			m_StencilBuffer->Unbind();
			m_FrameBuffer->Bind();

			m_CurrentStencilID += 1;
		}
	}
}