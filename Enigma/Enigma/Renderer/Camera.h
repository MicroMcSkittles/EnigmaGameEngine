#pragma once
#include "Core/IdHandler.h"
#include <glm/glm.hpp>

namespace Enigma {
	namespace Renderer {
		class Camera {
		public:
			Camera(const glm::vec3& position, const glm::vec3& direction)
				: m_Position(position), m_Direction(direction) { }
			~Camera() { s_IDHandler.Delete(m_ID); }

			virtual void Resize(uint32_t width, uint32_t height) = 0;

			virtual void CalculateView() = 0;
			virtual void CalculateProjection() = 0;

			const glm::vec3& GetPosition() const { return m_Position; }
			const glm::vec3& GetDirection() const { return m_Direction; }

			void SetPosition(const glm::vec3& position) { m_Position = position; CalculateView(); }
			void SetDirection(const glm::vec3& direction) { m_Direction = direction; CalculateView(); }

			const glm::mat4& GetView() const { return m_View; }
			const glm::mat4& GetProjection() const { return m_Projection; }
			const glm::mat4& GetViewProjection() const { return m_ViewProjection; }

			glm::vec2 WorldToScreen(const glm::vec3& worldPoint);
			glm::vec3 ScreenToWorld(const glm::vec2& screenPoint);

		public:
			Core::ID GetID() { return m_ID; }

		protected:
			glm::vec3 m_Position;
			glm::vec3 m_Direction;

			glm::mat4 m_View;
			glm::mat4 m_Projection;
			glm::mat4 m_ViewProjection;
			
			Core::ID m_ID;

		protected:
			inline static Core::IDHandler<Camera*> s_IDHandler;
		};

		struct Frustum {
			float fov;
			float aspectRatio;
			float near;
			float far;

			static Frustum ScreenFrustum();
		};

		class PerspectiveCamera : public Camera {
		public:
			PerspectiveCamera(const Frustum& frustum, const glm::vec3& position, const glm::vec3& direction);

			virtual void Resize(uint32_t width, uint32_t height) override;

			virtual void CalculateView() override;
			virtual void CalculateProjection() override;

			const Frustum& GetFrustum() { return m_Frustum; }

		private:
			Frustum m_Frustum;
		};

		struct ViewBox {
			float top;
			float bottom;
			float left;
			float right;
			float near;
			float far;

			ViewBox Zoom(float zoom) const;

			static ViewBox ScreenViewBox();
		};

		class OrthographicCamera : public Camera {
		public:
			OrthographicCamera(const ViewBox& viewBox, float zoom = 1.0f, const glm::vec3& position = { 0,0,0 }, const glm::vec3& direction = { 0,0,1 });

			virtual void Resize(uint32_t width, uint32_t height) override;

			virtual void CalculateView() override;
			virtual void CalculateProjection() override;

			const ViewBox& GetViewBox() { return m_ViewBox; }

		private:
			ViewBox m_ViewBox;
			ViewBox m_ZoomViewBox;
			float m_Zoom;
		};
	}
}