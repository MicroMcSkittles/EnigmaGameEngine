#include "Enigma/Renderer/Renderer2D/Render2D.h"
#include "Enigma/Renderer/DefaultShaders.h"
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
			if (!config.mainShader)     m_MainShader     = DefaultShaders::LoadMainShader();
			if (!config.postProcShader) m_PostProcShader = DefaultShaders::LoadPostProcShader();

			m_CircleStencilShader     = LoadCircleStencilShader();
			m_LineCircleStencilShader = LoadLineCircleStencilShader();
			m_LineQuadStencilShader   = LoadLineQuadStencilShader();
			m_TextStencilShader       = LoadTextStencilShader();

			// Init frame buffers
			FrameBufferConfig frameBufferConfig;
			frameBufferConfig.width = config.surface.scale.x;
			frameBufferConfig.height = config.surface.scale.y;
			frameBufferConfig.attachments = { { AttachmentType::ColorAttachment } };
			m_FrameBuffer = FrameBuffer::Create(frameBufferConfig);

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
			blankTextureConfig.data = malloc(3);
			if (blankTextureConfig.data) memset(blankTextureConfig.data, 255, 3);
			m_BlankTexture = Texture::Create(blankTextureConfig);
			free(blankTextureConfig.data);

			// Create quad vertex array
			s_Quad = VertexArray::Create();
			s_Quad->Bind();

			ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(s_VertexLayout, Usage::StaticDraw);
			vertexBuffer->SetData(&s_QuadVertices[0], s_QuadVertices.size() * sizeof(s_QuadVertices[0]));
			vertexBuffer->InitAttribs();
			s_Quad->AttachBuffer(vertexBuffer);

			ref<IndexBuffer> indexBuffer = IndexBuffer::Create(DataType::UnsignedInt, Usage::StaticDraw);
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

		void Render2D::Resize(i32 width, i32 height)
		{
			RenderAPI::SetViewport(width, height);
			m_FrameBuffer->Resize(width, height);
			m_StencilBuffer->Resize(width, height);
			if (m_OutputBuffer != nullptr)  m_OutputBuffer->Resize(width, height);
			if (m_CurrentCamera != nullptr) m_CurrentCamera->Resize(width, height);
		}
		void Render2D::StartFrame(ref<OrthographicCamera> camera)
		{
			Core::Application::UseRenderAPI(m_RenderAPI);
			RenderAPI::Clear();

			// Set global scene values
			m_CurrentStencilID = 0;
			SetCamera(camera);
			
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
			// Switch to the stencil buffer
			m_FrameBuffer->Unbind();
			m_StencilBuffer->Bind();
			s_Quad->Bind();

			// Render all stencil data to the stencil buffer
			ref<OrthographicCamera> lastCamera;

			for (auto& [batchHash, batch] : m_Batches) {
				if (batch.stencilType == StencilType::None) continue;
				// Bind stencil shader
				batch.stencilShader->Bind();
				batch.stencilShader->SetUniform("ViewProjection", (void*)&batch.camera->GetViewProjection());
				
				// Loop through each stencil in the batch
				for (void* stencil : batch.stencils) {
					switch (batch.stencilType)
					{
					case StencilType::Circle:     DrawCircleStencil((CircleStencil*)stencil);         break;
					case StencilType::LineCircle: DrawLineCircleStencil((LineCircleStencil*)stencil); break;
					case StencilType::LineQuad:   DrawLineQuadStencil((LineQuadStencil*)stencil);     break;
					case StencilType::Text: {
						s_Quad->Unbind();
						DrawTextStencil((TextStencil*)stencil);
						s_Quad->Bind();
					} break;
					}
					delete stencil;
				}

				// Unbind stencil shader
				batch.stencilShader->Unbind();
				batch.stencils.clear();
			}

			// Switch back to the frame buffer
			m_StencilBuffer->Unbind();
			m_FrameBuffer->Bind();
			m_MainShader->Bind();

			m_StencilTexture->Bind();
			m_MainShader->SetUniform("StencilMap", (void*)&m_StencilTexture);

			// Render the scene
			lastCamera = nullptr;
			ref<Texture> lastTexture = nullptr;
			i32 i = 0;
			for (auto& [batchHash, batch] : m_Batches) {
				// Set Camera data if it is differant from the last batch
				if (batch.camera != lastCamera) {
					m_MainShader->SetUniform("ViewProjection", (void*)&batch.camera->GetViewProjection());
					lastCamera = batch.camera;
				}
				// Set texture data if it is differant from the last batch, and unbind last texture
				if (batch.texture != lastTexture) {
					if (lastTexture != nullptr) lastTexture->Unbind();
					batch.texture->Bind();
					m_MainShader->SetUniform("TextureMap", (void*)&batch.texture);
					lastTexture = batch.texture;
					i += 1;
				}

				// Loop through each draw call
				for (DrawCall& drawCall : batch.drawCalls) {
					m_MainShader->SetUniform("Model", (void*)&drawCall.transform);
					m_MainShader->SetUniform("Tint", (void*)&drawCall.tint);
					m_MainShader->SetUniform("StencilID", (void*)&drawCall.stencilID);

					RenderAPI::DrawIndexed(6, DataType::UnsignedInt, NULL);
				}

				batch.drawCalls.clear();
			}

			// Unbind everything
			if (lastTexture != nullptr) lastTexture->Unbind();
			s_Quad->Unbind();
			m_StencilTexture->Unbind();
			m_MainShader->Unbind();
			m_FrameBuffer->Unbind();

			// If the surface passed to the renderer has a frame then render to that frame with the output buffer
			if (m_OutputBuffer != nullptr) {
				m_OutputBuffer->Bind();
				RenderAPI::Clear();
			}

			// Run the frame through the post process shader
			m_PostProcShader->Bind();
			ref<Texture> frame = m_FrameBuffer->GetColorAttachment(0);
			//Texture* frame = m_StencilTexture;

			frame->Bind();
			m_PostProcShader->SetUniform("FrameTexture", (void*)&frame);

			s_Quad->Bind();
			RenderAPI::DrawIndexed(6, DataType::UnsignedInt, NULL);
			s_Quad->Unbind();

			frame->Unbind();
			m_PostProcShader->Unbind();

			if (m_OutputBuffer != nullptr) m_OutputBuffer->Unbind();
		}

		void Render2D::SetCamera(ref<OrthographicCamera> camera)
		{
			m_CurrentCamera = camera;

			// Find the cameras world space bounds, this will be used for culling
			const glm::mat4& invViewProjection = glm::inverse(m_CurrentCamera->GetViewProjection());
			const ViewBox& viewbox = m_CurrentCamera->GetZoomViewBox();

			// Find/Transform the min and max points of the camera view
			glm::vec2 min = invViewProjection * glm::vec4(-1.0f,  1.0f, 0.0f, 1.0f);
			glm::vec2 max = invViewProjection * glm::vec4( 1.0f, -1.0f, 0.0f, 1.0f);

			// Store the world space camera bounds
			m_CameraWorldBounds.left   = min.x;
			m_CameraWorldBounds.right  = max.x;
			m_CameraWorldBounds.bottom = min.y;
			m_CameraWorldBounds.top    = max.y;
			m_CameraWorldBounds.near   = viewbox.near;
			m_CameraWorldBounds.far    = viewbox.far;
		}

		void Render2D::DrawQuad(const glm::vec2& position, const glm::vec2& scale, f32 rotation, i32 depth, const glm::vec4& tint)
		{
			if (!OnScreen(position, scale)) return;
			
			glm::mat4 transform = glm::mat4(1.0f);
			transform = glm::translate(transform, { position, depth -1.0f });
			transform = glm::rotate(transform, rotation, { 0.0f, 0.0f, 1.0f });
			transform = glm::scale(transform, { scale, 1.0f });

			Submit(transform, tint, StencilType::None, nullptr, m_BlankTexture);
		}
		void Render2D::DrawQuad(const glm::vec2& position, const glm::vec2& scale, f32 rotation, i32 depth, ref<Texture> texture, const glm::vec4& tint)
		{
			if (!OnScreen(position, scale)) return;

			glm::mat4 transform = glm::mat4(1.0f);
			transform = glm::translate(transform, { position, depth - 1.0f });
			transform = glm::rotate(transform, rotation, { 0.0f, 0.0f, 1.0f });
			transform = glm::scale(transform, { scale, 1.0f });

			Submit(transform, tint, StencilType::None, nullptr, texture);
		}
		void Render2D::DrawLineQuad(const glm::vec2& position, const glm::vec2& scale, f32 rotation, f32 thickness, i32 depth, const glm::vec4& tint)
		{
			if (!OnScreen(position, scale)) return;

			glm::mat4 transform = glm::mat4(1.0f);
			transform = glm::translate(transform, { position, depth - 1.0f });
			transform = glm::rotate(transform, rotation, { 0.0f, 0.0f, 1.0f });
			transform = glm::scale(transform, { scale, 1.0f });

			Submit(transform, tint, StencilType::LineQuad, new LineQuadStencil({ m_CurrentStencilID, transform, scale, thickness }), m_BlankTexture);
		}
		void Render2D::DrawLineQuad(const glm::vec2& position, const glm::vec2& scale, f32 rotation, f32 thickness, i32 depth, ref<Texture> texture, const glm::vec4& tint)
		{
			if (!OnScreen(position, scale)) return;

			glm::mat4 transform = glm::mat4(1.0f);
			transform = glm::translate(transform, { position, depth - 1.0f });
			transform = glm::rotate(transform, rotation, { 0.0f, 0.0f, 1.0f });
			transform = glm::scale(transform, { scale, 1.0f });

			Submit(transform, tint, StencilType::LineQuad, new LineQuadStencil({ m_CurrentStencilID, transform, scale, thickness }), texture);
		}

		void Render2D::DrawCircle(const glm::vec2& position, f32 radius, i32 depth, const glm::vec4& tint)
		{
			if (!OnScreen(position, { radius, radius })) return;

			glm::mat4 transform = glm::mat4(1.0f);
			transform = glm::translate(transform, { position, depth - 1.0f });
			transform = glm::scale(transform, { radius, radius, 1.0f });

			Submit(transform, tint, StencilType::Circle, new CircleStencil({ m_CurrentStencilID, transform }), m_BlankTexture);
		}
		void Render2D::DrawCircle(const glm::vec2& position, f32 radius, i32 depth, ref<Texture> texture, const glm::vec4& tint)
		{
			if (!OnScreen(position, { radius, radius })) return;

			glm::mat4 transform = glm::mat4(1.0f);
			transform = glm::translate(transform, { position, depth - 1.0f });
			transform = glm::scale(transform, { radius, radius, 1.0f });

			Submit(transform, tint, StencilType::Circle, new CircleStencil({ m_CurrentStencilID, transform }), texture);
		}
		void Render2D::DrawLineCircle(const glm::vec2& position, f32 radius, f32 thickness, i32 depth, const glm::vec4& tint)
		{
			if (!OnScreen(position, { radius, radius })) return;

			glm::mat4 transform = glm::mat4(1.0f);
			transform = glm::translate(transform, { position, depth - 1.0f });
			transform = glm::scale(transform, { radius, radius, 1.0f });

			Submit(transform, tint, StencilType::LineCircle, new LineCircleStencil({ m_CurrentStencilID, transform, thickness }), m_BlankTexture);
		}
		void Render2D::DrawLineCircle(const glm::vec2& position, f32 radius, f32 thickness, i32 depth, ref<Texture> texture, const glm::vec4& tint)
		{
			if (!OnScreen(position, { radius, radius })) return;
			
			glm::mat4 transform = glm::mat4(1.0f);
			transform = glm::translate(transform, { position, depth - 1.0f });
			transform = glm::scale(transform, { radius, radius, 1.0f });

			Submit(transform, tint, StencilType::LineCircle, new LineCircleStencil({ m_CurrentStencilID, transform, thickness }), texture);
		}

		void Render2D::DrawText(Text* text, const glm::vec2& position, f32 scale, f32 rotation, i32 depth, const glm::vec4& tint)
		{
			// Create stencil, the vao with glyph bounding box info uses a different transform matrix from the main quad
			glm::mat4 glyphTransform = glm::mat4(1.0f);
			glyphTransform = glm::translate(glyphTransform, { position, depth - 1.0f });
			glyphTransform = glm::rotate(glyphTransform, rotation, { 0.0f, 0.0f, 1.0f });
			glyphTransform = glm::scale(glyphTransform, { scale, scale, 1.0f });

			// Transform the text bounding box into world space
			glm::mat4 textTransform = glm::mat4(1.0f); 
			glm::vec3 textPosition  = glyphTransform * glm::vec4(text->GetBoundingBox().position, 0, 1);
			glm::vec3 textScale     = glyphTransform * glm::vec4(text->GetBoundingBox().size,     1, 1);
			textTransform           = glm::translate(textTransform, textPosition);
			textTransform           = glm::rotate(textTransform, rotation, { 0.0f, 0.0f, 1.0f });
			textTransform           = glm::scale(textTransform, textScale);
			// Push the draw call
			Submit(textTransform, tint, StencilType::Text, new TextStencil({ m_CurrentStencilID, glyphTransform, text }), m_BlankTexture);
		}
		void Render2D::DrawText(Text* text, const glm::vec2& position, f32 scale, f32 rotation, i32 depth, ref<Texture> texture, const glm::vec4& tint)
		{
			// Create stencil, the vao with glyph bounding box info uses a different transform matrix from the main quad
			glm::mat4 glyphTransform = glm::mat4(1.0f);
			glyphTransform = glm::translate(glyphTransform, { position, depth - 1.0f });
			glyphTransform = glm::rotate(glyphTransform, rotation, { 0.0f, 0.0f, 1.0f });
			glyphTransform = glm::scale(glyphTransform, { scale, scale, 1.0f });

			// Transform the text bounding box into world space
			glm::mat4 textTransform = glm::mat4(1.0f);
			glm::vec3 textPosition = glyphTransform * glm::vec4(text->GetBoundingBox().position, 0, 1);
			glm::vec3 textScale = glyphTransform * glm::vec4(text->GetBoundingBox().size, 1, 1);
			textTransform = glm::translate(textTransform, textPosition);
			textTransform = glm::rotate(textTransform, rotation, { 0.0f, 0.0f, 1.0f });
			textTransform = glm::scale(textTransform, textScale);
			// Push the draw call
			Submit(textTransform, tint, StencilType::Text, new TextStencil({ m_CurrentStencilID, glyphTransform, text }), texture);
		}

		void Render2D::DrawLineQuadStencil(LineQuadStencil* stencil)
		{
			m_LineQuadStencilShader->SetUniform("Model",     (void*)&stencil->transform);
			m_LineQuadStencilShader->SetUniform("StencilID", (void*)&stencil->id);
			m_LineQuadStencilShader->SetUniform("Thickness", (void*)&stencil->thickness);
			m_LineQuadStencilShader->SetUniform("Bounds",    (void*)&stencil->bounds);
			RenderAPI::DrawIndexed(6, DataType::UnsignedInt, NULL);
		}
		void Render2D::DrawCircleStencil(CircleStencil* stencil)
		{
			m_CircleStencilShader->SetUniform("Model", (void*)&stencil->transform);
			m_CircleStencilShader->SetUniform("StencilID", (void*)&stencil->id);
			RenderAPI::DrawIndexed(6, DataType::UnsignedInt, NULL);
		}
		void Render2D::DrawLineCircleStencil(LineCircleStencil* stencil)
		{
			m_LineCircleStencilShader->SetUniform("Model", (void*)&stencil->transform);
			m_LineCircleStencilShader->SetUniform("StencilID", (void*)&stencil->id);
			m_LineCircleStencilShader->SetUniform("Thickness", (void*)&stencil->thickness);
			RenderAPI::DrawIndexed(6, DataType::UnsignedInt, NULL);
		}
		void Render2D::DrawTextStencil(TextStencil* stencil)
		{
			stencil->text->GetGlyphData()->Bind();
			m_TextStencilShader->SetUniform("Model", (void*)&stencil->transform);
			m_TextStencilShader->SetUniform("StencilID", (void*)&stencil->id);
			m_TextStencilShader->SetUniform("TextData", (void*)&stencil->text->GetGlyphData());

			ref<VertexArray> vao = stencil->text->GetGlyphBoundsVAO();
			vao->Bind();
			RenderAPI::DrawIndexed(vao->GetIndexBuffer()->GetIndexCount(), vao->GetIndexBuffer()->GetIndexType(), NULL);
			vao->Unbind();

			stencil->text->GetGlyphData()->Unbind();
		}

		void Render2D::Submit(const glm::mat4& transform, const glm::vec4& tint, StencilType stencilType, void* stencil, ref<Texture> texture)
		{
			uint64_t batchHash = Batch::Hash({ m_CurrentCamera, texture, stencilType });

			// If this batch hasn't been seen before then register it
			if (!m_Batches.count(batchHash)) {
				m_Batches.insert({ batchHash, { m_CurrentCamera, texture, stencilType } });
				switch (stencilType)
				{
				case StencilType::Circle:     m_Batches[batchHash].stencilShader = m_CircleStencilShader;     break;
				case StencilType::LineCircle: m_Batches[batchHash].stencilShader = m_LineCircleStencilShader; break;
				case StencilType::LineQuad:   m_Batches[batchHash].stencilShader = m_LineQuadStencilShader;   break;
				case StencilType::Text:       m_Batches[batchHash].stencilShader = m_TextStencilShader;       break;
				}
			}

			// push draw call
			DrawCall drawCall;
			drawCall.transform = transform;
			drawCall.tint      = tint;
			if (stencilType != StencilType::None) {
				drawCall.stencilID = m_CurrentStencilID;
				m_Batches[batchHash].stencils.push_back(stencil);
				m_CurrentStencilID += 1;
			}
			else {
				drawCall.stencilID = -1;
			}
			m_Batches[batchHash].drawCalls.push_back(drawCall);
		}

		bool Render2D::OnScreen(const glm::vec2& position, const glm::vec2& scale)
		{
			glm::vec2 min = position - scale;
			glm::vec2 max = position + scale;
			return (
				(max.x >= m_CameraWorldBounds.left && m_CameraWorldBounds.right  >= min.x) &&
				(max.y >= m_CameraWorldBounds.top  && m_CameraWorldBounds.bottom >= min.y)
			);
		}

		uint64_t Render2D::Batch::Hash(const Batch& batch)
		{
			// No clue how well this will work
			u64 p1 = reinterpret_cast<u64>(batch.camera.get());
			u64 p2 = reinterpret_cast<u64>(batch.texture.get());
			u8  t  = static_cast<u8>(batch.stencilType);
			p1 = p1 << (3 + t);
			p2 = p2 >> (4 - t);
			u64 rslt = p1 & p2;
			return rslt;
		}
	}
}