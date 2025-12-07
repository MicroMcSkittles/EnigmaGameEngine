#pragma once
#include "Enigma/Renderer/Texture.h"

namespace Enigma::Platform::OpenGL {
	class OpenGLTexture : public Renderer::Texture {
	public:
		OpenGLTexture(const Renderer::TextureConfig& config = {});
		~OpenGLTexture();

		virtual void* GetNativeTexture() override;

		virtual void Resize(u32 width, u32 height, void* data) override;

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual u32 GetWidth() override { return m_Width; }
		virtual u32 GetHeight() override { return m_Height; }

		virtual Renderer::TextureConfig GetConfig() override { return m_Config; }

		i32 GetSlot() { return m_CurrentSlot; }

	private:
		u32 m_Handle;
		i32 m_CurrentSlot;

		Renderer::TextureConfig m_Config;
		u32 m_Width;
		u32 m_Height;

	private:
		inline static i32 s_OccupiedSlotCount = 0;
	};
}