#pragma once
#include "Renderer/RenderAPI.h"
#include "Renderer/Texture.h"
#include "Engine/Surface.h"

namespace Enigma {
	namespace Renderer {

		enum class RendererType {
			None = 0,
			Render2D,
			Render3D // Not impl yet
		};

		struct RendererConfig {
			RendererType type;
			API renderAPI;
			Engine::Surface surface;
		};

		class Renderer {
		public:
			Renderer(const RendererConfig& config) 
				: m_RenderAPI(config.renderAPI), m_Surface(config.surface) { }

			static void MakeCurrent(Renderer* context) { s_CurrentContext = context; }
			static Renderer* GetCurrent() { return s_CurrentContext; }

		protected:
			API m_RenderAPI;
			Engine::Surface m_Surface;

		protected:
			inline static Renderer* s_CurrentContext;
		};

	}
}