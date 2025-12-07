#pragma once
#include "Enigma/Core/Types.h"
#include "Enigma/Renderer/RenderEnum.h"
#include "Enigma/Renderer/Shader.h"

#include <glm/glm.hpp>

namespace Enigma::Renderer {

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
		u32 width = 0;
		u32 height = 0;
	};

	class Texture {
	public:
		static ref<Texture> Create(const TextureConfig& config = {});
		virtual ~Texture() { }

		virtual void* GetNativeTexture() = 0;

		virtual void Resize(u32 width, u32 height, void* data = nullptr) = 0;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual u32 GetWidth() = 0;
		virtual u32 GetHeight() = 0;

		virtual TextureConfig GetConfig() = 0;
	};
}