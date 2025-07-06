#pragma once
#include <Enigma/Core/Process/SubProcess.h>
#include <Enigma/Core/Event/InputEvent.h>
#include <Enigma/Core/Window.h>
#include <Enigma/Renderer/Render2D.h>
#include <Enigma/Engine/Input.h>

#include "Scene/Scene.h"

namespace Enigma {
	namespace Editor {

		class SceneView2D : public Core::SubProcess {
		public:
			SceneView2D() : 
				m_InputContext(nullptr),
				m_Window(nullptr),
				m_RendererContext(nullptr),
				m_SceneContext(nullptr),
				m_Camera(nullptr) { }

			virtual void StartUp() override;
			virtual void ShutDown() override;

			bool OnMouseScroll(Core::MouseScroll& e);
			virtual bool OnEvent(Core::Event& e)  override;
			virtual void Update(Engine::DeltaTime deltaTime) override;

			virtual void Render() override;

			void SetWindowContext(Core::Window* window);
			void SetSceneContext(Scene* scene) { m_SceneContext = scene; }

			void SetSurface(const Engine::Surface& surface);
			Engine::Surface GetSurface() { return m_Surface; }

			void Resize(int width, int height);
			Renderer::Texture* GetFrame();

		private:
			Engine::Input* m_InputContext;

			Core::Window* m_Window;

			Engine::Surface m_Surface;
			Renderer::Render2D* m_RendererContext;

			Renderer::OrthographicCamera* m_Camera;

			Scene* m_SceneContext;
		};

	}
}