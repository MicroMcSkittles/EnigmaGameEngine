#pragma once
#include "Renderer/RenderEnum.h"
#include "Renderer/Shader.h"

#include <glm/glm.hpp>

namespace Enigma {
	namespace Renderer {
		struct TextureConfig {
			// Used when loading a texture from file
			TexWrapMode sWrapMode = TexWrapMode::Repeat; // X wrap mode
			TexWrapMode tWrapMode = TexWrapMode::Repeat; // Y wrap mode
			TexFilterMode minFilter = TexFilterMode::Nearest;
			TexFilterMode magFilter = TexFilterMode::Linear;
			glm::vec3 borderColor = { 1.0f, 0.0f, 1.0f };
			bool flipY = false;


			// Not used when loading a texture from file
			DataType dataType = DataType::UnsignedByte;
			TexFormat format = TexFormat::RGB;
			TexFormat internalFormat = TexFormat::RGB;
			void* data = nullptr;
			uint32_t width = 0;
			uint32_t height = 0;

			// Used for atlases
			Shader* preProcShader = nullptr; // Applied to a texture when its loaded
		};

		class Texture {
		public:
			static Texture* Create(const std::string& path, const TextureConfig& config = {});
			static Texture* Create(const TextureConfig& config = {});

			virtual void Resize(int width, int height, void* data = (void*)NULL) = 0;

			virtual void Bind() = 0;
			virtual void Unbind() = 0;

			virtual int GetWidth() = 0;
			virtual int GetHeight() = 0;
		};

		// TODO: make this better, it sucks rn
		class TextureAtlas {
		public:
			TextureAtlas(const std::string& path, const glm::vec2& size, const TextureConfig& config);
			~TextureAtlas();

			Texture* GetTexture(const glm::vec2& id);

			std::vector<Texture*>& GetTextures() { return m_Textures; }

		private:
			TextureConfig m_Config;
			glm::vec2 m_Size;

			std::vector<Texture*> m_Textures;

		private:
			struct Vertex {
				glm::vec3 position;
				glm::vec2 textureCoord;
			};
			inline static std::vector<DataType> s_VertexLayout = {
				DataType::Float3,
				DataType::Float2
			};
			inline static std::vector<Vertex> s_QuadVertices = {
				{ {  1.0f,  1.0f, 0.0f }, { 1.0f, 1.0f } },
				{ {  1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } },
				{ { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },
				{ { -1.0f,  1.0f, 0.0f }, { 0.0f, 1.0f } }
			};
			inline static std::vector<unsigned int> s_QuadIndices = {
				0,1,3,
				1,2,3
			};
		};
	}
}