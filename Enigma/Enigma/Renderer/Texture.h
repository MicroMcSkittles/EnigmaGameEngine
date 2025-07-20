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

			// Not used when loading a texture from file
			DataType dataType = DataType::UnsignedByte;
			TexFormat format = TexFormat::RGB;
			TexFormat internalFormat = TexFormat::RGB;
			void* data = nullptr;
			int width = 0;
			int height = 0;
		};

		class Texture {
		public:
			static Texture* Create(const TextureConfig& config = {});
			virtual ~Texture() { }

			virtual void* GetNativeTexture() = 0;

			virtual void Resize(int width, int height, void* data = (void*)NULL) = 0;

			virtual void Bind() = 0;
			virtual void Unbind() = 0;

			virtual int GetWidth() = 0;
			virtual int GetHeight() = 0;
		};

	}
}