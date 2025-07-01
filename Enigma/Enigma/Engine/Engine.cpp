#include "Engine/Engine.h"
#include "Core/Process/Application.h"

#include "Renderer/RenderAPI.h"
#include "Renderer/Render2D.h"
#include "Engine/Render2DProc.h"

namespace Enigma {
	namespace Engine {
		Engine::Engine(const EngineConfig& config)
		{
			m_WindowID = config.windowID;
			m_Surface = config.rendererConfig->surface;

			// Create Input Context
			InputConfig inputConfig;
			inputConfig.surface = &m_Surface;
			inputConfig.window = Core::Application::GetWindow(m_WindowID);
			m_InputContext = Input::Create(inputConfig);

			// Create Entity Component System
			m_ECS = new ECS::ECS();

			// Create Renderer
			if (m_Surface.frame == nullptr) {
				Core::Window* window = Core::Application::GetWindow(m_WindowID);
				m_Surface.scale.x = window->GetWidth();
				m_Surface.scale.y = window->GetHeight();
			}
			switch (config.rendererConfig->type) {
			case Renderer::RendererType::Render2D: {
				Renderer::Render2DConfig render2DConfig = *(Renderer::Render2DConfig*)config.rendererConfig;
				render2DConfig.surface = m_Surface;
				m_RendererContext = new Renderer::Render2D(render2DConfig);
				Render2DProc* proc = CreateSubProc<Render2DProc>();
				proc->SetRenderContext((Renderer::Render2D*)m_RendererContext);
				break;
			}
			default: LOG_ERROR("Renderer not supported");
			}
			
			LOG_MESSAGE("Engine instance created", 4);
		}
		Engine::~Engine()
		{
		}

		bool Engine::OnWindowClose(Core::WindowClose& e)
		{
			Core::Application::DeleteEngineInstance(this);
			return false;
		}
		void Engine::OnEvent(Core::Event& e)
		{
			Core::EventHandler handler(e);
			handler.Dispatch<Core::WindowClose>([&](Core::WindowClose& e) { return OnWindowClose(e); });

			m_SubProcStack.OnEvent(e);
		}

		void Engine::Update(DeltaTime deltaTime)
		{
			Core::Application::GetWindow(m_WindowID)->MakeCurrent();
			ECS::ECS::MakeCurrent(m_ECS);

			m_SubProcStack.Update(deltaTime);

			Renderer::Renderer::MakeCurrent(m_RendererContext);
			m_SubProcStack.Render();
		}
		void Engine::ImGui()
		{
			m_SubProcStack.ImGui();
		}
	}
}