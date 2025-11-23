#pragma once
#include "Enigma/Core/Types.h"
#include "Enigma/Core/Window.h"
#include "Enigma/Renderer/RenderEnum.h"

namespace Enigma::Core {

	struct ImGuiConfig {
		ref<Window> window; // set automaticaly
		Renderer::API renderAPI; // set automaticaly
		bool docking;
	};

	class ImGuiHandler{
	public:

		virtual ~ImGuiHandler() { }

		static ref<ImGuiHandler> Create(const ImGuiConfig& config);

		static void MakeCurrent(ref<ImGuiHandler> context) {
			s_CurrentContext = context; 
			s_CurrentContext->MadeCurrent();
		}

		static void StartFrame() { s_CurrentContext->StartFrameImpl(); }
		static void EndFrame() { s_CurrentContext->EndFrameImpl(); }

	private:
		
		virtual void MadeCurrent() = 0;

		virtual void StartFrameImpl() = 0;
		virtual void EndFrameImpl() = 0;

	private:
		inline static ref<ImGuiHandler> s_CurrentContext;
	};
}
