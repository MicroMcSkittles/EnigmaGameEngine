#pragma once
#include "Scene/Scene.h"
#include "Scene/Entity.h"

#include <Enigma/Core/Event/InputEvent.h>
#include <Enigma/Engine/Input.h>
#include <Enigma/Renderer/Renderer2D.h>

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
		void OnScroll(Core::MouseScroll& e);
		void OnKeyboard(Core::Keyboard& e);

		void ShowEntityMenu();
		
		void ShowGizmoMenu();
		void ShowGizmos();
		void ShowGizmoOverlayWindow();

		void ShowRuntimeOverlayWindow();
		void ShowSettingsOverlayWindow();
		void ShowSettingsMenu();

		Entity GetHoveredEntity();

	private:
		Core::ID m_WindowID;
		
		// Context Variables
		ref<Scene> m_Context;
		Entity m_Selected;
		u64 m_EditorState;

		// Render Variables
		ref<Renderer::Renderer2D> m_RendererContext;
		ref<Renderer::Texture> m_Frame;
		ref<Renderer::Camera> m_ActiveCamera;
		ref<Renderer::Camera> m_EditorCamera;
		ref<Renderer::Camera> m_RuntimeCamera;

		// Input Context
		Engine::Surface m_Surface;
		ref<Engine::Input> m_InputContext;

		// Gizmo Variables
		bool m_ShowGizmos;
		bool m_GizmoHovered; // Is the user hovering a gizmo
		bool m_UsingGizmo; // Is the user using a gizmo
		i32 m_GizmoType;
		Engine::ECS::TransformComponent m_OriginalTransform; // The original transform of the entity currently being moved
		
		// Editor Camera Input Variables
		bool m_Hovered; // Is the user hovering the scene view image
		bool m_Focused; // Is the scene view window focused
		f32 m_CameraSpeed;
		f32 m_ZoomSpeed;
		f32 m_MaxZoom; // The most the user can be zoomed in
		f32 m_MinZoom; // The most the user can be zoomed out

		// Other
		glm::vec2 m_TopWindowPosition;
		glm::vec2 m_BottomWindowPosition;
	};

}