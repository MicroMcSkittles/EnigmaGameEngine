#pragma once
#include "Enigma/Core/Types.h"
#include "Enigma/Renderer/RenderEnum.h"
#include "Enigma/Renderer/Shader.h"

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
			i32 width = 0;
			i32 height = 0;
		};

		class Texture {
		public:
			static ref<Texture> Create(const TextureConfig& config = {});
			virtual ~Texture() { }

			virtual void* GetNativeTexture() = 0;

			virtual void Resize(i32 width, i32 height, void* data = (void*)NULL) = 0;

			virtual void Bind() = 0;
			virtual void Unbind() = 0;

			virtual i32 GetWidth() = 0;
			virtual i32 GetHeight() = 0;
		};

	}
}