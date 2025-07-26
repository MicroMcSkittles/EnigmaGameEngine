#pragma once
#include "Enigma/Renderer/Texture.h"

namespace Enigma {
	namespace Platform {
		namespace OpenGL {
			class OpenGLTexture : public Renderer::Texture {
			public:
				OpenGLTexture(const Renderer::TextureConfig& config = {});
				~OpenGLTexture();

				virtual void* GetNativeTexture() override;

				virtual void Resize(int width, int height, void* data = (void*)NULL) override;

				virtual void Bind() override;
				virtual void Unbind() override;

				virtual int GetWidth() override { return m_Width; }
				virtual int GetHeight() override { return m_Height; }

				int GetSlot() { return m_CurrentSlot; }

			private:
				uint32_t m_Handle;
				int m_CurrentSlot;

				Renderer::TextureConfig m_Config;
				int m_Width;
				int m_Height;

			private:
				inline static int s_OccupiedSlotCount = 0;
			};
		}
	}
}