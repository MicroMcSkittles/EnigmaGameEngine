#include "Enigma/Renderer/Texture.h"
#include "Enigma/Renderer/RenderAPI.h"
#include "Enigma/Renderer/VertexArray.h"

#include "Enigma/Platform/OpenGL/OpenGLTexture.h"

namespace Enigma {
	namespace Renderer {
		/*Texture* Texture::Create(const std::string& path, const TextureConfig& config)
		{
			switch (RenderAPI::GetAPI())
			{
			case API::OpenGL: return new Platform::OpenGL::OpenGLTexture(path, config); break;
			default:
				LOG_ERROR("Can't create shader, API not supported");
			}
			return nullptr;
		}
		Texture* Texture::Create(uint8_t* bytes, int length, const TextureConfig& config)
		{
			switch (RenderAPI::GetAPI())
			{
			case API::OpenGL: return new Platform::OpenGL::OpenGLTexture(bytes, length, config); break;
			default:
				LOG_ERROR("Can't create shader, API not supported");
			}
			return nullptr;
		}*/
		Texture* Texture::Create(const TextureConfig& config)
		{
			switch (RenderAPI::GetAPI())
			{
			case API::OpenGL: return new Platform::OpenGL::OpenGLTexture(config); break;
			default:
				LOG_ERROR("Can't create shader, API not supported");
				return nullptr;
			}
		}
		
		/*TextureAtlas::TextureAtlas(const std::string& path, const glm::vec2& size, const TextureConfig& config)
		{
			m_Config = config;
			m_Size = size;

			Texture* atlas = Texture::Create(path, m_Config);

			glm::vec2 lastViewportSize = RenderAPI::GetViewportSize();

			FrameBufferConfig frameConfig;
			frameConfig.width = floor((float)atlas->GetWidth() / m_Size.x);
			frameConfig.height = floor((float)atlas->GetHeight() / m_Size.y);
			Attachment attachment;
			attachment.type = AttachmentType::ColorAttachment;
			attachment.format = TexFormat::RGBA;
			attachment.internalFormat = TexFormat::RGBA;
			frameConfig.attachments = { attachment };

			std::vector<Vertex> viewQuad = s_QuadVertices;
			for (auto& v : viewQuad) {
				v.textureCoord.x = (v.textureCoord.x / m_Size.x);
				v.textureCoord.y = ((m_Size.y - v.textureCoord.y) / m_Size.y);
			}

			VertexArray* quad = VertexArray::Create();
			quad->Bind();
			VertexBuffer* vbo = VertexBuffer::Create(s_VertexLayout, Usage::StaticDraw);
			vbo->SetData((void*)&viewQuad[0], sizeof(Vertex) * viewQuad.size());
			vbo->InitAttribs();
			quad->AttachBuffer(vbo);

			IndexBuffer* ebo = IndexBuffer::Create(DataType::UnsignedInt, Usage::StaticDraw);
			ebo->SetData((void*)&s_QuadIndices[0], sizeof(unsigned int) * s_QuadIndices.size());
			quad->AttachBuffer(ebo);


			for (int y = 0; y < m_Size.y; ++y) {
				for (int x = 0; x < m_Size.x; ++x) {

					vbo->SetData((void*)&viewQuad[0], sizeof(Vertex)* viewQuad.size());
					vbo->InitAttribs();

					FrameBuffer* frame = FrameBuffer::Create(frameConfig);

					frame->Bind();
					RenderAPI::SetViewport(frameConfig.width, frameConfig.height);
					RenderAPI::Clear();

					m_Config.preProcShader->Bind();

					atlas->Bind();
					m_Config.preProcShader->SetUniform("FrameTexture", (void*)atlas);

					RenderAPI::DrawIndexed(6, DataType::UnsignedInt, NULL);

					atlas->Unbind();
					m_Config.preProcShader->Unbind();

					frame->Unbind();

					m_Textures.push_back(frame->SeverColorAttachment(0));

					for (auto& v : viewQuad) {
						v.textureCoord.x += (float)(x + 1) / m_Size.x;
						v.textureCoord.y += ((m_Size.y - (float)(y + 1)) / m_Size.y);
					}
					delete frame;
				}
			}

			quad->Unbind();

			delete quad;
			delete atlas;

			RenderAPI::SetViewport(lastViewportSize.x, lastViewportSize.y);
		}
		TextureAtlas::~TextureAtlas()
		{
			for (int i = 0; i < m_Textures.size(); ++i) {
				delete m_Textures[i];
			}
		}
		Texture* TextureAtlas::GetTexture(const glm::vec2& id)
		{
			int index = id.x + m_Size.x * id.y;
			return m_Textures[index];
		}*/
	}
}