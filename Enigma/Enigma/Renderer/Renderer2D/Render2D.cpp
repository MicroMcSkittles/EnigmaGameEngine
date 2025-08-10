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

			// Load all shaders
			m_MainShader     = config.mainShader;
			m_PostProcShader = config.postProcShader;
			if (!config.mainShader)     m_MainShader     = LoadDefaultMainShader();
			if (!config.postProcShader) m_PostProcShader = LoadDefaultPostProcShader();

			m_CircleStencilShader     = LoadCircleStencilShader();
			m_LineCircleStencilShader = LoadLineCircleStencilShader();
			m_LineQuadStencilShader   = LoadLineQuadStencilShader();
			m_TextStencilShader       = LoadTextStencilShader();

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
			m_FrameBuffer->Resize(width, height);
			m_StencilBuffer->Resize(width, height);
			if (m_OutputBuffer != nullptr) m_OutputBuffer->Resize(width, height);
			if(m_CurrentCamera != nullptr) m_CurrentCamera->Resize(width, height);
		}
		void Render2D::StartFrame(OrthographicCamera* camera)
		{
			PROFILE();
			Core::Application::UseRenderAPI(m_RenderAPI);
			RenderAPI::Clear();

			m_CurrentCamera = camera;
			m_CurrentStencilID = 0;

			// Clear stencil buffer
			m_StencilBuffer->Bind();
			RenderAPI::Clear();
			m_StencilBuffer->Unbind();

			// Bind and clear frame buffer
			m_FrameBuffer->Bind();
			RenderAPI::Clear();
		}
		void Render2D::EndFrame()
		{
			PROFILE();
			m_MainShader->Bind();

			// Loop through all the draw calls
			for (auto& call : m_DrawCalls) {
				// Set uniforms
				call.texture->Bind();
				m_MainShader->SetUniform("TextureMap", (void*)call.texture);
				m_MainShader->SetUniform("Tint", (void*)&call.tint);
				m_MainShader->SetUniform("ViewProjection", (void*)&call.camera->GetViewProjection());
				m_MainShader->SetUniform("Model", (void*)&call.model);

				// If the draw call is using a stencil then set the stencil uniforms
				int stencilID = -1;
				if (call.useStencil) {
					m_StencilTexture->Bind();
					m_MainShader->SetUniform("StencilMap", (void*)m_StencilTexture);
					stencilID = call.stencilID;
				}
				m_MainShader->SetUniform("StencilID", (void*)&stencilID);

				// Draw the thing
				s_Quad->Bind();
				RenderAPI::DrawIndexed(6, DataType::UnsignedInt, NULL);
				s_Quad->Unbind();

				if (call.useStencil) m_StencilTexture->Unbind();
				call.texture->Unbind();
			}
			m_DrawCalls.clear();

			m_MainShader->Unbind();
			m_FrameBuffer->Unbind();

			// If the surface passed to the renderer has a frame then render to that frame with the output buffer
			if (m_OutputBuffer != nullptr) {
				m_OutputBuffer->Bind();
				RenderAPI::Clear();
			}

			// Run the frame through the post process shader
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

		void Render2D::DrawText(Text* text, const glm::vec2& position, float scale, float rotation, int depth, const glm::vec4& tint)
		{
			PROFILE();
			// Create stencil, the vao with glyph bounding box info uses a different transform matrix from the main quad
			glm::mat4 glyphTransform = glm::mat4(1.0f);
			glyphTransform = glm::translate(glyphTransform, { position, depth - 1.0f });
			glyphTransform = glm::rotate(glyphTransform, rotation, { 0.0f, 0.0f, 1.0f });
			glyphTransform = glm::scale(glyphTransform, { scale, scale, 1.0f });
			DrawTextStencil(text, glyphTransform);

			// Transform the text bounding box into world space
			glm::mat4 textTransform = glm::mat4(1.0f); 
			glm::vec3 textPosition  = glyphTransform * glm::vec4(text->GetBoundingBox().position, 0, 1);
			glm::vec3 textScale     = glyphTransform * glm::vec4(text->GetBoundingBox().size,     1, 1);
			textTransform           = glm::translate(textTransform, textPosition);
			textTransform           = glm::rotate(textTransform, rotation, { 0.0f, 0.0f, 1.0f });
			textTransform           = glm::scale(textTransform, textScale);
			// Push the draw call
			m_DrawCalls.push_back({ m_CurrentCamera, m_BlankTexture, textTransform, tint, true, m_CurrentStencilID - 1 });
		}
		void Render2D::DrawText(Text* text, const glm::vec2& position, float scale, float rotation, int depth, Texture* texture, const glm::vec4& tint)
		{
			PROFILE();
			// Create stencil, the vao with glyph bounding box info uses a different transform matrix from the main quad
			glm::mat4 glyphTransform = glm::mat4(1.0f);
			glyphTransform = glm::translate(glyphTransform, { position, depth - 1.0f });
			glyphTransform = glm::rotate(glyphTransform, rotation, { 0.0f, 0.0f, 1.0f });
			glyphTransform = glm::scale(glyphTransform, { scale, scale, 1.0f });
			DrawTextStencil(text, glyphTransform);

			// Transform the text bounding box into world space
			glm::mat4 textTransform = glm::mat4(1.0f);
			glm::vec3 textPosition = glyphTransform * glm::vec4(text->GetBoundingBox().position, 0, 1);
			glm::vec3 textScale = glyphTransform * glm::vec4(text->GetBoundingBox().size, 1, 1);
			textTransform = glm::translate(textTransform, textPosition);
			textTransform = glm::rotate(textTransform, rotation, { 0.0f, 0.0f, 1.0f });
			textTransform = glm::scale(textTransform, textScale);
			// Push the draw call
			m_DrawCalls.push_back({ m_CurrentCamera, texture, textTransform, tint, true, m_CurrentStencilID - 1 });
		}

		void Render2D::DrawStencil(Shader* stencilShader, const glm::mat4& transform)
		{
			PROFILE();
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
		void Render2D::DrawTextStencil(Text* text, const glm::mat4& transform)
		{
			PROFILE();
			// Unbind frame buffer so that we can render to the stencil buffer
			m_FrameBuffer->Unbind();

			// Bind every thing
			m_StencilBuffer->Bind();
			m_TextStencilShader->Bind();
			text->GetGlyphData()->Bind();

			// Set uniforms
			m_TextStencilShader->SetUniform("ViewProjection", (void*)&m_CurrentCamera->GetViewProjection());
			m_TextStencilShader->SetUniform("Model", (void*)&transform);
			m_TextStencilShader->SetUniform("StencilID", (void*)&m_CurrentStencilID);
			m_TextStencilShader->SetUniform("TextData", (void*)text->GetGlyphData());

			// Draw to stencil buffer
			text->GetGlyphBoundsVAO()->Bind();
			RenderAPI::DrawIndexed(
				text->GetGlyphBoundsVAO()->GetIndexBuffer()->GetIndexCount(), 
				text->GetGlyphBoundsVAO()->GetIndexBuffer()->GetIndexType(), 
				NULL
			);
			text->GetGlyphBoundsVAO()->Unbind();

			// unbind every thing
			text->GetGlyphData()->Unbind();
			m_TextStencilShader->Unbind();
			m_StencilBuffer->Unbind();

			// Rebind frame buffer
			m_FrameBuffer->Bind();
			m_CurrentStencilID += 1;
		}
	}
}