#include "Enigma/Platform/OpenGL/OpenGLTexture.h"
#include "Enigma/Platform/OpenGL/OpenGLRenderAPI.h"

#include <glad/glad.h>

namespace Enigma {
	namespace Platform {
		namespace OpenGL {
			OpenGLTexture::OpenGLTexture(const Renderer::TextureConfig& config)
			{
				m_Config = config;
				m_Width = m_Config.width;
				m_Height = m_Config.height;
				
				glGenTextures(1, &m_Handle);
				glBindTexture(GL_TEXTURE_2D, m_Handle);

				if (m_Config.sWrapMode == Renderer::TexWrapMode::None || m_Config.tWrapMode == Renderer::TexWrapMode::None) {
					LOG_SOFT_ERROR("Failed to load texture because it's wrap mode can't equal None");
					return;
				}
				if (m_Config.minFilter == Renderer::TexFilterMode::None || m_Config.magFilter == Renderer::TexFilterMode::None) {
					LOG_SOFT_ERROR("Failed to load texture because it's wrap mode can't equal None");
					return;
				}
				if (m_Config.format == Renderer::TexFormat::None || m_Config.internalFormat == Renderer::TexFormat::None) {
					LOG_SOFT_ERROR("Failed to load texture because it's format can't equal None");
					return;
				}
				if (m_Config.dataType == Renderer::DataType::None) {
					LOG_SOFT_ERROR("Failed to load texture because it's data type can't equal None");
					return;
				}

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Conversions::TexWrapMode(m_Config.sWrapMode));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Conversions::TexWrapMode(m_Config.tWrapMode));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Conversions::TexFilterMode(m_Config.minFilter));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Conversions::TexFilterMode(m_Config.magFilter));

				glTexImage2D(GL_TEXTURE_2D, 0, Conversions::TexFormat(m_Config.internalFormat), m_Width, m_Height, 0,
					Conversions::TexFormat(m_Config.format), Conversions::DataType(m_Config.dataType), m_Config.data);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			OpenGLTexture::~OpenGLTexture()
			{
				glDeleteTextures(1, &m_Handle);
			}

			void* OpenGLTexture::GetNativeTexture()
			{
				return (void*)&m_Handle;
			}

			void OpenGLTexture::Resize(int width, int height, void* data)
			{
				m_Width = width;
				m_Height = height;
				
				if(m_Config.data != nullptr) free(m_Config.data);
				m_Config.data = data;

				unsigned char* image_data = (unsigned char*)m_Config.data;

				glBindTexture(GL_TEXTURE_2D, m_Handle);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Conversions::TexWrapMode(m_Config.sWrapMode));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Conversions::TexWrapMode(m_Config.tWrapMode));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Conversions::TexFilterMode(m_Config.minFilter));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Conversions::TexFilterMode(m_Config.magFilter));

				glTexImage2D(GL_TEXTURE_2D, 0, Conversions::TexFormat(m_Config.internalFormat), m_Width, m_Height, 0,
					Conversions::TexFormat(m_Config.format), Conversions::DataType(m_Config.dataType), image_data);
				glGenerateMipmap(GL_TEXTURE_2D);

				glBindTexture(GL_TEXTURE_2D, 0);
			}
			void OpenGLTexture::Bind()
			{
				int maxSlots;
				glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxSlots);
				if (s_OccupiedSlotCount >= maxSlots) {
					LOG_SOFT_ERROR("To many textures binded at once, the max is ( %i )", maxSlots);
					return;
				}
				m_CurrentSlot = s_OccupiedSlotCount;
				s_OccupiedSlotCount += 1;

				glActiveTexture(GL_TEXTURE0 + m_CurrentSlot);
				glBindTexture(GL_TEXTURE_2D, m_Handle);
			}
			void OpenGLTexture::Unbind()
			{
				if (s_OccupiedSlotCount <= 0) {
					s_OccupiedSlotCount = 0;
					return;
				}

				glActiveTexture(GL_TEXTURE0 + m_CurrentSlot);
				glBindTexture(GL_TEXTURE_2D, 0);

				s_OccupiedSlotCount -= 1;
			}
		}
	}
}