#pragma once
#include <Enigma/Renderer/RenderEnum.h>
#include <Enigma/Renderer/Texture.h>

#include <stdint.h>
#include <string>
#include <thread>

namespace Enigma {
	namespace Serialization {

		struct ImageConfig {
			Renderer::TexWrapMode sWrapMode = Renderer::TexWrapMode::Repeat; // X wrap mode
			Renderer::TexWrapMode tWrapMode = Renderer::TexWrapMode::Repeat; // Y wrap mode
			Renderer::TexFilterMode minFilter = Renderer::TexFilterMode::Nearest;
			Renderer::TexFilterMode magFilter = Renderer::TexFilterMode::Linear;
			glm::vec3 borderColor = { 1.0f, 0.0f, 1.0f };
			bool flipY = false;

			operator Renderer::TextureConfig() const {
				Renderer::TextureConfig rslt;
				rslt.sWrapMode = sWrapMode;
				rslt.tWrapMode = tWrapMode;
				rslt.minFilter = minFilter;
				rslt.magFilter = magFilter;
				rslt.borderColor = borderColor;
				return rslt;
			}
		};

		class ImageLoader {
		public:
			~ImageLoader();

			// Load image from file
			static ref<Renderer::Texture> Load(const std::string& filename, const ImageConfig& config);
			// Load image from byte buffer
			static ref<Renderer::Texture> Load(uint8_t* imageData, int length, const ImageConfig& config);

			void Load(ref<Renderer::Texture>& output, const std::string& filename, const ImageConfig& config);
			void Load(ref<Renderer::Texture>& output, uint8_t* imageData, int length, const ImageConfig& config);

			bool IsLoading();

		private:

			struct Image {
				ref<Renderer::Texture>& output;
				
				Renderer::TextureConfig textureConfig;
				std::thread thread;
				bool finished;

				std::string errorMessage;
				bool success;

				Image(ref<Renderer::Texture>& output) : output(output) { }

				// Starts a new thread that loads the image from file
				void StartLoad(const std::string& filename, const ImageConfig& config);
				// Starts a new thread that loads the image from a bytebuffer
				void StartLoad(uint8_t* imageData, int length, const ImageConfig& config);

				bool IsLoaded();

			private:
				void LoadDataFromFile(const std::string& filename, const ImageConfig& config);
				void LoadDataFromMemory(uint8_t* imageData, int length, const ImageConfig& config);
			};

		private:
			std::thread m_Thread;
			std::vector<Image*> m_Images;
		};

	}
}