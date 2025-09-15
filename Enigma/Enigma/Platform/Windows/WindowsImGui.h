#pragma once
#include "Enigma/ImGui/ImGuiHandler.h"
#include "Enigma/Core/Event/Event.h"

#include <imgui.h>

namespace Enigma {
	namespace Platform {

		class WindowsImGuiContext : public Core::ImGuiHandler {
		public:
			WindowsImGuiContext(const Core::ImGuiConfig& config);

			virtual void MadeCurrent() override;

			virtual void StartFrameImpl() override;
			virtual void EndFrameImpl() override;

		private:
			void InitOpenGL();

			void StartDocking();

		private:
			Core::Window* m_WindowInstance;
			Renderer::API m_RenderAPI;

			ImGuiContext* m_Context;

			bool m_EnableDocking;

		private:
			
			inline static bool s_InitGLFW = false;
		};

	}
}