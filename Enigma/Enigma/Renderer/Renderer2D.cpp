#include "Renderer/Renderer2D.h"
#include "Renderer/RenderAPI.h"

#include <glm/gtc/matrix_transform.hpp>
#include <cstring>

namespace Enigma {
	namespace Renderer {
		void Render2D::Init(const Render2DConfig& config)
		{
			if (s_Data) {
				LOG_WARNING("Renderer2D already initalized");
			}
			s_Data = new Data;

			ShaderConfig mainShaderConfig;
			mainShaderConfig.vertexPath = config.mainShaderPath + ".vert";
			mainShaderConfig.pixelPath = config.mainShaderPath + ".frag";
			s_Data->mainShader = Shader::Create(mainShaderConfig);

			TextureConfig textureConfig;
			textureConfig.width = 1;
			textureConfig.height = 1;
			textureConfig.data = malloc(3);
			if(textureConfig.data) std::memset(textureConfig.data, 255, 3);
			s_Data->emptyTexture = Texture::Create(textureConfig);

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

			RenderAPI::SetClearColor({0,0,0,1});
			RenderAPI::SetClearMask(ColorBufferBit | DepthBufferBit);
			RenderAPI::SetDrawMode(DrawMode::Triangles);
		}

		void Render2D::Resize(int width, int height)
		{
			RenderAPI::SetViewport(width, height);
			s_Data->currentCamera->Resize(width, height);
		}

		void Render2D::StartFrame(Camera* camera)
		{
			s_Data->currentCamera = camera;
		}
		void Render2D::EndFrame()
		{
			RenderAPI::Clear();

			s_Data->mainShader->Bind();
			for (auto it = s_Data->drawCallLookup.begin(); it != s_Data->drawCallLookup.end(); ++it) {
				int location = (*it).second;
				auto& calls = s_Data->drawCalls[location];
				if (calls.size() == 0) {
					s_Data->drawCallLookup.erase(it);
					it--;
					continue;
				}
				s_Data->mainShader->SetUniform("ViewProjection", (void*)&calls[0].camera->GetViewProjection());
				calls[0].texture->Bind();
				s_Data->mainShader->SetUniform("TextureMap", (void*)calls[0].texture);

				for (auto& call : calls) {
					s_Data->mainShader->SetUniform("Model", (void*)&call.model);
					s_Data->mainShader->SetUniform("Tint", (void*)&call.tint);
					
					s_Quad->Bind();
					RenderAPI::DrawIndexed(6, DataType::UnsignedInt, NULL);
					s_Quad->Unbind();
				}
				calls[0].texture->Unbind();

				calls.clear();
			}
			s_Data->mainShader->Unbind();
		}

		void Render2D::DrawQuad(const glm::vec2& position, float depth, const glm::vec2& scale, float rotation, const glm::vec3& tint)
		{
			glm::mat4 transform = glm::mat4(1.0f);
			transform = glm::translate(transform, { position, depth - 1.0f });
			transform = glm::rotate(transform, rotation, { 0.0f, 0.0f, 1.0f });
			transform = glm::scale(transform, { scale, 1.0f });
			
			SubmitDrawCall({ s_Data->currentCamera, s_Data->emptyTexture, transform, tint });
		}
		void Render2D::DrawQuad(const glm::vec2& position, float depth, const glm::vec2& scale, float rotation, Texture* texture, const glm::vec3& tint)
		{
			glm::mat4 transform = glm::mat4(1.0f);
			transform = glm::translate(transform, { position, depth - 1.0f });
			transform = glm::rotate(transform, rotation, { 0.0f, 0.0f, 1.0f });
			transform = glm::scale(transform, { scale, 1.0f });

			SubmitDrawCall({ s_Data->currentCamera, texture, transform, tint });
		}

		void Render2D::SubmitDrawCall(const DrawCall& call)
		{
			uint64_t hash = DrawCall::hash(call.camera, call.texture);
			if (!s_Data->drawCallLookup.count(hash)) {
				s_Data->drawCallLookup.insert({ hash, s_Data->drawCalls.size() });
				s_Data->drawCalls.push_back({ call });
				return;
			}
			int location = s_Data->drawCallLookup[hash];
			s_Data->drawCalls[location].push_back(call);
		}
	}
}