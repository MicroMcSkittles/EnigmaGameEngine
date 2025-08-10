#include "EnigmaSerialization/Image.h"

#include <Enigma/Core/Core.h>
#include <Enigma/Core/System.h>
#include <stb/stb_image.h>

namespace Enigma {
	namespace Serialization {

		ImageLoader::~ImageLoader()
		{
			if (m_Thread.joinable()) {
				m_Thread.join();
			}
		}

		Renderer::Texture* ImageLoader::Load(const std::string& filename, const ImageConfig& config)
		{
			Renderer::TextureConfig textureConfig = config;

			stbi_set_flip_vertically_on_load(!config.flipY);

			int nrChannels;

			textureConfig.data = nullptr;
			textureConfig.data = stbi_load(filename.c_str(), &textureConfig.width, &textureConfig.height, &nrChannels, STBI_rgb_alpha);

			if (!textureConfig.data) {
				LOG_SOFT_ERROR("Failed to load texture ( %s )", filename.c_str());
				return nullptr;
			}

			textureConfig.format = Renderer::TexFormat::RGBA;
			textureConfig.internalFormat = Renderer::TexFormat::RGBA;

			Renderer::Texture* texture = Renderer::Texture::Create(textureConfig);
			free(textureConfig.data);
			return texture;
		}
		Renderer::Texture* ImageLoader::Load(uint8_t* imageData, int length, const ImageConfig& config)
		{
			Renderer::TextureConfig textureConfig = config;

			stbi_set_flip_vertically_on_load(!config.flipY);

			int nrChannels;

			textureConfig.data = nullptr;
			textureConfig.data = stbi_load_from_memory(imageData, length, &textureConfig.width, &textureConfig.height, &nrChannels, 0);

			if (!textureConfig.data) {
				LOG_SOFT_ERROR("Failed to load texture");
				return nullptr;
			}

			Renderer::TexFormat format = Renderer::TexFormat::None;
			if (nrChannels == 3) format = Renderer::TexFormat::RGB;
			else if (nrChannels == 4) format = Renderer::TexFormat::RGBA;
			else {
				LOG_SOFT_ERROR("Failed to load texture because it's a unknown image format");
				return nullptr;
			}
			textureConfig.format = format;
			textureConfig.internalFormat = format;

			Renderer::Texture* texture = Renderer::Texture::Create(textureConfig);
			free(textureConfig.data);
			return texture;
		}

		void ImageLoader::Load(Renderer::Texture*& output, const std::string& filename, const ImageConfig& config)
		{
			Image* image = new Image(output);
			image->StartLoad(filename, config);
			m_Images.push_back(image);
		}
		void ImageLoader::Load(Renderer::Texture*& output, uint8_t* imageData, int length, const ImageConfig& config)
		{
			Image* image = new Image(output);
			image->StartLoad(imageData, length, config);
			m_Images.push_back(image);
		}

		bool ImageLoader::IsLoading()
		{
			bool loading = false;
			for (int i = 0; i < m_Images.size(); ++i) {
				Image* image = m_Images[i];
				if (image->IsLoaded()) {
					delete m_Images[i];
					m_Images.erase(m_Images.begin() + i);
					i -= 1;
				}
				else loading = true;
			}
			return loading;
		}

		void ImageLoader::Image::StartLoad(const std::string& filename, const ImageConfig& config)
		{
			if (thread.joinable()) {
				LOG_WARNING("Image already loading");
				return;
			}
			finished = false;
			success = true;
			thread = std::thread(&ImageLoader::Image::LoadDataFromFile, this, filename, config);
		}
		void ImageLoader::Image::StartLoad(uint8_t* imageData, int length, const ImageConfig& config)
		{
			if (thread.joinable()) {
				LOG_WARNING("Image already loading");
				return;
			}
			finished = false;
			success = true;
			thread = std::thread(&ImageLoader::Image::LoadDataFromMemory, this, imageData, length, config);
		}

		bool ImageLoader::Image::IsLoaded()
		{
			if (!finished) return false;
			thread.join();
			if (!success) {
				LOG_SOFT_ERROR("%s", errorMessage.c_str());
				return true;
			}
			output = Renderer::Texture::Create(textureConfig);
			free(textureConfig.data);
			textureConfig.data = nullptr;
			return true;
		}

		void ImageLoader::Image::LoadDataFromFile(const std::string& filename, const ImageConfig& config)
		{
			textureConfig = config;

			stbi_set_flip_vertically_on_load(!config.flipY);

			int nrChannels;

			textureConfig.data = nullptr;
			textureConfig.data = stbi_load(filename.c_str(), &textureConfig.width, &textureConfig.height, &nrChannels, STBI_rgb_alpha);

			if (!textureConfig.data) {
				errorMessage = "Failed to load texture ( " + filename + " )";
				errorMessage = "Failed to load texture ( " + filename + " ): " + std::string(stbi_failure_reason());
				success = false;
				finished = true;
				return;
			}

			textureConfig.format = Renderer::TexFormat::RGBA;
			textureConfig.internalFormat = Renderer::TexFormat::RGBA;

			finished = true;
		}
		void ImageLoader::Image::LoadDataFromMemory(uint8_t* imageData, int length, const ImageConfig& config)
		{
			textureConfig = config;

			stbi_set_flip_vertically_on_load(!config.flipY);

			int nrChannels;
			textureConfig.data = stbi_load_from_memory(imageData, length, &textureConfig.width, &textureConfig.height, &nrChannels, 0);

			if (!textureConfig.data) {
				errorMessage = "Failed to load texture: " + std::string(stbi_failure_reason());
				success = false;
				finished = true;
				return;
			}

			Renderer::TexFormat format = Renderer::TexFormat::None;
			if (nrChannels == 3) format = Renderer::TexFormat::RGB;
			else if (nrChannels == 4) format = Renderer::TexFormat::RGBA;
			else {
				errorMessage = "Failed to load texture: Unknown image format";
				success = false;
				finished = true;
				return;
			}
			textureConfig.format = format;
			textureConfig.internalFormat = format;

			finished = true;
		}

	}
}