#pragma once
#include "Scene/Scene.h"
#include "Scene/Entity.h"

#include <Enigma/Core/Event/InputEvent.h>
#include <Enigma/Engine/ECS/RenderSystem2D.h>
#include <Enigma/Engine/Input.h>

namespace Enigma::Editor {

	class SceneViewPanel {
	public:
		SceneViewPanel(Core::ID windowID);
		void SetContext(ref<Scene> context);
		void SetSelected(Entity entity);

		void OnEvent(Core::Event& e);
		void Update(Engine::DeltaTime deltaTime);
		void Render();
		void ShowGui();

	private:
		void OnKeyboard(Core::Keyboard& e);

		void ShowGizmos();
		void ShowOverlayWindow(f32 x, f32 y);

	private:
		Core::ID m_WindowID;
		
		ref<Scene> m_Context;
		Entity m_Selected;

		float m_CameraSpeed;
		i32 m_GizmoType;

		bool m_Hovered;
		Engine::Surface m_Surface;
		ref<Engine::Input> m_InputContext;

		ref<Renderer::Texture> m_Frame;
		ref<Engine::ECS::RenderSystem2D> m_RenderSystem;
		ref<Renderer::Camera> m_Camera;
	};

}