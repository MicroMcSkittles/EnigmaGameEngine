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
			Render2D(const glm::vec3& tint = { 1.0f, 1.0f, 1.0f },
				Renderer::Texture* texture = nullptr)
				: m_Tint(tint), m_Texture(texture)
			{
				m_ID = CreateID(this);
			}

			const glm::vec3& GetTint() { return m_Tint; }
			void SetTint(const glm::vec3& tint) { m_Tint = tint; }
			
			Renderer::Texture* GetTexture() { return m_Texture; }
			void SetTexture(Renderer::Texture* texture) { m_Texture = texture; }

		private:
			glm::vec3 m_Tint;
			Renderer::Texture* m_Texture;
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
			Camera(bool isActive, Type type = Type::Orthographic)
				: m_Type(type), m_IsActive(false)
			{
				m_ID = CreateID(this);
				s_CurrentActiveCamera = Core::ID::InvalidID();
				SetIsActive(isActive);
			}

			Type& GetType() { return m_Type; }

			bool GetIsActive() { return m_IsActive; }
			void SetIsActive(bool isActive) {
				m_IsActive = isActive;
				if (s_IDHandler.IsValid(s_CurrentActiveCamera))
					Get(s_CurrentActiveCamera)->m_IsActive = false;
				s_CurrentActiveCamera = m_ID;
			}

		private:
			Type m_Type;
			bool m_IsActive;

		private:
			inline static Core::ID s_CurrentActiveCamera;
		};
	}
}