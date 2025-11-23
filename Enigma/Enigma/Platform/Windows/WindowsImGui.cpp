#include "Enigma/EnigmaPCH.h"
#include "Enigma/Platform/Windows/WindowsImGui.h"
#include "Enigma/Platform/Windows/WindowsWindow.h"
#include "Enigma/Platform/OpenGL/OpenGLTexture.h"

#include <Windows.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_opengl3.h>

namespace Enigma::Core {

	ref<ImGuiHandler> ImGuiHandler::Create(const ImGuiConfig& config) {
		return CreateRef<Platform::WindowsImGuiContext>(config);
	}

}

namespace Enigma::Platform {
	WindowsImGuiContext::WindowsImGuiContext(const Core::ImGuiConfig& config)
	{
		m_WindowInstance = config.window;
		m_RenderAPI = config.renderAPI;
		m_EnableDocking = config.docking;

		switch (m_RenderAPI)
		{
		case Enigma::Renderer::API::OpenGL: InitOpenGL(); break;
		default:
			LOG_ERROR("ImGui doesn't support %s", Renderer::ToString(m_RenderAPI).c_str());
		}
	}

	void WindowsImGuiContext::InitOpenGL()
	{
		IMGUI_CHECKVERSION();

		m_Context = ref<ImGuiContext>(ImGui::CreateContext());

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		if (m_EnableDocking) io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		//if (m_EnableDocking) io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();
		
		if (!s_InitGLFW) {
			ImGui_ImplWin32_InitForOpenGL(m_WindowInstance->GetNativeWindow());
			ImGui_ImplOpenGL3_Init("#version 460");
			s_InitGLFW = true;
		}
	}

	void WindowsImGuiContext::StartDocking()
	{
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoBackground;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("InvisibleWindow", nullptr, windowFlags);
		ImGui::PopStyleVar(3);

		ImGuiID dockSpaceId = ImGui::GetID("InvisibleWindowDockSpace");

		ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
		ImGui::End();
	}

	void WindowsImGuiContext::MadeCurrent()
	{
		ImGui::SetCurrentContext(m_Context.get());
	}

	void WindowsImGuiContext::StartFrameImpl()
	{
		ImGui_ImplWin32_NewFrame();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		if (m_EnableDocking) StartDocking();
	}
	void WindowsImGuiContext::EndFrameImpl()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		//ImGui::UpdatePlatformWindows();
		//ImGui::RenderPlatformWindowsDefault();
	}
}