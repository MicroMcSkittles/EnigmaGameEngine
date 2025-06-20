#pragma once
#include "Engine/DeltaTime.h"
#include "Engine/Surface.h"
#include "Engine/Input.h"

#include "Core/Process/SubProcess.h"
#include "Core/Window.h"

#include "Core/Event/Event.h"
#include "Core/Event/WindowEvent.h"

#include "Renderer/Renderer.h"

//#include "ECS/EntityComponentSystem.h"

#include <glm/glm.hpp>

namespace Enigma {
	namespace Engine {

		struct EngineConfig {
			Renderer::RendererConfig* rendererConfig; // config for the renderer
			Core::ID windowID; // the window Engine is attached to
		};

		class Engine {
		public:
			Engine(const EngineConfig& config);
			~Engine();

			void OnEvent(Core::Event& e);
			void Update(DeltaTime deltaTime);
			void ImGui();

			Surface& GetSurface() { return m_Surface; }

			Renderer::Renderer* GetRenderer() { return m_RendererContext; }

			Core::ID GetWindowID() { return m_WindowID; }

			//ECS::ECS* GetECS() { return m_ECS; }

			void SetID(Core::ID id) { m_ID = id; }

			// Creates a sub process and stores it to the sub process stack
			// Returns a pointer to the process
			// T must be a inherited class of the SubProcess class
			template<typename T>
			T* CreateSubProc() {
				T* proc = new T;
				proc->m_EngineID = &m_ID;
				m_SubProcStack.PushProcBack(proc);
				return proc;
			}

		private:
			bool OnWindowClose(Core::WindowClose& e);

		private:
			Core::ID m_ID;
			Core::ID m_WindowID;
			Core::SubProcStack m_SubProcStack;
			Surface m_Surface;

			Renderer::Renderer* m_RendererContext;

			Input* m_InputContext;

			//ECS::ECS* m_ECS;
		};

	}
}