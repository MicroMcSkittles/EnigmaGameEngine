#pragma once
#include "Renderer/RenderEnum.h"
#include "Core/IdHandler.h"

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

		public:
			Core::ID GetID() { return m_ID; }
			~Texture() { s_IdHandler.Delete(m_ID); }

		private:
			Core::ID m_ID;

		private:
			inline static Core::IDHandler<Texture> s_IdHandler;
		};
	}
}