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
		void OnMouse(Core::MouseButton& e);
		void OnScroll(Core::MouseScroll& e);
		void OnKeyboard(Core::Keyboard& e);

		void EntityPicker();

		void ShowGizmos();
		void ShowOverlayWindow(f32 x, f32 y);

	private:
		Core::ID m_WindowID;
		
		// Context Variables
		ref<Scene> m_Context;
		Entity m_Selected;

		// Render Variables
		Engine::ECS::RenderSystem2DConfig m_RenderSystemConfig;
		ref<Engine::ECS::RenderSystem2D> m_RenderSystem;
		ref<Renderer::Texture> m_Frame;
		ref<Renderer::Camera> m_Camera;

		// Input Variables
		bool m_Hovered; // Is the user hovering the scene view image
		bool m_Focused; // Is the scene view window focused
		bool m_GizmoHovered; // Is the user hovering a gizmo
		bool m_UsingGizmo; // Is the user using a gizmo
		Engine::ECS::Transform m_OriginalTransform; // The original transform of the entity currently being moved
		Engine::Surface m_Surface;
		ref<Engine::Input> m_InputContext;
		
		f32 m_CameraSpeed;
		f32 m_ZoomSpeed;
		f32 m_MaxZoom; // The most the user can be zoomed in
		f32 m_MinZoom; // The most the user can be zoomed out
		i32 m_GizmoType;

		ref<Renderer::Shader> m_EntityPickerShader;
		ref<Renderer::FrameBuffer> m_EntityPickerBuffer;
	};

}