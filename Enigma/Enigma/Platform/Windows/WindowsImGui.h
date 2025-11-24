#pragma once
#include "Enigma/ImGui/ImGuiHandler.h"
#include "Enigma/Core/Event/Event.h"

#include <imgui.h>

namespace Enigma::Platform {

	class WindowsImGuiContext : public Core::ImGuiHandler {
	public:
		WindowsImGuiContext(const Core::ImGuiConfig& config);

		virtual void MadeCurrent() override;

		virtual void StartFrameImpl() override;
		virtual void EndFrameImpl() override;

		virtual void DockSpaceImpl(f32 x, f32 y, f32 widthOffset, f32 heightOffset) override;

	private:
		void InitOpenGL();

	private:
		ref<Core::Window> m_WindowInstance;
		Renderer::API m_RenderAPI;

		ref<ImGuiContext> m_Context;

		bool m_EnableDocking;

	private:
		
		inline static bool s_InitGLFW = false;
	};

}