#pragma once
#include "ECS/Component.h"
#include "Renderer/Texture.h"
#include "Renderer/Camera.h"

namespace Enigma {
	namespace ECS {

		// Entities with the Render2D component will be rendered by the 2D renderer
		// Uses only the x and y values of the position and scale vectors from the transform
		// only Uses the z value of the transforms rotation vector
		class Render2D : public Component {
		public:
			COMP_DEF(Render2D);
			Render2D(const glm::vec3& tint = { 1.0f, 1.0f, 1.0f }, float depth = 0.0f,
				Renderer::Texture* texture = nullptr)
				: m_Tint(tint), m_Depth(depth), m_Texture(texture)
			{
				m_ID = CreateID(this);
			}

			float& GetDepth() { return m_Depth; }
			glm::vec3& GetTint() { return m_Tint; }	
			Renderer::Texture* GetTexture() { return m_Texture; }
			void SetTexture(Renderer::Texture* texture) { m_Texture = texture; }

		private:
			glm::vec3 m_Tint;
			Renderer::Texture* m_Texture;
			float m_Depth; // controlls the order entities get drawn in
		};

		// TODO: make this class not awful
		// Their can only be one active camera at a time
		// When you set a different camera to be active it will deacivate the last active camera
		class Camera : public Component {
		public:
			enum class Type {
				Perspective,
				Orthographic
			};
		public:
			COMP_DEF(Camera);

			Type& GetType() { return m_Type; }

			static Camera* GetActiveCamera() { return Get(s_CurrentActiveCamera); }

			bool GetIsActive() { return m_IsActive; }
			void SetIsActive(bool isActive) {
				m_IsActive = isActive;
				if (s_IDHandler.IsValid(s_CurrentActiveCamera))
					Get(s_CurrentActiveCamera)->m_IsActive = false;
				s_CurrentActiveCamera = m_ID;
			}

			// Converts a world position to a screen position
			virtual glm::vec2 ScreenPosition(const glm::vec3& worldPos) = 0;

			// Converts a screen position to a world position, the z value will be 0
			virtual glm::vec3 WorldPosition(const glm::vec2& screenPos) = 0;

		protected:
			Camera(bool isActive, Type type = Type::Orthographic)
				: m_Type(type), m_IsActive(false)
			{
				m_ID = CreateID(this);
				s_CurrentActiveCamera = Core::ID::InvalidID();
				SetIsActive(isActive);
			}

		private:
			Type m_Type;
			bool m_IsActive;

		private:
			inline static Core::ID s_CurrentActiveCamera;
		};

		class OrthographicCamera : public Camera {
		public:
			OrthographicCamera(Renderer::ViewBox viewBox, float zoom, bool isActive) 
				: Camera(isActive, Type::Orthographic), m_ViewBox(viewBox), m_Zoom(zoom) { }

			Renderer::ViewBox& GetViewBox() { return m_ViewBox; }
			float& GetZoom() { return m_Zoom; }

			// Converts a world position to a screen position
			virtual glm::vec2 ScreenPosition(const glm::vec3& worldPos) override;

			// Converts a screen position to a world position, the z value will be 0
			virtual glm::vec3 WorldPosition(const glm::vec2& screenPos) override;

		private:
			Renderer::ViewBox m_ViewBox;
			float m_Zoom;
		};
	}
}