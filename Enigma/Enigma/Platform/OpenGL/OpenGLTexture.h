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

				virtual void Resize(i32 width, i32 height, void* data = (void*)NULL) override;

				virtual void Bind() override;
				virtual void Unbind() override;

				virtual i32 GetWidth() override { return m_Width; }
				virtual i32 GetHeight() override { return m_Height; }

				i32 GetSlot() { return m_CurrentSlot; }

			private:
				u32 m_Handle;
				i32 m_CurrentSlot;

				Renderer::TextureConfig m_Config;
				i32 m_Width;
				i32 m_Height;

			private:
				inline static i32 s_OccupiedSlotCount = 0;
			};
		}
	}
}